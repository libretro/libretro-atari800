#include <stdarg.h>
#include <string/stdstring.h>

#include "libretro.h"
#include "libretro_core_options.h"

#include "libretro-core.h"
#include "retro_strings.h"

// DISK CONTROL
#include "retro_disk_control.h"
static dc_storage* dc;

#include "antic.h"
#include "atari.h"
#include "afile.h"
#include "devices.h"
#include "esc.h"
#include "memory.h"
#include "cassette.h"
#include "artifact.h"
#include "statesav.h"

cothread_t mainThread;
cothread_t emuThread;

static void fallback_log(enum retro_log_level level, const char* fmt, ...);
retro_log_printf_t log_cb = fallback_log;

int CROP_WIDTH;
int CROP_HEIGHT;
int VIRTUAL_WIDTH;
int retrow = 400;
int retroh = 300;

#define ATARI_JOYPAD_BUTTONS 16
#define ATARI_NUMBER_JOYSTICKS 4

#define RETRO_DEVICE_ATARI_KEYBOARD RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_KEYBOARD, 0)
#define RETRO_DEVICE_ATARI_JOYSTICK RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 1)
#define RETRO_DEVICE_ATARI_5200_JOYSTICK RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 2)

/* Atari 800 Joysticks 1 thru 4 */
#define RETRO_DESCRIPTOR_BLOCK(_user)                                                       \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Up" },                  \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Down" },              \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Left" },              \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },            \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "Fire 1" },               \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "Fire 2" },               \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "Space" },                 \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X, "Return" },               \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },          \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },            \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L, "Option" },               \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R, "Atari800 Menu" },        \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2, "Esc" },               \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2, "Help" },                 \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3, "Virtual Keyboard" },    \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3, "R3" }

static struct retro_input_descriptor inputDescriptors_a800[] =
{
   RETRO_DESCRIPTOR_BLOCK(0),
   RETRO_DESCRIPTOR_BLOCK(1),
   RETRO_DESCRIPTOR_BLOCK(2),
   RETRO_DESCRIPTOR_BLOCK(3),
   {0},
};
#undef RETRO_DESCRIPTOR_BLOCK

/* Atari 5200 Joysticks 1 thru 4 */
#define RETRO_DESCRIPTOR_BLOCK(_user)                                                               \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Joystick Up (Digital)" },       \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Joystick Down (Digital)" },   \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Joystick Left (Digital)" },   \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Joystick Right (Digital)" }, \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "Fire 1" },                       \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "Fire 2" },                       \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "Numpad #" },                     \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X, "Numpad *" },                     \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Pause" },                   \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },                    \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L, "Numpad 0" },                     \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R, "Numpad 1" },                     \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2, "Numpad 2" },                    \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2, "Numpad 3" },                    \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3, "Numpad 7" },                    \
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3, "Virtual Keyboard" }

static struct retro_input_descriptor inputDescriptors_a5200[] =
{
   RETRO_DESCRIPTOR_BLOCK(0),
   RETRO_DESCRIPTOR_BLOCK(1),
   RETRO_DESCRIPTOR_BLOCK(2),
   RETRO_DESCRIPTOR_BLOCK(3),
   {0},
};
#undef RETRO_DESCRIPTOR_BLOCK

/* Dynamic inputdescripter */
static struct retro_input_descriptor inputDescriptors_dyna[(ATARI_JOYPAD_BUTTONS * ATARI_NUMBER_JOYSTICKS) + 1];

unsigned atari_devices[4];

#define A800_SAVE_STATE_SIZE 1300000

// libretro-Atari800 core options variables
int keyboard_type = 0;
int autorunCartridge = 0;
int atari_joyhack = 0;
int paddle_mode = 0;
int paddle_speed = 3;

extern int INPUT_joy_5200_center;
extern int INPUT_joy_5200_min;
extern int INPUT_joy_5200_max;
extern int INPUT_digital_5200_min;
extern int INPUT_digital_5200_center;
extern int INPUT_digital_5200_max;

int pot_analog_deadzone = (int)(0.15f * (float)LIBRETRO_ANALOG_RANGE);

int RETROJOY = 0, RETROPT0 = 0, RETROSTATUS = 0, RETRODRVTYPE = 0;
int retrojoy_init = 0, retro_ui_finalized = 0;
int retro_sound_finalized = 0;

int libretro_runloop_active = 0;

char old_Atari800_machine_type[100];

float retro_fps = 49.8607597;
long long retro_frame_counter;
extern int ToggleTV;
extern int CURRENT_TV;

extern int SHIFTON, pauseg, SND;
extern unsigned char SNDBUF[];

char RPATH[512];
char RETRO_DIR[512];
int cap32_statusbar = 0;

#include "cmdline.c"

extern void update_input(void);
extern void texture_init(void);
extern void texture_uninit(void);
extern void Emu_init();
extern void Emu_uninit();
extern void input_gui(void);

const char* retro_save_directory;
const char* retro_system_directory;
const char* retro_content_directory;
char retro_system_data_directory[512];;

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;

bool libretro_supports_bitmasks = false;

