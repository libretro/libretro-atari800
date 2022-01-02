#include <stdarg.h>

#include "libretro.h"

#include "libretro-core.h"
#include "atari.h"
#include "devices.h"
#include "esc.h"
#include "memory.h"
#include "cassette.h"
#include "artifact.h"

cothread_t mainThread;
cothread_t emuThread;

static void fallback_log(enum retro_log_level level, const char* fmt, ...);

retro_log_printf_t log_cb = fallback_log;

int CROP_WIDTH;
int CROP_HEIGHT;
int VIRTUAL_WIDTH;
int retrow = 400;
int retroh = 300;

#define RETRO_DEVICE_ATARI_KEYBOARD RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_KEYBOARD, 0)
#define RETRO_DEVICE_ATARI_JOYSTICK RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 1)

unsigned atari_devices[4];

int keyboard_type = 0;
int autorun5200 = 0;
int a5200_joyhack = 0;

int RETROJOY = 0, RETROPT0 = 0, RETROSTATUS = 0, RETRODRVTYPE = 0;
int retrojoy_init = 0, retro_ui_finalized = 0;
int retro_sound_finalized = 0;

float retro_fps = 49.8607597;
long long retro_frame_counter;
extern int ToggleTV;
extern int CURRENT_TV;

extern int SHIFTON, pauseg, SND, snd_sampler_pal;
extern short signed int SNDBUF[1024 * 2];
extern char RPATH[512];
extern char RETRO_DIR[512];
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

void retro_set_environment(retro_environment_t cb)
{
    environ_cb = cb;

    static const struct retro_controller_description p1_controllers[] = {
      { "ATARI Joystick", RETRO_DEVICE_ATARI_JOYSTICK },
      { "ATARI Keyboard", RETRO_DEVICE_ATARI_KEYBOARD },
    };
    static const struct retro_controller_description p2_controllers[] = {
      { "ATARI Joystick", RETRO_DEVICE_ATARI_JOYSTICK },
      { "ATARI Keyboard", RETRO_DEVICE_ATARI_KEYBOARD },
    };
    static const struct retro_controller_description p3_controllers[] = {
      { "ATARI Joystick", RETRO_DEVICE_ATARI_JOYSTICK },
      { "ATARI Keyboard", RETRO_DEVICE_ATARI_KEYBOARD },
    };
    static const struct retro_controller_description p4_controllers[] = {
      { "ATARI Joystick", RETRO_DEVICE_ATARI_JOYSTICK },
      { "ATARI Keyboard", RETRO_DEVICE_ATARI_KEYBOARD },
    };

    static const struct retro_controller_info ports[] = {
      { p1_controllers, 2  }, // port 1
      { p2_controllers, 2  }, // port 2
      { p3_controllers, 2  }, // port 3
      { p4_controllers, 2  }, // port 4
      { NULL, 0 }
    };

    cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);

    struct retro_variable variables[] = {
      {
        "atari800_system",
        "Atari System; 400/800 (OS B)|800XL (64K)|130XE (128K)|Modern XL/XE(320K CS)|Modern XL/XE(576K)|Modern XL/XE(1088K)|5200",
      },
      {
        "atari800_ntscpal",
        "Video Standard; NTSC|PAL",
      },
      {
        "atari800_internalbasic",
        "Internal BASIC (hold OPTION on boot); disabled|enabled",
      },
      {
        "atari800_sioaccel",
        "SIO Acceleration; disabled|enabled",
      },
      {
        "atari800_cassboot",
        "Boot from Cassette; disabled|enabled",
      },
      {
        "atari800_artifacting",
        "Hi-Res Artifacting; disabled|enabled",
      },
       {
         "atari800_opt1",
         "Autodetect A5200 CartType; disabled|enabled" ,
       },
       {
         "atari800_opt2",
         "Joy hack A5200 for robotron; disabled|enabled" ,
       },
       {
          "atari800_resolution",
          "Internal resolution; 336x240|320x240|384x240|384x272|384x288|400x300",
       },
      {
        "atari800_keyboard",
        "Retroarch Keyboard type; poll|callback",
      },

       { NULL, NULL },
    };

    cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);
}

