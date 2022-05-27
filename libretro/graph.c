
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "graph.h"
#include "libretro-core.h"

extern int VIRTUAL_WIDTH;
extern int VKBD_OPACITY;

unsigned Opacity(unsigned color1, unsigned color2){
	
	unsigned r,g,b;
	
	if (VKBD_OPACITY==1){
		r=(R_RGB565(color1)/2)+(R_RGB565(color2)/2);
		g=(G_RGB565(color1)/2)+(G_RGB565(color2)/2);
		b=(B_RGB565(color1)/2)+(B_RGB565(color2)/2);
		return RGB565(r,g,b);
	}
	else
	{
		return color2;
	}
	
}

void DrawFBoxBmp(char  *buffer,int x,int y,int dx,int dy,unsigned   color){
	
	int i,j,idx;
		
			
#if defined PITCH && PITCH == 4
unsigned *mbuffer=(unsigned*)buffer;
#else
unsigned short *mbuffer=(unsigned short *)buffer;
#endif

	for(i=x;i<x+dx;i++){
		for(j=y;j<y+dy;j++){
			
			idx=i+j*VIRTUAL_WIDTH;
			mbuffer[idx]=Opacity(mbuffer[idx],color);	
		}
	}
	
}

void DrawBoxBmp(char  *buffer,int x,int y,int dx,int dy,unsigned    color){
	
	int i,j,idx;
	
			
#if defined PITCH && PITCH == 4
unsigned *mbuffer=(unsigned*)buffer;
#else
unsigned short *mbuffer=(unsigned short *)buffer;
#endif
	
	for(i=x;i<x+dx;i++){
		idx=i+y*VIRTUAL_WIDTH;
		mbuffer[idx]=Opacity(mbuffer[idx],color);
		idx=i+(y+dy)*VIRTUAL_WIDTH;
		mbuffer[idx]=Opacity(mbuffer[idx],color);
	}

	for(j=y;j<y+dy;j++){
			
		idx=x+j*VIRTUAL_WIDTH;
		mbuffer[idx]=Opacity(mbuffer[idx],color);	
		idx=(x+dx)+j*VIRTUAL_WIDTH;
		mbuffer[idx]=Opacity(mbuffer[idx],color);	
	}
	
}

void DrawPointBmp(char  *buffer,int x,int y,unsigned   color){
	
	int idx;
			
#if defined PITCH && PITCH == 4
unsigned *mbuffer=(unsigned*)buffer;
#else
unsigned short *mbuffer=(unsigned short *)buffer;
#endif

		idx=x+y*VIRTUAL_WIDTH;
		mbuffer[idx]=Opacity(mbuffer[idx],color);		
	
}

void DrawHlineBmp(char  *buffer,int x,int y,int dx,int dy,unsigned    color){
	int i,idx;
#if defined PITCH && PITCH == 4
	unsigned *mbuffer=(unsigned*)buffer;
#else
	unsigned short *mbuffer=(unsigned short *)buffer;
#endif
	for(i=x;i<x+dx;i++)
	{
		idx=i+y*VIRTUAL_WIDTH;
		mbuffer[idx]=Opacity(mbuffer[idx],color);		
	}
}

void DrawVlineBmp(char *buffer,int x,int y,int dx,int dy,unsigned    color){
	
	int i,j,idx;
#if defined PITCH && PITCH == 4
	unsigned *mbuffer=(unsigned*)buffer;
#else
	unsigned short *mbuffer=(unsigned short *)buffer;
#endif
	for(j=y;j<y+dy;j++)
	{
		idx=x+j*VIRTUAL_WIDTH;
		mbuffer[idx]=Opacity(mbuffer[idx],color);		
	}	
}

void DrawlineBmp(char  *buffer,int x1,int y1,int x2,int y2,unsigned    color){
	int pixx, pixy;
 	int x, y;
 	int swaptmp;
 	int idx;
#if defined PITCH && PITCH == 4
	unsigned *mbuffer=(unsigned*)buffer;
#else
	unsigned short *mbuffer=(unsigned short *)buffer;
#endif
	int dx = x2 - x1;
	int dy = y2 - y1;
	int sx = (dx >= 0) ? 1 : -1;
	int sy = (dy >= 0) ? 1 : -1;

	if (dx==0) {
 		if (dy>0) {
 			DrawVlineBmp(buffer, x1, y1,0, dy, color);
			return;

 		} else if (dy<0) {
 			DrawVlineBmp(buffer, x1, y2,0, -dy, color);
			return;

 		} else {
			idx=x1+y1*VIRTUAL_WIDTH;
 			mbuffer[idx]=Opacity(mbuffer[idx],color);
			return ;
 		}
 	}
 	if (dy == 0) {
 		if (dx>0) {
 			DrawHlineBmp(buffer, x1, y1, dx, 0, color);
			return;

 		} else if (dx<0) {
 			DrawHlineBmp(buffer, x2, y1, -dx,0, color);
			return;
 		}
 	}

	dx   = sx * dx + 1;
 	dy   = sy * dy + 1;
	
	pixx = 1;
 	pixy = VIRTUAL_WIDTH;

 	pixx *= sx;
 	pixy *= sy;

 	if (dx < dy) {
	 	swaptmp = dx;
	 	dx = dy;
	 	dy = swaptmp;
	 	swaptmp = pixx;
	 	pixx = pixy;
	 	pixy = swaptmp;
 	}

	x = 0;
 	y = 0;

	idx=x1+y1*VIRTUAL_WIDTH;

	for (; x < dx; x++, idx +=pixx) {
		mbuffer[idx]=Opacity(mbuffer[idx],color);
 		y += dy;
 		if (y >= dx) {
 			y -= dx;
 			idx += pixy;
 		}
	}

}