static void fallback_log(enum retro_log_level level, const char* fmt, ...)
{
    va_list va;

    (void)level;

    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
}

int HandleExtension(char* path, char* ext)
{
    int len = strlen(path);

    if (len >= 4 &&
        path[len - 4] == '.' &&
        path[len - 3] == ext[0] &&
        path[len - 2] == ext[1] &&
        path[len - 1] == ext[2])
    {
        return 1;
    }

    return 0;
}

//*****************************************************************************
//*****************************************************************************
// Disk control

extern void SIO_Dismount(int diskno);
extern int SIO_Mount(int diskno, const char* filename, int b_open_readonly);
extern void CART_Remove(void);
extern int CASSETTE_Insert(const char* filename);
extern void CASSETTE_Remove(void);
extern void CASSETTE_Seek(unsigned int position);

//extern int SIO_RotateDisks(void);

void retro_message(const char* text, unsigned int frames, int alt) {
    struct retro_message msg;
    struct retro_message_ext msg_ext;

    char msg_local[256];
    unsigned int message_interface_version;

    snprintf(msg_local, sizeof(msg_local), "Atari800: %s", text);
    msg.msg = msg_local;
    msg.frames = frames;

    msg_ext.msg = msg_local;
    msg_ext.duration = frames;
    msg_ext.priority = 3;
    msg_ext.level = RETRO_LOG_INFO;
    msg_ext.target = RETRO_MESSAGE_TYPE_NOTIFICATION_ALT;
    msg_ext.type = -1 ;

    if (environ_cb(RETRO_ENVIRONMENT_GET_MESSAGE_INTERFACE_VERSION, &message_interface_version) && (message_interface_version >= 1))
    {
        if (alt)
            environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE_EXT, (void*)&msg_ext);
        else
            environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE_EXT, (void*)&msg);
    }
    else
        environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, (void*)&msg);
}

static int get_image_unit()
{
    int unit = dc->unit;
    if (dc->index < dc->count)
    {
        if (dc_get_image_type(dc->files[dc->index]) == DC_IMAGE_TYPE_TAPE)
            dc->unit = DC_IMAGE_TYPE_TAPE;
        else if (dc_get_image_type(dc->files[dc->index]) == DC_IMAGE_TYPE_FLOPPY)
            dc->unit = DC_IMAGE_TYPE_FLOPPY;
        else
            dc->unit = DC_IMAGE_TYPE_FLOPPY;
    }
    else
        unit = DC_IMAGE_TYPE_FLOPPY;

    return unit;
}

static void retro_insert_image()
{
    if (dc->unit == DC_IMAGE_TYPE_TAPE)
    {
        int error = CASSETTE_Insert(dc->files[dc->index]);
        if (!error)
        {
            CASSETTE_Seek(0);
            log_cb(RETRO_LOG_INFO,"[retro_insert_image] Tape (%d) inserted: %s\n", dc->index + 1, dc->names[dc->index]);
        }
        else
        {
            log_cb(RETRO_LOG_INFO,"[retro_insert_image] Tape Error (%d): %s\n", error, dc->files[dc->index]);
        }
    }
    else if (dc->unit == DC_IMAGE_TYPE_FLOPPY)
    {
        int error = SIO_Mount(1, dc->files[dc->index], 0);
        if (error)
        {
            log_cb(RETRO_LOG_INFO,"[retro_insert_image] Disk (%d) Error : %s\n", dc->index + 1, dc->files[dc->index]);
            return;
        }
        log_cb(RETRO_LOG_INFO,"[retro_insert_image] Disk (%d) inserted into drive 1 : %s\n", dc->index + 1, dc->files[dc->index]);
    }
    else
    {
        log_cb(RETRO_LOG_INFO,"[retro_insert_image] unsupported image-type : %u\n", dc->unit);
    }
}

static bool retro_set_eject_state(bool ejected)
{
    if (dc)
    {
        int unit = get_image_unit();

        if (dc->eject_state == ejected)
            return true;

        if (ejected && dc->index <= dc->count && dc->files[dc->index] != NULL)
        {
            if (unit == DC_IMAGE_TYPE_TAPE)
            {
                CASSETTE_Remove();
                log_cb(RETRO_LOG_INFO,"[retro_set_eject_state] Tape (%d/%d) ejected %s\n", dc->index + 1, dc->count, dc->names[dc->index]);
            }
            else
            {
                SIO_Dismount(1);
                log_cb(RETRO_LOG_INFO,"[retro_set_eject_state] Disk (%d/%d) ejected: %s\n", dc->index + 1, dc->count, dc->names[dc->index]);
            }
        }
        else if (!ejected && dc->index < dc->count && dc->files[dc->index] != NULL)
        {
            retro_insert_image();
        }

        dc->eject_state = ejected;
        return true;
    }

    return false;
}

/* Gets current eject state. The initial state is 'not ejected'. */
static bool retro_get_eject_state(void)
{
    if (dc)
        return dc->eject_state;

    return true;
}

static unsigned retro_get_image_index(void)
{
    if (dc)
        return dc->index;

    return 0;
}

