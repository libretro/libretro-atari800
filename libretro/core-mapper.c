#include "libretro.h"
#include "libretro-core.h"
#include "retroscreen.h"
#include "platform.h"
#include "vkbd.h"
#include "input.h"
#include "pokey.h"

//CORE VAR
#ifdef _WIN32
char slash = '\\';
#else
char slash = '/';
#endif

//extern void retro_message(const char* text, unsigned int frames, int alt);

long frame=0;
unsigned long  Ktime=0 , LastFPSTime=0;

//VIDEO
#ifdef  RENDER16B
	uint16_t Retro_Screen[400*300];
#else
	unsigned int Retro_Screen[400*300];
#endif 

//SOUND
unsigned char SNDBUF[1024*2*2];
int snd_sampler_pal = 44100 / 50;
int snd_sampler_ntsc = 44100 / 60;

//EMU FLAGS
int NPAGE = -1, KCOL = 1, BKGCOLOR = 0;
int SHOWKEY = -1, SHOWKEYDELAY = 0;
int VKBD_OPACITY = -1;

#if defined(ANDROID) || defined(__ANDROID__)
int MOUSE_EMULATED = 1;
#else
int MOUSE_EMULATED = -1;
#endif

int SHIFTON=-1,MOUSEMODE=-1,PAS=4;
int SND=1; //SOUND ON/OFF
int pauseg=0; //enter_gui
int touch=-1; // gui mouse btn

//JOY
int al[2][2];//left analog1
int ar[2][2];//right analog1
unsigned char MXjoy[4]; // joy
int16_t joypad_bits[4];
extern UBYTE consol_mask;

#define JOYRANGE_UP_VALUE     -16384     /* Joystick ranges in XY */
#define JOYRANGE_DOWN_VALUE    16383
#define JOYRANGE_LEFT_VALUE   -16384
#define JOYRANGE_RIGHT_VALUE   16383

// retro core option variables
extern int atari_joyhack;
extern int keyboard_type;
extern int pot_analog_deadzone;
extern int paddle_mode;
extern int paddle_speed;

extern int INPUT_joy_5200_center;
extern int INPUT_joy_5200_min;
extern int INPUT_joy_5200_max;
extern int INPUT_digital_5200_min;
extern int INPUT_digital_5200_center;
extern int INPUT_digital_5200_max;

//MOUSE                                 Is this even tied to anything?
extern int pushi;  // gui mouse btn
int gmx,gmy; //gui mouse
int mouse_wu=0,mouse_wd=0;

//Atari800 EMU mouse
extern UBYTE POKEY_POT_input[8];
extern int INPUT_mouse_pot_min;
extern int INPUT_mouse_pot_max;

//KEYBOARD
char Key_State[512];
static char old_Key_State[512];

int mbt[4][16] = { {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }
               , { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } };

//STATS GUI
int BOXDEC= 32+2;
int STAT_BASEY;

/*static*/ retro_input_state_t input_state_cb;
static retro_input_poll_t input_poll_cb;

extern bool libretro_supports_bitmasks;

void retro_set_input_state(retro_input_state_t cb)
{
   input_state_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   input_poll_cb = cb;
}

int slowdown=0;

#if defined(ANDROID) || defined(__ANDROID__)
#define DEFAULT_PATH "/mnt/sdcard/"
#else

#ifdef PS3PORT
#define DEFAULT_PATH "/dev_hdd0/HOMEBREW/"
#else
#define DEFAULT_PATH "/"
#endif

#endif

#define RETRO_DEVICE_ATARI_KEYBOARD RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_KEYBOARD, 0)
#define RETRO_DEVICE_ATARI_JOYSTICK RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 1)
#define RETRO_DEVICE_ATARI_5200_JOYSTICK RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 2)

void texture_uninit(void) { }

void texture_init(void)
{
   memset(Retro_Screen, 0, sizeof(Retro_Screen));
   memset(old_Key_State ,0, sizeof(old_Key_State));

   gmx=(retrow/2)-1;
   gmy=(retroh/2)-1;
}

int pushi=0; //mouse button
int keydown=0,keyup=0;
int KBMOD=-1;
int RSTOPON=-1;
int CTRLON=-1;

extern unsigned short int bmp[400*300];
extern unsigned atari_devices[ 4 ];

