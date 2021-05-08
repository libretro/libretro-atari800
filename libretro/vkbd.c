
#include "libretro.h"

#include "retroscreen.h"
#include "libretro-core.h"
#include "vkbd_def.h"
#include "graph.h"

extern int NPAGE;
extern int KCOL;
extern int BKGCOLOR;
extern int SHIFTON;
extern int CTRLON;

void virtual_kdb(char *buffer,int vx,int vy)
{

   int x, y, page;
   unsigned coul;

#if defined PITCH && PITCH == 4
unsigned *pix=(unsigned*)buffer;
#else
unsigned short *pix=(unsigned short *)buffer;
#endif

   page = (NPAGE == -1) ? 0 : NLIGN*NPLGN;
   coul = RGB565(28, 28, 31);
   BKGCOLOR = (KCOL>0?0xFF808080:0);


   for(x=0;x<NPLGN;x++)
   {
      for(y=0;y<NLIGN;y++)
      {
         DrawBoxBmp((char*)pix,XBASE3+x*XSIDE,YBASE3+y*YSIDE, XSIDE,YSIDE, RGB565(7, 2, 1));
		 if (SHIFTON==1)
		 {
			Draw_text((char*)pix,XBASE0-2+x*XSIDE ,YBASE0+YSIDE*y,coul, BKGCOLOR ,1, 1,3,MVk[(y*NPLGN)+x+page].shift);
		 }
		 else if (CTRLON==1)
		 {
			Draw_text((char*)pix,XBASE0-2+x*XSIDE ,YBASE0+YSIDE*y,coul, BKGCOLOR ,1, 1,3,MVk[(y*NPLGN)+x+page].ctrl);
		 }
		 else
		 {
			Draw_text((char*)pix,XBASE0-2+x*XSIDE ,YBASE0+YSIDE*y,coul, BKGCOLOR ,1, 1,3,MVk[(y*NPLGN)+x+page].norml);
		 }
      }
   }
   
   // draw Shift and Control keys status
   // Shift - position 0,4
   if (SHIFTON==1)
   {
	   Draw_text((char*)pix,XBASE0-2+0*XSIDE ,YBASE0+YSIDE*4,RGB565(2,2,31), BKGCOLOR ,1, 1,3,MVk[(4*NPLGN)+0+page].shift);
   }
   // Control - position 0,3
   if (CTRLON==1)
   {
	   Draw_text((char*)pix,XBASE0-2+0*XSIDE ,YBASE0+YSIDE*3,RGB565(2,2,31), BKGCOLOR ,1, 1,3,MVk[(3*NPLGN)+0+page].ctrl);
   }

   DrawBoxBmp((char*)pix,XBASE3+vx*XSIDE,YBASE3+vy*YSIDE, XSIDE,YSIDE, RGB565(31, 2, 1));
	if (SHIFTON==1)
	{
		Draw_text((char*)pix,XBASE0-2+vx*XSIDE ,YBASE0+YSIDE*vy,RGB565(2,31,1), BKGCOLOR ,1, 1,3,MVk[(vy*NPLGN)+vx+page].shift);
	}
	else if (CTRLON==1)
	{
		Draw_text((char*)pix,XBASE0-2+vx*XSIDE ,YBASE0+YSIDE*vy,RGB565(2,31,1), BKGCOLOR ,1, 1,3,MVk[(vy*NPLGN)+vx+page].ctrl);
	}
	else
	{
		Draw_text((char*)pix,XBASE0-2+vx*XSIDE ,YBASE0+YSIDE*vy,RGB565(2,31,1), BKGCOLOR ,1, 1,3,MVk[(vy*NPLGN)+vx+page].norml);
	}
	if (vx==0 && vy==4 && SHIFTON==1) // diferent Shift color if Shift is ON - position 0,4
	{
		Draw_text((char*)pix,XBASE0-2+vx*XSIDE ,YBASE0+YSIDE*vy,RGB565(2,31,21), BKGCOLOR ,1, 1,3,MVk[(vy*NPLGN)+vx+page].shift);
	}
	if (vx==0 && vy==3 && CTRLON==1) // diferent Conrol color if Control is ON - position 0,3
	{
		Draw_text((char*)pix,XBASE0-2+vx*XSIDE ,YBASE0+YSIDE*vy,RGB565(2,31,21), BKGCOLOR ,1, 1,3,MVk[(vy*NPLGN)+vx+page].ctrl);
	}
	
}

int check_vkey2(int x,int y)
{
   int page;
   //check which key is press
   page= (NPAGE==-1) ? 0 : 5*NPLGN;
   return MVk[y*NPLGN+x+page].val;
}