/* Sets image index. Can only be called when disk is ejected.
 * The implementation supports setting "no disk" by using an
 * index >= get_num_images().
 */
static bool retro_set_image_index(unsigned index)
{
    // Insert image
    if (dc)
    {
        if (index == dc->index)
            return true;

        if (dc->replace)
        {
            dc->replace = false;
            index = 0;
        }

        if (index < dc->count && dc->files[index])
        {
            dc->index = index;
            int unit = get_image_unit();
            log_cb(RETRO_LOG_INFO,"[retro_set_image_index] Unit (%d) image (%d/%d) inserted: %s\n", dc->index + 1, unit, dc->count, dc->files[dc->index]);
            return true;
        }
    }

    return false;
}

static unsigned retro_get_num_images(void)
{
    if (dc)
        return dc->count;

    return 0;
}

/* Adds a new valid index (get_num_images()) to the internal disk list.
 * This will increment subsequent return values from get_num_images() by 1.
 * This image index cannot be used until a disk image has been set
 * with replace_image_index. */
static bool retro_add_image_index(void)
{
    if (dc)
    {
        if (dc->count <= DC_MAX_SIZE)
        {
            dc->files[dc->count] = NULL;
            dc->names[dc->count] = NULL;
            dc->types[dc->count] = DC_IMAGE_TYPE_NONE;
            dc->count++;
            return true;
        }
    }

    return false;
}

static bool retro_replace_image_index(unsigned index, const struct retro_game_info* info)
{
    if (dc)
    {
        if (info != NULL)
        {
            int error = dc_replace_file(dc, index, info->path);

            if ( error == 2)
                retro_message("Duplicate Disk selected.  Use Index", 6000, 1);
        }
        else
        {
            dc_remove_file(dc, index);
        }

        return true;
    }

    return false;
}

static bool retro_get_image_path(unsigned index, char* path, size_t len)
{
    if (len < 1)
        return false;

    if (dc)
    {
        if (index < dc->count)
        {
            if (!string_is_empty(dc->files[index]))
            {
                strncpy(path, dc->files[index], len);
                return true;
            }
        }
    }

    return false;
}

static bool retro_get_image_label(unsigned index, char* label, size_t len)
{
    if (len < 1)
        return false;

    if (dc)
    {
        if (index < dc->count)
        {
            if (!string_is_empty(dc->names[index]))
            {
                strncpy(label, dc->names[index], len);
                return true;
            }
        }
    }

    return false;
}

static struct retro_disk_control_callback disk_interface = {
   retro_set_eject_state,
   retro_get_eject_state,
   retro_get_image_index,
   retro_set_image_index,
   retro_get_num_images,
   retro_replace_image_index,
   retro_add_image_index,
};

static struct retro_disk_control_ext_callback disk_interface_ext = {
   retro_set_eject_state,
   retro_get_eject_state,
   retro_get_image_index,
   retro_set_image_index,
   retro_get_num_images,
   retro_replace_image_index,
   retro_add_image_index,
   NULL, /* disk_set_initial_image, not even sure if I want to use this */
   retro_get_image_path,
   retro_get_image_label,
};

void retro_set_environment(retro_environment_t cb)
{
    bool option_cats_supported;

    environ_cb = cb;

    static const struct retro_controller_description p1_controllers[] = {
      { "ATARI Joystick", RETRO_DEVICE_ATARI_JOYSTICK },
      { "ATARI 5200 Joystick", RETRO_DEVICE_ATARI_5200_JOYSTICK },
      { "ATARI Keyboard", RETRO_DEVICE_ATARI_KEYBOARD },
    };
    static const struct retro_controller_description p2_controllers[] = {
      { "ATARI Joystick", RETRO_DEVICE_ATARI_JOYSTICK },
      { "ATARI 5200 Joystick", RETRO_DEVICE_ATARI_5200_JOYSTICK },
      { "ATARI Keyboard", RETRO_DEVICE_ATARI_KEYBOARD },
    };
    static const struct retro_controller_description p3_controllers[] = {
      { "ATARI Joystick", RETRO_DEVICE_ATARI_JOYSTICK },
      { "ATARI 5200 Joystick", RETRO_DEVICE_ATARI_5200_JOYSTICK },
      { "ATARI Keyboard", RETRO_DEVICE_ATARI_KEYBOARD },
    };
    static const struct retro_controller_description p4_controllers[] = {
      { "ATARI Joystick", RETRO_DEVICE_ATARI_JOYSTICK },
      { "ATARI 5200 Joystick", RETRO_DEVICE_ATARI_5200_JOYSTICK },
      { "ATARI Keyboard", RETRO_DEVICE_ATARI_KEYBOARD },
    };

    static const struct retro_controller_info ports[] = {
      { p1_controllers, 3  }, // port 1
      { p2_controllers, 3  }, // port 2
      { p3_controllers, 3  }, // port 3
      { p4_controllers, 3  }, // port 4
      { NULL, 0 }
    };

    /* Initialise core options */
    libretro_set_core_options(environ_cb, &option_cats_supported);

    cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);
}