extern int UI_is_active;
extern int CURRENT_TV;

void retro_sound_update(void)
{	
   int x,stop=CURRENT_TV==312?885:742;//FIXME: 882/735?

   if (! UI_is_active)
   {
      Sound_Callback(SNDBUF, 1024*2*2);
      for(x=0;x<stop*2;x+=2)
         retro_audio_cb(SNDBUF[x],SNDBUF[x+2]);

   }
}

//extern void vkbd_key(int key,int pressed);

void Screen_SetFullUpdate(int scr)
{
    if (scr == 0 || scr > 1)
        memset(Retro_Screen, 0, sizeof(Retro_Screen));
}

void vkbd_key(int key,int pressed)
{
   if(pressed)
   {
      if(SHIFTON==1)
         ;
      Key_State[key]=1;
      // key is being held down
   }
   else
   {
      if(SHIFTON==1)
         ;
      Key_State[key]=0;
      // key is being RELEASE 
   }
}

void retro_virtualkb(void)
{
   // VKBD
   int i;
   //   RETRO        B    Y    SLT  STA  UP   DWN  LEFT RGT  A    X    L    R    L2   R2   L3   R3
   //   INDEX        0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15
   static int oldi=-1;
   static int vkx=0,vky=0;

   if(oldi!=-1)
   {
      vkbd_key(oldi,0);
      oldi=-1;
   }

   if(SHOWKEY==1)
   {
      static int vkflag[5]={0,0,0,0,0};		

      if ( (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) && vkflag[0]==0 )
         vkflag[0]=1;
      else if (vkflag[0]==1 && !(joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) )
      {
         vkflag[0]=0;
         vky -= 1; 
      }

      if ( (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) && vkflag[1]==0 )
         vkflag[1]=1;
      else if (vkflag[1]==1 && !(joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) )
      {
         vkflag[1]=0;
         vky += 1; 
      }

      if ( (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) && vkflag[2]==0 )
         vkflag[2]=1;
      else if (vkflag[2]==1 && !(joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) )
      {
         vkflag[2]=0;
         vkx -= 1;
      }

      if ( (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) && vkflag[3]==0 )
         vkflag[3]=1;
      else if (vkflag[3]==1 && !(joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) )
      {
         vkflag[3]=0;
         vkx += 1;
      }

      if(vkx<0)vkx=NPLGN-1;
      if(vkx>NPLGN-1)vkx=0;
      if(vky<0)vky=NLIGN-1;
      if(vky>NLIGN-1)vky=0;

      virtual_kdb(( char *)Retro_Screen,vkx,vky);

      i=0;  // swapped Button 1 and 2 definitions ( used to be 8 )
      if( (joypad_bits[0] & (1 << i)) && vkflag[4]==0 ) 	
         vkflag[4]=1;
      else if( !(joypad_bits[0] & (1 << i)) && vkflag[4]==1 )
      {
         vkflag[4]=0;
         i=check_vkey2(vkx,vky);

         if(i==-1)
            oldi=-1;
         if(i==-2)
         {
            NPAGE=-NPAGE;
            oldi=-1;
         }
         else if(i==-3)
         {
            //KDB bgcolor
            KCOL=-KCOL;
            oldi=-1;
         }
         else if(i==-4)
         {
            //VKbd show/hide 			
            oldi=-1;
            Screen_SetFullUpdate(0);
            SHOWKEY=-SHOWKEY;
         }
         else if(i==-5)
		 {
			//VKbd opacitu on/off
			VKBD_OPACITY=-VKBD_OPACITY;
			oldi=-1;
		 }
         else if(i==-6)
            oldi=-1;
         else if(i==-7)
            oldi=-1;
         else if(i==-8)
            oldi=-1;
         else
         {
            if(i==RETROK_LSHIFT) //SHIFT
            {
               SHIFTON=-SHIFTON;

               oldi=-1;
            }
            else if(i==RETROK_LCTRL) //CTRL
            {     
               CTRLON=-CTRLON;

               oldi=-1;
            }
            else if(i==-12) //RSTOP
            {
               //(RETROK_ESCAPE);            
               RSTOPON=-RSTOPON;

               oldi=-1;
            }
            else if(i==-13) //GUI
            {     
               SHOWKEY=-SHOWKEY;
               oldi=-1;
            }
            else if(i==-14) //JOY PORT TOGGLE
            {    
               SHOWKEY=-SHOWKEY;
               oldi=-1;
            }
            else
            {
               oldi=i;
               vkbd_key(oldi,1);            
            }
         }
      }
   }
}

