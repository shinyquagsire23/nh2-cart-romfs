#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include "draw.h"

void clearScreen(u8* screen,gfxScreen_t screenPos)
{
	int width;
	int height=240;

	switch(screenPos){
	case GFX_BOTTOM:
		width=BOTTOM_WIDTH;
		break;
	default:
		width=TOP_WIDTH;
		break;
	}

	int i, j;
	for(i=1;i<width;i++)
	{
		for(j=1;j<height;j++)
		{
			drawPixel(i,j,0x00,0x00,0x00,screen);
		}
	}
}

void drawPixel(int x, int y, char r, char g, char b, u8* screen)
{
	int height=240;
	
	u32 v=(height-y+x*height)*3;
	screen[v]=b;
	screen[v+1]=g;
	screen[v+2]=r;
}


/*
drawLine - H_line and V_line only
*/

void drawLine( int x1, int y1, int x2, int y2, char r, char g, char b, u8* screen)
{

	int x, y;
	if (x1 == x2){
		if (y1<y2) for (y = y1; y < y2; y++) drawPixel(x1,y,r,g,b,screen);
		else for (y = y2; y < y1; y++) drawPixel(x1,y,r,g,b,screen);
	} else {
		if (x1<x2) for (x = x1; x < x2; x++) drawPixel(x,y1,r,g,b,screen);
		else for (x = x2; x < x1; x++) drawPixel(x,y1,r,g,b,screen);
	}
}

void drawRect( int x1, int y1, int x2, int y2, char r, char g, char b, u8* screen)
{
	drawLine( x1, y1, x2, y1, r, g, b, screen);
	drawLine( x2, y1, x2, y2, r, g, b, screen);
	drawLine( x1, y2, x2, y2, r, g, b, screen);
	drawLine( x1, y1, x1, y2, r, g, b, screen);
}

void drawFillRect( int x1, int y1, int x2, int y2, char r, char g, char b, u8* screen)
{
	int X1,X2,Y1,Y2,i,j;

	if (x1<x2){ 
		X1=x1;
		X2=x2;
	} else { 
		X1=x2;
		X2=x1;
	} 

	if (y1<y2){ 
		Y1=y1;
		Y2=y2;
	} else { 
		Y1=y2;
		Y2=y1;
	} 
	for(i=X1;i<=X2;i++){
		for(j=Y1;j<=Y2;j++){
			drawPixel(i,j, r, g, b, screen);
		}
	}
}

void drawCircle(int xCen, int yCen, int radius, char r, char g, char b, u8* screen)
{
	int x = 0;
	int y = radius;
	int p = (5 - radius*4)/4;
	drawCircleCircum(xCen, yCen, x, y, r, g, b, screen);
	while(x < y){
		x++;
		if(p < 0){
			p += 2*x+1;
		}
		else{
			y--;
			p += 2*(x-y)+1;
		}
		drawCircleCircum(xCen, yCen, x, y, r, g, b, screen);
	}
}

void drawFillCircle(int xCen, int yCen, int radius, char r, char g, char b, u8* screen)
{
	drawCircle(xCen, yCen, radius, r, g, b, screen);
	int x,y;
	for(y = -radius;y <= radius;y++){
		for(x = -radius;x <= radius;x++)
			if(x*x + y*y <= radius*radius + radius * .8f)
				drawPixel(xCen + x, yCen + y, r, g, b, screen);
	}
}