static void update_variables(void)
{
    struct retro_variable var;

    /* Atari Cartridge Autodetect leave at TOP!!!!!*/
    var.key = "atari800_opt1";
    var.value = NULL;

    /* Moved here for better consistency and when system options that require EMU reset to occur */
    if (HandleExtension((char*)RPATH, "a52") || HandleExtension((char*)RPATH, "A52"))
        autorunCartridge = 1;
    else if (HandleExtension((char*)RPATH, "bin") || HandleExtension((char*)RPATH, "BIN")
            || HandleExtension((char*)RPATH, "rom") || HandleExtension((char*)RPATH, "ROM"))
        autorunCartridge = 2;
    else if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "enabled") == 0)
            autorunCartridge = 1;
        else
            autorunCartridge = 0;
    }

    /* Controller Hack for Dual Stick or Swap Ports*/
    var.key = "atari800_opt2";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if ( (strcmp(var.value, "Dual Stick") == 0) ||
             (strcmp(var.value, "enabled") == 0))        // to accomodate for old value
            atari_joyhack = 1;
        else if ( strcmp(var.value,"Swap Ports") == 0)
            atari_joyhack = 2;
        else
            atari_joyhack = 0;
    }

    /* Sets the Atari 800 internal emulated resolution */
    var.key = "atari800_resolution";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        char* pch;
        char str[100];
        snprintf(str, sizeof(str), "%s", var.value);

        pch = strtok(str, "x");
        if (pch)
            retrow = strtoul(pch, NULL, 0);
        pch = strtok(NULL, "x");
        if (pch)
            retroh = strtoul(pch, NULL, 0);

        fprintf(stderr, "[libretro-atari800]: Got size: %u x %u.\n", retrow, retroh);

        CROP_WIDTH = retrow;
        CROP_HEIGHT = (retroh - 80);
        VIRTUAL_WIDTH = retrow;
        texture_init();
        //reset_screen();
    }

    /* Set the Atari system emulated */
    var.key = "atari800_system";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        /* I moved "5200" comparison up to top because I'm lazy.  :P  Leave it here! */
        if (autorunCartridge ==1 || (strcmp(var.value, "5200") == 0))
        {
            Atari800_machine_type = Atari800_MACHINE_5200;
            MEMORY_ram_size = 16;
            Atari800_builtin_basic = FALSE;
            Atari800_keyboard_leds = FALSE;
            Atari800_f_keys = FALSE;
            Atari800_jumper = FALSE;
            Atari800_builtin_game = FALSE;
            Atari800_keyboard_detached = FALSE;
        }
        else if (strcmp(var.value, "400/800 (OS B)") == 0)
        {
            Atari800_machine_type = Atari800_MACHINE_800;
            MEMORY_ram_size = 48;
            Atari800_builtin_basic = FALSE;
            Atari800_keyboard_leds = FALSE;
            Atari800_f_keys = FALSE;
            Atari800_jumper = FALSE;
            Atari800_builtin_game = FALSE;
            Atari800_keyboard_detached = FALSE;
        }
        else if (strcmp(var.value, "800XL (64K)") == 0)
        {
            Atari800_machine_type = Atari800_MACHINE_XLXE;
            MEMORY_ram_size = 64;
            Atari800_builtin_basic = TRUE;
            Atari800_keyboard_leds = FALSE;
            Atari800_f_keys = FALSE;
            Atari800_jumper = FALSE;
            Atari800_builtin_game = FALSE;
            Atari800_keyboard_detached = FALSE;
        }
        else if (strcmp(var.value, "130XE (128K)") == 0)
        {
            Atari800_machine_type = Atari800_MACHINE_XLXE;
            MEMORY_ram_size = 128;
            Atari800_builtin_basic = TRUE;
            Atari800_keyboard_leds = FALSE;
            Atari800_f_keys = FALSE;
            Atari800_jumper = FALSE;
            Atari800_builtin_game = FALSE;
            Atari800_keyboard_detached = FALSE;
        }
        else if (strcmp(var.value, "Modern XL/XE(320K CS)") == 0)
        {
            Atari800_machine_type = Atari800_MACHINE_XLXE;
            MEMORY_ram_size = MEMORY_RAM_320_COMPY_SHOP;
            Atari800_builtin_basic = TRUE;
            Atari800_keyboard_leds = FALSE;
            Atari800_f_keys = FALSE;
            Atari800_jumper = FALSE;
            Atari800_builtin_game = FALSE;
            Atari800_keyboard_detached = FALSE;
        }
        else if (strcmp(var.value, "Modern XL/XE(576K)") == 0)
        {
            Atari800_machine_type = Atari800_MACHINE_XLXE;
            MEMORY_ram_size = 576;
            Atari800_builtin_basic = TRUE;
            Atari800_keyboard_leds = FALSE;
            Atari800_f_keys = FALSE;
            Atari800_jumper = FALSE;
            Atari800_builtin_game = FALSE;
            Atari800_keyboard_detached = FALSE;
        }
        else if (strcmp(var.value, "Modern XL/XE(1088K)") == 0)
        {
            Atari800_machine_type = Atari800_MACHINE_XLXE;
            MEMORY_ram_size = 1088;
            Atari800_builtin_basic = TRUE;
            Atari800_keyboard_leds = FALSE;
            Atari800_f_keys = FALSE;
            Atari800_jumper = FALSE;
            Atari800_builtin_game = FALSE;
            Atari800_keyboard_detached = FALSE;
        }

        if (!libretro_runloop_active || (strcmp(var.value, old_Atari800_machine_type) != 0 ))
        {
            Atari800_InitialiseMachine();
            strcpy(old_Atari800_machine_type, var.value);
        }
    }

    /* Are we running in NTSC (60hz) or Pal Mode (50hz) */
    var.key = "atari800_ntscpal";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "NTSC") == 0)
        {
            Atari800_tv_mode = Atari800_TV_NTSC;
        }
        else if (strcmp(var.value, "PAL") == 0)
        {
            Atari800_tv_mode = Atari800_TV_PAL;
        }
    }

    /* Activate or deactivate built in internal BASIC*/
    var.key = "atari800_internalbasic";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "enabled") == 0)
        {
            Atari800_disable_basic = FALSE;
        }
        else if (strcmp(var.value, "disabled") == 0)
        {
            Atari800_disable_basic = TRUE;
        }
        
        if (!libretro_runloop_active)
            Atari800_InitialiseMachine();
    }

    /* Set whether SIO acceleration is activated.  Currently an ALL or nothing setup. */
    var.key = "atari800_sioaccel";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "enabled") == 0)
        {
            ESC_enable_sio_patch = Devices_enable_h_patch = Devices_enable_p_patch = Devices_enable_r_patch = TRUE;
        }
        else if (strcmp(var.value, "disabled") == 0)
        {
            ESC_enable_sio_patch = Devices_enable_h_patch = Devices_enable_p_patch = Devices_enable_r_patch = FALSE;
        }

        if (!libretro_runloop_active)
            Atari800_InitialiseMachine();
        else
            ESC_UpdatePatches();    /* We need to do this for it to take while the emulator is running */
    }

    /* Set whether a cassette image will autoboot ( Hold start on boot ). */
    var.key = "atari800_cassboot";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "enabled") == 0)
        {
            CASSETTE_hold_start = 1;
        }
        else if (strcmp(var.value, "disabled") == 0)
        {
            CASSETTE_hold_start = 0;
        }

        if (!libretro_runloop_active)
            Atari800_InitialiseMachine();
    }

    /* Set artifacting type.  */
    var.key = "atari800_artifacting_mode";
    var.value = NULL;

    if ( environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        int old_artif_mode = ANTIC_artif_mode;

        if (strcmp(var.value, "none") == 0)
        {
            ANTIC_artif_mode = 0;
        }
        if (strcmp(var.value, "blue/brown 1") == 0)
        {
            ANTIC_artif_mode = 1;
        }
        else if (strcmp(var.value, "blue/brown 2") == 0)
        {
            ANTIC_artif_mode = 2;
        }
        else if (strcmp(var.value, "GTIA") == 0)
        {
            ANTIC_artif_mode = 3;
        }
        else if (strcmp(var.value, "CTIA") == 0)
        {
            ANTIC_artif_mode = 4;
        }

        /* only do this if changed from off to on */
        if (ANTIC_artif_mode && !old_artif_mode)
        {
            if (Atari800_tv_mode == Atari800_TV_NTSC)
            {
                ARTIFACT_Set(ARTIFACT_NTSC_OLD);
                ARTIFACT_SetTVMode(Atari800_TV_NTSC);
            }
            else if (Atari800_tv_mode == Atari800_TV_PAL)
            {
                ARTIFACT_Set(ARTIFACT_NONE); // PAL Blending has been flipped off in config for now.
                ARTIFACT_SetTVMode(Atari800_TV_PAL);
            }
        }
        /* only do this if changed from on to off*/
        else if (!ANTIC_artif_mode && old_artif_mode)
        {
            if (Atari800_tv_mode == Atari800_TV_NTSC)
            {
                ARTIFACT_Set(ARTIFACT_NONE);
                ARTIFACT_SetTVMode(Atari800_TV_NTSC);
            }
            else if (Atari800_tv_mode == Atari800_TV_PAL)
            {
                ARTIFACT_Set(ARTIFACT_NONE);
                ARTIFACT_SetTVMode(Atari800_TV_PAL);
            }
        }

        ANTIC_UpdateArtifacting();
    }

    /* Set whether paddle mode is active. */
    var.key = "paddle_active";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "enabled") == 0)
            paddle_mode = 1;
        else
            paddle_mode = 0;
    }

    /* Set paddle movement speed. */
    var.key = "paddle_movement_speed";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        paddle_speed = (int)string_to_unsigned(var.value);
    }

    var.key = "atari800_keyboard";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "poll") == 0)
        {
            keyboard_type = 0;
        }
        else if (strcmp(var.value, "callback") == 0)
        {
            keyboard_type = 1;
        }
    }

    /* Digital Joystick/Paddle Sensitivity */
    var.key = "pot_digital_sensitivity";
    var.value = NULL;
    INPUT_digital_5200_min = JOY_5200_MIN;
    INPUT_digital_5200_max = JOY_5200_MAX;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value )
    {
        float range;

        if (string_is_equal(var.value, "100"))
            range = 1;
        else
            range = (float)string_to_unsigned(var.value) / 100.0f;

        INPUT_digital_5200_min = JOY_5200_CENTER -
            (unsigned int)(((float)(JOY_5200_CENTER - JOY_5200_MIN) *
                range) + 0.5f);
        INPUT_digital_5200_max = JOY_5200_CENTER +
            (unsigned int)(((float)(JOY_5200_MAX - JOY_5200_CENTER) *
                range) + 0.5f);
    }

    /* Analog Joystick Sensitivity */
    var.key = "pot_analog_sensitivity";
    var.value = NULL;
    INPUT_joy_5200_min = JOY_5200_MIN;
    INPUT_joy_5200_max = JOY_5200_MAX;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value )
    {
        float range;

        if (string_is_equal(var.value, "100"))
            range = 1;
        else
            range = (float)string_to_unsigned(var.value) / 100.0f;

        INPUT_joy_5200_min = JOY_5200_CENTER -
            (unsigned int)(((float)(JOY_5200_CENTER - JOY_5200_MIN) *
                range) + 0.5f);
        INPUT_joy_5200_max = JOY_5200_CENTER +
            (unsigned int)(((float)(JOY_5200_MAX - JOY_5200_CENTER) *
                range) + 0.5f);
    }

    /* Analog Joystick Deadzone */
    var.key = "pot_analog_deadzone";
    var.value = NULL;
    pot_analog_deadzone = (int)(0.15f * (float)LIBRETRO_ANALOG_RANGE);

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value )
    {
        unsigned deadzone = string_to_unsigned(var.value);
        pot_analog_deadzone = (int)((float)deadzone * 0.01f * (float)LIBRETRO_ANALOG_RANGE);
    }
}