void Process_key(void)
{
	int i;

	if(keyboard_type==1)
      return;

	for(i=0;i<320;i++)
        	Key_State[i]=input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0,i) ? 0x80: 0;
   
	if(memcmp( Key_State,old_Key_State , sizeof(Key_State) ) )
   {
	  for(i=0;i<320;i++)
      {
		 if(Key_State[i] && Key_State[i]!=old_Key_State[i]  )
         {
            if(i==RETROK_RCTRL)
            {
               CTRLON=-CTRLON;
               continue;
            }
            if(i==RETROK_RSHIFT)
            {
               SHIFTON=-SHIFTON;
               continue;
            }

            if(i==RETROK_LALT)
            {
               KBMOD=-KBMOD;
               continue;
            }
            //printf("press: %d \n",i);
            //retro_key_down(i);

         }
         else if ( !Key_State[i] && Key_State[i]!=old_Key_State[i]  )
         {
            if(i==RETROK_RCTRL)
            {
               CTRLON=-CTRLON;
               continue;
            }
            if(i==RETROK_RSHIFT)
            {
               SHIFTON=-SHIFTON;
               continue;
            }

            if(i==RETROK_LALT)
            {
               KBMOD=-KBMOD;
               continue;
            }

            //printf("release: %d \n",i);
            //retro_key_up(i);

         }
      }
   }

	memcpy(old_Key_State,Key_State , sizeof(Key_State) );
}

