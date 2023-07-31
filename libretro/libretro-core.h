#ifndef LIBRETRO_CORE_H
#define LIBRETRO_CORE_H 1

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <stdbool.h>

#define TEX_WIDTH 400
#define TEX_HEIGHT 300

#define UINT16 uint16_t
#define UINT32 uint32_t

#define RENDER16B
#ifdef  RENDER16B
	extern uint16_t Retro_Screen[400*300];
	#define PIXEL_BYTES 1
	#define PIXEL_TYPE UINT16
	#define PITCH 2	
#else
	extern unsigned int Retro_Screen[400*300];
	#define PIXEL_BYTES 2
	#define PIXEL_TYPE UINT32
	#define PITCH 4	
#endif 

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 300

#include "libco.h"

extern cothread_t mainThread;
extern cothread_t emuThread;

extern char Key_State[512];

extern int pauseg; 

#define NPLGN 12
#define NLIGN 6
#define NLETT 5

#define XSIDE  (CROP_WIDTH/NPLGN -1)
#define YSIDE  (CROP_HEIGHT/8 -1)

#define YBASE0 (CROP_HEIGHT - NLIGN*YSIDE -8)
#define XBASE0 0+4+2
#define XBASE3 0
#define YBASE3 YBASE0 -4

#define STAT_DECX 120
#define STAT_YSZ  20

#ifndef  RENDER16B
#define RGB565(r, g, b)  (((r) << (5+16)) | ((g) << (5+8)) | (b<<5))
#define R_RGB565(rgb)	(((rgb) >> (5+16)) & 255)
#define G_RGB565(rgb)	(((rgb) >> (5+8)) & 255)
#define B_RGB565(rgb)	((rgb>>5) & 255)
#else
#if defined(ABGR1555)
#define RGB565(r, g, b)  (((b) << (10)) | ((g) << 5) | (r))
#define B_RGB565(rgb)	(((rgb) >> (10)) & 31)
#define G_RGB565(rgb)	(((rgb) >> 5) & 31)
#define R_RGB565(rgb)	(rgb & 31)
#else
#define RGB565(r, g, b)  (((r) << (5+6)) | ((g) << 6) | (b))
#define R_RGB565(rgb)	(((rgb) >> (5+6)) & 31)
#define G_RGB565(rgb)	(((rgb) >> 6) & 31)
#define B_RGB565(rgb)	(rgb & 31)
#endif
#endif
#define uint32 unsigned int
#define uint8 unsigned char

//Paddle & 5200 POT
#define LIBRETRO_ANALOG_RANGE 0x8000
#define JOY_5200_MIN 6
#define JOY_5200_MAX 220
#define JOY_5200_CENTER 114

void retro_message(const char* text, unsigned int frames, int alt);
void retro_audio_cb(short l, short r);
#endif