static void retro_wrap_emulator()
{
    log_cb(RETRO_LOG_INFO, "WRAP EMU THD\n");
    pre_main(RPATH);


    log_cb(RETRO_LOG_INFO, "EXIT EMU THD\n");
    pauseg = -1;

    //environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, 0);

    // Were done here
    co_switch(mainThread);

    // Dead emulator, but libco says not to return
    while (true)
    {
        log_cb(RETRO_LOG_INFO, "Running a dead emulator.");
        co_switch(mainThread);
    }

}

void Emu_init() {

#ifdef RETRO_AND
    MOUSEMODE = 1;
#endif

    //  update_variables();

    memset(Key_State, 0, 512);

    if (!emuThread && !mainThread)
    {
        mainThread = co_active();
        emuThread = co_create(65536 * sizeof(void*), retro_wrap_emulator);
    }

    old_Atari800_machine_type[0] = 0;

    update_variables();
}

void Emu_uninit() {

    texture_uninit();
}

void retro_shutdown_core(void)
{
    log_cb(RETRO_LOG_INFO, "SHUTDOWN\n");

    texture_uninit();
    environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, NULL);
}

void retro_reset(void) {

    if (dc)
    {
        retro_set_eject_state(TRUE);
        retro_set_image_index(0);
    }

    AFILE_OpenFile(RPATH, 1, 1, 0);
}

