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
#include "input.h"
#include "memory.h"
#include "sysrom.h"
#include "screen.h"
#include "xep80.h"
#include "rtime.h"
#include "binload.h"
#include "cassette.h"
#include "artifact.h"
#include "statesav.h"
#include "pokeysnd.h"
#include "sound.h"
#include "cartridge_info.h"

#include "carts_hash.h"
#include "crc32.h"
#include "colours.h"

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
       { _user, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "Space/Fire 3" },                 \
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
int autorunCartridge = NO_CART;
int autorun5200CartType = 0;    /* atari800 CARTRIDGE_* type when autorunCartridge == A5200_CART */
int atari_joyhack = 0;
int paddle_mode = 0;
int paddle_speed = 3;
int atarixegs_keyboard_detached = 0;

int color_first_time = TRUE;
double color_hue = 0.0;
double color_saturation = 0.0;
double color_contrast = 0.0;
double color_brightness = 0.0;
double color_gamma = 2.35;
#define COLOR_VARIABLE(name) \
    var.key = "color_" #name; \
    var.value = NULL; \
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value) { \
        float var_value; \
        var_value = atof(var.value); \
        if (var_value != color_##name) { \
            colors_changed = TRUE; \
            color_##name = var_value; \
        } \
    } /* END OF COLOR_VARIABLE */

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

float retro_fps = Atari800_FPS_PAL;
long long retro_frame_counter;
extern int ToggleTV;
extern int CURRENT_TV;

extern int SHIFTON, pauseg, SND;
extern int SHOWKEY, SHOWKEYDELAY;
extern UBYTE SNDBUF[];

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
char retro_system_data_directory[512];
int legacy_configuration_file; 

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
extern int CASSETTE_hold_start;
extern int CASSETTE_hold_start_on_reboot;

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

    bool no_content = true;
    cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_content);
}

/* Store a chosen SYSROM revision and reinitialise the machine if the change
   affects the currently emulated system and we are not yet running. Pass
   machine_idx = -1 for settings (e.g. BASIC) that apply to any machine. */
static void apply_sysrom_version(int *slot, int version, int machine_idx)
{
    if (*slot == version)
        return;
    *slot = version;
    if (!libretro_runloop_active &&
        (machine_idx < 0 || Atari800_machine_type == machine_idx))
        Atari800_InitialiseMachine();
}

