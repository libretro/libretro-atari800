/*
 * platform.c - platform interface implementation for libretro
 *
 * Copyright (C) 2010 Atari800 development team (see DOC/CREDITS)
 *
 * This file is part of the Atari800 emulator project which emulates
 * the Atari 400, 800, 800XL, 130XE, and 5200 8-bit computers.
 *
 * Atari800 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Atari800 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Atari800; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
/* Atari800 includes */
#include "atari.h"
#include "input.h"
#include "log.h"
#include "binload.h"
#include "monitor.h"
#include "platform.h"
#include "sound.h"
#include "videomode.h"
#include "cpu.h"
#include "devices.h"
#include "akey.h"
#include "pokeysnd.h"
#include "sound.h"
#include "screen.h"
#include "colours.h"
#include "ui.h"
#include "libretro-core.h"
#include "libretro.h"
#include "retroscreen.h"

#define RETRO_DEVICE_ATARI_KEYBOARD RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_KEYBOARD, 0)
#define RETRO_DEVICE_ATARI_JOYSTICK RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 1)
#define RETRO_DEVICE_ATARI_5200_JOYSTICK RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 2)

extern char Key_State[512];
extern int SHIFTON;
extern int CTRLON;
extern int UI_is_active;
extern int SHOWKEY, SHOWKEYDELAY;

static int swap_joysticks = FALSE;
int PLATFORM_kbd_joy_0_enabled = TRUE;	/* enabled by default, doesn't hurt */
int PLATFORM_kbd_joy_1_enabled = TRUE;//FALSE;	/* disabled, would steal normal keys */
int PLATFORM_kbd_joy_2_enabled = TRUE;//FALSE;	/* disabled, would steal normal keys */
int PLATFORM_kbd_joy_3_enabled = TRUE;//FALSE;	/* disabled, would steal normal keys */

extern unsigned char MXjoy[4]; // joy
extern int mbt[4][16];
extern unsigned atari_devices[4];
extern int paddle_mode;

extern int retro_sound_finalized;

int CURRENT_TV=Atari800_TV_PAL;
int ToggleTV=0;

static UWORD *palette = NULL;

int skel_main(int argc, char **argv)
{

	/* initialise Atari800 core */
	if (!Atari800_Initialise(&argc, argv)){
		printf("Failed to initialise!\n");
		return 3;
	}
	//POKEYSND_Init(POKEYSND_FREQ_17_EXACT, 44100, 1, 1);
	retro_sound_finalized=1;

	printf("First retrun to main thread!\n");
    co_switch(mainThread);

	/* main loop */
	for (;;) {
		INPUT_key_code = PLATFORM_Keyboard();
		//SDL_INPUT_Mouse();
		Atari800_Frame();
		if (Atari800_display_screen)
			PLATFORM_DisplayScreen();
		
		if(CURRENT_TV!=Atari800_tv_mode){
			CURRENT_TV=Atari800_tv_mode;
			ToggleTV=1;
		}
	}
}

void retro_PaletteUpdate(void)
{
	int i;

	if (!palette) {
		if ( !(palette = malloc(256 * sizeof(UWORD))) ) {
			Log_print("Cannot allocate memory for palette conversion.");
			return;
		}
	}
	memset(palette, 0, 256 * sizeof(UWORD));

	for (i = 0; i < 256; i++){

		palette[i] = ((Colours_table[i] & 0x00f80000) >> 8) |
					 ((Colours_table[i] & 0x0000fc00) >> 5) | 
	 	   			 ((Colours_table[i] & 0x000000f8) >> 3);

	}

	/* force full redraw */
	Screen_EntireDirty();
}

int retro_InitGraphics(void)
{

	/* Initialize palette */
	retro_PaletteUpdate();

	return TRUE;
}

int PLATFORM_Initialise(int *argc, char *argv[])
{

	Log_print("Core init");

	retro_InitGraphics();

	Devices_enable_h_patch = FALSE;
	INPUT_direct_mouse = TRUE;

	return TRUE;
}

void retro_ExitGraphics(void)
{

	if (palette)
		free(palette);
	palette = NULL;
}

int PLATFORM_Exit(int run_monitor)
{
	if (CPU_cim_encountered) {
		Log_print("CIM encountered");
		return TRUE;
	}

	Log_print("Core_exit");

	retro_ExitGraphics();

	return FALSE;
}

static int key_control = 0;

