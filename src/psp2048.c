/*
* 2048.c : 2048-PSP Clone
*
* Licensed under the GPLv2 license 
*
* clone & port by Anderain Ryu <anderain.develop@gmail.com>
*
* PORTED TO VITA BY DOTS_TB
* 
**  Special thanks to: 
**  TEAM MOLECULE for smelling good and making everything on earth 
**  xepri for vita2dlib
**  
**  wololo for helping me find the Lord 
**  m0skit0 for advance psp tk or whatever the site was
**
**  SMOKE, meetpatty, DrakonPL for code to study(or copy)
**  Rinnegatamante for makefiles to study 
**  minilgos for his notes
**
*/

#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <psp2/kernel/processmgr.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <vita2d.h>
#include <math.h>

#define VERS 1
#define REVS 0

#define MULTIPLIER      2
#define TABLE_WIDTH 	4
#define TABLE_HEIGHT 	4

#define SCORE_DRAW_X_OFFSET 48*MULTIPLIER
#define SCORE_DRAW_Y_OFFSET 44*MULTIPLIER
#define HISCORE_DRAW_X_OFFSET SCORE_DRAW_X_OFFSET
#define HISCORE_DRAW_Y_OFFSET 144*MULTIPLIER

#define NUM_CHAR_WIDTH  16*MULTIPLIER
#define NUM_CHAR_HEIGHT 22*MULTIPLIER

#define NUM_SQR_WIDTH	54*MULTIPLIER
#define NUM_SQR_HEIGHT	54*MULTIPLIER

#define STR_BUFFER_SIZE	64

#define NUM_DRAW_X_OFFSET 226*MULTIPLIER
#define NUM_DRAW_Y_OFFSET 22*MULTIPLIER
#define NUM_DRAW_SPACING 5*MULTIPLIER

#define NUMBER_IMAGE_MAX 11

#define NUM_DRAW_X_POS(x)	(NUM_DRAW_X_OFFSET + (NUM_DRAW_SPACING + NUM_SQR_WIDTH) * (x))
#define NUM_DRAW_Y_POS(y)	(NUM_DRAW_Y_OFFSET + (NUM_DRAW_SPACING + NUM_SQR_HEIGHT) * (y))

#define NEW_POINT_ANI_SIZE 4
#define BLACK   RGBA8(  0,   0,   0, 255)
#define WHITE   RGBA8(255, 255, 255, 255)

#define TOUCH_DEADZONE 75

#define lerp(value, from_max, to_max) ((((value*10) * (to_max*10))/(from_max*10))/10)


extern unsigned char _binary_graphics_0002_png_start;
extern unsigned char _binary_graphics_0004_png_start;
extern unsigned char _binary_graphics_0008_png_start;
extern unsigned char _binary_graphics_0016_png_start;
extern unsigned char _binary_graphics_0032_png_start;
extern unsigned char _binary_graphics_0064_png_start;
extern unsigned char _binary_graphics_0128_png_start;
extern unsigned char _binary_graphics_0256_png_start;
extern unsigned char _binary_graphics_0512_png_start;
extern unsigned char _binary_graphics_1024_png_start;
extern unsigned char _binary_graphics_2048_png_start;
extern unsigned char _binary_graphics_bg_png_start;
extern unsigned char _binary_graphics_b0_png_start;
extern unsigned char _binary_graphics_b1_png_start;
extern unsigned char _binary_graphics_b2_png_start;
extern unsigned char _binary_graphics_b3_png_start;
extern unsigned char _binary_graphics_numtable_png_start;

vita2d_texture * imgBack;
vita2d_texture * imgNum[NUMBER_IMAGE_MAX];
vita2d_texture * imgNumTable;
vita2d_texture * imgNewPointAni[NEW_POINT_ANI_SIZE];

SceCtrlData pad;
SceTouchData touch;
unsigned int touched = 0;
unsigned int touchX = 0;
unsigned int touchY = 0;
int fxTouch;
int fyTouch;

int table[TABLE_HEIGHT][TABLE_WIDTH] = {
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0}
};

int iScore,iHighScore;

/*!
* @brief draw a number to screen
* @param iNum number to draw
* @param x x pos
* @param y y pos
*/