static void update_variables(void)
{
    struct retro_variable var;

    /* Stereo POKEY user option. Stock Atari 800/XL/XE has a single POKEY chip,
       so default is mono -- most authentic and most compatible (Bounty Bob,
       Road Race and other titles that hit mirrored POKEY registers lock up
       or lose audio with stereo enabled). User can enable to emulate the
       Atari Stereo Sound Mod (second POKEY at $D210-$D21F). 5200 always
       forces mono below regardless of this option. */
    var.key = "atari800_pokey_stereo";
    var.value = NULL;
    {
        int want_stereo = FALSE;
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value
            && strcmp(var.value, "enabled") == 0)
            want_stereo = TRUE;
        POKEYSND_stereo_enabled = want_stereo;
        Sound_desired.channels = want_stereo ? 2 : 1;
    }

    /* Moved here for better consistency and when system options that require EMU reset to occur */
    autorun5200CartType = 0;
    if (strcmp(RPATH, "") == 0)  // Start core with no content
        autorunCartridge = NO_CART;
    /* Most complex case - .bin (common) and .rom (rare) can be used for both Atari 5200 console and Atari 8bit computers */
    else if  (HandleExtension((char*)RPATH, "bin") || HandleExtension((char*)RPATH, "BIN")
           || HandleExtension((char*)RPATH, "rom") || HandleExtension((char*)RPATH, "ROM")
           || HandleExtension((char*)RPATH, "a52") || HandleExtension((char*)RPATH, "A52")) {
        int is_a52 = HandleExtension((char*)RPATH, "a52") || HandleExtension((char*)RPATH, "A52");
        autorunCartridge = is_a52 ? A5200_CART : A800_CART;
        ULONG crc = 0;
        long fsize = 0;
        FILE *fp;
        fp = fopen((char*)RPATH, "rb");
        if (fp != NULL) {
            CRC32_FromFile(fp, &crc);
            fseek(fp, 0, SEEK_END);
            fsize = ftell(fp);
            fclose(fp);
            if (is_5200_cart(crc)) {
                autorunCartridge = A5200_CART;
                log_cb(RETRO_LOG_INFO,"[update_variables] Found Atari 5200 cart in DB for: %s\n", (char*)RPATH);
            } else if (!is_a52) {
                log_cb(RETRO_LOG_INFO,"[update_variables] Assumming Atari 8bit bin file: %s\n", (char*)RPATH);
            }
            if (autorunCartridge == A5200_CART) {
                autorun5200CartType = get_5200_cart_atari800_type(crc, (int)fsize);
                log_cb(RETRO_LOG_INFO,"[update_variables] 5200 cart type: %d (size %ld, crc %08lx)\n",
                    autorun5200CartType, fsize, (unsigned long)crc);
            }
        } else {
            log_cb(RETRO_LOG_INFO,"[update_variables] Error opening cart file: %s\n", (char*)RPATH );
        }
    }   /* bin, rom, a52 files */
    else if   (HandleExtension((char*)RPATH, "car") || HandleExtension((char*)RPATH, "CAR")) {
        autorunCartridge = A800_CART;
        /* Bounty Bob Strikes Back (CARTRIDGE_BBSB_40 = 18) writes to mirrored
           POKEY registers (e.g. $D210 aliasing to $D200 on a single-POKEY 800).
           Stereo POKEY decodes bit 4 as chip-select, which routes those writes
           to a nonexistent second chip and locks the game in the menu. Peek at
           the .car header (16 bytes: "CART" + 4-byte big-endian type) so we
           can force mono before Sound_Initialise(). */
        FILE *fp = fopen((char*)RPATH, "rb");
        if (fp != NULL) {
            unsigned char hdr[8];
            if (fread(hdr, 1, 8, fp) == 8
             && hdr[0] == 'C' && hdr[1] == 'A' && hdr[2] == 'R' && hdr[3] == 'T') {
                int car_type = (hdr[4] << 24) | (hdr[5] << 16) | (hdr[6] << 8) | hdr[7];
                if (car_type == CARTRIDGE_BBSB_40) {
                    POKEYSND_stereo_enabled = FALSE;
                    Sound_desired.channels = 1;
                    log_cb(RETRO_LOG_INFO, "[update_variables] BBSB_40 .car detected, forcing mono POKEY\n");
                }
            }
            fclose(fp);
        }
    }
    /* Non cartridges extensions*/
    else if   (HandleExtension((char*)RPATH, "xex") || HandleExtension((char*)RPATH, "XEX")
            || HandleExtension((char*)RPATH, "com") || HandleExtension((char*)RPATH, "COM")
            || HandleExtension((char*)RPATH, "m3u") || HandleExtension((char*)RPATH, "M3U")
            || HandleExtension((char*)RPATH, "atx") || HandleExtension((char*)RPATH, "ATX")
            || HandleExtension((char*)RPATH, "cas") || HandleExtension((char*)RPATH, "CAS")
            || HandleExtension((char*)RPATH, "dcm") || HandleExtension((char*)RPATH, "DCM")
            || HandleExtension((char*)RPATH, "atr") || HandleExtension((char*)RPATH, "ATR"))
        autorunCartridge = NO_CART;
    /* Other unknown extensions - priority is Atari 8bit computer*/
    else
        autorunCartridge = A800_CART;

    /* Controller Hack for Dual Stick, Swap Ports or Joy 2B+*/
    var.key = "atari800_opt2";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if ( (strcmp(var.value, "Dual Stick") == 0) ||
             (strcmp(var.value, "enabled") == 0))        // to accomodate for old value
            atari_joyhack = 1;
        else if ( strcmp(var.value,"Swap Ports") == 0)
            atari_joyhack = 2;
        else if ( strcmp(var.value,"Joy 2B+") == 0)
            atari_joyhack = 3;
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
        if (autorunCartridge == A5200_CART || (strcmp(var.value, "5200") == 0))
        {
            Atari800_machine_type = Atari800_MACHINE_5200;
            MEMORY_ram_size = 16;
            Atari800_builtin_basic = FALSE;
            Atari800_keyboard_leds = FALSE;
            Atari800_f_keys = FALSE;
            Atari800_jumper = FALSE;
            Atari800_builtin_game = FALSE;
            Atari800_keyboard_detached = FALSE;
            /* Real Atari 5200 has a single POKEY. Stereo POKEY makes the chip
               decode bit 4 of the address as a chip-select, so writes to mirrored
               POKEY registers (e.g. $E810 aliasing to $E800 on real hardware)
               hit a nonexistent second chip. Bounty Bob Strikes Back relies on
               this aliasing and locks up otherwise. */
            POKEYSND_stereo_enabled = FALSE;
            Sound_desired.channels = 1;
            /* Force Atari 5200 joystick layout for Atari 5200 machine emulation */
            retro_set_controller_port_device(0, RETRO_DEVICE_ATARI_5200_JOYSTICK);
            retro_set_controller_port_device(1, RETRO_DEVICE_ATARI_5200_JOYSTICK);
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
        else if (strcmp(var.value, "XEGS") == 0)
        {
            Atari800_machine_type = Atari800_MACHINE_XLXE;
            MEMORY_ram_size = 64;
            Atari800_builtin_basic = TRUE;
            Atari800_keyboard_leds = FALSE;
            Atari800_f_keys = FALSE;
            Atari800_jumper = FALSE;
            Atari800_builtin_game = TRUE;
            Atari800_keyboard_detached = atarixegs_keyboard_detached;
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
            CURRENT_TV = Atari800_TV_NTSC;   
        }
        else if (strcmp(var.value, "PAL") == 0)
        {
            CURRENT_TV = Atari800_TV_PAL;
        }

        if(CURRENT_TV != Atari800_tv_mode)
			ToggleTV=1;
    
        retro_fps = CURRENT_TV == Atari800_TV_PAL ? Atari800_FPS_PAL : Atari800_FPS_NTSC;  // Just in case.


    }

    /* Set colors */

    int colors_changed = FALSE;

    /* COLOR_VARIABLE macro is defined on L128 */
    COLOR_VARIABLE(hue)
    COLOR_VARIABLE(saturation)
    COLOR_VARIABLE(contrast)
    COLOR_VARIABLE(brightness)
    COLOR_VARIABLE(gamma)

    if ((color_first_time || colors_changed) && Colours_setup != NULL) {
        Colours_setup->hue = color_hue;
        Colours_setup->saturation = color_saturation;
        Colours_setup->contrast = color_contrast;
        Colours_setup->brightness = color_brightness;
        Colours_setup->gamma = color_gamma;
        Colours_Update();
        log_cb(RETRO_LOG_INFO, "[Atari800 CORE] Colours_setup changed\n");
        color_first_time = FALSE;
    }

    /* GTIA delay (colorburst phase). Unlike the offset-style colour controls
       above, this is an absolute angle whose neutral value differs between
       NTSC (26.8) and PAL (23.2), so the "default" choice leaves the active
       palette's built-in default untouched and only an explicit value overrides
       it. */
    var.key = "color_delay";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value
        && strcmp(var.value, "default") != 0 && Colours_setup != NULL)
    {
        double delay = atof(var.value);
        if (Colours_setup->color_delay != delay)
        {
            Colours_setup->color_delay = delay;
            Colours_Update();
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

    /* OS ROM revision for the Atari 400/800. */
    var.key = "atari800_os_800";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        int v = SYSROM_AUTO;
        if (strcmp(var.value, "Rev. A NTSC") == 0)
            v = SYSROM_A_NTSC;
        else if (strcmp(var.value, "Rev. A PAL") == 0)
            v = SYSROM_A_PAL;
        else if (strcmp(var.value, "Rev. B NTSC") == 0)
            v = SYSROM_B_NTSC;
        else if (strcmp(var.value, "AltirraOS") == 0)
            v = SYSROM_ALTIRRA_800;
        apply_sysrom_version(&SYSROM_os_versions[Atari800_MACHINE_800], v, Atari800_MACHINE_800);
    }

    /* OS ROM revision for the Atari XL/XE/XEGS. */
    var.key = "atari800_os_xl";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        int v = SYSROM_AUTO;
        if (strcmp(var.value, "AA00 Rev. 10") == 0)
            v = SYSROM_AA00R10;
        else if (strcmp(var.value, "AA01 Rev. 11") == 0)
            v = SYSROM_AA01R11;
        else if (strcmp(var.value, "BB00 Rev. 1") == 0)
            v = SYSROM_BB00R1;
        else if (strcmp(var.value, "BB01 Rev. 2") == 0)
            v = SYSROM_BB01R2;
        else if (strcmp(var.value, "BB02 Rev. 3") == 0)
            v = SYSROM_BB02R3;
        else if (strcmp(var.value, "BB02 Rev. 3 Ver. 4") == 0)
            v = SYSROM_BB02R3V4;
        else if (strcmp(var.value, "CC01 Rev. 4") == 0)
            v = SYSROM_CC01R4;
        else if (strcmp(var.value, "BB01 Rev. 3") == 0)
            v = SYSROM_BB01R3;
        else if (strcmp(var.value, "BB01 Rev. 4") == 0)
            v = SYSROM_BB01R4_OS;
        else if (strcmp(var.value, "BB01 Rev. 59") == 0)
            v = SYSROM_BB01R59;
        else if (strcmp(var.value, "BB01 Rev. 59 alt.") == 0)
            v = SYSROM_BB01R59A;
        else if (strcmp(var.value, "AltirraOS") == 0)
            v = SYSROM_ALTIRRA_XL;
        apply_sysrom_version(&SYSROM_os_versions[Atari800_MACHINE_XLXE], v, Atari800_MACHINE_XLXE);
    }

    /* BIOS ROM revision for the Atari 5200. */
    var.key = "atari800_os_5200";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        int v = SYSROM_AUTO;
        if (strcmp(var.value, "Original") == 0)
            v = SYSROM_5200;
        else if (strcmp(var.value, "Rev. A") == 0)
            v = SYSROM_5200A;
        else if (strcmp(var.value, "AltirraOS") == 0)
            v = SYSROM_ALTIRRA_5200;
        apply_sysrom_version(&SYSROM_os_versions[Atari800_MACHINE_5200], v, Atari800_MACHINE_5200);
    }

    /* Atari BASIC ROM revision (applies to any 8-bit machine). */
    var.key = "atari800_basic_version";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        int v = SYSROM_AUTO;
        if (strcmp(var.value, "Rev. A") == 0)
            v = SYSROM_BASIC_A;
        else if (strcmp(var.value, "Rev. B") == 0)
            v = SYSROM_BASIC_B;
        else if (strcmp(var.value, "Rev. C") == 0)
            v = SYSROM_BASIC_C;
        else if (strcmp(var.value, "Altirra BASIC") == 0)
            v = SYSROM_ALTIRRA_BASIC;
        apply_sysrom_version(&SYSROM_basic_version, v, -1);
    }

    /* Mosaic RAM expansion (Atari 400/800 only; mutually exclusive with Axlon). */
    var.key = "atari800_mosaic";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        int banks = 0;
        if (strcmp(var.value, "16 KB") == 0)
            banks = 4;
        else if (strcmp(var.value, "80 KB") == 0)
            banks = 20;
        else if (strcmp(var.value, "144 KB") == 0)
            banks = 36;

        if (MEMORY_mosaic_num_banks != banks)
        {
            MEMORY_mosaic_num_banks = banks;
            if (banks > 0)
                MEMORY_axlon_num_banks = 0; /* the two expansions are mutually exclusive */
            if (!libretro_runloop_active)
                Atari800_InitialiseMachine();
        }
    }

    /* Axlon RAM expansion (Atari 400/800 only; mutually exclusive with Mosaic). */
    var.key = "atari800_axlon";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        int banks = 0;
        if (strcmp(var.value, "128 KB") == 0)
            banks = 8;
        else if (strcmp(var.value, "256 KB") == 0)
            banks = 16;
        else if (strcmp(var.value, "512 KB") == 0)
            banks = 32;
        else if (strcmp(var.value, "1 MB") == 0)
            banks = 64;
        else if (strcmp(var.value, "2 MB") == 0)
            banks = 128;
        else if (strcmp(var.value, "4 MB") == 0)
            banks = 256;

        if (MEMORY_axlon_num_banks != banks)
        {
            MEMORY_axlon_num_banks = banks;
            if (banks > 0)
                MEMORY_mosaic_num_banks = 0; /* the two expansions are mutually exclusive */
            if (!libretro_runloop_active)
                Atari800_InitialiseMachine();
        }
    }

    /* Axlon $0F bank-select register shadow. */
    var.key = "atari800_axlon_shadow";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        int shadow = (strcmp(var.value, "enabled") == 0);
        if (MEMORY_axlon_0f_mirror != shadow)
        {
            MEMORY_axlon_0f_mirror = shadow;
            if (!libretro_runloop_active)
                Atari800_InitialiseMachine();
        }
    }

    /* MapRAM RAM-under-ROM mapping (Atari XL/XE only). */
    var.key = "atari800_mapram";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        int mapram = (strcmp(var.value, "enabled") == 0);
        if (MEMORY_enable_mapram != mapram)
        {
            MEMORY_enable_mapram = mapram;
            if (!libretro_runloop_active)
                Atari800_InitialiseMachine();
        }
    }

    /* Set whether SIO (fast disk) acceleration is activated. The P: and R:
       device patches, which are unrelated to disk speed, have their own
       options (atari800_pdevice / atari800_rdevice) below. */
    var.key = "atari800_sioaccel";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "enabled") == 0)
        {
            ESC_enable_sio_patch = Devices_enable_h_patch = TRUE;
        }
        else if (strcmp(var.value, "disabled") == 0)
        {
            ESC_enable_sio_patch = Devices_enable_h_patch = FALSE;
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
            /* Persist the START-held state across reboots: the cassette boot
             * handshake (gtia.c) consumes CASSETTE_hold_start into
             * CASSETTE_hold_start_on_reboot once the tape boots. With this at 0
             * (the default), a boot tape loads the first time but a subsequent
             * reset cold-starts without holding START -> the machine drops to
             * the self-test screen instead of reloading. Keeping it at 1 makes
             * every reset re-boot the (rewound) tape. */
            CASSETTE_hold_start_on_reboot = 1;
        }
        else if (strcmp(var.value, "disabled") == 0)
        {
            CASSETTE_hold_start = 0;
            CASSETTE_hold_start_on_reboot = 0;
        }

        if (!libretro_runloop_active)
            Atari800_InitialiseMachine();
    }

    /* Set whether a new legacy configuration file will be loaded or created if not found. */
    var.key = "atari800_cfg";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "enabled") == 0)
        {
            legacy_configuration_file = TRUE;
        }
        else if (strcmp(var.value, "disabled") == 0)
        {
            legacy_configuration_file = FALSE;
        }

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

    /* Set joystick autofire mode for all ports. */
    var.key = "atari800_autofire";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        int mode = INPUT_AUTOFIRE_OFF;
        if (strcmp(var.value, "fire") == 0)
            mode = INPUT_AUTOFIRE_FIRE;
        else if (strcmp(var.value, "always") == 0)
            mode = INPUT_AUTOFIRE_CONT;
        INPUT_joy_autofire[0] = INPUT_joy_autofire[1] =
        INPUT_joy_autofire[2] = INPUT_joy_autofire[3] = mode;
    }

    /* On-screen status indicators (take effect immediately). */
    var.key = "atari800_show_speed";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        Screen_show_atari_speed = (strcmp(var.value, "enabled") == 0);

    var.key = "atari800_show_diskled";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        Screen_show_disk_led = (strcmp(var.value, "enabled") == 0);

    var.key = "atari800_show_sector";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        Screen_show_sector_counter = (strcmp(var.value, "enabled") == 0);

    var.key = "atari800_show_1200leds";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        Screen_show_1200_leds = (strcmp(var.value, "enabled") == 0);

    /* XEP80 80-column display (applied on next machine init). */
    var.key = "atari800_xep80";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "port 1") == 0)      { XEP80_enabled = TRUE;  XEP80_port = 0; }
        else if (strcmp(var.value, "port 2") == 0) { XEP80_enabled = TRUE;  XEP80_port = 1; }
        else                                       { XEP80_enabled = FALSE; }
    }

    /* R-Time 8 real-time clock cartridge. */
    var.key = "atari800_rtime";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        RTIME_enabled = (strcmp(var.value, "enabled") == 0);

    /* P: device (printer) SIO patch. */
    var.key = "atari800_pdevice";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        Devices_enable_p_patch = (strcmp(var.value, "enabled") == 0);

    /* R: device (serial) SIO patch. */
    var.key = "atari800_rdevice";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        Devices_enable_r_patch = (strcmp(var.value, "enabled") == 0);

    /* Slow (accurate) loading of DOS binary (.xex) files. */
    var.key = "atari800_slowxex";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        BINLOAD_slow_xex_loading = (strcmp(var.value, "enabled") == 0);

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

    var.key = "atarixegs_keyboard_detached";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "attached") == 0)
        {
            atarixegs_keyboard_detached = 0;
        }
        else if (strcmp(var.value, "detached") == 0)
        {
            atarixegs_keyboard_detached = 1;
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

    /* Virtual keyboard show/hide. SHOWKEY is also toggled at runtime by the
       mapped controller button (L3/R3), so only follow this option when its
       value actually changes -- otherwise a re-poll of the variables would
       override the user's in-game toggle every frame. */
    var.key = "atari800_vkbd_enabled";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        static int last_vkbd_enabled = -1;
        int vkbd_enabled = (strcmp(var.value, "enabled") == 0) ? 1 : 0;

        if (vkbd_enabled != last_vkbd_enabled)
        {
            SHOWKEY = vkbd_enabled ? 1 : -1;
            SHOWKEYDELAY = 20;
            last_vkbd_enabled = vkbd_enabled;
        }
    }
}