int PLATFORM_Keyboard(void)
{	
	int shiftctrl = 0;
	int whichButton = 0;

	UI_alt_function = -1;

	if (Key_State[RETROK_LALT]){

		if (Key_State[RETROK_r])
			UI_alt_function = UI_MENU_RUN;
		else if (Key_State[RETROK_y])
			UI_alt_function = UI_MENU_SYSTEM;
		else if (Key_State[RETROK_o])
			UI_alt_function = UI_MENU_SOUND;
		else if (Key_State[RETROK_w])
			UI_alt_function = UI_MENU_SOUND_RECORDING;
		else if (Key_State[RETROK_a])
			UI_alt_function = UI_MENU_ABOUT;
		else if (Key_State[RETROK_s])
			UI_alt_function = UI_MENU_SAVESTATE;
		else if (Key_State[RETROK_d])
			UI_alt_function = UI_MENU_DISK;
		else if (Key_State[RETROK_l])
			UI_alt_function = UI_MENU_LOADSTATE;
		else if (Key_State[RETROK_c])
			UI_alt_function = UI_MENU_CARTRIDGE;
		else if (Key_State[RETROK_t])
			UI_alt_function = UI_MENU_CASSETTE;
		else if (Key_State[RETROK_BACKSLASH])
			return AKEY_PBI_BB_MENU;

	}

	/* SHIFT STATE */
	//if ((Key_State[RETROK_LSHIFT]) || (Key_State[RETROK_RSHIFT]))
	if (SHIFTON == 1)
		INPUT_key_shift = 1;
	else
		INPUT_key_shift = 0;

	/* CONTROL STATE */
	//if ((Key_State[RETROK_LCTRL]) || (Key_State[RETROK_RCTRL]))
	if (CTRLON == 1)
		key_control = 1;
	else
		key_control = 0;

	BINLOAD_pause_loading = FALSE;

	/* OPTION / SELECT / START keys */
	INPUT_key_consol = INPUT_CONSOL_NONE;
	if (Key_State[RETROK_F2])
		INPUT_key_consol &= (~INPUT_CONSOL_OPTION);
	if (Key_State[RETROK_F3])
		INPUT_key_consol &= (~INPUT_CONSOL_SELECT);
	if (Key_State[RETROK_F4])
		INPUT_key_consol &= (~INPUT_CONSOL_START);

	/* Handle movement and special keys. */
	if (Key_State[RETROK_F1])	return AKEY_UI;

	if (Key_State[RETROK_F5] && (Atari800_machine_type != Atari800_MACHINE_5200))
		return INPUT_key_shift ? AKEY_COLDSTART : AKEY_WARMSTART;


	if (Key_State[RETROK_F12])	return AKEY_TURBO;

	if (UI_alt_function != -1) {
		return AKEY_UI;
	}

	if (INPUT_key_shift)
		shiftctrl ^= AKEY_SHFT;

	// Atari 5200 specific.  Don't accept joypad input when UI is active. or virtual keyboard is active.
	if (Atari800_machine_type == Atari800_MACHINE_5200 && !UI_is_active)
	{
		INPUT_key_shift = 0;

		if (SHOWKEYDELAY)
			SHOWKEYDELAY--;

		/* Don't accept joypad input when virtual keyboard is active */
		for (int i = 0; i < 4; i++)
		{
			if (SHOWKEY == -1)
			{
				//if (MXjoy[0]&0x40) {
				/* 2nd action button */
				if (atari_devices[i] == RETRO_DEVICE_ATARI_JOYSTICK)
					whichButton = RETRO_DEVICE_ID_JOYPAD_X;
				else if (atari_devices[i] == RETRO_DEVICE_ATARI_5200_JOYSTICK)
					whichButton = RETRO_DEVICE_ID_JOYPAD_A;
				else if (atari_devices[i] == RETRO_DEVICE_ATARI_KEYBOARD && (Key_State[RETROK_RCTRL]))
					INPUT_key_shift = 1;

				if (mbt[i][whichButton]) {
					INPUT_key_shift = 1;
					break;
				}

				/* shared between ATARI_JOYSTICK and ATARI_5200*/
				if (mbt[i][RETRO_DEVICE_ID_JOYPAD_START])
					return AKEY_5200_START ^ shiftctrl;

				/* only Hash # and Asterick * */
				if (atari_devices[i] == RETRO_DEVICE_ATARI_JOYSTICK)
				{
					if (mbt[i][RETRO_DEVICE_ID_JOYPAD_SELECT]) return AKEY_5200_HASH ^ shiftctrl;
					if (mbt[i][RETRO_DEVICE_ID_JOYPAD_L])	   return AKEY_5200_ASTERISK ^ shiftctrl;
				}
				/* We map most all keys.  Only number keys done are 0-4.  This might change */
				/* Not a big fan of this method.  Likely creates issues on 2 or more player games. */
				/* But testing seems to show otherwise.  Maybe OR the values and then return the result */
				else if (atari_devices[i] == RETRO_DEVICE_ATARI_5200_JOYSTICK)
				{
					if (mbt[i][RETRO_DEVICE_ID_JOYPAD_Y])		return AKEY_5200_HASH ^ shiftctrl;
					if (mbt[i][RETRO_DEVICE_ID_JOYPAD_X])		return AKEY_5200_ASTERISK ^ shiftctrl;
					if (mbt[i][RETRO_DEVICE_ID_JOYPAD_SELECT]) return AKEY_5200_PAUSE ^ shiftctrl;
					if (mbt[i][RETRO_DEVICE_ID_JOYPAD_L])		return AKEY_5200_0 ^ shiftctrl;
					if (mbt[i][RETRO_DEVICE_ID_JOYPAD_R])		return AKEY_5200_1 ^ shiftctrl;
					if (mbt[i][RETRO_DEVICE_ID_JOYPAD_L2])		return AKEY_5200_2 ^ shiftctrl;
					if (mbt[i][RETRO_DEVICE_ID_JOYPAD_R2])		return AKEY_5200_3 ^ shiftctrl;
					if (mbt[i][RETRO_DEVICE_ID_JOYPAD_L3])		return AKEY_5200_7 ^ shiftctrl;
				}
			}

			if (atari_devices[i] == RETRO_DEVICE_ATARI_JOYSTICK)
				whichButton = RETRO_DEVICE_ID_JOYPAD_L3;
			else if (atari_devices[i] == RETRO_DEVICE_ATARI_5200_JOYSTICK)
				whichButton = RETRO_DEVICE_ID_JOYPAD_R3;

			if (mbt[i][whichButton])
				if (!SHOWKEYDELAY)
				{
					SHOWKEY = -SHOWKEY;
					SHOWKEYDELAY = 20;
				}
		}

		/* keyboard mapped to 5200 controls*/
		if (Key_State[RETROK_F4])	 		return AKEY_5200_START ^ shiftctrl;
		if (Key_State[RETROK_p])			return AKEY_5200_PAUSE ^ shiftctrl;
		if (Key_State[RETROK_r])			return AKEY_5200_RESET ^ shiftctrl;
		if (Key_State[RETROK_0])			return AKEY_5200_0 ^ shiftctrl;
		if (Key_State[RETROK_1])			return AKEY_5200_1 ^ shiftctrl;
		if (Key_State[RETROK_2])			return AKEY_5200_2 ^ shiftctrl;
		if (Key_State[RETROK_3])			return AKEY_5200_3 ^ shiftctrl;
		if (Key_State[RETROK_4])			return AKEY_5200_4 ^ shiftctrl;
		if (Key_State[RETROK_5])			return AKEY_5200_5 ^ shiftctrl;
		if (Key_State[RETROK_6])			return AKEY_5200_6 ^ shiftctrl;
		if (Key_State[RETROK_7])			return AKEY_5200_7 ^ shiftctrl;
		if (Key_State[RETROK_8])			return AKEY_5200_8 ^ shiftctrl;
		if (Key_State[RETROK_9])			return AKEY_5200_9 ^ shiftctrl;
		if (Key_State[RETROK_HASH])			return AKEY_5200_HASH ^ shiftctrl;
		if (Key_State[RETROK_EQUALS])		return AKEY_5200_HASH ^ shiftctrl;
		if (Key_State[RETROK_ASTERISK])		return AKEY_5200_ASTERISK ^ shiftctrl;
		if (Key_State[RETROK_KP_MULTIPLY])	return AKEY_5200_ASTERISK ^ shiftctrl;

		/* virtual keyboard mapped to 5200 controls */
		if (Key_State[RETROK_F5])
			return AKEY_5200_RESET ^ shiftctrl;

		if (Key_State[RETROK_F7])
		{
			SHOWKEY = -1;
			SHOWKEYDELAY = 20;
			return AKEY_UI;
		}

		return AKEY_NONE;
	}

//else if (Atari800_machine_type != Atari800_MACHINE_5200 && !UI_is_active)

	if (key_control)
		shiftctrl ^= AKEY_CTRL;

	if (Key_State[RETROK_BACKQUOTE] || Key_State[RETROK_LSUPER] )
		return AKEY_ATARI ^ shiftctrl;
	if (Key_State[RETROK_RSUPER] ){
		if (INPUT_key_shift)
			return AKEY_CAPSLOCK;
		else
			return AKEY_CAPSTOGGLE;
	}
	if (Key_State[RETROK_END] || Key_State[RETROK_F6] )
		return AKEY_HELP ^ shiftctrl;

	if (Key_State[RETROK_PAGEDOWN])
		return AKEY_F2 | AKEY_SHFT;

	if (Key_State[RETROK_PAGEUP])
		return AKEY_F1 | AKEY_SHFT;

	if (Key_State[RETROK_HOME])
		return key_control ? AKEY_LESS|shiftctrl : AKEY_CLEAR;

	if (Key_State[RETROK_PAUSE])
	{
		if (BINLOAD_wait_active) {
			BINLOAD_pause_loading = TRUE;
			return AKEY_NONE;
		}
		else
			return AKEY_BREAK;
	}
	if (Key_State[RETROK_CAPSLOCK]){
		if (INPUT_key_shift)
			return AKEY_CAPSLOCK|shiftctrl;
		else
			return AKEY_CAPSTOGGLE|shiftctrl;
	}

	if (Key_State[RETROK_SPACE])
		return AKEY_SPACE ^ shiftctrl;

	if (Key_State[RETROK_BACKSPACE]){
		if (INPUT_key_shift)
			return AKEY_DELETE_CHAR;
		else if (key_control)
			return AKEY_DELETE_LINE;
		else
			return AKEY_BACKSPACE;
	}

	if (Key_State[RETROK_RETURN])
		return AKEY_RETURN ^ shiftctrl;

	if (Key_State[RETROK_LEFT])
		return (!UI_is_active && Atari800_f_keys ? AKEY_F3 : (INPUT_key_shift ? AKEY_PLUS : AKEY_LEFT)) ^ shiftctrl;

	if (Key_State[RETROK_RIGHT])
		return (!UI_is_active && Atari800_f_keys ? AKEY_F4 : (INPUT_key_shift ? AKEY_ASTERISK : AKEY_RIGHT)) ^ shiftctrl;
	if (Key_State[RETROK_UP])
		return (!UI_is_active && Atari800_f_keys ? AKEY_F1 : (INPUT_key_shift ? AKEY_MINUS : AKEY_UP)) ^ shiftctrl;
	if (Key_State[RETROK_DOWN])
		return (!UI_is_active && Atari800_f_keys ? AKEY_F2 : (INPUT_key_shift ? AKEY_EQUAL : AKEY_DOWN)) ^ shiftctrl;

	if (Key_State[RETROK_ESCAPE])
		return AKEY_ESCAPE ^ shiftctrl;

	if (Key_State[RETROK_TAB])
		return AKEY_TAB ^ shiftctrl;

	if (Key_State[RETROK_DELETE]){
		if (INPUT_key_shift)
			return AKEY_DELETE_LINE|shiftctrl;
		else
			return AKEY_DELETE_CHAR;
	}
	if (Key_State[RETROK_INSERT]){
		if (INPUT_key_shift)
			return AKEY_INSERT_LINE|shiftctrl;
		else
			return AKEY_INSERT_CHAR;
	}

	if (INPUT_cx85){
		if (Key_State[RETROK_KP1])				return AKEY_CX85_1;
		else if (Key_State[RETROK_KP2])			return AKEY_CX85_2;
		else if (Key_State[RETROK_KP2])			return AKEY_CX85_3;
		else if (Key_State[RETROK_KP3])			return AKEY_CX85_4;
		else if (Key_State[RETROK_KP4])			return AKEY_CX85_5;
		else if (Key_State[RETROK_KP5])			return AKEY_CX85_6;
		else if (Key_State[RETROK_KP6])			return AKEY_CX85_7;
		else if (Key_State[RETROK_KP7])			return AKEY_CX85_8;
		else if (Key_State[RETROK_KP8])			return AKEY_CX85_9;
		else if (Key_State[RETROK_KP9])			return AKEY_CX85_0;
		else if (Key_State[RETROK_KP0])			return AKEY_CX85_2;
		else if (Key_State[RETROK_KP_PERIOD])	return AKEY_CX85_PERIOD;
		else if (Key_State[RETROK_KP_MINUS])	return AKEY_CX85_MINUS;
		else if (Key_State[RETROK_KP_ENTER])	return AKEY_CX85_PLUS_ENTER;
		else if (Key_State[RETROK_KP_DIVIDE])	return (key_control ? AKEY_CX85_ESCAPE : AKEY_CX85_NO);
		else if (Key_State[RETROK_KP_MULTIPLY])	return AKEY_CX85_DELETE;
		else if (Key_State[RETROK_KP_PLUS])		return AKEY_CX85_YES;
	}

	/* Handle CTRL-0 to CTRL-9 and other control characters */
	if (key_control) {

		if (Key_State[RETROK_PERIOD])		return AKEY_FULLSTOP|shiftctrl;
		if (Key_State[RETROK_COMMA])		return AKEY_COMMA|shiftctrl;
		if (Key_State[RETROK_SEMICOLON])	return AKEY_SEMICOLON|shiftctrl;
		if (Key_State[RETROK_SLASH])		return AKEY_SLASH|shiftctrl;
		if (Key_State[RETROK_BACKSLASH])	return AKEY_ESCAPE|shiftctrl;
		if (Key_State[RETROK_0])			return AKEY_CTRL_0|shiftctrl;
		if (Key_State[RETROK_1])			return AKEY_CTRL_1|shiftctrl;
		if (Key_State[RETROK_2])			return AKEY_CTRL_2|shiftctrl;
		if (Key_State[RETROK_3])			return AKEY_CTRL_3|shiftctrl;
		if (Key_State[RETROK_4])			return AKEY_CTRL_4|shiftctrl;
		if (Key_State[RETROK_5])			return AKEY_CTRL_5|shiftctrl;
		if (Key_State[RETROK_6])			return AKEY_CTRL_6|shiftctrl;
		if (Key_State[RETROK_7])			return AKEY_CTRL_7|shiftctrl;
		if (Key_State[RETROK_8])			return AKEY_CTRL_8|shiftctrl;
		if (Key_State[RETROK_9])			return AKEY_CTRL_9|shiftctrl;

		if (Key_State[RETROK_a])	return AKEY_CTRL_a;
		if (Key_State[RETROK_b])	return AKEY_CTRL_b;
		if (Key_State[RETROK_c])	return AKEY_CTRL_c;
		if (Key_State[RETROK_d])	return AKEY_CTRL_d;
		if (Key_State[RETROK_e])	return AKEY_CTRL_e;
		if (Key_State[RETROK_f])	return AKEY_CTRL_f;
		if (Key_State[RETROK_g])	return AKEY_CTRL_g;
		if (Key_State[RETROK_h])	return AKEY_CTRL_h;
		if (Key_State[RETROK_i])	return AKEY_CTRL_i;
		if (Key_State[RETROK_j])	return AKEY_CTRL_j;
		if (Key_State[RETROK_k])	return AKEY_CTRL_k;
		if (Key_State[RETROK_l])	return AKEY_CTRL_l;
		if (Key_State[RETROK_m])	return AKEY_CTRL_m;
		if (Key_State[RETROK_n])	return AKEY_CTRL_n;
		if (Key_State[RETROK_o])	return AKEY_CTRL_o;
		if (Key_State[RETROK_p])	return AKEY_CTRL_p;
		if (Key_State[RETROK_q])	return AKEY_CTRL_q;
		if (Key_State[RETROK_r])	return AKEY_CTRL_r;
		if (Key_State[RETROK_s])	return AKEY_CTRL_s;
		if (Key_State[RETROK_t])	return AKEY_CTRL_t;
		if (Key_State[RETROK_u])	return AKEY_CTRL_u;
		if (Key_State[RETROK_v])	return AKEY_CTRL_v;
		if (Key_State[RETROK_w])	return AKEY_CTRL_w;
		if (Key_State[RETROK_x])	return AKEY_CTRL_x;
		if (Key_State[RETROK_y])	return AKEY_CTRL_y;
		if (Key_State[RETROK_z])	return AKEY_CTRL_z;

		/* these three keys also type control-graphics characters, but
		   there don't seem to be AKEY_ values for them! */
		if (Key_State[RETROK_COMMA])		return (AKEY_CTRL | AKEY_COMMA);
		if (Key_State[RETROK_PERIOD])		return (AKEY_CTRL | AKEY_FULLSTOP);
		if (Key_State[RETROK_SEMICOLON])	return (AKEY_CTRL | AKEY_SEMICOLON);
		
		if (Key_State[RETROK_F7])	return (AKEY_CTRL | AKEY_F1);
		if (Key_State[RETROK_F8])	return (AKEY_CTRL | AKEY_F2);
		if (Key_State[RETROK_F9])	return (AKEY_CTRL | AKEY_F3);
		if (Key_State[RETROK_F10])	return (AKEY_CTRL | AKEY_F4);
		
		// cursor keys
		if (Key_State[RETROK_PLUS])			return AKEY_LEFT;
		if (Key_State[RETROK_ASTERISK])		return AKEY_RIGHT;
		if (Key_State[RETROK_EQUALS])		return AKEY_DOWN;
		if (Key_State[RETROK_UNDERSCORE])	return AKEY_UP;
	}

	/* handle all keys */

	if (INPUT_key_shift) {
		if (Key_State[RETROK_a])	return AKEY_A;
		if (Key_State[RETROK_b])	return AKEY_B;
		if (Key_State[RETROK_c])	return AKEY_C;
		if (Key_State[RETROK_d])	return AKEY_D;
		if (Key_State[RETROK_e])	return AKEY_E;
		if (Key_State[RETROK_f])	return AKEY_F;
		if (Key_State[RETROK_g])	return AKEY_G;
		if (Key_State[RETROK_h])	return AKEY_H;
		if (Key_State[RETROK_i])	return AKEY_I;
		if (Key_State[RETROK_j])	return AKEY_J;
		if (Key_State[RETROK_k])	return AKEY_K;
		if (Key_State[RETROK_l])	return AKEY_L;
		if (Key_State[RETROK_m])	return AKEY_M;
		if (Key_State[RETROK_n])	return AKEY_N;
		if (Key_State[RETROK_o])	return AKEY_O;
		if (Key_State[RETROK_p])	return AKEY_P;
		if (Key_State[RETROK_q])	return AKEY_Q;
		if (Key_State[RETROK_r])	return AKEY_R;
		if (Key_State[RETROK_s])	return AKEY_S;
		if (Key_State[RETROK_t])	return AKEY_T;
		if (Key_State[RETROK_u])	return AKEY_U;
		if (Key_State[RETROK_v])	return AKEY_V;
		if (Key_State[RETROK_w])	return AKEY_W;
		if (Key_State[RETROK_x])	return AKEY_X;
		if (Key_State[RETROK_y])	return AKEY_Y;
		if (Key_State[RETROK_z])	return AKEY_Z;

		if (Key_State[RETROK_1])	return AKEY_EXCLAMATION;
		if (Key_State[RETROK_2])	return AKEY_DBLQUOTE;
		if (Key_State[RETROK_3])	return AKEY_HASH;
		if (Key_State[RETROK_4])	return AKEY_DOLLAR;
		if (Key_State[RETROK_5])	return AKEY_PERCENT;
		if (Key_State[RETROK_6])	return AKEY_AMPERSAND;
		if (Key_State[RETROK_7])	return AKEY_QUOTE;
		if (Key_State[RETROK_8])	return AKEY_AT;
		if (Key_State[RETROK_9])	return AKEY_PARENLEFT;
		if (Key_State[RETROK_0])	return AKEY_PARENRIGHT;

		if (Key_State[RETROK_BACKSLASH])	return AKEY_BAR;
		if (Key_State[RETROK_COMMA])		return AKEY_BRACKETLEFT;
		if (Key_State[RETROK_PERIOD])		return AKEY_BRACKETRIGHT;
		if (Key_State[RETROK_UNDERSCORE])	return AKEY_MINUS;
		if (Key_State[RETROK_PLUS])			return AKEY_BACKSLASH;
		if (Key_State[RETROK_EQUALS])		return AKEY_BAR;
		if (Key_State[RETROK_LEFTBRACKET])	return AKEY_BRACKETLEFT; // no curly braces on Atari
		if (Key_State[RETROK_RIGHTBRACKET])	return AKEY_BRACKETRIGHT; // no curly braces on Atari
		if (Key_State[RETROK_SEMICOLON])	return AKEY_COLON;
		if (Key_State[RETROK_QUOTE])		return AKEY_DBLQUOTE;
		if (Key_State[RETROK_SLASH])		return AKEY_QUESTION;
		if (Key_State[RETROK_ASTERISK])		return AKEY_CIRCUMFLEX;
		if (Key_State[RETROK_LESS])			return AKEY_LESS;
		if (Key_State[RETROK_GREATER])		return AKEY_GREATER;
		
		if (Key_State[RETROK_F7])	return (AKEY_SHFT | AKEY_F1);
		if (Key_State[RETROK_F8])	return (AKEY_SHFT | AKEY_F2);
		if (Key_State[RETROK_F9])	return (AKEY_SHFT | AKEY_F3);
		if (Key_State[RETROK_F10])	return (AKEY_SHFT | AKEY_F4);

	} else {
		if (Key_State[RETROK_a])	return AKEY_a;
		if (Key_State[RETROK_b])	return AKEY_b;
		if (Key_State[RETROK_c])	return AKEY_c;
		if (Key_State[RETROK_d])	return AKEY_d;
		if (Key_State[RETROK_e])	return AKEY_e;
		if (Key_State[RETROK_f])	return AKEY_f;
		if (Key_State[RETROK_g])	return AKEY_g;
		if (Key_State[RETROK_h])	return AKEY_h;
		if (Key_State[RETROK_i])	return AKEY_i;
		if (Key_State[RETROK_j])	return AKEY_j;
		if (Key_State[RETROK_k])	return AKEY_k;
		if (Key_State[RETROK_l])	return AKEY_l;
		if (Key_State[RETROK_m])	return AKEY_m;
		if (Key_State[RETROK_n])	return AKEY_n;
		if (Key_State[RETROK_o])	return AKEY_o;
		if (Key_State[RETROK_p])	return AKEY_p;
		if (Key_State[RETROK_q])	return AKEY_q;
		if (Key_State[RETROK_r])	return AKEY_r;
		if (Key_State[RETROK_s])	return AKEY_s;
		if (Key_State[RETROK_t])	return AKEY_t;
		if (Key_State[RETROK_u])	return AKEY_u;
		if (Key_State[RETROK_v])	return AKEY_v;
		if (Key_State[RETROK_w])	return AKEY_w;
		if (Key_State[RETROK_x])	return AKEY_x;
		if (Key_State[RETROK_y])	return AKEY_y;
		if (Key_State[RETROK_z])	return AKEY_z;

		if (Key_State[RETROK_0])	return AKEY_0;
		if (Key_State[RETROK_1])	return AKEY_1;
		if (Key_State[RETROK_2])	return AKEY_2;
		if (Key_State[RETROK_3])	return AKEY_3;
		if (Key_State[RETROK_4])	return AKEY_4;
		if (Key_State[RETROK_5])	return AKEY_5;
		if (Key_State[RETROK_6])	return AKEY_6;
		if (Key_State[RETROK_7])	return AKEY_7;
		if (Key_State[RETROK_8])	return AKEY_8;
		if (Key_State[RETROK_9])	return AKEY_9;

		if (Key_State[RETROK_BACKSLASH])	return AKEY_BACKSLASH;
		if (Key_State[RETROK_COMMA])		return AKEY_COMMA;
		if (Key_State[RETROK_PERIOD])		return AKEY_FULLSTOP;
		if (Key_State[RETROK_MINUS])		return AKEY_MINUS;
		if (Key_State[RETROK_PLUS])			return AKEY_PLUS;
		if (Key_State[RETROK_EQUALS])		return AKEY_EQUAL;
		if (Key_State[RETROK_LEFTBRACKET])	return AKEY_BRACKETLEFT;
		if (Key_State[RETROK_RIGHTBRACKET])	return AKEY_BRACKETRIGHT;
		if (Key_State[RETROK_SEMICOLON])	return AKEY_SEMICOLON;
		if (Key_State[RETROK_QUOTE])		return AKEY_QUOTE;
		if (Key_State[RETROK_SLASH])		return AKEY_SLASH;
		if (Key_State[RETROK_ASTERISK])		return AKEY_ASTERISK;
		if (Key_State[RETROK_LESS])			return AKEY_LESS;
		if (Key_State[RETROK_GREATER])		return AKEY_GREATER;
		if (Key_State[RETROK_UNDERSCORE])	return AKEY_UNDERSCORE;
		
		if (Key_State[RETROK_F7])	return AKEY_F1;
		if (Key_State[RETROK_F8])	return AKEY_F2;
		if (Key_State[RETROK_F9])	return AKEY_F3;
		if (Key_State[RETROK_F10])	return AKEY_F4;
	}

	/* FIXME joy bind */
	if (!UI_is_active)
	{
		if (SHOWKEYDELAY)
			SHOWKEYDELAY--;

		for (int i = 0; i < 4; i++)
		{
			if (SHOWKEY == -1)
			{
				if (mbt[i][RETRO_DEVICE_ID_JOYPAD_SELECT])
					INPUT_key_consol &= (~INPUT_CONSOL_SELECT);
				if (mbt[i][RETRO_DEVICE_ID_JOYPAD_START])
					INPUT_key_consol &= (~INPUT_CONSOL_START);
				if (mbt[i][RETRO_DEVICE_ID_JOYPAD_L])
					INPUT_key_consol &= (~INPUT_CONSOL_OPTION);
				if (mbt[i][RETRO_DEVICE_ID_JOYPAD_R])
					return AKEY_UI;
				if (mbt[i][RETRO_DEVICE_ID_JOYPAD_Y])
					return AKEY_SPACE;
				if (mbt[i][RETRO_DEVICE_ID_JOYPAD_L2])
					return AKEY_ESCAPE;
				if (mbt[i][RETRO_DEVICE_ID_JOYPAD_X])
					return AKEY_RETURN;
				if (mbt[i][RETRO_DEVICE_ID_JOYPAD_R2])
					return AKEY_HELP;
			}

			if (mbt[i][RETRO_DEVICE_ID_JOYPAD_L3])
				if (!SHOWKEYDELAY)
				{
					SHOWKEY = -SHOWKEY;
					SHOWKEYDELAY = 20;
				}
		}

		/* not quite working when controller Device Type is ATARI Keyboard.. FIXME!*/
		//if (atari_devices[0] == RETRO_DEVICE_ATARI_KEYBOARD && (Key_State[RETROK_F11]))
		//	if (!SHOWKEYDELAY)
		//	{
		//		SHOWKEY = -SHOWKEY;
		//		SHOWKEYDELAY = 20;
		//		Key_State[RETROK_F11] = 0;
		//	}
	}

	/* not a big fan of hard coding input.. but it helps in a pinch of one accidently unmaps the controls
		and this code is only used when the UI is active */
	if (UI_is_active){
	// whithout kbd in GUI 
		if (MXjoy[0] & 0x04)
			return AKEY_LEFT;
		if (MXjoy[0] & 0x08)
			return AKEY_RIGHT;
		if (MXjoy[0] & 0x01)
			return AKEY_UP;
		if (MXjoy[0] & 0x02)
			return AKEY_DOWN;
		if (MXjoy[0] & 0x10)
			return AKEY_RETURN;
		if (MXjoy[0] & 0x80)
			return AKEY_ESCAPE;
	}

	/* This is my alternate internal UI controls... mapped based on players controller mapping.  I can't decide which is the best to use 
		Maybe give the player the option? */
	//if (UI_is_active) {
	//	for (int i = 0; i < 4; i++)
	//	{
	//		if (mbt[i][RETRO_DEVICE_ID_JOYPAD_LEFT])
	//			return AKEY_LEFT;
	//		if (mbt[i][RETRO_DEVICE_ID_JOYPAD_RIGHT])
	//			return AKEY_RIGHT;
	//		if (mbt[i][RETRO_DEVICE_ID_JOYPAD_UP])
	//			return AKEY_UP;
	//		if (mbt[i][RETRO_DEVICE_ID_JOYPAD_DOWN])
	//			return AKEY_DOWN;

	//		if (atari_devices[i] == RETRO_DEVICE_ATARI_JOYSTICK)
	//			whichButton = RETRO_DEVICE_ID_JOYPAD_A;
	//		else if (atari_devices[i] == RETRO_DEVICE_ATARI_5200_JOYSTICK)
	//			whichButton = RETRO_DEVICE_ID_JOYPAD_B;
	//		if (mbt[i][whichButton])
	//			return AKEY_RETURN;

	//		if (atari_devices[i] == RETRO_DEVICE_ATARI_JOYSTICK)
	//			whichButton = RETRO_DEVICE_ID_JOYPAD_R2;
	//		else if (atari_devices[i] == RETRO_DEVICE_ATARI_5200_JOYSTICK)
	//			whichButton = RETRO_DEVICE_ID_JOYPAD_A;
	//		if (mbt[i][whichButton])
	//			return AKEY_ESCAPE;
	//	}
	//}
	
	return AKEY_NONE;
}