const float DEG2RAD = 3.14159/180;

void DrawCircle(char *buf,int x, int y, int radius,unsigned  rgba,int full)
{ 
	int i;
#if defined PITCH && PITCH == 4
	unsigned *mbuffer=(unsigned*)buf;
#else
	unsigned short *mbuffer=(unsigned short *)buf;
#endif

   	for ( i=0; i < 360; i++)
	{
		float degInRad = i*DEG2RAD;
   		int x1         = x+cos(degInRad)*radius;
		int y1         = y+sin(degInRad)*radius;

		if(full)
			DrawlineBmp(buf,x,y, x1,y1,rgba); 
		else
			mbuffer[x1+y1*VIRTUAL_WIDTH]=rgba;
     	}
    	
}
#ifdef FILTER16X2
//UINT16
void filter_scale2x(unsigned char *srcPtr, unsigned srcPitch, 
                      unsigned char *dstPtr, unsigned dstPitch,
		      int width, int height)
{
	unsigned int nextlineSrc = srcPitch / sizeof(short);
	short *p = (short *)srcPtr;

	unsigned int nextlineDst = dstPitch / sizeof(short);
	short *q = (short *)dstPtr;
  
	while(height--) {
		int i = 0, j = 0;
		for(i = 0; i < width; ++i, j += 2) {
			short B = *(p + i - nextlineSrc);
			short D = *(p + i - 1);
			short E = *(p + i);
			short F = *(p + i + 1);
			short H = *(p + i + nextlineSrc);

			*(q + j) = D == B && B != F && D != H ? D : E;
			*(q + j + 1) = B == F && B != D && F != H ? F : E;
			*(q + j + nextlineDst) = D == H && D != B && H != F ? D : E;
			*(q + j + nextlineDst + 1) = H == F && D != H && B != F ? F : E;
		}
		p += nextlineSrc;
		q += nextlineDst << 1;
	}
}
#endif

#include "font2.i"


void Retro_Draw_string(char *surf, signed short int x, signed short int y, const  char *string,unsigned short maxstrlen,unsigned short xscale, unsigned short yscale, unsigned  fg, unsigned  bg)
{
    	int k,strlen;
    	unsigned char *linesurf;
    	signed  int ypixel;
    	int col, bit;
    	unsigned char b;

    	int xrepeat, yrepeat;

#if defined PITCH && PITCH == 4
   	unsigned  *yptr; 
#elif defined PITCH && PITCH == 2
	unsigned short *yptr; 
#elif defined PITCH && PITCH == 1
	unsigned char *yptr; 
#endif
			
#if defined PITCH && PITCH == 4
unsigned *mbuffer=(unsigned*)surf;
#elif defined PITCH && PITCH == 2
unsigned short *mbuffer=(unsigned short *)surf;
#elif defined PITCH && PITCH == 1
unsigned char *mbuffer=(unsigned char *)surf;
#endif

    	if(string==NULL)return;
		strlen = sizeof(string);
		if (strlen>maxstrlen) strlen = maxstrlen;

	int surfw=strlen * 7 * xscale;
	int surfh=8 * yscale;

#if defined PITCH && PITCH == 4	
        linesurf =(unsigned char *)malloc(sizeof(unsigned )*surfw*surfh );
    	yptr = (unsigned *)&linesurf[0];
#elif defined PITCH && PITCH == 2
        linesurf =(unsigned char *)malloc(sizeof(unsigned short)*surfw*surfh );
    	yptr = (unsigned short *)&linesurf[0];
#elif defined PITCH && PITCH == 1
        linesurf =(unsigned char *)malloc(sizeof(unsigned char)*surfw*surfh );
    	yptr = (unsigned char *)&linesurf[0];
#endif

	for(ypixel = 0; ypixel<8; ypixel++) {

        	for(col=0; col<strlen; col++) {

            		b = font_array[(string[col]^0x80)*8 + ypixel];

            		for(bit=0; bit<7; bit++, yptr++) {              
				*yptr = (b & (1<<(7-bit))) ? fg : bg;
                		for(xrepeat = 1; xrepeat < xscale; xrepeat++, yptr++)
                    			yptr[1] = *yptr;
                        }
        	}

        	for(yrepeat = 1; yrepeat < yscale; yrepeat++) 
            		for(xrepeat = 0; xrepeat<surfw; xrepeat++, yptr++)
                		*yptr = yptr[-surfw];
           
    	}

#if defined PITCH && PITCH == 4	
    	yptr = (unsigned *)&linesurf[0];
#elif defined PITCH && PITCH == 2
    	yptr = (unsigned short*)&linesurf[0];
#elif defined PITCH && PITCH == 1
	yptr = (unsigned char*)&linesurf[0];
#endif

    	for(yrepeat = y; yrepeat < y+ surfh; yrepeat++) 
        	for(xrepeat = x; xrepeat< x+surfw; xrepeat++,yptr++)
             		if(*yptr!=0)mbuffer[xrepeat+yrepeat*VIRTUAL_WIDTH] = Opacity(mbuffer[xrepeat+yrepeat*VIRTUAL_WIDTH],*yptr);

	free(linesurf);

}


void Draw_text(char *buffer,int x,int y,unsigned    fgcol,unsigned   int bgcol ,int scalex,int scaley , int max,const  char *string,...)
{
	int boucle=0;  
   	char text[256];	   	
   	va_list	ap;	

   	if (string == NULL)return;		
		
   	va_start(ap, string);		
      		vsprintf(text, string, ap);	
   	va_end(ap);	

   	Retro_Draw_string(buffer, x,y, (const  char *)text,max, scalex, scaley,fgcol,bgcol);	
}



