#ifndef LIBRETRO_CORE_OPTIONS_H__
#define LIBRETRO_CORE_OPTIONS_H__

#include <stdlib.h>
#include <string.h>

#include <libretro.h>
#include <retro_inline.h>

#ifndef HAVE_NO_LANGEXTRA
#include "libretro_core_options_intl.h"
#endif

/*
 ********************************
 * VERSION: 1.0
 ********************************
 *
 * - 1.0: First commit.  Support for cor options v2 interfaec.
 *        - libretro_core_options_intl.h includes BOM and utf-8
 *          fix for MSVC 2010-2013
 *        - Contains HAVE_NO_LANGEXTRA flag to disable translations
 *          on platforms/compilers without BOM support
 *        - Uses core options v1 interface when
 *          RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION is >= 1
 *          (previously required RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION == 1)
 *        - Support for generation of core options v0 retro_core_option_value
 *          arrays containing options with a single value
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 ********************************
 * Core Option Definitions
 ********************************
*/

/* RETRO_LANGUAGE_ENGLISH */

/* Default language:
 * - All other languages must include the same keys and values
 * - Will be used as a fallback in the event that frontend language
 *   is not available
 * - Will be used as a fallback for any missing entries in
 *   frontend language definition */

//struct retro_core_option_v2_category option_cats_us[] = {
//   { NULL, NULL, NULL },
//};

struct retro_core_option_v2_category option_cats_us[] = {
   {
      "video",
      "Video",
      "Configure video standard (NTSC/PAL).  Enable Hi-res artifacting and set artifacting mode.  Set internal resolution"
   },
   {
      "input",
      "Input",
      "Configure 5200 Digital and Analog Joystick sensitivity and Analog deadzone.  Activate Swap or Dual Joysticks.  Activate Paddle mode and set Paddle speed.  Set retroarch keyboard type."
   },
   { NULL, NULL, NULL },
};