/*
int PLATFORM_GetRawKey(void)
{
	input_poll_cb();

	for(i=0;i<320;i++)
        	Key_State[i]=input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0,i) ? 0x80: 0;
}
*/

void retro_Render(void)
{
	int x, y;
	UBYTE *src, *src_line;
	UWORD *dst, *dst_line;

	src_line = ((UBYTE *) Screen_atari) + 24;
	dst_line = Retro_Screen;

	for (y = 0; y < 240; y++) {

		src = src_line;
		dst = dst_line;

		for (x = 0; x < 336; x += 8) {

			*dst++ = palette[*src++]; *dst++ = palette[*src++];
					*dst++ = palette[*src++]; *dst++ = palette[*src++];
					*dst++ = palette[*src++]; *dst++ = palette[*src++];
					*dst++ = palette[*src++]; *dst++ = palette[*src++];
		}

		src_line += 384;
		dst_line += 336;
	}
}

void PLATFORM_DisplayScreen(void)
{
	retro_Render();
}

extern float retro_fps;
extern long long retro_frame_counter;

double PLATFORM_Time(void)
{
        return retro_frame_counter * (1000.0 / retro_fps);
}

void PLATFORM_PaletteUpdate(void)
{
	retro_PaletteUpdate();
}

static void get_platform_PORT(unsigned char *s0, unsigned char *s1, unsigned char *s2, unsigned char *s3)
{
	int stick0, stick1, stick2, stick3;
	stick0 = stick1 = stick2 = stick3 = INPUT_STICK_CENTRE;

	/* only send trigger (joyleft) if paddle mode is active and trigger button pressed*/
	if (!UI_is_active && paddle_mode)
	{
		if (PLATFORM_kbd_joy_0_enabled) {
			if ((MXjoy[0] & 0x80))
				stick0 &= INPUT_STICK_LEFT;
		}

		if (PLATFORM_kbd_joy_1_enabled) {
			if ((MXjoy[1] & 0x80))
				stick1 &= INPUT_STICK_LEFT;
		}

		if (PLATFORM_kbd_joy_2_enabled) {
			if ((MXjoy[2] & 0x80))
				stick2 &= INPUT_STICK_LEFT;
		}

		if (PLATFORM_kbd_joy_3_enabled) {
			if ((MXjoy[3] & 0x80))
				stick3 &= INPUT_STICK_LEFT;
		}

		*s0 = stick0;
		*s1 = stick1;
		*s2 = stick2;
		*s3 = stick3;

		/* trig is all we care about */
		return;
	}

	if (PLATFORM_kbd_joy_0_enabled) {
		if (MXjoy[0] & 0x04)
			stick0 &= INPUT_STICK_LEFT;
		if (MXjoy[0] & 0x08)
			stick0 &= INPUT_STICK_RIGHT;
		if (MXjoy[0] & 0x01)
			stick0 &= INPUT_STICK_FORWARD;
		if (MXjoy[0] & 0x02)
			stick0 &= INPUT_STICK_BACK;
	}
	if (PLATFORM_kbd_joy_1_enabled) {

		if (MXjoy[1] & 0x04)
			stick1 &= INPUT_STICK_LEFT;
		if (MXjoy[1] & 0x08)
			stick1 &= INPUT_STICK_RIGHT;
		if (MXjoy[1] & 0x01)
			stick1 &= INPUT_STICK_FORWARD;
		if (MXjoy[1] & 0x02)
			stick1 &= INPUT_STICK_BACK;
	}
	if (PLATFORM_kbd_joy_2_enabled) {
		if (MXjoy[2] & 0x04)
			stick2 &= INPUT_STICK_LEFT;
		if (MXjoy[2] & 0x08)
			stick2 &= INPUT_STICK_RIGHT;
		if (MXjoy[2] & 0x01)
			stick2 &= INPUT_STICK_FORWARD;
		if (MXjoy[2] & 0x02)
			stick2 &= INPUT_STICK_BACK;
	}
	if (PLATFORM_kbd_joy_3_enabled) {
		if (MXjoy[3] & 0x04)
			stick3 &= INPUT_STICK_LEFT;
		if (MXjoy[3] & 0x08)
			stick3 &= INPUT_STICK_RIGHT;
		if (MXjoy[3] & 0x01)
			stick3 &= INPUT_STICK_FORWARD;
		if (MXjoy[3] & 0x02)
			stick3 &= INPUT_STICK_BACK;
	}

	if (swap_joysticks) {
		*s1 = stick0;
		*s0 = stick1;
		*s2 = stick2;
		*s3 = stick3;
	}
	else {
		*s0 = stick0;
		*s1 = stick1;
		*s2 = stick2;
		*s3 = stick3;
	}

 }

