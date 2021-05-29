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

extern char Key_Sate[512];

#include "libretro-core.h"
#include "libretro.h"
#include "retroscreen.h"

extern int SHIFTON;
extern int CTRLON;
extern int UI_is_active;

static int swap_joysticks = FALSE;
int PLATFORM_kbd_joy_0_enabled = TRUE;	/* enabled by default, doesn't hurt */
int PLATFORM_kbd_joy_1_enabled = TRUE;//FALSE;	/* disabled, would steal normal keys */
int PLATFORM_kbd_joy_2_enabled = TRUE;//FALSE;	/* disabled, would steal normal keys */
int PLATFORM_kbd_joy_3_enabled = TRUE;//FALSE;	/* disabled, would steal normal keys */
extern unsigned char MXjoy[4]; // joy
extern int mbt[16];
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

	UI_alt_function = -1;

	if (Key_Sate[RETROK_LALT]){

		if (Key_Sate[RETROK_r])
			UI_alt_function = UI_MENU_RUN;
		else if (Key_Sate[RETROK_y])
			UI_alt_function = UI_MENU_SYSTEM;
		else if (Key_Sate[RETROK_o])
			UI_alt_function = UI_MENU_SOUND;
		else if (Key_Sate[RETROK_w])
			UI_alt_function = UI_MENU_SOUND_RECORDING;
		else if (Key_Sate[RETROK_a])
			UI_alt_function = UI_MENU_ABOUT;
		else if (Key_Sate[RETROK_s])
			UI_alt_function = UI_MENU_SAVESTATE;
		else if (Key_Sate[RETROK_d])
			UI_alt_function = UI_MENU_DISK;
		else if (Key_Sate[RETROK_l])
			UI_alt_function = UI_MENU_LOADSTATE;
		else if (Key_Sate[RETROK_c])
			UI_alt_function = UI_MENU_CARTRIDGE;
		else if (Key_Sate[RETROK_t])
			UI_alt_function = UI_MENU_CASSETTE;
		else if (Key_Sate[RETROK_BACKSLASH])
			return AKEY_PBI_BB_MENU;

	}

	/* SHIFT STATE */
	//if ((Key_Sate[RETROK_LSHIFT]) || (Key_Sate[RETROK_RSHIFT]))
	if (SHIFTON == 1)
		INPUT_key_shift = 1;
	else
		INPUT_key_shift = 0;

	/* CONTROL STATE */
	//if ((Key_Sate[RETROK_LCTRL]) || (Key_Sate[RETROK_RCTRL]))
	if (CTRLON == 1)
		key_control = 1;
	else
		key_control = 0;

	BINLOAD_pause_loading = FALSE;

	/* OPTION / SELECT / START keys */
	INPUT_key_consol = INPUT_CONSOL_NONE;
	if (Key_Sate[RETROK_F2])
		INPUT_key_consol &= (~INPUT_CONSOL_OPTION);
	if (Key_Sate[RETROK_F3])
		INPUT_key_consol &= (~INPUT_CONSOL_SELECT);
	if (Key_Sate[RETROK_F4])
		INPUT_key_consol &= (~INPUT_CONSOL_START);

	/* Handle movement and special keys. */
	if (Key_Sate[RETROK_F1])return AKEY_UI;

	if (Key_Sate[RETROK_F5])
		return INPUT_key_shift ? AKEY_COLDSTART : AKEY_WARMSTART;


	if (Key_Sate[RETROK_F12])return AKEY_TURBO;

	if (UI_alt_function != -1) {
		return AKEY_UI;
	}

	if (INPUT_key_shift)
		shiftctrl ^= AKEY_SHFT;

	if (Atari800_machine_type == Atari800_MACHINE_5200 && !UI_is_active) {

		if (MXjoy[0]&0x40) {	/* 2nd action button */
			INPUT_key_shift = 1;
		}
		else {
			INPUT_key_shift = 0;
		}

	        if (mbt[RETRO_DEVICE_ID_JOYPAD_START])
			return AKEY_5200_START ^ shiftctrl;

		if (Key_Sate[RETROK_F4])
			return AKEY_5200_START ^ shiftctrl;

		if(Key_Sate[RETROK_p])return AKEY_5200_PAUSE ^ shiftctrl;
		if(Key_Sate[RETROK_r])return AKEY_5200_RESET ^ shiftctrl;
		if(Key_Sate[RETROK_0])return AKEY_5200_0 ^ shiftctrl;
		if(Key_Sate[RETROK_1])return AKEY_5200_1 ^ shiftctrl;
		if(Key_Sate[RETROK_2])return AKEY_5200_2 ^ shiftctrl;
		if(Key_Sate[RETROK_3])return AKEY_5200_3 ^ shiftctrl;
		if(Key_Sate[RETROK_4])return AKEY_5200_4 ^ shiftctrl;
		if(Key_Sate[RETROK_5])return AKEY_5200_5 ^ shiftctrl;
		if(Key_Sate[RETROK_6])return AKEY_5200_6 ^ shiftctrl;
		if(Key_Sate[RETROK_7])return AKEY_5200_7 ^ shiftctrl;
		if(Key_Sate[RETROK_8])return AKEY_5200_8 ^ shiftctrl;
		if(Key_Sate[RETROK_9])return AKEY_5200_9 ^ shiftctrl;
		if(Key_Sate[RETROK_HASH])return AKEY_5200_HASH ^ shiftctrl;
		if(Key_Sate[RETROK_EQUALS])return AKEY_5200_HASH ^ shiftctrl;
		if(Key_Sate[RETROK_ASTERISK])return AKEY_5200_ASTERISK ^ shiftctrl;
		if(Key_Sate[RETROK_KP_MULTIPLY])return AKEY_5200_ASTERISK ^ shiftctrl;


		return AKEY_NONE;
	}