void drawCircleCircum(int cx, int cy, int x, int y, char r, char g, char b, u8* screen)
{

	if(x == 0){
		drawPixel(cx, cy + y, r, g, b, screen);
		drawPixel(cx, cy - y, r, g, b, screen);
		drawPixel(cx + y, cy, r, g, b, screen);
		drawPixel(cx - y, cy, r, g, b, screen);
	}
	if(x == y){
		drawPixel(cx + x, cy + y, r, g, b, screen);
		drawPixel(cx - x, cy + y, r, g, b, screen);
		drawPixel(cx + x, cy - y, r, g, b, screen);
		drawPixel(cx - x, cy - y, r, g, b, screen);
	}
	if(x < y){
		drawPixel(cx + x, cy + y, r, g, b, screen);
		drawPixel(cx - x, cy + y, r, g, b, screen);
		drawPixel(cx + x, cy - y, r, g, b, screen);
		drawPixel(cx - x, cy - y, r, g, b, screen);
		drawPixel(cx + y, cy + x, r, g, b, screen);
		drawPixel(cx - y, cy + x, r, g, b, screen);
		drawPixel(cx + y, cy - x, r, g, b, screen);
		drawPixel(cx - y, cy - x, r, g, b, screen);
	}
}

void gfxDrawSprite(gfxScreen_t screen, gfx3dSide_t side, u8* spriteData, u16 width, u16 height, s16 x, s16 y)
{
	if(!spriteData)return;

	u16 fbWidth, fbHeight;
	u8* fbAdr=gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

	if(x+width<0 || x>=fbWidth)return;
	if(y+height<0 || y>=fbHeight)return;

	u16 xOffset=0, yOffset=0;
	u16 widthDrawn=width, heightDrawn=height;

	if(x<0)xOffset=-x;
	if(y<0)yOffset=-y;
	if(x+width>=fbWidth)widthDrawn=fbWidth-x;
	if(y+height>=fbHeight)heightDrawn=fbHeight-y;
	widthDrawn-=xOffset;
	heightDrawn-=yOffset;

	int j;
	for(j=yOffset; j<yOffset+heightDrawn; j++)
	{
		memcpy(&fbAdr[((x+xOffset)+(y+j)*fbWidth)*3], &spriteData[((xOffset)+(j)*width)*3], widthDrawn*3);
	}
}

void gfxDrawDualSprite(u8* spriteData, u16 width, u16 height, s16 x, s16 y)
{
	if(!spriteData)return;

	gfxDrawSprite(GFX_TOP, GFX_LEFT, spriteData, width, height, x-240, y);
	gfxDrawSprite(GFX_BOTTOM, GFX_LEFT, spriteData, width, height, x, y-40);
}