static void get_platform_TRIG(unsigned char *t0, unsigned char *t1, unsigned char *t2, unsigned char *t3)
{
	int trig0, trig1, trig2, trig3;
	trig0 = trig1 = trig2 = trig3 = 1;

	if (PLATFORM_kbd_joy_0_enabled) {
		trig0 = MXjoy[0] & 0x80 ? 0:1;
	}

	if (PLATFORM_kbd_joy_1_enabled) {
		trig1 = MXjoy[1] & 0x80 ? 0:1;
	}

	if (PLATFORM_kbd_joy_2_enabled) {
		trig2 = MXjoy[2] & 0x80 ? 0:1;
	}

	if (PLATFORM_kbd_joy_3_enabled) {
		trig3 = MXjoy[3] & 0x80 ? 0:1;
	}

	if (swap_joysticks) {
		*t1 = trig0;
		*t0 = trig1;
		*t2 = trig2;
		*t3 = trig3;
	}
	else {
		*t0 = trig0;
		*t1 = trig1;
		*t2 = trig2;
		*t3 = trig3;
	}

}

int PLATFORM_PORT(int num)
{
	if (SHOWKEY == 1 )
		return 0xff;

	if (num == 0) {
		UBYTE a, b, c, d;
		get_platform_PORT(&a, &b, &c, &d);

		return (b << 4) | (a & 0x0f);
	}
	if (num == 1) {
		UBYTE a, b, c, d;
		get_platform_PORT(&a, &b, &c, &d);

		return (d << 4) | (c & 0x0f);
	}

	return 0xff;
}

int PLATFORM_TRIG(int num)
{
	UBYTE a, b, c, d;

	if (SHOWKEY == 1)
		return 0x01;

	get_platform_TRIG(&a, &b, &c, &d);

	switch (num) {
	case 0:
		return a;
	case 1:
		return b;
	case 2:
		return c;
	case 3:
		return d;
	default:
		break;
	}

	return 0x01;
}


/////////////////////////////////////////////////////////////
//   SOUND
/////////////////////////////////////////////////////////////


int PLATFORM_SoundSetup(Sound_setup_t *setup)
{
	//force 16 bit stereo sound at 44100
	setup->freq=44100;
	setup->sample_size=2;
	setup->channels=2;
//	setup->buffer_ms=20;
	setup->buffer_frames = 1024;

	return TRUE;
}

void PLATFORM_SoundExit(void)
{

}

void PLATFORM_SoundPause(void)
{

}

void PLATFORM_SoundContinue(void)
{

}

void PLATFORM_SoundLock(void)
{

}

void PLATFORM_SoundUnlock(void)
{

}