//else if (Atari800_machine_type != Atari800_MACHINE_5200 && !UI_is_active)
{

	if (key_control)
		shiftctrl ^= AKEY_CTRL;

	if (Key_Sate[RETROK_BACKQUOTE] || Key_Sate[RETROK_LSUPER] )
		return AKEY_ATARI ^ shiftctrl;
	if (Key_Sate[RETROK_RSUPER] ){
		if (INPUT_key_shift)
			return AKEY_CAPSLOCK;
		else
			return AKEY_CAPSTOGGLE;
	}
	if (Key_Sate[RETROK_END] || Key_Sate[RETROK_F6] )
		return AKEY_HELP ^ shiftctrl;

	if (Key_Sate[RETROK_PAGEDOWN])
		return AKEY_F2 | AKEY_SHFT;

	if (Key_Sate[RETROK_PAGEUP])
		return AKEY_F1 | AKEY_SHFT;

	if (Key_Sate[RETROK_HOME])
		return key_control ? AKEY_LESS|shiftctrl : AKEY_CLEAR;

	if (Key_Sate[RETROK_PAUSE])
	{
		if (BINLOAD_wait_active) {
			BINLOAD_pause_loading = TRUE;
			return AKEY_NONE;
		}
		else
			return AKEY_BREAK;
	}
	if (Key_Sate[RETROK_CAPSLOCK]){
		if (INPUT_key_shift)
			return AKEY_CAPSLOCK|shiftctrl;
		else
			return AKEY_CAPSTOGGLE|shiftctrl;
	}

	if (Key_Sate[RETROK_SPACE])
		return AKEY_SPACE ^ shiftctrl;

	if (Key_Sate[RETROK_BACKSPACE]){
		if (INPUT_key_shift)
			return AKEY_DELETE_CHAR;
		else if (key_control)
			return AKEY_DELETE_LINE;
		else
			return AKEY_BACKSPACE;
	}

	if (Key_Sate[RETROK_RETURN])
		return AKEY_RETURN ^ shiftctrl;

	if (Key_Sate[RETROK_LEFT])
		return (!UI_is_active && Atari800_f_keys ? AKEY_F3 : (INPUT_key_shift ? AKEY_PLUS : AKEY_LEFT)) ^ shiftctrl;

	if (Key_Sate[RETROK_RIGHT])
		return (!UI_is_active && Atari800_f_keys ? AKEY_F4 : (INPUT_key_shift ? AKEY_ASTERISK : AKEY_RIGHT)) ^ shiftctrl;
	if (Key_Sate[RETROK_UP])
		return (!UI_is_active && Atari800_f_keys ? AKEY_F1 : (INPUT_key_shift ? AKEY_MINUS : AKEY_UP)) ^ shiftctrl;
	if (Key_Sate[RETROK_DOWN])
		return (!UI_is_active && Atari800_f_keys ? AKEY_F2 : (INPUT_key_shift ? AKEY_EQUAL : AKEY_DOWN)) ^ shiftctrl;

	if (Key_Sate[RETROK_ESCAPE])
		return AKEY_ESCAPE ^ shiftctrl;

	if (Key_Sate[RETROK_TAB])
		return AKEY_TAB ^ shiftctrl;

	if (Key_Sate[RETROK_DELETE]){
		if (INPUT_key_shift)
			return AKEY_DELETE_LINE|shiftctrl;
		else
			return AKEY_DELETE_CHAR;
	}
	if (Key_Sate[RETROK_INSERT]){
		if (INPUT_key_shift)
			return AKEY_INSERT_LINE|shiftctrl;
		else
			return AKEY_INSERT_CHAR;
	}

	if (INPUT_cx85){

		if (Key_Sate[RETROK_KP1])
			return AKEY_CX85_1;
		else if (Key_Sate[RETROK_KP2])
			return AKEY_CX85_2;
		else if (Key_Sate[RETROK_KP2])
			return AKEY_CX85_3;
		else if (Key_Sate[RETROK_KP3])
			return AKEY_CX85_4;
		else if (Key_Sate[RETROK_KP4])
			return AKEY_CX85_5;
		else if (Key_Sate[RETROK_KP5])
			return AKEY_CX85_6;
		else if (Key_Sate[RETROK_KP6])
			return AKEY_CX85_7;
		else if (Key_Sate[RETROK_KP7])
			return AKEY_CX85_8;
		else if (Key_Sate[RETROK_KP8])
			return AKEY_CX85_9;
		else if (Key_Sate[RETROK_KP9])
			return AKEY_CX85_0;
		else if (Key_Sate[RETROK_KP0])
			return AKEY_CX85_2;
		else if (Key_Sate[RETROK_KP_PERIOD])
			return AKEY_CX85_PERIOD;
		else if (Key_Sate[RETROK_KP_MINUS])
			return AKEY_CX85_MINUS;
		else if (Key_Sate[RETROK_KP_ENTER])
			return AKEY_CX85_PLUS_ENTER;
		else if (Key_Sate[RETROK_KP_DIVIDE])
			return (key_control ? AKEY_CX85_ESCAPE : AKEY_CX85_NO);
		else if (Key_Sate[RETROK_KP_MULTIPLY])
			return AKEY_CX85_DELETE;
		else if (Key_Sate[RETROK_KP_PLUS])
			return AKEY_CX85_YES;
	}

	/* Handle CTRL-0 to CTRL-9 and other control characters */
	if (key_control) {

		if (Key_Sate[RETROK_PERIOD])
			return AKEY_FULLSTOP|shiftctrl;
		if (Key_Sate[RETROK_COMMA])
			return AKEY_COMMA|shiftctrl;
		if (Key_Sate[RETROK_SEMICOLON])
			return AKEY_SEMICOLON|shiftctrl;
		if (Key_Sate[RETROK_SLASH])
			return AKEY_SLASH|shiftctrl;
		if (Key_Sate[RETROK_BACKSLASH])
			return AKEY_ESCAPE|shiftctrl;
		if (Key_Sate[RETROK_0])
			return AKEY_CTRL_0|shiftctrl;
		if (Key_Sate[RETROK_1])
			return AKEY_CTRL_1|shiftctrl;
		if (Key_Sate[RETROK_2])
			return AKEY_CTRL_2|shiftctrl;
		if (Key_Sate[RETROK_3])	
			return AKEY_CTRL_3|shiftctrl;
		if (Key_Sate[RETROK_4])
			return AKEY_CTRL_4|shiftctrl;
		if (Key_Sate[RETROK_5])
			return AKEY_CTRL_5|shiftctrl;
		if (Key_Sate[RETROK_6])	
			return AKEY_CTRL_6|shiftctrl;
		if (Key_Sate[RETROK_7])
			return AKEY_CTRL_7|shiftctrl;
		if (Key_Sate[RETROK_8])	
			return AKEY_CTRL_8|shiftctrl;
		if (Key_Sate[RETROK_9])	
			return AKEY_CTRL_9|shiftctrl;

		if (Key_Sate[RETROK_a])return AKEY_CTRL_a;
		if (Key_Sate[RETROK_b])return AKEY_CTRL_b;
		if (Key_Sate[RETROK_c])return AKEY_CTRL_c;
		if (Key_Sate[RETROK_d])return AKEY_CTRL_d;
		if (Key_Sate[RETROK_e])return AKEY_CTRL_e;
		if (Key_Sate[RETROK_f])return AKEY_CTRL_f;
		if (Key_Sate[RETROK_g])return AKEY_CTRL_g;
		if (Key_Sate[RETROK_h])return AKEY_CTRL_h;
		if (Key_Sate[RETROK_i])return AKEY_CTRL_i;
		if (Key_Sate[RETROK_j])return AKEY_CTRL_j;
		if (Key_Sate[RETROK_k])return AKEY_CTRL_k;
		if (Key_Sate[RETROK_l])return AKEY_CTRL_l;
		if (Key_Sate[RETROK_m])return AKEY_CTRL_m;
		if (Key_Sate[RETROK_n])return AKEY_CTRL_n;
		if (Key_Sate[RETROK_o])return AKEY_CTRL_o;
		if (Key_Sate[RETROK_p])return AKEY_CTRL_p;
		if (Key_Sate[RETROK_q])return AKEY_CTRL_q;
		if (Key_Sate[RETROK_r])return AKEY_CTRL_r;
		if (Key_Sate[RETROK_s])return AKEY_CTRL_s;
		if (Key_Sate[RETROK_t])return AKEY_CTRL_t;
		if (Key_Sate[RETROK_u])return AKEY_CTRL_u;
		if (Key_Sate[RETROK_v])return AKEY_CTRL_v;
		if (Key_Sate[RETROK_w])return AKEY_CTRL_w;
		if (Key_Sate[RETROK_x])return AKEY_CTRL_x;
		if (Key_Sate[RETROK_y])return AKEY_CTRL_y;
		if (Key_Sate[RETROK_z])return AKEY_CTRL_z;

		/* these three keys also type control-graphics characters, but
		   there don't seem to be AKEY_ values for them! */
		if (Key_Sate[RETROK_COMMA])return (AKEY_CTRL | AKEY_COMMA);
		if (Key_Sate[RETROK_PERIOD])return (AKEY_CTRL | AKEY_FULLSTOP);
		if (Key_Sate[RETROK_SEMICOLON])return (AKEY_CTRL | AKEY_SEMICOLON);
		
		if (Key_Sate[RETROK_F7])return (AKEY_CTRL | AKEY_F1);
		if (Key_Sate[RETROK_F8])return (AKEY_CTRL | AKEY_F2);
		if (Key_Sate[RETROK_F9])return (AKEY_CTRL | AKEY_F3);
		if (Key_Sate[RETROK_F10])return (AKEY_CTRL | AKEY_F4);
		
		// cursor keys
		if (Key_Sate[RETROK_PLUS])return AKEY_LEFT;
		if (Key_Sate[RETROK_ASTERISK])return AKEY_RIGHT;
		if (Key_Sate[RETROK_EQUALS])return AKEY_DOWN;
		if (Key_Sate[RETROK_UNDERSCORE])return AKEY_UP;


	}

	/* handle all keys */

	if (INPUT_key_shift) {
		if (Key_Sate[RETROK_a])return AKEY_A;
		if (Key_Sate[RETROK_b])return AKEY_B;
		if (Key_Sate[RETROK_c])return AKEY_C;
		if (Key_Sate[RETROK_d])return AKEY_D;
		if (Key_Sate[RETROK_e])return AKEY_E;
		if (Key_Sate[RETROK_f])return AKEY_F;
		if (Key_Sate[RETROK_g])return AKEY_G;
		if (Key_Sate[RETROK_h])return AKEY_H;
		if (Key_Sate[RETROK_i])return AKEY_I;
		if (Key_Sate[RETROK_j])return AKEY_J;
		if (Key_Sate[RETROK_k])return AKEY_K;
		if (Key_Sate[RETROK_l])return AKEY_L;
		if (Key_Sate[RETROK_m])return AKEY_M;
		if (Key_Sate[RETROK_n])return AKEY_N;
		if (Key_Sate[RETROK_o])return AKEY_O;
		if (Key_Sate[RETROK_p])return AKEY_P;
		if (Key_Sate[RETROK_q])return AKEY_Q;
		if (Key_Sate[RETROK_r])return AKEY_R;
		if (Key_Sate[RETROK_s])return AKEY_S;
		if (Key_Sate[RETROK_t])return AKEY_T;
		if (Key_Sate[RETROK_u])return AKEY_U;
		if (Key_Sate[RETROK_v])return AKEY_V;
		if (Key_Sate[RETROK_w])return AKEY_W;
		if (Key_Sate[RETROK_x])return AKEY_X;
		if (Key_Sate[RETROK_y])return AKEY_Y;
		if (Key_Sate[RETROK_z])return AKEY_Z;

		if (Key_Sate[RETROK_1])return AKEY_EXCLAMATION;
		if (Key_Sate[RETROK_2])return AKEY_DBLQUOTE;
		if (Key_Sate[RETROK_3])return AKEY_HASH;
		if (Key_Sate[RETROK_4])return AKEY_DOLLAR;
		if (Key_Sate[RETROK_5])return AKEY_PERCENT;
		if (Key_Sate[RETROK_6])return AKEY_AMPERSAND;
		if (Key_Sate[RETROK_7])return AKEY_QUOTE;
		if (Key_Sate[RETROK_8])return AKEY_AT;
		if (Key_Sate[RETROK_9])return AKEY_PARENLEFT;
		if (Key_Sate[RETROK_0])return AKEY_PARENRIGHT;

		if (Key_Sate[RETROK_BACKSLASH])return AKEY_BAR;
		if (Key_Sate[RETROK_COMMA])return AKEY_BRACKETLEFT;
		if (Key_Sate[RETROK_PERIOD])return AKEY_BRACKETRIGHT;
		if (Key_Sate[RETROK_UNDERSCORE])return AKEY_MINUS;
		if (Key_Sate[RETROK_PLUS])return AKEY_BACKSLASH;
		if (Key_Sate[RETROK_EQUALS])return AKEY_BAR;
		if (Key_Sate[RETROK_LEFTBRACKET])return AKEY_BRACKETLEFT; // no curly braces on Atari
		if (Key_Sate[RETROK_RIGHTBRACKET])return AKEY_BRACKETRIGHT; // no curly braces on Atari
		if (Key_Sate[RETROK_SEMICOLON])return AKEY_COLON;
		if (Key_Sate[RETROK_QUOTE])return AKEY_DBLQUOTE;
		if (Key_Sate[RETROK_SLASH])return AKEY_QUESTION;
		if (Key_Sate[RETROK_ASTERISK])return AKEY_CIRCUMFLEX;
		if (Key_Sate[RETROK_LESS])return AKEY_LESS;
		if (Key_Sate[RETROK_GREATER])return AKEY_GREATER;
		
		if (Key_Sate[RETROK_F7])return (AKEY_SHFT | AKEY_F1);
		if (Key_Sate[RETROK_F8])return (AKEY_SHFT | AKEY_F2);
		if (Key_Sate[RETROK_F9])return (AKEY_SHFT | AKEY_F3);
		if (Key_Sate[RETROK_F10])return (AKEY_SHFT | AKEY_F4);

	} else {
		if (Key_Sate[RETROK_a])return AKEY_a;
		if (Key_Sate[RETROK_b])return AKEY_b;
		if (Key_Sate[RETROK_c])return AKEY_c;
		if (Key_Sate[RETROK_d])return AKEY_d;
		if (Key_Sate[RETROK_e])return AKEY_e;
		if (Key_Sate[RETROK_f])return AKEY_f;
		if (Key_Sate[RETROK_g])return AKEY_g;
		if (Key_Sate[RETROK_h])return AKEY_h;
		if (Key_Sate[RETROK_i])return AKEY_i;
		if (Key_Sate[RETROK_j])return AKEY_j;
		if (Key_Sate[RETROK_k])return AKEY_k;
		if (Key_Sate[RETROK_l])return AKEY_l;
		if (Key_Sate[RETROK_m])return AKEY_m;
		if (Key_Sate[RETROK_n])return AKEY_n;
		if (Key_Sate[RETROK_o])return AKEY_o;
		if (Key_Sate[RETROK_p])return AKEY_p;
		if (Key_Sate[RETROK_q])return AKEY_q;
		if (Key_Sate[RETROK_r])return AKEY_r;
		if (Key_Sate[RETROK_s])return AKEY_s;
		if (Key_Sate[RETROK_t])return AKEY_t;
		if (Key_Sate[RETROK_u])return AKEY_u;
		if (Key_Sate[RETROK_v])return AKEY_v;
		if (Key_Sate[RETROK_w])return AKEY_w;
		if (Key_Sate[RETROK_x])return AKEY_x;
		if (Key_Sate[RETROK_y])return AKEY_y;
		if (Key_Sate[RETROK_z])return AKEY_z;

		if (Key_Sate[RETROK_0])return AKEY_0;
		if (Key_Sate[RETROK_1])return AKEY_1;
		if (Key_Sate[RETROK_2])return AKEY_2;
		if (Key_Sate[RETROK_3])return AKEY_3;
		if (Key_Sate[RETROK_4])return AKEY_4;
		if (Key_Sate[RETROK_5])return AKEY_5;
		if (Key_Sate[RETROK_6])return AKEY_6;
		if (Key_Sate[RETROK_7])return AKEY_7;
		if (Key_Sate[RETROK_8])return AKEY_8;
		if (Key_Sate[RETROK_9])return AKEY_9;

		if (Key_Sate[RETROK_BACKSLASH])return AKEY_BACKSLASH;
		if (Key_Sate[RETROK_COMMA])return AKEY_COMMA;
		if (Key_Sate[RETROK_PERIOD])return AKEY_FULLSTOP;
		if (Key_Sate[RETROK_MINUS])return AKEY_MINUS;
		if (Key_Sate[RETROK_PLUS])return AKEY_PLUS;
		if (Key_Sate[RETROK_EQUALS])return AKEY_EQUAL;
		if (Key_Sate[RETROK_LEFTBRACKET])return AKEY_BRACKETLEFT;
		if (Key_Sate[RETROK_RIGHTBRACKET])return AKEY_BRACKETRIGHT;
		if (Key_Sate[RETROK_SEMICOLON])return AKEY_SEMICOLON;
		if (Key_Sate[RETROK_QUOTE])return AKEY_QUOTE;
		if (Key_Sate[RETROK_SLASH])return AKEY_SLASH;
		if (Key_Sate[RETROK_ASTERISK])return AKEY_ASTERISK;
		if (Key_Sate[RETROK_LESS])return AKEY_LESS;
		if (Key_Sate[RETROK_GREATER])return AKEY_GREATER;
		if (Key_Sate[RETROK_UNDERSCORE])return AKEY_UNDERSCORE;
		
		if (Key_Sate[RETROK_F7])return AKEY_F1;
		if (Key_Sate[RETROK_F8])return AKEY_F2;
		if (Key_Sate[RETROK_F9])return AKEY_F3;
		if (Key_Sate[RETROK_F10])return AKEY_F4;

	}

	/* FIXME joy bind */

	if (mbt[RETRO_DEVICE_ID_JOYPAD_SELECT])
		INPUT_key_consol &= (~INPUT_CONSOL_SELECT);
	if (mbt[RETRO_DEVICE_ID_JOYPAD_START])
		INPUT_key_consol &= (~INPUT_CONSOL_START);
	if (mbt[RETRO_DEVICE_ID_JOYPAD_L])
		INPUT_key_consol &= (~INPUT_CONSOL_OPTION);
	if (mbt[RETRO_DEVICE_ID_JOYPAD_R])
		return AKEY_UI;
	if (mbt[RETRO_DEVICE_ID_JOYPAD_L2])
		return AKEY_SPACE;
	if (mbt[RETRO_DEVICE_ID_JOYPAD_R2])
		return AKEY_ESCAPE;
	if (mbt[RETRO_DEVICE_ID_JOYPAD_B])
		return AKEY_RETURN;
	if (mbt[RETRO_DEVICE_ID_JOYPAD_Y])
		return AKEY_HELP;
}

	if (UI_is_active){
	// whitout kbd in GUI 
		if (MXjoy[0]&0x04)
			return AKEY_LEFT;
		if (MXjoy[0]&0x08)
			return AKEY_RIGHT;
		if (MXjoy[0]&0x01)
			return AKEY_UP;
		if (MXjoy[0]&0x02)
			return AKEY_DOWN;
		if (MXjoy[0]&0x80)
			return AKEY_RETURN;
		if (MXjoy[0]&0x40)
			return AKEY_ESCAPE;
	}

	return AKEY_NONE;

}