int Atari_POT(int input)
{
    int which = input / 2;
    int xval, yval, pval, result;

    //if (consol_mask == 0x0f)
    //    return 228;

    /* account for Joystick swap enabled */
    if (atari_joyhack == 2)
        which = (which + 1) % 2;

    xval = input_state_cb(which, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
    yval = input_state_cb(which, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);

    pval = (input & 1) ? yval : xval;

    if ((pval > -pot_analog_deadzone) && (pval < pot_analog_deadzone))
        return INPUT_joy_5200_center;

   /* Convert to amplitude */
    float amplitude = (float)((pval > pot_analog_deadzone) ?
        (pval - pot_analog_deadzone) :
        (pval + pot_analog_deadzone)) /
        (float)(LIBRETRO_ANALOG_RANGE - pot_analog_deadzone);

    /* Map to Atari 5200 values */
    if (amplitude >= 0.0f)
        result = (JOY_5200_CENTER + (unsigned int)(((float)(INPUT_joy_5200_max - JOY_5200_CENTER) * amplitude) + 0.5f));
    else
        result = (JOY_5200_CENTER - (unsigned int)(((float)(JOY_5200_CENTER - INPUT_joy_5200_min) * -amplitude) + 0.5f));

    /* for debug purposes */
    //if (input == 1)
    //{
    //    char msg[256];

    //    //sprintf(msg, "X=%f, Y=%f, Deadzone=%f.  Result- %i\n",  xval / (float)LIBRETRO_ANALOG_RANGE, yval / (float)LIBRETRO_ANALOG_RANGE, (float)pot_analog_deadzone, result);
    //    sprintf(msg, "Joy Min=%i, Joy Max=%i.  Result- %i\n", INPUT_joy_5200_max, INPUT_joy_5200_min, result);
    //    retro_message(msg, 60, 0);
    //}

    return result;
}

int Retro_PollEvent()
{
   //   RETRO        B    Y    SLT  STA  UP   DWN  LEFT RGT  A    X    L    R    L2   R2   L3   R3
   //   INDEX        0    1     2    3   4     5    6    7   8    9    10   11   12   13   14   15

   int i,j;
   static int vbt[4][16]={
      {0x80,0x0,0x0,0x0,0x01,0x02,0x04,0x08,0x10,0x40,0x0,0x0,0x0,0x0,0x0,0x0},
      {0x80,0x0,0x0,0x0,0x01,0x02,0x04,0x08,0x10,0x40,0x0,0x0,0x0,0x0,0x0,0x0},
      {0x80,0x0,0x0,0x0,0x01,0x02,0x04,0x08,0x10,0x40,0x0,0x0,0x0,0x0,0x0,0x0},
      {0x80,0x0,0x0,0x0,0x01,0x02,0x04,0x08,0x10,0x40,0x0,0x0,0x0,0x0,0x0,0x0},
   };

   input_poll_cb();

   for (j = 0; j < 4; j++)
   {
      if (libretro_supports_bitmasks)
         joypad_bits[j] = input_state_cb(j, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
      else
      {
         joypad_bits[j] = 0;
         for (i = 0; i < (RETRO_DEVICE_ID_JOYPAD_R3+1); i++)
            joypad_bits[j] |= input_state_cb(j, RETRO_DEVICE_JOYPAD, 0, i) ? (1 << i) : 0;
      }
   }

   int mouse_l;
   int mouse_r;
   int16_t mouse_x,mouse_y;
   mouse_x=mouse_y=0;

   if (SHOWKEY==-1 && pauseg==0)
       Process_key();

   //Joy mode
   for(j=0;j<4;j++)
   {
      for(i=0;i<16;i++)
      {
         if(joypad_bits[j] & (1 << i))
            MXjoy[j] |= vbt[j][i]; // Joy press
         else if( MXjoy[j] & vbt[j][i])
            MXjoy[j] &= ~vbt[j][i]; // Joy press
      }
   }

   if (atari_joyhack == 1 && !paddle_mode) //hack for robotron right analog act as Joy1
   {
      //emulate Joy1 with joy analog right 
      ar[0][0] = (input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X));
      ar[0][1] = (input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y));

      /* Directions */

      if (ar[0][1] <= JOYRANGE_UP_VALUE)
         MXjoy[1] |= 0x01;
      else if (ar[0][1] >= JOYRANGE_DOWN_VALUE)
         MXjoy[1] |= 0x02;

      if (ar[0][0] <= JOYRANGE_LEFT_VALUE)
         MXjoy[1] |= 0x04;
      else if (ar[0][0] >= JOYRANGE_RIGHT_VALUE)
         MXjoy[1] |= 0x08;
   }
   else if (atari_joyhack == 2 && !paddle_mode) //hack for Joy 1 / 2 swap.
   {
       //emulate Joy1 with joy analog right 
       al[0][0] = (input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X));
       al[0][1] = (input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y));

       if (libretro_supports_bitmasks)
           joypad_bits[1] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
       else
       {
           joypad_bits[1] = 0;
           joypad_bits[1] |= input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK) ? 1 : 0;
       }

       for (i = 0 ; i < 16; i++)
       {
           if (joypad_bits[1] & (1 << i))
               MXjoy[1] |= vbt[0][i]; // Joy press
           else if (MXjoy[1] & vbt[0][i])
               MXjoy[1] &= ~vbt[0][i]; // Joy press
       }

       /* Directions */

       if (al[0][1] <= JOYRANGE_UP_VALUE)
           MXjoy[1] |= 0x01;
       else if (al[0][1] >= JOYRANGE_DOWN_VALUE)
           MXjoy[1] |= 0x02;

       if (al[0][0] <= JOYRANGE_LEFT_VALUE)
           MXjoy[1] |= 0x04;
       else if (al[0][0] >= JOYRANGE_RIGHT_VALUE)
           MXjoy[1] |= 0x08;
   }

   if ( atari_devices[0] != RETRO_DEVICE_ATARI_KEYBOARD)
   {
      //shortcut for joy mode only

      //Button  B    Y    SLT  STA
      //	    0    1     2    3

       for (i = 0; i < 16; i++)
       {
           for (int j = 0; j < 4; j++)
           {
               if ((joypad_bits[j] & (1 << i)) && mbt[j][i] == 0)
                   mbt[j][i] = 1;
               else if (mbt[j][i] == 1 && !(joypad_bits[j] & (1 << i)))
               {
                   mbt[j][i] = 0;
#if defined(ANDROID) || defined(__ANDROID__)
                   /* apparently this is used by the ANDROID port?  Keep in for now.  Android only*/
                   if(i==2)
                      MOUSE_EMULATED = -MOUSE_EMULATED;
#endif
               }
           }
       }
      
      /* Huh?  */
      //for(i=0;i<4;i++)
      //{
      //   if ( (joypad_bits[0] & (1 << i)) && mbt[i]==0 )
      //      mbt[i]=1;
      //   else if (mbt[i]==1 && !(joypad_bits[0] & (1 << i)) )
      //   {
      //      mbt[i]=0;
      //      //if(i==2)                              /* hard coded? android only? */
      //      //   MOUSE_EMULATED = -MOUSE_EMULATED;
      //   }
      //}
      ////Button  L    R    L2   R2   L3   R3
      ////        10   11   12   13   14   15
      //for(i=10;i<16;i++)
      //{
      //   if ( (joypad_bits[0] & (1 << i)) && mbt[i]==0 )
      //      mbt[i]=1;
      //   else if ( mbt[i]==1 && !(joypad_bits[0] & (1 << i)) )
      //   {
      //      mbt[i]=0;
      //      //if(i==14)                             /* removed can be defined in controller settings screen? */
      //         //SHOWKEY = -SHOWKEY;
      //   }
      //}
   }
   else         //Emulate joystick controls with keyboard/retro_keyboard only Joy 1 atm... experimental
   {
       // these are the same for both computer and 5200
       if (Key_State[RETROK_KP8])           // up
           MXjoy[0] |= 0x01;
       else if (Key_State[RETROK_KP2])      // down
           MXjoy[0] |= 0x02;
       if (Key_State[RETROK_KP4])           // left
           MXjoy[0] |= 0x04;
       else if (Key_State[RETROK_KP6])      // right
           MXjoy[0] |= 0x08;

       if (Key_State[RETROK_RALT])          // fire 1
           MXjoy[0] |= 0x80;

       // 5200 fire button 2
       if (Atari800_machine_type == Atari800_MACHINE_5200 && !UI_is_active)
           if ( Key_State[RETROK_RCTRL])
               INPUT_key_shift = 1;
   }

   if (paddle_mode)
   {
       for (int i = 0; i < 4; i++)
       {
           int pval = input_state_cb(i, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);

           if ((pval < -pot_analog_deadzone) || (pval > pot_analog_deadzone))
           {
               if (pval > 0)
                   POKEY_POT_input[i] -= paddle_speed;
               else
                   POKEY_POT_input[i] += paddle_speed;
           }
           else
           {
               if (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))
                   POKEY_POT_input[i] -= paddle_speed; 
               if (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))
                   POKEY_POT_input[i] += paddle_speed;
           }

           if (POKEY_POT_input[i] < INPUT_mouse_pot_min)
               POKEY_POT_input[i] = INPUT_mouse_pot_min;

           if (POKEY_POT_input[i] > INPUT_mouse_pot_max)
               POKEY_POT_input[i] = INPUT_mouse_pot_max;
       }
   }

   if(MOUSE_EMULATED==1)
   {
      if(slowdown>0)
         return 1;

      if (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))
         mouse_x += PAS;
      if (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))
         mouse_x -= PAS;
      if (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN))
         mouse_y += PAS;
      if (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_UP))
         mouse_y -= PAS;
      mouse_l = (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_B)) ? 1 : 0;
      mouse_r = (joypad_bits[0] & (1 << RETRO_DEVICE_ID_JOYPAD_A)) ? 1 : 0;

      slowdown=1;
   }
   else
   {
      mouse_wu   = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_WHEELUP);
      mouse_wd   = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_WHEELDOWN);
      mouse_x    = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
      mouse_y    = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
      mouse_l    = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
      mouse_r    = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
   }

   static int mmbL=0,mmbR=0;

   if(mmbL==0 && mouse_l)
   {
      mmbL=1;
      pushi=1;
      touch=1;
   }
   else if(mmbL==1 && !mouse_l)
   {
      mmbL=0;
      pushi=0;
      touch=-1;
   }

   if(mmbR==0 && mouse_r)
      mmbR=1;
   else if(mmbR==1 && !mouse_r)
      mmbR=0;

   gmx += mouse_x;
   gmy += mouse_y;
   if(gmx<0)
      gmx = 0;
   if(gmx>retrow-1)
      gmx = retrow-1;
   if(gmy<0)
      gmy = 0;
   if(gmy>retroh-1)
      gmy = retroh-1;

   if(SHOWKEY==1 && pauseg==0)
      retro_virtualkb();

   return 1;
}