static void update_variables(void)
{

    struct retro_variable var;

    var.key = "atari800_opt1";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "enabled") == 0)
            autorun5200 = 1;
    }

    var.key = "atari800_opt2";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "enabled") == 0)
            a5200_joyhack = 1;
    }

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

    var.key = "atari800_system";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "400/800 (OS B)") == 0)
        {
            Atari800_machine_type = Atari800_MACHINE_800;
            MEMORY_ram_size = 48;
            Atari800_builtin_basic = FALSE;
            Atari800_keyboard_leds = FALSE;
            Atari800_f_keys = FALSE;
            Atari800_jumper = FALSE;
            Atari800_builtin_game = FALSE;
            Atari800_keyboard_detached = FALSE;
            Atari800_InitialiseMachine();
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
            Atari800_InitialiseMachine();
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
            Atari800_InitialiseMachine();
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
            Atari800_InitialiseMachine();
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
            Atari800_InitialiseMachine();
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
            Atari800_InitialiseMachine();
        }
        else if (strcmp(var.value, "5200") == 0)
        {
            Atari800_machine_type = Atari800_MACHINE_5200;
            MEMORY_ram_size = 16;
            Atari800_builtin_basic = FALSE;
            Atari800_keyboard_leds = FALSE;
            Atari800_f_keys = FALSE;
            Atari800_jumper = FALSE;
            Atari800_builtin_game = FALSE;
            Atari800_keyboard_detached = FALSE;
            Atari800_InitialiseMachine();
        }
    }

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

    var.key = "atari800_internalbasic";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "enabled") == 0)
        {
            Atari800_disable_basic = FALSE;
            Atari800_InitialiseMachine();
        }
        else if (strcmp(var.value, "disabled") == 0)
        {
            Atari800_disable_basic = TRUE;
            Atari800_InitialiseMachine();
        }
    }

    var.key = "atari800_sioaccel";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "enabled") == 0)
        {
            ESC_enable_sio_patch = Devices_enable_h_patch = Devices_enable_p_patch = Devices_enable_r_patch = TRUE;
            Atari800_InitialiseMachine();
        }
        else if (strcmp(var.value, "disabled") == 0)
        {
            ESC_enable_sio_patch = Devices_enable_h_patch = Devices_enable_p_patch = Devices_enable_r_patch = FALSE;
            Atari800_InitialiseMachine();
        }
    }

    var.key = "atari800_cassboot";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "enabled") == 0)
        {
            CASSETTE_hold_start = 1;
            Atari800_InitialiseMachine();
        }
        else if (strcmp(var.value, "disabled") == 0)
        {
            CASSETTE_hold_start = 0;
            Atari800_InitialiseMachine();
        }
    }

    var.key = "atari800_artifacting";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "enabled") == 0)
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
        else if (strcmp(var.value, "disabled") == 0)
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

    memset(Key_Sate, 0, 512);
    memset(Key_Sate2, 0, 512);

    if (!emuThread && !mainThread)
    {
        mainThread = co_active();
        emuThread = co_create(65536 * sizeof(void*), retro_wrap_emulator);
    }

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
    struct retro_log_callback log;

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

    if (retro_system_directory == NULL)sprintf(RETRO_DIR, "%s\0", ".");
    else sprintf(RETRO_DIR, "%s\0", retro_system_directory);

    sprintf(retro_system_data_directory, "%s/data\0", RETRO_DIR);

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

    struct retro_input_descriptor inputDescriptors[] = {
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "Fire 1" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "Return" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X, "Fire 2" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "Help" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Left" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Up" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Down" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R, "Atari800 Menu" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L, "Option" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2, "Esc" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2, "Space" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3, "R3" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3, "Virtual keyboard" },
        { 0 }
    };
    environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, &inputDescriptors);

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
    }
}

void retro_get_system_info(struct retro_system_info* info)
{
    memset(info, 0, sizeof(*info));
    info->library_name = "Atari800";
    info->library_version = "3.1.0" GIT_VERSION;
    info->valid_extensions = "xfd|atr|cdm|cas|bin|a52|zip|atx|car|rom|com|xex";
    info->need_fullpath = true;
    info->block_extract = false;

}
/*
void retro_get_system_av_info(struct retro_system_av_info *info)
{
   struct retro_game_geometry geom = { retrow, retroh, 400, 300,4.0 / 3.0 };
   struct retro_system_timing timing = { retro_fps, 44100.0 };

   info->geometry = geom;
   info->timing   = timing;
}
*/
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
void Retro_PollEvent(void);

void retro_run(void)
{
    int x;

    bool updated = false;

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
extern char Key_Sate[512];
unsigned int lastdown, lastup, lastchar;
static void keyboard_cb(bool down, unsigned keycode,
    uint32_t character, uint16_t mod)
{
    if (keyboard_type == 0)return;
    Key_Sate[keycode] = down ? 1 : 0;

    /*
      printf( "Down: %s, Code: %d, Char: %u, Mod: %u.\n",
             down ? "yes" : "no", keycode, character, mod);
    */
    /*
    if(down)lastdown=keycode;
    else lastup=keycode;
    lastchar=character;
    */
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

bool retro_load_game(const struct retro_game_info* info)
{
    const char* full_path;

    (void)info;


    struct retro_keyboard_callback cb = { keyboard_cb };
    environ_cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &cb);

    full_path = info->path;

    strcpy(RPATH, full_path);

    update_variables();

    if (HandleExtension((char*)RPATH, "a52") || HandleExtension((char*)RPATH, "A52"))
        autorun5200 = 1;

#ifdef RENDER16B
    memset(Retro_Screen, 0, 400 * 300 * 2);
#else
    memset(Retro_Screen, 0, 400 * 300 * 2 * 2);
#endif
    memset(SNDBUF, 0, 1024 * 2 * 2);

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
    return 0;
}

bool retro_serialize(void* data_, size_t size)
{
    return false;
}

bool retro_unserialize(const void* data_, size_t size)
{
    return false;
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