void retro_get_system_av_info(struct retro_system_av_info* info)
{
    update_variables();

    info->geometry.base_width = retrow;
    info->geometry.base_height = retroh;

    if (log_cb)
        log_cb(RETRO_LOG_INFO, "AV_INFO: width=%d height=%d\n", info->geometry.base_width, info->geometry.base_height);

    info->geometry.max_width = 400;
    info->geometry.max_height = 300;

    if (log_cb)
        log_cb(RETRO_LOG_INFO, "AV_INFO: max_width=%d max_height=%d\n", info->geometry.max_width, info->geometry.max_height);

    info->geometry.aspect_ratio = 4.0 / 3.0;

    if (log_cb)
        log_cb(RETRO_LOG_INFO, "AV_INFO: aspect_ratio = %f\n", info->geometry.aspect_ratio);

    info->timing.fps = retro_fps;
    info->timing.sample_rate = 44100.0;

    if (log_cb)
        log_cb(RETRO_LOG_INFO, "AV_INFO: fps = %f sample_rate = %f\n", info->timing.fps, info->timing.sample_rate);

}

void retro_init(void)
{
    unsigned dci_version = 0;
    struct retro_log_callback log;
    dc = dc_create();

    libretro_runloop_active = 0;

    if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
        log_cb = log.log;

    const char* system_dir = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &system_dir) && system_dir)
    {
        // if defined, use the system directory
        retro_system_directory = system_dir;
    }

    const char* content_dir = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY, &content_dir) && content_dir)
    {
        // if defined, use the system directory
        retro_content_directory = content_dir;
    }

    const char* save_dir = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &save_dir) && save_dir)
    {
        // If save directory is defined use it, otherwise use system directory
        retro_save_directory = *save_dir ? save_dir : retro_system_directory;
    }
    else
    {
        // make retro_save_directory the same in case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY is not implemented by the frontend
        retro_save_directory = retro_system_directory;
    }

    if (retro_system_directory == NULL)
        sprintf(RETRO_DIR, "%s", "." );
    else 
        sprintf(RETRO_DIR, "%s%c", retro_system_directory, '0');

    sprintf(retro_system_data_directory, "%s/data", RETRO_DIR);

    log_cb(RETRO_LOG_INFO, "Retro SYSTEM_DIRECTORY %s\n", retro_system_directory);
    log_cb(RETRO_LOG_INFO, "Retro SAVE_DIRECTORY %s\n", retro_save_directory);
    log_cb(RETRO_LOG_INFO, "Retro CONTENT_DIRECTORY %s\n", retro_content_directory);