struct retro_core_option_v2_definition option_defs_us[] = {
   {
      "atari800_ntscpal",
      "Video Standard",
      NULL,
      "Select whether emulated system is NTSC (60hz) or PAL (50hz)",
      NULL,
      "video",
      {
         { "NTSC", NULL },
         { "PAL",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "atari800_artifacting_mode",
      "Hi-Res Artifacting Mode",
      NULL,
      "Set Hi-Res Artifacting mode used.  Typically dependant on the actual emulated system.  Pick the color combination that pleases you.",
      NULL,
      "video",
      {
         { "none", "None "},
         { "blue/brown 1", NULL },
         { "blue/brown 2",  NULL },
         { "GTIA",  NULL },
         { "CTIA",  NULL },
         { NULL, NULL },
      },
      "none"
   },
   {
      "atari800_resolution",
      "Internal resolution",
      NULL,
      "Set emulated systems internal resolution.",
      NULL,
      "video",
      {
         { "336x240", NULL },
         { "320x240",  NULL },
         { "384x240",  NULL },
         { "384x272",  NULL },
         { "384x288",  NULL },
         { "400x300",  NULL },
         { NULL, NULL },
      },
      "336x240"
   },
   {
      "atari800_opt2",
      "Controller Hacks",
      NULL,
      "Apply gamepad input hacks required for specific games. 'Dual Stick' maps Player 2's joystick to the right analog stick of Player 1's RetroPad, enabling dual stick control in 'Robotron 2084' and 'Space Dungeon'. 'Swap Ports' maps Player 1 to port 2 and Player 2 to port 1 of the emulated console, correcting the swapped inputs of 'Wizard of Wor', 'Apple Panic' and a few other games",
      NULL,
      "input",
      {
         { "none", NULL },
         { "enabled",  "Dual Stick" },
         { "Swap Ports",  "Swap Ports" },
         { NULL, NULL },
      },
      "none"
   },
   {
      "paddle_active",
      "Activate Paddle Mode",
      NULL,
      "Use analog stick and digital pad to control paddle games.  Dual Stick and Swap Ports will be de-activated if this is enabled",
      NULL,
      "input",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "paddle_movement_speed",
      "Paddle Movement Speed",
      NULL,
      "Determines how fast the paddle will move.",
      NULL,
      "input",
      {
         { "1", NULL },
         { "2", NULL },
         { "3", NULL },
         { "4", NULL },
         { "5", NULL },
         { "6", NULL },
         { "7", NULL },
         { "8", NULL },
         { "9", NULL },
         { NULL, NULL },
      },
      "3"
   },
   {
      "pot_digital_sensitivity",
      "Digital Joystick Sensitivity",
      NULL,
      "Set the effective range of the emulated analog joystick when using the gamepad's digital D-Pad for movement. Lower values equate to slower speeds.  Also can be used to make games like Centipede, Kaboom, Missile Command, Pengo, Realsports Tennis, Pole Position, Xari Arena and Super Breakout more playable.",
      NULL,
      "input",
      {
         { "5",    "5%" },
         { "10",   "10%" },
         { "15",   "15%" },
         { "20",   "20%" },
         { "25",   "25%" },
         { "30",   "30%" },
         { "35",   "35%" },
         { "40",   "40%" },
         { "45",   "45%" },
         { "50",   "50%" },
         { "55",   "55%" },
         { "60",   "60%" },
         { "65",   "65%" },
         { "70",   "70%" },
         { "75",   "75%" },
         { "80",   "80%" },
         { "85",   "85%" },
         { "90",   "90%" },
         { "95",   "95%" },
         { "100",  "100%" },
         { NULL, NULL },
      },
      "100"
   },
   {
      "pot_analog_sensitivity",
      "Analog Joystick Sensitivity",
      NULL,
      "Set the effective range of the emulated analog joystick when using the gamepad's left analog stick for movement. Lower values equate to slower speeds.  Also can be used to make games like Centipede, Kaboom, Missile Command, Pengo, Realsports Tennis, Pole Position, Xari Arena and Super Breakout more playable.",
      NULL,
      "input",
      {
         { "5",    "5%" },
         { "10",   "10%" },
         { "15",   "15%" },
         { "20",   "20%" },
         { "25",   "25%" },
         { "30",   "30%" },
         { "35",   "35%" },
         { "40",   "40%" },
         { "45",   "45%" },
         { "50",   "50%" },
         { "55",   "55%" },
         { "60",   "60%" },
         { "65",   "65%" },
         { "70",   "70%" },
         { "75",   "75%" },
         { "80",   "80%" },
         { "85",   "85%" },
         { "90",   "90%" },
         { "95",   "95%" },
         { "100",  "100%" },
         { NULL, NULL },
      },
      "100"
   },
   {
      "pot_analog_deadzone",
      "Analog Joystick Deadzone",
      NULL,
      "Set the deadzone of the gamepad's analog sticks. Use this to eliminate controller drift.",
      NULL,
      "input",
      {
         { "0",  "0%" },
         { "3",  "3%" },
         { "5",  "5%" },
         { "7",  "7%" },
         { "10", "10%" },
         { "13", "13%" },
         { "15", "15%" },
         { "17", "17%" },
         { "20", "20%" },
         { "23", "23%" },
         { "25", "25%" },
         { "27", "27%" },
         { "30", "30%" },
         { NULL, NULL },
      },
      "15"
   },
   {
      "atari800_keyboard",
      "Retroarch Keyboard type",
      NULL,
      "Set whether keyboard is polled or used callback.",
      NULL,
      "input",
      {
         { "poll", NULL },
         { "callback",  NULL },
         { NULL, NULL },
      },
      "poll"
   },
   {
      "keyboard_defines",
      "Atari Keyboard Defines",
      NULL,
      "Joystick and Console Key mappings for\n\"Atari Keyboard\".\n \n Joystick \"Keyboard Numpad 8, 2, 4 & 6\".\n \n5200 analog joystick. Left analog stick or \"Keyboard Numpad 8, 2, 4 & 6\".\n \nFire 1 \"Right CTRL\". Fire 2 \"Right ALT\".\n \n5200 *  \"Keyboard Numpad * \". # \"Keyboard =\"\n \n5200 Keypad 0-9 \"Keyboard 0-9\".\nPause \"P\", Reset \"R\".\n \n'Console' keys Option \"F2\", Select \"F3\",\nStart \"F4\".\n \nController 1 only.",
      NULL,
      "input",
      {
         { "informational", NULL },
         { NULL, NULL },
      },
      "informational"
   },
   {
      "atari800_system",
      "Atari System",
      NULL,
      "Select system emulated.  Atari 5200 for Atari 5200 console games.  400/800 (OS B) for <48K games or ones that require OS/B.  800XL (64K) works for most content.  130XE (128K), Modern XL/XE(320K Compy Shop),  Modern XL/XE(576K), Modern XL/XE(1088K) for content that needs more than 64K.",
      NULL,
      NULL,
      {
         { "400/800 (OS B)", "Atari 400/800 (OS B)" },
         { "800XL (64K)", "Atari 800XL (64K)" },
         { "130XE (128K)", "Atari 130XE (128K)" },
         { "Modern XL/XE(320K CS)", "Modern Atari XL/XE(320K Compy Shop)" },
         { "Modern XL/XE(576K)", "Modern Atari XL/XE(576K)" },
         { "Modern XL/XE(1088K)", "Modern Atari XL/XE(1088K)" },
         { "5200", "Atari 5200 Super System" },
         { NULL, NULL },
      },
      "400/800 (OS B)"
   },
   {
      "atari800_internalbasic",
      "Internal BASIC (hold OPTION on boot) (Restart)",
      NULL,
      "Enable for content that needs Atari BASIC in order to run.  A proper ROM file (ATARIBAS.ROM) is needed.",
      NULL,
      NULL,
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "atari800_sioaccel",
      "SIO Acceleration",
      NULL,
      "This enables ALL SIO acceleration.  Enabled improves loading speed for Disk and Cassette images.  Disable only for protected disk (.ATX) and protected cassette images.  Reboot required if change made while loading a cassette image.",
      NULL,
      NULL,
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "enabled"
   },
   {
      "atari800_cassboot",
      "Boot from Cassette (Reboot)",
      NULL,
      "Forces emulated system to boot from autoboot cassette images by holding down the \"START\" key on boot.",
      NULL,
      NULL,
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "atari800_opt1",
      "Autodetect Atari Cartridge Type (Restart)",
      NULL,
      "Emulator will auto detect Atari cartridge based on checksum. (requires good ROM dumps).",
      NULL,
      NULL,
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   /* Not sure if I want to implement this.  Only helps a small handful of games ( Kaboom, Super Breakout and Star Wars The Arcade Game
   {
      "pot_analog_center",
      "Analog Joystick/Paddle Center",
      NULL,
      "Set the value for emulated analog joystick or paddle center.  Use this to properly center player for some games like Kaboom, Starwars or Super Breakout.  Default is 114",
      NULL,
      "5200",
      {
         { "80", NULL },
         { "82", NULL },
         { "84", NULL },
         { "86", NULL },
         { "88", NULL },
         { "90", NULL },
         { "92", NULL },
         { "94", NULL },
         { "96", NULL },
         { "98", NULL },
         { "100", NULL },
         { "102", NULL },
         { "104", NULL },
         { "106", NULL },
         { "108", NULL },
         { "110", NULL },
         { "112", NULL },
         { "114", NULL },
         { "116", NULL },
         { "118", NULL },
         { "120", NULL },
         { "122", NULL },
         { "124", NULL },
         { "126", NULL },
         { "128", NULL },
         { "130", NULL },
         { "132", NULL },
         { "134", NULL },
         { "136", NULL },
         { "130", NULL },
         { "140", NULL },
         { "142", NULL },
         { "144", NULL },
         { "146", NULL },
         { "148", NULL },
         { "150", NULL },
         { NULL, NULL },
      },
      "114"
   }, */
   { NULL, NULL, NULL, NULL, NULL, NULL, {{0}}, NULL },
};

struct retro_core_options_v2 options_us = {
   option_cats_us,
   option_defs_us
};

/*
 ********************************
 * Language Mapping
 ********************************
*/

#ifndef HAVE_NO_LANGEXTRA
struct retro_core_options_v2 *options_intl[RETRO_LANGUAGE_LAST] = {
   &options_us, /* RETRO_LANGUAGE_ENGLISH */
   NULL,        /* RETRO_LANGUAGE_JAPANESE */
   NULL,        /* RETRO_LANGUAGE_FRENCH */
   NULL,        /* RETRO_LANGUAGE_SPANISH */
   NULL,        /* RETRO_LANGUAGE_GERMAN */
   NULL,        /* RETRO_LANGUAGE_ITALIAN */
   NULL,        /* RETRO_LANGUAGE_DUTCH */
   NULL,        /* RETRO_LANGUAGE_PORTUGUESE_BRAZIL */
   NULL,        /* RETRO_LANGUAGE_PORTUGUESE_PORTUGAL */
   NULL,        /* RETRO_LANGUAGE_RUSSIAN */
   NULL,        /* RETRO_LANGUAGE_KOREAN */
   NULL,        /* RETRO_LANGUAGE_CHINESE_TRADITIONAL */
   NULL,        /* RETRO_LANGUAGE_CHINESE_SIMPLIFIED */
   NULL,        /* RETRO_LANGUAGE_ESPERANTO */
   NULL,        /* RETRO_LANGUAGE_POLISH */
   NULL,        /* RETRO_LANGUAGE_VIETNAMESE */
   NULL,        /* RETRO_LANGUAGE_ARABIC */
   NULL,        /* RETRO_LANGUAGE_GREEK */
   NULL,        /* RETRO_LANGUAGE_TURKISH */
   NULL,        /* RETRO_LANGUAGE_SLOVAK */
   NULL,        /* RETRO_LANGUAGE_PERSIAN */
   NULL,        /* RETRO_LANGUAGE_HEBREW */
   NULL,        /* RETRO_LANGUAGE_ASTURIAN */
   NULL,        /* RETRO_LANGUAGE_FINNISH */
};
#endif

/*
 ********************************
 * Functions
 ********************************
*/

/* Handles configuration/setting of core options.
 * Should be called as early as possible - ideally inside
 * retro_set_environment(), and no later than retro_load_game()
 * > We place the function body in the header to avoid the
 *   necessity of adding more .c files (i.e. want this to
 *   be as painless as possible for core devs)
 */

static INLINE void libretro_set_core_options(retro_environment_t environ_cb,
      bool *categories_supported)
{
   unsigned version  = 0;
#ifndef HAVE_NO_LANGEXTRA
   unsigned language = 0;
#endif

   if (!environ_cb || !categories_supported)
      return;

   *categories_supported = false;

   if (!environ_cb(RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION, &version))
      version = 0;

   if (version >= 2)
   {
#ifndef HAVE_NO_LANGEXTRA
      struct retro_core_options_v2_intl core_options_intl;

      core_options_intl.us    = &options_us;
      core_options_intl.local = NULL;

      if (environ_cb(RETRO_ENVIRONMENT_GET_LANGUAGE, &language) &&
          (language < RETRO_LANGUAGE_LAST) && (language != RETRO_LANGUAGE_ENGLISH))
         core_options_intl.local = options_intl[language];

      *categories_supported = environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2_INTL,
            &core_options_intl);
#else
      *categories_supported = environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2,
            &options_us);
#endif
   }
   else
   {
      size_t i, j;
      size_t option_index              = 0;
      size_t num_options               = 0;
      struct retro_core_option_definition
            *option_v1_defs_us         = NULL;
#ifndef HAVE_NO_LANGEXTRA
      size_t num_options_intl          = 0;
      struct retro_core_option_v2_definition
            *option_defs_intl          = NULL;
      struct retro_core_option_definition
            *option_v1_defs_intl       = NULL;
      struct retro_core_options_intl
            core_options_v1_intl;
#endif
      struct retro_variable *variables = NULL;
      char **values_buf                = NULL;

      /* Determine total number of options */
      while (true)
      {
         if (option_defs_us[num_options].key)
            num_options++;
         else
            break;
      }

      if (version >= 1)
      {
         /* Allocate US array */
         option_v1_defs_us = (struct retro_core_option_definition *)
               calloc(num_options + 1, sizeof(struct retro_core_option_definition));

         /* Copy parameters from option_defs_us array */
         for (i = 0; i < num_options; i++)
         {
            struct retro_core_option_v2_definition *option_def_us = &option_defs_us[i];
            struct retro_core_option_value *option_values         = option_def_us->values;
            struct retro_core_option_definition *option_v1_def_us = &option_v1_defs_us[i];
            struct retro_core_option_value *option_v1_values      = option_v1_def_us->values;

            option_v1_def_us->key           = option_def_us->key;
            option_v1_def_us->desc          = option_def_us->desc;
            option_v1_def_us->info          = option_def_us->info;
            option_v1_def_us->default_value = option_def_us->default_value;

            /* Values must be copied individually... */
            while (option_values->value)
            {
               option_v1_values->value = option_values->value;
               option_v1_values->label = option_values->label;

               option_values++;
               option_v1_values++;
            }
         }

#ifndef HAVE_NO_LANGEXTRA
         if (environ_cb(RETRO_ENVIRONMENT_GET_LANGUAGE, &language) &&
             (language < RETRO_LANGUAGE_LAST) && (language != RETRO_LANGUAGE_ENGLISH) &&
             options_intl[language])
            option_defs_intl = options_intl[language]->definitions;

         if (option_defs_intl)
         {
            /* Determine number of intl options */
            while (true)
            {
               if (option_defs_intl[num_options_intl].key)
                  num_options_intl++;
               else
                  break;
            }

            /* Allocate intl array */
            option_v1_defs_intl = (struct retro_core_option_definition *)
                  calloc(num_options_intl + 1, sizeof(struct retro_core_option_definition));

            /* Copy parameters from option_defs_intl array */
            for (i = 0; i < num_options_intl; i++)
            {
               struct retro_core_option_v2_definition *option_def_intl = &option_defs_intl[i];
               struct retro_core_option_value *option_values           = option_def_intl->values;
               struct retro_core_option_definition *option_v1_def_intl = &option_v1_defs_intl[i];
               struct retro_core_option_value *option_v1_values        = option_v1_def_intl->values;

               option_v1_def_intl->key           = option_def_intl->key;
               option_v1_def_intl->desc          = option_def_intl->desc;
               option_v1_def_intl->info          = option_def_intl->info;
               option_v1_def_intl->default_value = option_def_intl->default_value;

               /* Values must be copied individually... */
               while (option_values->value)
               {
                  option_v1_values->value = option_values->value;
                  option_v1_values->label = option_values->label;

                  option_values++;
                  option_v1_values++;
               }
            }
         }

         core_options_v1_intl.us    = option_v1_defs_us;
         core_options_v1_intl.local = option_v1_defs_intl;

         environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL, &core_options_v1_intl);
#else
         environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS, option_v1_defs_us);
#endif
      }
      else
      {
         /* Allocate arrays */
         variables  = (struct retro_variable *)calloc(num_options + 1,
               sizeof(struct retro_variable));
         values_buf = (char **)calloc(num_options, sizeof(char *));

         if (!variables || !values_buf)
            goto error;

         /* Copy parameters from option_defs_us array */
         for (i = 0; i < num_options; i++)
         {
            const char *key                        = option_defs_us[i].key;
            const char *desc                       = option_defs_us[i].desc;
            const char *default_value              = option_defs_us[i].default_value;
            struct retro_core_option_value *values = option_defs_us[i].values;
            size_t buf_len                         = 3;
            size_t default_index                   = 0;

            values_buf[i] = NULL;

            if (desc)
            {
               size_t num_values = 0;

               /* Determine number of values */
               while (true)
               {
                  if (values[num_values].value)
                  {
                     /* Check if this is the default value */
                     if (default_value)
                        if (strcmp(values[num_values].value, default_value) == 0)
                           default_index = num_values;

                     buf_len += strlen(values[num_values].value);
                     num_values++;
                  }
                  else
                     break;
               }

               /* Build values string */
               if (num_values > 0)
               {
                  buf_len += num_values - 1;
                  buf_len += strlen(desc);

                  values_buf[i] = (char *)calloc(buf_len, sizeof(char));
                  if (!values_buf[i])
                     goto error;

                  strcpy(values_buf[i], desc);
                  strcat(values_buf[i], "; ");

                  /* Default value goes first */
                  strcat(values_buf[i], values[default_index].value);

                  /* Add remaining values */
                  for (j = 0; j < num_values; j++)
                  {
                     if (j != default_index)
                     {
                        strcat(values_buf[i], "|");
                        strcat(values_buf[i], values[j].value);
                     }
                  }
               }
            }

            variables[option_index].key   = key;
            variables[option_index].value = values_buf[i];
            option_index++;
         }

         /* Set variables */
         environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);
      }

error:
      /* Clean up */

      if (option_v1_defs_us)
      {
         free(option_v1_defs_us);
         option_v1_defs_us = NULL;
      }

#ifndef HAVE_NO_LANGEXTRA
      if (option_v1_defs_intl)
      {
         free(option_v1_defs_intl);
         option_v1_defs_intl = NULL;
      }
#endif

      if (values_buf)
      {
         for (i = 0; i < num_options; i++)
         {
            if (values_buf[i])
            {
               free(values_buf[i]);
               values_buf[i] = NULL;
            }
         }

         free(values_buf);
         values_buf = NULL;
      }

      if (variables)
      {
         free(variables);
         variables = NULL;
      }
   }
}

#ifdef __cplusplus
}
#endif

#endif