void gfxDrawSpriteAlpha(gfxScreen_t screen, gfx3dSide_t side, u8* spriteData, u16 width, u16 height, s16 x, s16 y)
{
	if(!spriteData)return;

	u16 fbWidth, fbHeight;
	u8* fbAdr=gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

	if(x+width<0 || x>=fbWidth)return;
	if(y+height<0 || y>=fbHeight)return;

	u16 xOffset=0, yOffset=0;
	u16 widthDrawn=width, heightDrawn=height;

	if(x<0)xOffset=-x;
	if(y<0)yOffset=-y;
	if(x+width>=fbWidth)widthDrawn=fbWidth-x;
	if(y+height>=fbHeight)heightDrawn=fbHeight-y;
	widthDrawn-=xOffset;
	heightDrawn-=yOffset;

	//TODO : optimize
	fbAdr+=(y+yOffset)*fbWidth*3;
	spriteData+=yOffset*width*4;
	int j, i;
	for(j=yOffset; j<yOffset+heightDrawn; j++)
	{
		u8* fbd=&fbAdr[(x+xOffset)*3];
		u8* data=&spriteData[(xOffset)*4];
		for(i=xOffset; i<xOffset+widthDrawn; i++)
		{
			if(data[3])
			{
				fbd[0]=data[0];
				fbd[1]=data[1];
				fbd[2]=data[2];
			}
			fbd+=3;
			data+=4;
		}
		fbAdr+=fbWidth*3;
		spriteData+=width*4;
	}
}
void gfxDrawSpriteAlphaBlend(gfxScreen_t screen, gfx3dSide_t side, u8* spriteData, u16 width, u16 height, s16 x, s16 y)
{
	if(!spriteData)return;

	u16 fbWidth, fbHeight;
	u8* fbAdr=gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

	if(x+width<0 || x>=fbWidth)return;
	if(y+height<0 || y>=fbHeight)return;

	u16 xOffset=0, yOffset=0;
	u16 widthDrawn=width, heightDrawn=height;

	if(x<0)xOffset=-x;
	if(y<0)yOffset=-y;
	if(x+width>=fbWidth)widthDrawn=fbWidth-x;
	if(y+height>=fbHeight)heightDrawn=fbHeight-y;
	widthDrawn-=xOffset;
	heightDrawn-=yOffset;

	//TODO : optimize
	fbAdr+=(y+yOffset)*fbWidth*3;
	spriteData+=yOffset*width*4;
	int j, i;
	for(j=yOffset; j<yOffset+heightDrawn; j++)
	{
		u8* fbd=&fbAdr[(x+xOffset)*3];
		u8* data=&spriteData[(xOffset)*4];
		for(i=xOffset; i<xOffset+widthDrawn; i++)
		{
			if(data[3])
			{
				u8 alphaSource = data[3];
				fbd[0] = ((data[0] * alphaSource)+(fbd[0] * (255 - alphaSource))) / 256;
				fbd[1] = ((data[1] * alphaSource)+(fbd[1] * (255 - alphaSource))) / 256;
				fbd[2] = ((data[2] * alphaSource)+(fbd[2] * (255 - alphaSource))) / 256;
			}
			fbd+=3;
			data+=4;
		}
		fbAdr+=fbWidth*3;
		spriteData+=width*4;
	}
}
void gfxDrawSpriteAlphaBlendFade(gfxScreen_t screen, gfx3dSide_t side, u8* spriteData, u16 width, u16 height, s16 x, s16 y, u8 fadeValue)
{
	if(!spriteData)return;

	u16 fbWidth, fbHeight;
	u8* fbAdr=gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

	if(x+width<0 || x>=fbWidth)return;
	if(y+height<0 || y>=fbHeight)return;

	u16 xOffset=0, yOffset=0;
	u16 widthDrawn=width, heightDrawn=height;

	if(x<0)xOffset=-x;
	if(y<0)yOffset=-y;
	if(x+width>=fbWidth)widthDrawn=fbWidth-x;
	if(y+height>=fbHeight)heightDrawn=fbHeight-y;
	widthDrawn-=xOffset;
	heightDrawn-=yOffset;

	//TODO : optimize
	fbAdr+=(y+yOffset)*fbWidth*3;
	spriteData+=yOffset*width*4;
	int j, i;
	for(j=yOffset; j<yOffset+heightDrawn; j++)
	{
		u8* fbd=&fbAdr[(x+xOffset)*3];
		u8* data=&spriteData[(xOffset)*4];
		for(i=xOffset; i<xOffset+widthDrawn; i++)
		{
			if(data[3])
			{
				u8 alphaSource = (fadeValue * data[3]) / 256;
				fbd[0]=((data[0] * alphaSource) / 256)+((fbd[0] * (255 - alphaSource)) / 256);
				fbd[1]=((data[1] * alphaSource) / 256)+((fbd[1] * (255 - alphaSource)) / 256);
				fbd[2]=((data[2] * alphaSource) / 256)+((fbd[2] * (255 - alphaSource)) / 256);
			}
			fbd+=3;
			data+=4;
		}
		fbAdr+=fbWidth*3;
		spriteData+=width*4;
	}
}

void gfxFadeScreen(gfxScreen_t screen, gfx3dSide_t side, u32 f)
{
	u16 fbWidth, fbHeight;
	u8* fbAdr=gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

	int i; for(i=0; i<fbWidth*fbHeight/2; i++)
	{
		*fbAdr=(*fbAdr*f)>>8;fbAdr++;
		*fbAdr=(*fbAdr*f)>>8;fbAdr++;
		*fbAdr=(*fbAdr*f)>>8;fbAdr++;
		*fbAdr=(*fbAdr*f)>>8;fbAdr++;
		*fbAdr=(*fbAdr*f)>>8;fbAdr++;
		*fbAdr=(*fbAdr*f)>>8;fbAdr++;
	}
}