#ifndef RENDER16B
    enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
#else
    enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
#endif

    if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
    {
        fprintf(stderr, "PIXEL FORMAT is not supported.\n");
        log_cb(RETRO_LOG_INFO, "PIXEL FORMAT is not supported.\n");
        exit(0);
    }

    /* set these up early retro_set_controller_port_device() will adjust later */
    for (int i = 0; i < 4; i++)
    {
        memcpy(inputDescriptors_dyna + ATARI_JOYPAD_BUTTONS * i, inputDescriptors_a800 + ATARI_JOYPAD_BUTTONS * i, (ATARI_JOYPAD_BUTTONS + 1) * sizeof(struct retro_input_descriptor));
        atari_devices[i] = RETRO_DEVICE_ATARI_JOYSTICK;
    }

    environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, &inputDescriptors_dyna);

    // Disk control interface
    if (environ_cb(RETRO_ENVIRONMENT_GET_DISK_CONTROL_INTERFACE_VERSION, &dci_version) && (dci_version >= 1))
        environ_cb(RETRO_ENVIRONMENT_SET_DISK_CONTROL_EXT_INTERFACE, &disk_interface_ext);
    else
        environ_cb(RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE, &disk_interface);

    Emu_init();
    texture_init();

    if (environ_cb(RETRO_ENVIRONMENT_GET_INPUT_BITMASKS, NULL))
        libretro_supports_bitmasks = true;
}

extern void main_exit();
void retro_deinit(void)
{
    Emu_uninit();

    co_switch(emuThread);
    log_cb(RETRO_LOG_INFO, "exit emu\n");
    // main_exit();
    co_switch(mainThread);
    log_cb(RETRO_LOG_INFO, "exit main\n");
    if (emuThread)
    {
        co_delete(emuThread);
        emuThread = 0;
    }

    // Clean the m3u storage
    if (dc)
    {
        dc_free(dc);
        dc = 0;
    }

    log_cb(RETRO_LOG_INFO, "Retro DeInit\n");

    libretro_supports_bitmasks = false;
}

unsigned retro_api_version(void)
{
    return RETRO_API_VERSION;
}


void retro_set_controller_port_device(unsigned port, unsigned device)
{
    if (port < 4)
    {
        atari_devices[port] = device;

        printf(" port(%d)=%d \n", port, device);

        if (device == RETRO_DEVICE_ATARI_JOYSTICK)
            memcpy(inputDescriptors_dyna + ATARI_JOYPAD_BUTTONS * port, inputDescriptors_a800 + ATARI_JOYPAD_BUTTONS * port, (ATARI_JOYPAD_BUTTONS + 1) * sizeof(struct retro_input_descriptor));
        else if (device == RETRO_DEVICE_ATARI_5200_JOYSTICK)
            memcpy(inputDescriptors_dyna + ATARI_JOYPAD_BUTTONS * port, inputDescriptors_a5200 + ATARI_JOYPAD_BUTTONS * port, (ATARI_JOYPAD_BUTTONS + 1) * sizeof(struct retro_input_descriptor));

        environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, &inputDescriptors_dyna);
    }
}

void retro_get_system_info(struct retro_system_info* info)
{
    memset(info, 0, sizeof(*info));
    info->library_name = "Atari800";
#ifdef GIT_VERSION
    info->library_version = "3.1.0" GIT_VERSION;
#else
    info->library_version = "3.1.0";
#endif
    info->valid_extensions = "xfd|atr|dcm|cas|bin|a52|zip|atx|car|rom|com|xex|m3u";
    info->need_fullpath = true;
    info->block_extract = false;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
    audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
    audio_batch_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
    video_cb = cb;
}

void retro_audio_cb(short l, short r)
{
    audio_cb(l, r);
}

/* Forward declarations */
void retro_sound_update(void);
int Retro_PollEvent(void);

void retro_run(void)
{
    bool updated = false;

    libretro_runloop_active = 1;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
        update_variables();

    retro_frame_counter++;

    if (pauseg == 0) {

        if (ToggleTV == 1)
        {
            struct retro_system_av_info ninfo;

            retro_fps = CURRENT_TV == 312 ? 49.8607597 : 59.9227434;

            retro_get_system_av_info(&ninfo);

            environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &ninfo);

            if (log_cb)
                log_cb(RETRO_LOG_INFO, "ChangeAV: w:%d h:%d ra:%f.\n",
                    ninfo.geometry.base_width, ninfo.geometry.base_height, ninfo.geometry.aspect_ratio);

            ToggleTV = 0;
        }

        if (retro_sound_finalized)
            retro_sound_update();

        Retro_PollEvent();
    }

    video_cb(Retro_Screen, retrow, retroh, retrow << PIXEL_BYTES);

    co_switch(emuThread);
}