/* Performs the one-time emulator initialisation (pre_main -> skel_main ->
 * Atari800_Initialise). Called once from retro_load_game after RPATH and
 * the machine/TV settings have been configured, which is the same point
 * the old code reached via the first co_switch(emuThread). No fiber. */
void libretro_emu_init_run(void)
{
    log_cb(RETRO_LOG_INFO, "INIT EMU\n");
    log_cb(RETRO_LOG_INFO, "Atari800_machine_type =%d\n", Atari800_machine_type);
    log_cb(RETRO_LOG_INFO, "Atari800_tv_mode =%d\n", Atari800_tv_mode);
    log_cb(RETRO_LOG_INFO, "CURRENT_TV =%d\n", CURRENT_TV);
    log_cb(RETRO_LOG_INFO, "retro_fps =%f\n", retro_fps);
    log_cb(RETRO_LOG_INFO, "RPATH =%s\n", RPATH);

    pre_main(RPATH);
}

void Emu_init() {

#ifdef RETRO_AND
    MOUSEMODE = 1;
#endif

    //  update_variables();

    memset(Key_State, 0, 512);

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

    /* For tapes, AFILE_OpenFile() force-sets CASSETTE_hold_start = TRUE
     * (boot-tape autoload). That overrides the user's "Cassette boot"
     * option and breaks games that are loaded by hand from BASIC
     * (LOAD/CLOAD): after a reset they would try to autoboot and never
     * load, so the game had to be fully reloaded. Re-insert and rewind the
     * tape and cold-start while keeping the configured hold_start. */
    if (dc && get_image_unit() == DC_IMAGE_TYPE_TAPE)
    {
        int hold = CASSETTE_hold_start;
        if (CASSETTE_Insert(RPATH))
        {
            CASSETTE_Seek(0);
            dc->eject_state = false;
        }
        CASSETTE_hold_start = hold;
        Atari800_Coldstart();
    }
    else
    {
        AFILE_OpenFile(RPATH, 1, 1, 0);
    }
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
    log_cb(RETRO_LOG_INFO, "exit emu\n");

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
    info->library_version = "7.0.0" GIT_VERSION;
#else
    info->library_version = "7.0.0";
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

void retro_audio_cb(int16_t l, int16_t r)
{
    audio_cb(l, r);
}

/* Emit a block of interleaved stereo frames in a single call. Used by the
 * per-frame audio path so one retro_run() emits exactly one frame's worth
 * of consecutive samples. */
size_t retro_audio_batch_cb(const int16_t *data, size_t frames)
{
    return audio_batch_cb(data, frames);
}

/* Forward declarations */
void retro_sound_update(void);
int Retro_PollEvent(void);
void retro_virtualkb(void);
extern int SHOWKEY;

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

            retro_fps = CURRENT_TV == Atari800_TV_PAL ? Atari800_FPS_PAL : Atari800_FPS_NTSC;

            retro_get_system_av_info(&ninfo);

            environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &ninfo);

            if (log_cb)
                log_cb(RETRO_LOG_INFO, "ChangeAV: w:%d h:%d ar:%f fq:%f sr:%f.\n",
                    ninfo.geometry.base_width, ninfo.geometry.base_height, ninfo.geometry.aspect_ratio, ninfo.timing.fps, ninfo.timing.sample_rate);

            ToggleTV = 0;
            Atari800_SetTVMode(CURRENT_TV); // Should include POKEYSND_Init(POKEYSND_FREQ_17_EXACT, 44100, 2, 1);
        }

        /* Deterministic single-frame step (no fiber, no self-pacing):
         *   1. sample input once, up front
         *   2. emulate exactly one frame
         *   3. emit that frame's audio batch
         *   4. emit that frame's video
         * Steps 3 and 4 cover the same frame, emitted together, so audio
         * and video never straddle a frame boundary. */
        Retro_PollEvent();

        libretro_run_frame();

        if (retro_sound_finalized)
            retro_sound_update();

        /* Draw the on-screen keyboard overlay last, on top of the freshly
           rendered frame, so PLATFORM_DisplayScreen() cannot overwrite it.
           It must land in Retro_Screen just before video_cb() below. */
        if (SHOWKEY == 1)
            retro_virtualkb();
    }

    video_cb(Retro_Screen, retrow, retroh, retrow << PIXEL_BYTES);
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
    struct retro_keyboard_callback cb = { keyboard_cb };
    environ_cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &cb);    

    if (info!=NULL) {
        const char* full_path;
        bool media_is_disk_tape = TRUE;

        (void)info;

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
    }

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

    /* One-time emulator init now runs synchronously (no fiber). After
     * this returns the core is ready and retro_run() will drive frames. */
    libretro_emu_init_run();

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

    return size;
}

bool retro_serialize(void* data_, size_t size)
{
    int returned_size;

    returned_size = Retro_SaveAtariState(data_, size, 1);

    return returned_size;
}

bool retro_unserialize(const void* data_, size_t size)
{
    int returned_size;

    returned_size = Retro_ReadAtariState(data_, size);

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