/*
int PLATFORM_GetRawKey(void)
{

	input_poll_cb();

	for(i=0;i<320;i++)
        	Key_Sate[i]=input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0,i) ? 0x80: 0;

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

	if (PLATFORM_kbd_joy_0_enabled) {
		if (MXjoy[0]&0x04)
			stick0 &= INPUT_STICK_LEFT;
		if (MXjoy[0]&0x08)
			stick0 &= INPUT_STICK_RIGHT;
		if (MXjoy[0]&0x01)
			stick0 &= INPUT_STICK_FORWARD;
		if (MXjoy[0]&0x02)
			stick0 &= INPUT_STICK_BACK;
	}
	if (PLATFORM_kbd_joy_1_enabled) {

		if (MXjoy[1]&0x04)
			stick1 &= INPUT_STICK_LEFT;
		if (MXjoy[1]&0x08)
			stick1 &= INPUT_STICK_RIGHT;
		if (MXjoy[1]&0x01)
			stick1 &= INPUT_STICK_FORWARD;
		if (MXjoy[1]&0x02)
			stick1 &= INPUT_STICK_BACK;
	}
	if (PLATFORM_kbd_joy_2_enabled) {
		if (MXjoy[2]&0x04)
			stick2 &= INPUT_STICK_LEFT;
		if (MXjoy[2]&0x08)
			stick2 &= INPUT_STICK_RIGHT;
		if (MXjoy[2]&0x01)
			stick2 &= INPUT_STICK_FORWARD;
		if (MXjoy[2]&0x02)
			stick2 &= INPUT_STICK_BACK;
	}
	if (PLATFORM_kbd_joy_3_enabled) {
		if (MXjoy[3]&0x04)
			stick3 &= INPUT_STICK_LEFT;
		if (MXjoy[3]&0x08)
			stick3 &= INPUT_STICK_RIGHT;
		if (MXjoy[3]&0x01)
			stick3 &= INPUT_STICK_FORWARD;
		if (MXjoy[3]&0x02)
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
		trig0 = MXjoy[0]&0x80?0:1;
	}

	if (PLATFORM_kbd_joy_1_enabled) {
		trig1 = MXjoy[1]&0x80?0:1;
	}

	if (PLATFORM_kbd_joy_2_enabled) {
		trig2 = MXjoy[2]&0x80?0:1;
	}

	if (PLATFORM_kbd_joy_3_enabled) {
		trig3 = MXjoy[3]&0x80?0:1;
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