extern char Key_State[512];
unsigned int lastdown, lastup, lastchar;
static void keyboard_cb(bool down, unsigned keycode,
    uint32_t character, uint16_t mod)
{
    if (keyboard_type == 0)return;
    Key_State[keycode] = down ? 1 : 0;

    /*
      printf( "Down: %s, Code: %d, Char: %u, Mod: %u.\n",
             down ? "yes" : "no", keycode, character, mod);
    */
}

bool retro_load_game(const struct retro_game_info* info)
{
    const char* full_path;
    bool media_is_disk_tape = TRUE;

    (void)info;

    struct retro_keyboard_callback cb = { keyboard_cb };

    environ_cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &cb);

    full_path = info->path;

    // If it's a m3u file
    if (strendswith(full_path, "M3U"))
    {
        // Parse the m3u file
        dc_parse_m3u(dc, full_path);

        // Some debugging
        //log_cb(RETRO_LOG_INFO, "m3u file parsed, %d file(s) found\n", dc->count);
        //for (unsigned i = 0; i < dc->count; i++)
        //{
        //    fprintf(fp, "file %d: %s\n", i + 1, dc->files[i]);
        //    log_cb(RETRO_LOG_INFO, "file %d: %s\n", i + 1, dc->files[i]);
        //}
    }
    else if (strendswith(full_path, "XFD") ||
             strendswith(full_path, "ATR") ||
             strendswith(full_path, "DCM") ||
             strendswith(full_path, "ATX") ||
             strendswith(full_path, "CAS"))
    {
        // Add the file to disk control context
        // Maybe, in a later version of retroarch, we could add disk on the fly (didn't find how to do this)
        dc_add_file(dc, full_path);
    }
    else
        media_is_disk_tape = FALSE;

    if (media_is_disk_tape)
    {
        // Init first disk
        dc->index = 0;
        dc->eject_state = false;
        log_cb(RETRO_LOG_INFO, "Disk/Cassette (%d) inserted into drive 1 : %s\n", dc->index + 1, dc->files[dc->index]);
        strcpy(RPATH, dc->files[0]);
    }
    else
        strcpy(RPATH, full_path);

    update_variables();

    // Moved elsewhere
    //if (HandleExtension((char*)RPATH, "a52") || HandleExtension((char*)RPATH, "A52"))
    //    autorunCartridge = 1;

#ifdef RENDER16B
    memset(Retro_Screen, 0, 400 * 300 * 2);
#else
    memset(Retro_Screen, 0, 400 * 300 * 2 * 2);
#endif
    memset(SNDBUF, 0, 1024 * 2 * 2);

    /* trouble shooting */
    //int fileType = 0;
    //char msg[256];
    //fileType = AFILE_DetectFileType(RPATH);
    //sprintf(msg, "File type detected=%i\n", fileType);
    //retro_message(msg, 10000, 0);

    co_switch(emuThread);

    return true;
}

void retro_unload_game(void) {

    pauseg = -1;
}

unsigned retro_get_region(void)
{
    return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info* info, size_t num)
{
    (void)type;
    (void)info;
    (void)num;
    return false;
}

size_t retro_serialize_size(void)
{
    uint8_t* data_;
    int size;

    data_ = calloc(A800_SAVE_STATE_SIZE, 1);
    size = Retro_SaveAtariState(data_, A800_SAVE_STATE_SIZE, 1);
    free(data_);

    //FILE* fp1 = fopen("E:\\debugme.txt", "a");
    //fprintf(fp1, "Estimated savestate size %i\n", size);
    //fclose(fp1);

    return size;
}

bool retro_serialize(void* data_, size_t size)
{
    int returned_size;

    returned_size = Retro_SaveAtariState(data_, size, 1);

    //FILE* fp1 = fopen("E:\\debugme.txt", "a");
    //fprintf(fp1, "Actual savestate size %i\n", returned_size);
    //fclose(fp1);

    return returned_size;
}

bool retro_unserialize(const void* data_, size_t size)
{
    int returned_size;

    returned_size = Retro_ReadAtariState(data_, size);

    //FILE* fp1 = fopen("E:\\debugme.txt", "a");
    //fprintf(fp1, "Savestate size read in %i\nFor game %s\n", returned_size,RPATH);
    //fclose(fp1);

    return returned_size;
}

void* retro_get_memory_data(unsigned id)
{
    if (id == RETRO_MEMORY_SYSTEM_RAM)
        return MEMORY_mem;

    return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
    if (id == RETRO_MEMORY_SYSTEM_RAM)
        return 65536;

    return 0;
}

void retro_cheat_reset(void) {}

void retro_cheat_set(unsigned index, bool enabled, const char* code)
{
    (void)index;
    (void)enabled;
    (void)code;
}