void drawNumber(int iNum,int x,int y) {
	
	char szBuf[STR_BUFFER_SIZE],*p;
	if (iNum < 0)
	return;
	sprintf(p = szBuf,"%d",iNum);
	while (*p) {
		vita2d_draw_texture_part(imgNumTable, x, y, (*p - '0') * NUM_CHAR_WIDTH, 0, NUM_CHAR_WIDTH,NUM_CHAR_HEIGHT);

		x += NUM_CHAR_WIDTH;
		p++;
	}


}
/*!
* @brief load all images
*/
void loadImages () {
	imgNum[0] = vita2d_load_PNG_buffer(&_binary_graphics_0002_png_start);
	imgNum[1] = vita2d_load_PNG_buffer(&_binary_graphics_0004_png_start);
	imgNum[2] = vita2d_load_PNG_buffer(&_binary_graphics_0008_png_start);
	imgNum[3] = vita2d_load_PNG_buffer(&_binary_graphics_0016_png_start);
	imgNum[4] = vita2d_load_PNG_buffer(&_binary_graphics_0032_png_start);
	imgNum[5] = vita2d_load_PNG_buffer(&_binary_graphics_0064_png_start);
	imgNum[6] = vita2d_load_PNG_buffer(&_binary_graphics_0128_png_start);
	imgNum[7] = vita2d_load_PNG_buffer(&_binary_graphics_0256_png_start);
	imgNum[8] = vita2d_load_PNG_buffer(&_binary_graphics_0512_png_start);
	imgNum[9] = vita2d_load_PNG_buffer(&_binary_graphics_1024_png_start);
	imgNum[10] = vita2d_load_PNG_buffer(&_binary_graphics_2048_png_start);

	imgNewPointAni[0] = vita2d_load_PNG_buffer(&_binary_graphics_b0_png_start);
	imgNewPointAni[1] = vita2d_load_PNG_buffer(&_binary_graphics_b1_png_start);
	imgNewPointAni[2] = vita2d_load_PNG_buffer(&_binary_graphics_b2_png_start);
	imgNewPointAni[3] = vita2d_load_PNG_buffer(&_binary_graphics_b3_png_start);

	imgBack = vita2d_load_PNG_buffer(&_binary_graphics_bg_png_start);
	imgNumTable = vita2d_load_PNG_buffer(&_binary_graphics_numtable_png_start);
}
/*!
* @brief redraw screen content
* @param ix ingore square x pos 
* @param iy ingore square y pos
*/
void redrawScreen (int ix,int iy) {

	int y,x;
	vita2d_draw_texture(imgBack, 0, 0);
	drawNumber(iScore,SCORE_DRAW_X_OFFSET,SCORE_DRAW_Y_OFFSET);
	drawNumber(iHighScore,HISCORE_DRAW_X_OFFSET,HISCORE_DRAW_Y_OFFSET);
	
	for (y=0;y<TABLE_HEIGHT;++y) {
		for (x=0;x<TABLE_WIDTH;++x) {
			int index = 0,t,rx,ry;
			if (x==ix && y==iy)
			continue;
			t = table[y][x];
			if (t == 0)
			continue;
			while (t > 0) {
				t >>= 1;
				index ++;
			}
			index--,index--;
			rx = NUM_DRAW_X_POS(x); 
			ry = NUM_DRAW_Y_POS(y);
			vita2d_draw_texture(imgNum[index], rx, ry);

		}
	}

	
}
/*!
* @brief refresh screen 
*/
void refreshScreen (int x, int y) {
	vita2d_start_drawing();
	vita2d_clear_screen();
	redrawScreen(x,y);

}
void refreshScreen2() {
	vita2d_end_drawing();
	vita2d_swap_buffers();
}
/*!
* @brief check table is full
* @return 1 if table is full
*/
int tableFull () {
	int y,x,f = 0;
	for (y=0;y<TABLE_HEIGHT;++y)
	for (x=0;x<TABLE_WIDTH;++x)
	if (table[y][x] == 0)
	f++;
	return f <= 0;
}
/*!
* @biref play a animation new point
* @param ox  x pos 
* @param oy  y pos
*/
void playNewPoint(int ox,int oy) {
	int y;
	
	int rx = NUM_DRAW_X_POS(ox);
	int ry = NUM_DRAW_Y_POS(oy);
	
	for (y=0;y<NEW_POINT_ANI_SIZE;++y) {

		refreshScreen(-1,-1);
		vita2d_draw_texture(imgNewPointAni[y], rx, ry);
		refreshScreen2();
		sceKernelDelayThread(10000);
	}
}
/*!
* @brief set a new numer '2' at random
* @return 1 if table is full
*/
int newPoint () {
	int r;
	if (tableFull())
	return 1;
	r = (rand() % 100 < 10) ? 4 : 2;
	while (1) {
		int x,y;
		x = rand() % TABLE_WIDTH;
		y = rand() % TABLE_HEIGHT;
		if (table[y][x] == 0) {
			table[y][x] = r;
			playNewPoint(x,y);
			break;
		}
	}
	return 0;
}
/*!
* @biref add score
*/
void addScore (int a) {
	iScore += a;
	if (iHighScore < iScore)
	iHighScore = iScore;
}
/*!
* @biref play a animation of move square
* @param ox original x
* @param oy original y
* @param dx dest x
* @param dy dest y
*/
void playMove(int ox,int oy,int dx,int dy) {
	int frames;
	int y,rx,ry,stepX,stepY,t,index;
	
	if (abs(ox-dy)>=3 || (oy-dy)>=3) 		frames = 15;
	else if (abs(ox-dy)>=2 || (oy-dy)>=2) 	frames = 10;
	else frames = 5;
	
	rx = NUM_DRAW_X_POS(ox);
	ry = NUM_DRAW_Y_POS(oy);
	stepX = (NUM_DRAW_X_POS(dx) - rx) / frames;
	stepY = (NUM_DRAW_Y_POS(dy) - ry) / frames;
	t = table[oy][ox];
	index = 0;
	
	if (t == 0)
	return;
	while (t > 0) {
		t >>= 1;
		index ++;
	}
	index--,index--;
	for (y=0;y<frames;++y,rx+=stepX,ry+=stepY) {

		refreshScreen(ox,oy);
		
		vita2d_draw_texture(imgNum[index], rx, ry);
		refreshScreen2();
	}
}
/*!
* @brief move left
*/
void moveLeft () {
	int y,x,k;
	for (y=0;y<TABLE_HEIGHT;++y) {
		for (x=1;x<TABLE_WIDTH;++x) {
			int done = 0;
			if (table[y][x] == 0)
			continue;
			for (k=x-1;k>=0;--k) {
				if (table[y][k] == 0)
				continue;
				if (table[y][k] == table[y][x]) {
					playMove(x,y,k,y);
					addScore(table[y][k] += table[y][x]);
					table[y][x] = 0;
					done = 1;
					break;
				}
				else {
					break;
				}
			}
			if (!done) {
				k++;
				if (k != x) {
					playMove(x,y,k,y);
					table[y][k] = table[y][x];
					table[y][x] = 0;
				}
			}
			
		}
	}
}
/*!
* @brief move up
*/
void moveUp () {
	int y,x,k;
	for (x=0;x<TABLE_WIDTH;++x) {
		for (y=1;y<TABLE_HEIGHT;++y) {
			int done = 0;
			if (table[y][x] == 0)
			continue;
			for (k=y-1;k>=0;--k) {
				if (table[k][x] == 0)
				continue;
				if (table[k][x] == table[y][x]) {
					playMove(x,y,x,k);
					addScore(table[k][x] += table[y][x]);
					table[y][x] = 0;
					done = 1;
					break;
				}
				else {
					break;
				}
			}
			if (!done) {
				k++;
				if (k != y) {
					playMove(x,y,x,k);
					table[k][x] = table[y][x];
					table[y][x] = 0;
					done = 1;
				}
			}
		}
	}
}
/*!
* @brief move down
*/
void moveDown () {
	int y,x,k;
	for (x=0;x<TABLE_WIDTH;++x) {
		for (y=TABLE_HEIGHT-2;y>=0;--y) {
			int done = 0;
			if (table[y][x] == 0)
			continue;
			for (k=y+1;k<TABLE_HEIGHT;++k) {
				if (table[k][x] == 0)
				continue;
				if (table[k][x] == table[y][x]) {
					playMove(x,y,x,k);
					addScore(table[k][x] += table[y][x]);
					table[y][x] = 0;
					done = 1;
					break;
				}
				else {
					break;
				}
			}
			if (!done) {
				k--;
				if (k != y) {
					playMove(x,y,x,k);
					table[k][x] = table[y][x];
					table[y][x] = 0;
				}
			}
			
		}
	}
}
/*!
* @brief move right
*/
void moveRight () {
	int y,x,k;
	for (y=0;y<TABLE_HEIGHT;++y) {
		for (x=TABLE_WIDTH-2;x>=0;--x) {
			int done = 0;
			if (table[y][x] == 0)
			continue;
			for (k=x+1;k<TABLE_WIDTH;++k) {
				if (table[y][k] == 0)
				continue;
				if (table[y][k] == table[y][x]) {
					playMove(x,y,k,y);
					addScore(table[y][k] += table[y][x]);
					table[y][x] = 0;
					done = 1;
					break;
				}
				else {
					break;
				}
			}
			if (!done) {
				k--;
				if (k != x) {
					playMove(x,y,k,y);
					table[y][k] = table[y][x];
					table[y][x] = 0;
				}
			}
			
		}
	}
}
/*!
* @biref entry point of game
*/
int main() {
	unsigned int refresh  = 1;
	vita2d_init();
	vita2d_set_clear_color(BLACK);
	memset(&pad, 0, sizeof(pad));
	loadImages();
	srand(time(NULL));
	iScore = 0;
	iHighScore = 0;
	newPoint();
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
	while(1) {
		sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, 1);
		sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);
		if (touch.reportNum > 0)
		{
			fxTouch = (lerp(touch.report[0].x, 1920, 960));
			fyTouch = (lerp(touch.report[0].y, 1088, 544));

			if(!touched) {
				touched = 1;
				touchX = fxTouch;
				touchY = fyTouch;
			}
			
		}  else {
			if(touched){
				
				int touchDiffX = fxTouch-touchX;
				int touchDiffY = fyTouch-touchY;
				if(abs(touchDiffX) >TOUCH_DEADZONE||abs(touchDiffY)>TOUCH_DEADZONE){
					if(abs(touchDiffX)>abs(touchDiffY)) {
						if(touchDiffX<0) {
							moveLeft();
							newPoint();
							refresh = 1;
							touched = 0;
						} else {
							moveRight();
							newPoint();
							refresh = 1;
							touched = 0;
						}
					} else {
						if(touchDiffY>0) {
							moveDown();
							newPoint();
							refresh = 1;
							touched = 0;
						} else {
							moveUp();
							newPoint();
							refresh = 1;
							touched = 0;
						}
					}
					
				}
			}
		}
		if (refresh) {	
			refreshScreen(-1,-1);
			refreshScreen2();
			refresh = 0;
		}
		sceCtrlPeekBufferPositive(0, &pad, 1);
		if(pad.buttons & SCE_CTRL_UP) {
			moveUp();
			newPoint();
			refresh = 1;
		}
		if(pad.buttons & SCE_CTRL_DOWN) {
			moveDown();
			newPoint();
			refresh = 1;
		}
		if(pad.buttons & SCE_CTRL_LEFT) {
			moveLeft();
			newPoint();
			refresh = 1;
		}
		if(pad.buttons & SCE_CTRL_RIGHT) {
			moveRight();
			newPoint();
			refresh = 1;
		}
		if (pad.buttons & SCE_CTRL_START) {
			break;
		}
		if (pad.buttons & SCE_CTRL_SELECT) {
			iScore = 0;
			memset(table,0,sizeof(table));
			newPoint();
			refresh = 1;
			
		}	
	}
	vita2d_fini();
	int y = 0;
	for (y=0;y<=10;++y) {
		vita2d_free_texture(imgNum[y]);
	}
	for (y=0;y < NEW_POINT_ANI_SIZE;++y) {
		vita2d_free_texture(imgNewPointAni[y]);
	}
	vita2d_free_texture(imgBack);
	vita2d_free_texture(imgNumTable);

	sceKernelExitProcess(0);
	return 0;
}
