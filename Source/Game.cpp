#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <algorithm>
#include <windows.h>
#include <windowsx.h>
#include "Game.h"


//TODO - set up const

/*Piece types: */
/*-1 = empty*/
/*0 = 2x2 square*/
/*1 = 4x1 long*/
/*2 = T */
/*3 = L */
/*4 = backwards L*/
/*5 = _|-*/
/*6 = -|_*/

/*Graphics*/
uint64_t frame_width_px;
uint64_t frame_height_px;
uint64_t frame_Size;
unsigned char *level_array;
unsigned char *frame_array;
unsigned char *DIGITS_arr;
unsigned char *SCORE_arr;
unsigned char *PNEXT_arr;
HBITMAP BM_Main_picture = NULL;
unsigned char LVL_BG_col[3]={0,0,0};
unsigned char MENU_flash_col[3];
unsigned char MENU_BG_col[3]={64,64,64};
unsigned char MENU_edge_col[3]={0,102,204};
unsigned char BORDER_col[3]={102,102,0};
/*Fill Piece*/
unsigned char block_primary_col[21]={
	0,128,255,
	255,128,0,
	0,255,128,
	0,0,255,
	255,0,255,
	202,155,171,
	0,170,170};
/*Border of Piece*/
unsigned char block_secondary_col[21]={
	0,108,217,
	217,108,0,
	0,217,108,
	0,0,160,
	128,0,128,
	130,100,110,
	0,128,128};


/*Level*/
struct LVL {
	char type;
	/*Player 0,1; -1 - no player*/
	char owner;
};
LVL *LVL_squares;
uint64_t level_width_px;
uint64_t level_height_px;
BOOL game_running=false;
/*Counter-clockwise rotation is variation ++*/
uint64_t Piece_variations[7]={1,2,4,4,4,2,2};
uint64_t Piece_starts[7]={0,4,12,28,44,60,68};
int64_t Piece_X[76]={0,0,1,1, -1,0,1,2, 0,0,0,0, -1,0,1,0, 0,0,0,1,-1,0,1, 0, 0,0,0,-1, -1,0,1,-1, 0,0,0,1,-1,0,1, 1, 0,0,0,-1, -1,0,1,1, 0,0,0, 1,-1,0,1,-1, 0,0,0,-1, -1,0,0,1, 0,0,1,1, -1,0,0,1,0,0,1, 1};
int64_t Piece_Y[76]={0,1,0,1,  0,0,0,0,-1,0,1,2,  0,0,0,1,-1,0,1,0, 0,0,0,-1,-1,0,1, 0,  0,0,0, 1,-1,0,1,1, 0,0,0,-1,-1,0,1,-1,  0,0,0,1,-1,0,1,-1, 0,0,0,-1,-1,0,1, 1,  1,1,0,0,-1,0,0,1,  0,0,1,1,1,0,0,-1};


/*Player assets*/
/*0 - top going down*/
/*1 - bottom going up*/
uint64_t Piece_X_anchor[2];
uint64_t Piece_Y_anchor[2];
uint64_t Piece_orientation[2];
int Piece_current[2]={0,0};
int Piece_next[2]={0,0};
int i_Player_score=0;
char str_Player_score[8];
uint64_t Score_anchor_X=380;
uint64_t Score_anchor_Y=320;
uint64_t Score_label_anchor_X=380;
uint64_t Score_label_anchor_Y=280;
uint64_t PNext_label_anchor_X[2]={380,380};
uint64_t PNext_label_anchor_Y[2]={110,410};
uint64_t PNext_num_anchor_X[2]={445,445};
uint64_t PNext_num_anchor_Y[2]={110,410};
uint64_t PNext_anchor_X[2]={385,385};
uint64_t PNext_anchor_Y[2]={155,455};


/*Menu*/
BOOL menu_active=true;
/*Menu buttons*/
/*0 - Continue*/
/*1 - New Game*/
int MENU_Bn_active=1;
int MENU_frame_num=0;
BOOL Bn_restart_possible;
uint64_t MENU_BN_Size;
unsigned char *MENU_array;
unsigned char *MENU_BN_arr;
unsigned char *MENU_Inst_arr;
uint64_t MENU_BN_anchor_X[2]={120,290};
uint64_t MENU_BN_anchor_Y[2]={550,550};
uint64_t MENU_Inst_anchor_X=5;
uint64_t MENU_Inst_anchor_Y=5;


/*Timing*/
LARGE_INTEGER TIME_current;
LARGE_INTEGER TIME_last;
LARGE_INTEGER TIME_wait;
static const uint64_t TIME_s_to_PC=10000000;	//QueryPerformanceCounter ticks per second
uint64_t TIME_target_frame=166666;		//QueryPerformanceCounter ticks per frame
uint64_t TIME_remaining;
uint64_t current_period;
uint64_t frames_in_game;
uint64_t frames_since_move;


void Create_Border(unsigned char *array)
{
	uint64_t idx_x, idx_y, idx, diag=0;
	bool diag_in=true;
	for(idx_y=PAD_px_up;idx_y<frame_height_px-PAD_px_down;idx_y++)
		{
		/*2 lines down left side*/
		idx=NBYTE_PX*(idx_y*frame_width_px+PAD_px_left);
		memcpy(array+idx,BORDER_col,3);
		idx=NBYTE_PX*(idx_y*frame_width_px+PAD_px_left+1+SIZE_BORDER);
		memcpy(array+idx,BORDER_col,3);
		/*2 lines down right side*/
		idx=NBYTE_PX*(idx_y*frame_width_px+PAD_px_left+2+SIZE_BORDER+level_width_px);
		memcpy(array+idx,BORDER_col,3);
		idx=NBYTE_PX*(idx_y*frame_width_px+PAD_px_left+3+SIZE_BORDER*2+level_width_px);
		memcpy(array+idx,BORDER_col,3);
		/*Diagonal zigzag*/
		idx=NBYTE_PX*(idx_y*frame_width_px+PAD_px_left+diag);
		memcpy(array+idx,BORDER_col,3);
		idx=NBYTE_PX*(idx_y*frame_width_px+PAD_px_left+3+SIZE_BORDER*2+level_width_px-diag);
		memcpy(array+idx,BORDER_col,3);
		if (diag_in)
			{
			if(diag>=1+SIZE_BORDER)
				{
				diag_in=false;
				diag--;
				}
			else
				{
				diag++;
				}
			}
		else
			{
			if(diag<=0)
				{
				diag_in=true;
				diag++;
				}
			else
				{
				diag--;
				}
			}
		}
}


void Menu_Create()
{
	uint64_t idx_x, idx_y, idx, idx2;
	/*Fill middle and create border*/
	for(idx=0;idx<MENU_width_px*MENU_height_px;idx++)
		{
		idx2=NBYTE_PX*idx;
		memcpy(MENU_array+idx2,MENU_BG_col,3);
		MENU_array[idx2+3]=255;
		}

	for(idx_x=0;idx_x<MENU_width_px;idx_x++)
		{
		idx=NBYTE_PX*idx_x;
		memcpy(MENU_array+idx,MENU_edge_col,3);
		}
	for(idx_y=1;idx_y<MENU_height_px-1;idx_y++)
		{
		idx=NBYTE_PX*(idx_y*MENU_width_px);
		memcpy(MENU_array+idx,MENU_edge_col,3);
		idx=NBYTE_PX*((idx_y+1)*MENU_width_px-1);
		memcpy(MENU_array+idx,MENU_edge_col,3);
		}
	for(idx_x=0;idx_x<MENU_width_px;idx_x++)
		{
		idx=NBYTE_PX*((MENU_height_px-1)*MENU_width_px+idx_x);
		memcpy(MENU_array+idx,MENU_edge_col,3);
		}
	/*Put in instructions*/
	for(idx_y=0;idx_y<MENU_Inst_Height;idx_y++)
		{
		idx=NBYTE_PX*((idx_y+MENU_Inst_anchor_Y)*MENU_width_px+MENU_Inst_anchor_X);
		memcpy(MENU_array+idx,MENU_Inst_arr+idx_y*NBYTE_PX*MENU_Inst_Width,NBYTE_PX*MENU_Inst_Width);
		}
	/*Put in buttons*/
	/*Cancel (greyed out at the start)*/
	for(idx_y=0;idx_y<MENU_BN_Height;idx_y++)
		{
		idx=NBYTE_PX*((idx_y+MENU_BN_anchor_Y[0])*MENU_width_px+MENU_BN_anchor_X[0]);
		memcpy(MENU_array+idx,MENU_BN_arr+idx_y*NBYTE_PX*MENU_BN_Width+MENU_BN_Size*4,NBYTE_PX*MENU_BN_Width);
		}
	/*New game (active at the start)*/
	for(idx_y=0;idx_y<MENU_BN_Height;idx_y++)
		{
		idx=NBYTE_PX*((idx_y+MENU_BN_anchor_Y[1])*MENU_width_px+MENU_BN_anchor_X[1]);
		memcpy(MENU_array+idx,MENU_BN_arr+idx_y*NBYTE_PX*MENU_BN_Width,NBYTE_PX*MENU_BN_Width);
		}
}


void Menu_Set_Buttons(int *states)
{
	uint64_t idx_y, idx, offset;
	/*Cancel*/
	if (states[0]==0)//Selected
		{
		offset=2;
		}
	else if (states[0]==1)//Deselected
		{
		offset=3;
		}
	else //Greyed out
		{
		offset=4;
		}
	for(idx_y=0;idx_y<MENU_BN_Height;idx_y++)
		{
		idx=NBYTE_PX*((idx_y+MENU_BN_anchor_Y[0])*MENU_width_px+MENU_BN_anchor_X[0]);
		memcpy(MENU_array+idx,MENU_BN_arr+idx_y*NBYTE_PX*MENU_BN_Width+MENU_BN_Size*offset,NBYTE_PX*MENU_BN_Width);
		}
	/*New Game*/
	if (states[1]==0)//Selected
		{
		offset=0;
		}
	else //Deselected
		{
		offset=1;
		}
	for(idx_y=0;idx_y<MENU_BN_Height;idx_y++)
		{
		idx=NBYTE_PX*((idx_y+MENU_BN_anchor_Y[1])*MENU_width_px+MENU_BN_anchor_X[1]);
		memcpy(MENU_array+idx,MENU_BN_arr+idx_y*NBYTE_PX*MENU_BN_Width+MENU_BN_Size*offset,NBYTE_PX*MENU_BN_Width);
		}
}


void Menu_Add()
{
	uint64_t idx_y, idx;
	/*Copy level pixels to frame pixels*/
	for(idx_y=0;idx_y<MENU_height_px;idx_y++)
		{
		idx=NBYTE_PX*((idx_y+MENU_anchor_Y)*frame_width_px+MENU_anchor_X);
		memcpy(frame_array+idx,MENU_array+idx_y*NBYTE_PX*MENU_width_px,NBYTE_PX*MENU_width_px);
		}
}


void Draw_Hollow_Rectangle(uint64_t TLC_x, uint64_t TLC_y, uint64_t BRC_x, uint64_t BRC_y, unsigned char *array, uint64_t array_width, uint64_t array_height, unsigned char *colors)
{
	uint64_t width=BRC_x-TLC_x+1, height=BRC_y-TLC_y+1, idx_x, idx_y, idx;
	for(idx_x=0;idx_x<width;idx_x++)
		{
		idx=NBYTE_PX*(TLC_y*array_width+idx_x+TLC_x);
		memcpy(array+idx,colors,3);
		idx=NBYTE_PX*(BRC_y*array_width+idx_x+TLC_x);
		memcpy(array+idx,colors,3);
		}

	for(idx_y=1;idx_y<height-1;idx_y++)
		{
		idx=NBYTE_PX*((idx_y+TLC_y)*array_width+TLC_x);
		memcpy(array+idx,colors,3);
		idx=NBYTE_PX*((idx_y+TLC_y)*array_width+BRC_x);
		memcpy(array+idx,colors,3);
		}
}


void Draw_Piece_Rectangle(int Piece_type, uint64_t anchor_X, uint64_t anchor_Y, uint64_t X, uint64_t Y)
{
	uint64_t idx_x, idx_y, idx, anchor_X_actual, anchor_Y_actual, PT3;
	anchor_X_actual=anchor_X+(SIZE_SQUARE+2)*X;
	anchor_Y_actual=anchor_Y+(SIZE_SQUARE+2)*Y;
	if (Piece_type<0)
		{
		for (idx_y=0;idx_y<SIZE_SQUARE+2;idx_y++)
			{
			for (idx_x=0;idx_x<SIZE_SQUARE+2;idx_x++)
				{
				idx=NBYTE_PX*((idx_y+anchor_Y_actual)*frame_width_px+anchor_X_actual+idx_x);
				memcpy(level_array+idx,LVL_BG_col,3);
				}
			}
		}
	else
		{
		PT3=3*Piece_type;
		for (idx_x=0;idx_x<SIZE_SQUARE+2;idx_x++)
			{
			idx=NBYTE_PX*(anchor_Y_actual*frame_width_px+anchor_X_actual+idx_x);
			memcpy(level_array+idx,block_secondary_col+PT3,3);
			idx=NBYTE_PX*((anchor_Y_actual+SIZE_SQUARE+1)*frame_width_px+anchor_X_actual+idx_x);
			memcpy(level_array+idx,block_secondary_col+PT3,3);
			}
		for (idx_y=1;idx_y<SIZE_SQUARE+1;idx_y++)
			{				
			idx=NBYTE_PX*((anchor_Y_actual+idx_y)*frame_width_px+anchor_X_actual);
			memcpy(level_array+idx,block_secondary_col+PT3,3);
			idx=NBYTE_PX*((anchor_Y_actual+idx_y)*frame_width_px+anchor_X_actual+SIZE_SQUARE+1);
			memcpy(level_array+idx,block_secondary_col+PT3,3);
			}
		for (idx_y=1;idx_y<SIZE_SQUARE+1;idx_y++)
			{
			for (idx_x=1;idx_x<SIZE_SQUARE+1;idx_x++)
				{
				idx=NBYTE_PX*((anchor_Y_actual+idx_y)*frame_width_px+anchor_X_actual+idx_x);
				memcpy(level_array+idx,block_primary_col+PT3,3);
				}
			}
		}
}


int score_function(uint64_t lines_removed)
{
	return 10*(lines_removed+1)*lines_removed/2;
}


void Draw_Score()
{
	uint64_t idx, idx2, idx_d, idx_y;
	sprintf(str_Player_score,"%08d",i_Player_score);
	for (idx_d=0;idx_d<8;idx_d++)
		{
		for (idx_y=0;idx_y<DIGIT_HEIGHT;idx_y++)
			{
			idx=NBYTE_PX*((Score_anchor_Y+idx_y)*frame_width_px+Score_anchor_X+idx_d*DIGIT_WIDTH);
			idx2=std::max(std::min(static_cast<uint64_t>(str_Player_score[idx_d]-48),(uint64_t)9),(uint64_t)0);
			memcpy(level_array+idx,DIGITS_arr+NBYTE_PX*(idx_y*DIGIT_WIDTH*10+idx2*DIGIT_WIDTH),NBYTE_PX*DIGIT_WIDTH);
			}
		}
}


void Render_frame(HWND hwnd)
{
	/*Update bitmap*/
	memcpy(frame_array,level_array,frame_Size);
	if (menu_active)
		{
		if (MENU_frame_num<60)
			{
			MENU_flash_col[0]=MENU_frame_num*4;
			MENU_flash_col[1]=MENU_frame_num*2;
			MENU_flash_col[2]=0;
			}
		else
			{
			MENU_flash_col[0]=(120-MENU_frame_num)*4;
			MENU_flash_col[1]=(120-MENU_frame_num)*2;
			MENU_flash_col[2]=0;
			}
		Draw_Hollow_Rectangle(MENU_BN_anchor_X[MENU_Bn_active]-5, MENU_BN_anchor_Y[MENU_Bn_active]-5, MENU_BN_anchor_X[MENU_Bn_active]+MENU_BN_Width+4, MENU_BN_anchor_Y[MENU_Bn_active]+MENU_BN_Height+4, MENU_array, MENU_width_px, MENU_height_px, MENU_flash_col);
		Menu_Add();
		MENU_frame_num=(MENU_frame_num+1) % 120;
		}

	/*Do bit-block transfer*/
	//TODO check if optimal use of BitBlt
	SetBitmapBits(BM_Main_picture,frame_Size,frame_array);
	HDC hdc = GetDC(hwnd);
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, BM_Main_picture);
	BitBlt(hdc, 0, 0, frame_width_px, frame_height_px, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);
	ReleaseDC(hwnd, hdc);
	
}


Reset_LVL()
{
	uint64_t idx, idx_x, idx_y;
	for(idx=0;idx<LVL_WIDTH*LVL_HEIGHT;idx++)
		{
		LVL_squares[idx].type=-1;
		LVL_squares[idx].owner=-1;
		}

	for (idx_y=0;idx_y<(SIZE_SQUARE+2)*LVL_HEIGHT;idx_y++)
		{
		for (idx_x=0;idx_x<(SIZE_SQUARE+2)*LVL_WIDTH;idx_x++)
			{
			idx=NBYTE_PX*((PAD_px_up+idx_y)*frame_width_px+PAD_px_left+SIZE_BORDER+2+idx_x);
			memcpy(level_array+idx,LVL_BG_col,3);
			}
		}
}


BOOL Check_Piece_Escaping(uint64_t Player_num)
{
	BOOL Piece_Escaped=false;
	uint64_t idx=0, Piece_Yc;
	if (Player_num==0)
		{
		while(!Piece_Escaped && idx<4)
			{
			Piece_Yc=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
			if(Piece_Yc>=LVL_HEIGHT-1)
				{
				Piece_Escaped=true;
				}
			idx++;
			}
		}
	else
		{
		while(!Piece_Escaped && idx<4)
			{
			Piece_Yc=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
			if(Piece_Yc<=0)
				{
				Piece_Escaped=true;
				}
			idx++;
			}
		}
	return Piece_Escaped;
}


/*Directions:*/
/*0 - down*/
/*1 - right*/
/*2 - up*/
/*3 - left*/
BOOL Check_Piece_Collision(uint64_t Player_num, uint64_t Direction)
{
	BOOL Piece_Collided=false;
	uint64_t idx=0, idx2, Piece_Xc, Piece_Yc, Piece_Xc2, Piece_Yc2;
	if (Direction==0) //Down
		{
		while(!Piece_Collided && idx<4)
			{
			Piece_Xc=Piece_X_anchor[Player_num]+Piece_X[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
			Piece_Yc=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
			/*Do not return true if piece is moving down out of the level - this should be handled by Check_Piece_Escaping()*/
			if(Piece_Yc+1<LVL_HEIGHT && LVL_squares[(Piece_Yc+1)*LVL_WIDTH+Piece_Xc].type>=0)
				{
				Piece_Collided=true;
				}
			for(idx2=0;idx2<4;idx2++)
				{
				Piece_Xc2=Piece_X_anchor[Player_num]+Piece_X[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx2];
				Piece_Yc2=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx2];
				if(idx2!=idx && Piece_Yc+1==Piece_Yc2 && Piece_Xc==Piece_Xc2)
					{
					Piece_Collided=false;
					}
				}
			idx++;
			}
		}
	else if (Direction==1) //Right
		{
		while(!Piece_Collided && idx<4)
			{
			Piece_Xc=Piece_X_anchor[Player_num]+Piece_X[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
			Piece_Yc=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
			if(Piece_Xc+1>=LVL_WIDTH)
				{
				Piece_Collided=true;
				}
			else if(LVL_squares[(Piece_Yc)*LVL_WIDTH+Piece_Xc+1].type>=0)
				{
				Piece_Collided=true;
				for(idx2=0;idx2<4;idx2++)
					{
					Piece_Xc2=Piece_X_anchor[Player_num]+Piece_X[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx2];
					Piece_Yc2=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx2];
					if(idx2!=idx && Piece_Yc==Piece_Yc2 && Piece_Xc+1==Piece_Xc2)
						{
						Piece_Collided=false;
						}
					}
				}
			idx++;
			}
		}
	else if (Direction==2) //Up
		{
		while(!Piece_Collided && idx<4)
			{
			Piece_Xc=Piece_X_anchor[Player_num]+Piece_X[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
			Piece_Yc=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
			/*Do not return true if piece is moving up out of the level - this should be handled by Check_Piece_Escaping()*/
			if(Piece_Yc>0 && LVL_squares[(Piece_Yc-1)*LVL_WIDTH+Piece_Xc].type>=0)
				{
				Piece_Collided=true;
				}
			for(idx2=0;idx2<4;idx2++)
				{
				Piece_Xc2=Piece_X_anchor[Player_num]+Piece_X[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx2];
				Piece_Yc2=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx2];
				if(idx2!=idx && Piece_Yc-1==Piece_Yc2 && Piece_Xc==Piece_Xc2)
					{
					Piece_Collided=false;
					}
				}
			idx++;
			}
		}
	else //Left
		{
		while(!Piece_Collided && idx<4)
			{
			Piece_Xc=Piece_X_anchor[Player_num]+Piece_X[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
			Piece_Yc=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
			if(Piece_Xc<=0)
				{
				Piece_Collided=true;
				}
			else if(LVL_squares[(Piece_Yc)*LVL_WIDTH+Piece_Xc-1].type>=0)
				{
				Piece_Collided=true;
				for(idx2=0;idx2<4;idx2++)
					{
					Piece_Xc2=Piece_X_anchor[Player_num]+Piece_X[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx2];
					Piece_Yc2=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx2];
					if(idx2!=idx && Piece_Yc==Piece_Yc2 && Piece_Xc-1==Piece_Xc2)
						{
						Piece_Collided=false;
						}
					}
				}
			idx++;
			}
		}
	return Piece_Collided;
}


void Place_Piece(uint64_t Player_num)
{
	uint64_t idx, Piece_Xc, Piece_Yc;
	for(idx=0;idx<4;idx++)
		{
		Piece_Xc=Piece_X_anchor[Player_num]+Piece_X[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
		Piece_Yc=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
		Draw_Piece_Rectangle(Piece_current[Player_num], PAD_px_left+SIZE_BORDER+2, PAD_px_up, Piece_Xc, Piece_Yc);
		LVL_squares[Piece_Yc*LVL_WIDTH+Piece_Xc].type=Piece_current[Player_num];
		LVL_squares[Piece_Yc*LVL_WIDTH+Piece_Xc].owner=Player_num;
		}
}


void Remove_Piece(uint64_t Player_num)
{
	uint64_t idx, Piece_Xc, Piece_Yc;
	for(idx=0;idx<4;idx++)
		{
		Piece_Xc=Piece_X_anchor[Player_num]+Piece_X[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
		Piece_Yc=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
		Draw_Piece_Rectangle(-1, PAD_px_left+SIZE_BORDER+2, PAD_px_up, Piece_Xc, Piece_Yc);
		LVL_squares[Piece_Yc*LVL_WIDTH+Piece_Xc].type=-1;
		LVL_squares[Piece_Yc*LVL_WIDTH+Piece_Xc].owner=-1;
		}
}


void Rotate_Piece(uint64_t Player_num, BOOL counter_clockwise)
{
	uint64_t New_orientation, idx, Piece_Xc, Piece_Yc;
	BOOL rotation_possible=true;
	if(counter_clockwise)
		{
		New_orientation=(Piece_orientation[Player_num]+1) % Piece_variations[Piece_current[Player_num]];
		}
	else
		{
		if(Piece_orientation[Player_num]==0)
			{
			New_orientation=Piece_variations[Piece_current[Player_num]]-1;
			}
		else
			{
			New_orientation=Piece_orientation[Player_num]-1;
			}
		}
	/*Try rotation around anchor*/
	for(idx=0;idx<4;idx++)
		{
		Piece_Xc=Piece_X_anchor[Player_num]+Piece_X[Piece_starts[Piece_current[Player_num]]+4*New_orientation+idx];
		Piece_Yc=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*New_orientation+idx];
		if((LVL_squares[Piece_Yc*LVL_WIDTH+Piece_Xc].type>=0 && LVL_squares[Piece_Yc*LVL_WIDTH+Piece_Xc].owner!=Player_num) || Piece_Xc<0 || Piece_Xc>=LVL_WIDTH || Piece_Yc<0 || Piece_Yc>=LVL_HEIGHT)
			{
			rotation_possible=false;
			}
		}
	if(rotation_possible)
		{
		Remove_Piece(Player_num);
		Piece_orientation[Player_num]=New_orientation;
		Place_Piece(Player_num);
		}
	/*Try rotation around anchor_X+1*/
	else
		{//TODO
		}
}


uint64_t Delete_Lines()
{
	uint64_t idx_y_d, idx_x_d;

	uint64_t idx, idx2, idx_x, idx_y, lines_removed=0;
	BOOL line_filled;
	/*Top half of level*/
	idx=0;
	for (idx_y=0;idx_y<LVL_HEIGHT/2;idx_y++)
		{
		line_filled=true;
		for (idx_x=0;idx_x<LVL_WIDTH;idx_x++)
			{
			if(LVL_squares[idx].type<0 || LVL_squares[idx].owner>=0)
				{
				line_filled=false;
				}
			idx++;
			}
		/*Remove line, move everything down*/
		if(line_filled)
			{
			for (idx2=idx_y;idx2>0;idx2--)
				{
				memcpy(&LVL_squares[idx2*LVL_WIDTH],&LVL_squares[(idx2-1)*LVL_WIDTH],LVL_WIDTH*sizeof(LVL));
				}
			if(Piece_Y_anchor[0]<idx_y)
				{
				Piece_Y_anchor[0]++;
				}
			if(Piece_Y_anchor[1]<idx_y)
				{
				Piece_Y_anchor[1]++;
				}
			for (idx2=0;idx2<LVL_WIDTH;idx2++)
				{
				LVL_squares[idx2].type=-1;
				LVL_squares[idx2].owner=-1;
				}
			lines_removed++;
			}
		}
	/*Bottom half of level*/
	idx=LVL_HEIGHT*LVL_WIDTH-1;
	for (idx_y=LVL_HEIGHT-1;idx_y>=LVL_HEIGHT/2;idx_y--)
		{
		line_filled=true;
		for (idx_x=0;idx_x<LVL_WIDTH;idx_x++)
			{
			if(LVL_squares[idx].type<0 || LVL_squares[idx].owner>=0)
				{
				line_filled=false;
				}
			idx--;
			}
		/*Remove line, move everything up*/
		if(line_filled)
			{
			for (idx2=idx_y;idx2<LVL_HEIGHT-1;idx2++)
				{
				memcpy(&LVL_squares[idx2*LVL_WIDTH],&LVL_squares[(idx2+1)*LVL_WIDTH],LVL_WIDTH*sizeof(LVL));
				}
			if(Piece_Y_anchor[0]>idx_y)
				{
				Piece_Y_anchor[0]--;
				}
			if(Piece_Y_anchor[1]>idx_y)
				{
				Piece_Y_anchor[1]--;
				}
			for (idx2=(LVL_HEIGHT-1)*LVL_WIDTH;idx2<LVL_HEIGHT*LVL_WIDTH;idx2++)
				{
				LVL_squares[idx2].type=-1;
				LVL_squares[idx2].owner=-1;
				}
			lines_removed++;
			}
		}
	/*Redraw*/
	if(lines_removed>0)
		{
		for (idx_y=0;idx_y<(SIZE_SQUARE+2)*LVL_HEIGHT;idx_y++)
			{
			for (idx_x=0;idx_x<(SIZE_SQUARE+2)*LVL_WIDTH;idx_x++)
				{
				idx=NBYTE_PX*((PAD_px_up+idx_y)*frame_width_px+PAD_px_left+SIZE_BORDER+2+idx_x);
				memcpy(level_array+idx,LVL_BG_col,3);
				}
			}
		idx=0;
		for (idx_y=0;idx_y<LVL_HEIGHT;idx_y++)
			{
			for (idx_x=0;idx_x<LVL_WIDTH;idx_x++)
				{
				if(LVL_squares[idx].type>=0)
					{
					Draw_Piece_Rectangle(LVL_squares[idx].type, PAD_px_left+SIZE_BORDER+2, PAD_px_up, idx_x, idx_y);
					}
				idx++;
				}
			}
		}
	return lines_removed;
}


BOOL Spawn_New_Piece(uint64_t Player_num, uint64_t New_Piece_type)
{
	int64_t idx, idx2, idx_P, anchor_X, anchor_Y, LVL_half_width;
	LVL_half_width=LVL_WIDTH/2;
	uint64_t Piece_starts_current;
	BOOL Piece_fits;
	if (Player_num==0) //From top
		{
		Piece_starts_current=Piece_starts[New_Piece_type];
		Piece_orientation[Player_num]=0;
		/*Ensure no square of the Piece is above the Level*/
		anchor_Y=0;
		for(idx=0;idx<4;idx++)
			{
			if(-1*Piece_Y[Piece_starts_current+idx]>anchor_Y)
				{
				anchor_Y=-1*Piece_Y[Piece_starts_current+idx];
				}
			}
		/*Attempt to find a place to fit the Piece*/
		Piece_fits=true;
		anchor_X=LVL_half_width;
		for(idx=0;idx<4;idx++)
			{
			idx_P=(Piece_Y[Piece_starts_current+idx]+anchor_Y)*LVL_WIDTH+Piece_X[Piece_starts_current+idx]+anchor_X;
			if(LVL_squares[idx_P].type>=0)
				{
				Piece_fits=false;
				}
			}
		idx2=0;
		while(!Piece_fits && idx2<LVL_half_width)
			{
			Piece_fits=true;
			anchor_X=LVL_half_width+idx2;
			for(idx=0;idx<4;idx++)
				{
				idx_P=(Piece_Y[Piece_starts_current+idx]+anchor_Y)*LVL_WIDTH+Piece_X[Piece_starts_current+idx]+anchor_X;
				if(LVL_squares[idx_P].type>=0)
					{
					Piece_fits=false;
					}
				}
			if(!Piece_fits)
				{
				Piece_fits=true;
				anchor_X=LVL_half_width-idx2;
				for(idx=0;idx<4;idx++)
					{
					idx_P=(Piece_Y[Piece_starts_current+idx]+anchor_Y)*LVL_WIDTH+Piece_X[Piece_starts_current+idx]+anchor_X;
					if(LVL_squares[idx_P].type>=0)
						{
						Piece_fits=false;
						}
					}
				}
			idx2++;
			}
		}
	else //From bottom
		{
		if(New_Piece_type==2||New_Piece_type==3||New_Piece_type==4)
			{
			/*Rotate T, L, backwards L by 180 degrees*/
			Piece_starts_current=Piece_starts[New_Piece_type]+8;
			Piece_orientation[Player_num]=2;
			}
		else
			{
			Piece_starts_current=Piece_starts[New_Piece_type];
			Piece_orientation[Player_num]=0;
			}
		/*Ensure no square of the Piece is below the Level*/
		anchor_Y=0;
		for(idx=0;idx<4;idx++)
			{
			if(Piece_Y[Piece_starts_current+idx]>anchor_Y)
				{
				anchor_Y=Piece_Y[Piece_starts_current+idx];
				}
			}
		anchor_Y=LVL_HEIGHT-1-anchor_Y;
		/*Attempt to find a place to fit the Piece*/
		Piece_fits=true;
		anchor_X=LVL_half_width;
		for(idx=0;idx<4;idx++)
			{
			idx_P=(Piece_Y[Piece_starts_current+idx]+anchor_Y)*LVL_WIDTH+Piece_X[Piece_starts_current+idx]+anchor_X;
			if(LVL_squares[idx_P].type>=0)
				{
				Piece_fits=false;
				}
			}
		idx2=0;
		while(!Piece_fits && idx2<LVL_half_width)
			{
			Piece_fits=true;
			anchor_X=LVL_half_width+idx2;
			for(idx=0;idx<4;idx++)
				{
				idx_P=(Piece_Y[Piece_starts_current+idx]+anchor_Y)*LVL_WIDTH+Piece_X[Piece_starts_current+idx]+anchor_X;
				if(LVL_squares[idx_P].type>=0)
					{
					Piece_fits=false;
					}
				}
			if(!Piece_fits)
				{
				Piece_fits=true;
				anchor_X=LVL_half_width-idx2;
				for(idx=0;idx<4;idx++)
					{
					idx_P=(Piece_Y[Piece_starts_current+idx]+anchor_Y)*LVL_WIDTH+Piece_X[Piece_starts_current+idx]+anchor_X;
					if(LVL_squares[idx_P].type>=0)
						{
						Piece_fits=false;
						}
					}
				}
			idx2++;
			}
		}
	if (Piece_fits)
		{
		Piece_X_anchor[Player_num]=anchor_X;
		Piece_Y_anchor[Player_num]=anchor_Y;
		Place_Piece(Player_num);
		return false;//Game not over
		}
	else
		{
		return true;//Game is over
		}
}


void Spawn_Next_Piece(uint64_t Player_num)
{
	uint64_t idx, Piece_Xc, Piece_Yc, Piece_X_anchor_actual, Piece_Y_anchor_actual, orientation_actual;
	if (Player_num==0)
		{
		Piece_X_anchor_actual=2;
		Piece_Y_anchor_actual=1;
		orientation_actual=0;
		}
	else
		{
		Piece_X_anchor_actual=2;
		if(Piece_next[Player_num]==0||Piece_next[Player_num]==5||Piece_next[Player_num]==6)
			{
			Piece_Y_anchor_actual=1;
			}
		else
			{
			Piece_Y_anchor_actual=2;
			}
		if(Piece_next[Player_num]==2||Piece_next[Player_num]==3||Piece_next[Player_num]==4)
			{
			/*Rotate T, L, backwards L by 180 degrees*/
			orientation_actual=2;
			}
		else
			{
			orientation_actual=0;
			}
		}
	for(idx=0;idx<4;idx++)
		{
		Piece_Xc=Piece_X_anchor_actual+Piece_X[Piece_starts[Piece_next[Player_num]]+4*orientation_actual+idx];
		Piece_Yc=Piece_Y_anchor_actual+Piece_Y[Piece_starts[Piece_next[Player_num]]+4*orientation_actual+idx];
		Draw_Piece_Rectangle(Piece_next[Player_num], PNext_anchor_X[Player_num]+1, PNext_anchor_Y[Player_num]+1, Piece_Xc, Piece_Yc);
		}
}


void Destroy_Next_Piece(uint64_t Player_num)
{
	uint64_t idx, Piece_Xc, Piece_Yc, Piece_X_anchor_actual, Piece_Y_anchor_actual, orientation_actual;
	if (Player_num==0)
		{
		Piece_X_anchor_actual=2;
		Piece_Y_anchor_actual=1;
		orientation_actual=0;
		}
	else
		{
		Piece_X_anchor_actual=2;
		if(Piece_next[Player_num]==0||Piece_next[Player_num]==5||Piece_next[Player_num]==6)
			{
			Piece_Y_anchor_actual=1;
			}
		else
			{
			Piece_Y_anchor_actual=2;
			}
		if(Piece_next[Player_num]==2||Piece_next[Player_num]==3||Piece_next[Player_num]==4)
			{
			/*Rotate T, L, backwards L by 180 degrees*/
			orientation_actual=2;
			}
		else
			{
			orientation_actual=0;
			}
		}
	for(idx=0;idx<4;idx++)
		{
		Piece_Xc=Piece_X_anchor_actual+Piece_X[Piece_starts[Piece_next[Player_num]]+4*orientation_actual+idx];
		Piece_Yc=Piece_Y_anchor_actual+Piece_Y[Piece_starts[Piece_next[Player_num]]+4*orientation_actual+idx];
		Draw_Piece_Rectangle(-1, PNext_anchor_X[Player_num]+1, PNext_anchor_Y[Player_num]+1, Piece_Xc, Piece_Yc);
		}
}


/*Only up/down, depending on player*/
BOOL Push_Piece(uint64_t Player_num)
{
	BOOL Game_is_Over=false;
	uint64_t Check_Direction, idx, Piece_Xc, Piece_Yc;

	if(Player_num==0)
		{
		Check_Direction=0;
		}
	else
		{
		Check_Direction=2;
		}
	if (Check_Piece_Collision(Player_num,Check_Direction))
		{
		/*Disown old piece*/
		for(idx=0;idx<4;idx++)
			{
			Piece_Xc=Piece_X_anchor[Player_num]+Piece_X[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
			Piece_Yc=Piece_Y_anchor[Player_num]+Piece_Y[Piece_starts[Piece_current[Player_num]]+4*Piece_orientation[Player_num]+idx];
			LVL_squares[Piece_Yc*LVL_WIDTH+Piece_Xc].owner=-1;
			}
		Destroy_Next_Piece(Player_num);
		Piece_current[Player_num]=Piece_next[Player_num];
		Game_is_Over=Spawn_New_Piece(Player_num,Piece_current[Player_num]);
		Piece_next[Player_num]=(rand() % 7);
		Spawn_Next_Piece(Player_num);
		return Game_is_Over;
		}
	else if (Check_Piece_Escaping(Player_num))
		{
		Game_is_Over=true;
		return Game_is_Over;
		}
	else
		{
		Remove_Piece(Player_num);
		if(Player_num==0)
			{
			Piece_Y_anchor[0]++;
			}
		else
			{
			Piece_Y_anchor[1]--;
			}
		Place_Piece(Player_num);
		}
	return Game_is_Over;
}


void Game_Over()
{
	int MENU_BN_states[2];
	Destroy_Next_Piece(0);
	Destroy_Next_Piece(1);
	game_running=false;
	menu_active=true;
	memcpy(MENU_flash_col,MENU_BG_col,3);
	Draw_Hollow_Rectangle(MENU_BN_anchor_X[0]-5, MENU_BN_anchor_Y[0]-5, MENU_BN_anchor_X[0]+MENU_BN_Width+4, MENU_BN_anchor_Y[0]+MENU_BN_Height+4, MENU_array, MENU_width_px, MENU_height_px, MENU_flash_col);
	MENU_Bn_active=1;
	MENU_BN_states[0]=2;
	MENU_BN_states[1]=0;
	Menu_Set_Buttons(MENU_BN_states);
	MENU_frame_num=0;
}


void Setup(HWND hwnd)
{
	uint64_t idx, idx2, idx_y;

	/*Setup RNG*/
	QueryPerformanceCounter(&TIME_current);
	srand(TIME_current.QuadPart);

	/*Set up blank level*/
	LVL_squares=new LVL [LVL_WIDTH*LVL_HEIGHT];

	/*Set up empty level with border*/
	frame_Size=NBYTE_PX*frame_width_px*frame_height_px;
	frame_array=new unsigned char [frame_Size];
	level_array=new unsigned char [frame_Size];
	for(idx=0;idx<frame_width_px*frame_height_px;idx++)
		{
		idx2=NBYTE_PX*idx;
		memcpy(level_array+idx2,LVL_BG_col,3);
		level_array[idx2+3]=255;
		}
	Reset_LVL();
	Create_Border(level_array);
	for (idx_y=0;idx_y<SCORE_HEIGHT;idx_y++)
		{
		idx=NBYTE_PX*((Score_label_anchor_Y+idx_y)*frame_width_px+Score_label_anchor_X);
		memcpy(level_array+idx,SCORE_arr+idx_y*NBYTE_PX*SCORE_WIDTH,NBYTE_PX*SCORE_WIDTH);
		}
	for (idx2=0;idx2<2;idx2++)
		{
		for (idx_y=0;idx_y<PNEXT_HEIGHT;idx_y++)
			{
			idx=NBYTE_PX*((PNext_label_anchor_Y[idx2]+idx_y)*frame_width_px+PNext_label_anchor_X[idx2]);
			memcpy(level_array+idx,PNEXT_arr+idx_y*NBYTE_PX*PNEXT_WIDTH,NBYTE_PX*PNEXT_WIDTH);
			}
		for (idx_y=0;idx_y<DIGIT_HEIGHT;idx_y++)
			{
			idx=NBYTE_PX*((PNext_num_anchor_Y[idx2]+idx_y)*frame_width_px+PNext_num_anchor_X[idx2]);
			memcpy(level_array+idx,DIGITS_arr+NBYTE_PX*(idx_y*DIGIT_WIDTH*10+(idx2+1)*DIGIT_WIDTH),NBYTE_PX*DIGIT_WIDTH);
			}
		}
	Draw_Hollow_Rectangle(PNext_anchor_X[0], PNext_anchor_Y[0], PNext_anchor_X[0]+NEXT_WIDTH*(SIZE_SQUARE+2)+2, PNext_anchor_Y[0]+NEXT_HEIGHT*(SIZE_SQUARE+2)+2, level_array, frame_width_px, frame_height_px, BORDER_col);
	Draw_Hollow_Rectangle(PNext_anchor_X[1], PNext_anchor_Y[1], PNext_anchor_X[1]+NEXT_WIDTH*(SIZE_SQUARE+2)+2, PNext_anchor_Y[1]+NEXT_HEIGHT*(SIZE_SQUARE+2)+2, level_array, frame_width_px, frame_height_px, BORDER_col);

	MENU_BN_Size=NBYTE_PX*MENU_BN_Width*MENU_BN_Height;
	MENU_array=new unsigned char [NBYTE_PX*MENU_width_px*MENU_height_px];
	Menu_Create();


	BM_Main_picture = CreateBitmap(frame_width_px,frame_height_px,1,NBIT_PX,(void*) frame_array);

	Render_frame(hwnd);
}


void Cleanup()
{
	DeleteObject(BM_Main_picture);
	delete[] LVL_squares;
	delete[] frame_array;
	delete[] level_array;
	delete[] MENU_array;
}


void KeyPressed(WPARAM wParam, LPARAM lParam)
{
	BOOL Game_is_Over;
	int MENU_BN_states[2];
	if (wParam == VK_ESCAPE)
		{
		if (menu_active)
			{
			if (game_running)
				{
				menu_active=false;
				}
			}
		else
			{
			/*Remove previous flashing*/
			memcpy(MENU_flash_col,MENU_BG_col,3);
			Draw_Hollow_Rectangle(MENU_BN_anchor_X[1]-5, MENU_BN_anchor_Y[1]-5, MENU_BN_anchor_X[1]+MENU_BN_Width+4, MENU_BN_anchor_Y[1]+MENU_BN_Height+4, MENU_array, MENU_width_px, MENU_height_px, MENU_flash_col);
			MENU_Bn_active=0;
			MENU_BN_states[0]=0;
			MENU_BN_states[1]=1;
			Menu_Set_Buttons(MENU_BN_states);
			MENU_frame_num=0;
			menu_active=true;
			}
		}
	else if (wParam == VK_RETURN)//AKA ENTER
		{
		if (menu_active)
			{
			if (MENU_Bn_active==0) //Continue
				{
				menu_active=false;
				}
			else if (MENU_Bn_active==1) //New Game
				{
				Destroy_Next_Piece(0);
				Destroy_Next_Piece(1);
				Reset_LVL();
				frames_in_game=0;
				frames_since_move=0;
				current_period=40; //Start with slow speed
				i_Player_score=0;
				Draw_Score();
				Piece_current[0]=(rand() % 7);
				Piece_current[1]=(rand() % 7);
				Piece_next[0]=(rand() % 7);
				Piece_next[1]=(rand() % 7);
				Spawn_New_Piece(0,Piece_current[0]);
				Spawn_New_Piece(1,Piece_current[1]);
				Spawn_Next_Piece(0);
				Spawn_Next_Piece(1);
				
				game_running=true;
				menu_active=false;
				}
			}
		}
	else if (wParam == VK_UP)
		{
		if (!menu_active)
			{
			Game_is_Over=Push_Piece(1); //Move Player 1 (bottom going up)
			if(Game_is_Over)
				{
				Game_Over();
				}
			}
		}
	else if (wParam == VK_RIGHT)
		{
		if (menu_active) //In menu, Continue selected beforehand
			{
			if (MENU_Bn_active==0)
				{
				//Remove previous flashing
				memcpy(MENU_flash_col,MENU_BG_col,3);
				Draw_Hollow_Rectangle(MENU_BN_anchor_X[MENU_Bn_active]-5, MENU_BN_anchor_Y[MENU_Bn_active]-5, MENU_BN_anchor_X[MENU_Bn_active]+MENU_BN_Width+4, MENU_BN_anchor_Y[MENU_Bn_active]+MENU_BN_Height+4, MENU_array, MENU_width_px, MENU_height_px, MENU_flash_col);
				MENU_Bn_active=1;
				MENU_BN_states[0]=1;
				MENU_BN_states[1]=0;
				Menu_Set_Buttons(MENU_BN_states);
				}
			}
		else
			{
			if (!Check_Piece_Collision(1,1))
				{
				Remove_Piece(1);
				Piece_X_anchor[1]++;
				Place_Piece(1);
				}
			}
		}
	else if (wParam == VK_LEFT)
		{
		if (menu_active) //In menu, New Game selected beforehand
			{
			if (MENU_Bn_active==1 && game_running)
				{
				/*Remove previous flashing*/
				memcpy(MENU_flash_col,MENU_BG_col,3);
				Draw_Hollow_Rectangle(MENU_BN_anchor_X[MENU_Bn_active]-5, MENU_BN_anchor_Y[MENU_Bn_active]-5, MENU_BN_anchor_X[MENU_Bn_active]+MENU_BN_Width+4, MENU_BN_anchor_Y[MENU_Bn_active]+MENU_BN_Height+4, MENU_array, MENU_width_px, MENU_height_px, MENU_flash_col);
				MENU_Bn_active=0;
				MENU_BN_states[0]=0;
				MENU_BN_states[1]=1;
				Menu_Set_Buttons(MENU_BN_states);
				}
			}
		else
			{
			if (!Check_Piece_Collision(1,3))
				{
				Remove_Piece(1);
				Piece_X_anchor[1]--;
				Place_Piece(1);
				}
			}
		}
	else if (wParam == 0x53)//S
		{
		if (!menu_active)
			{
			Game_is_Over=Push_Piece(0); //Move Player 0 (top going down)
			if(Game_is_Over)
				{
				Game_Over();
				}
			}
		}
	else if (wParam == 0x44)//D
		{
		if (!menu_active)
			{
			if (!Check_Piece_Collision(0,1))
				{
				Remove_Piece(0);
				Piece_X_anchor[0]++;
				Place_Piece(0);
				}
			}
		}
	else if (wParam == 0x41)//A
		{
		if (!menu_active)
			{
			if (!Check_Piece_Collision(0,3))
				{
				Remove_Piece(0);
				Piece_X_anchor[0]--;
				Place_Piece(0);
				}
			}
		}
	else if (wParam == VK_CONTROL)
		{
		if ((lParam & 0x01000000) == 0)//Left CTRL
			{
			if (!menu_active)
				{
				Rotate_Piece(0,false);
				}
			}
		else //Right CTRL
			{
			if (!menu_active)
				{
				Rotate_Piece(1,false);
				}
			}
		}
	else if (wParam == VK_LCONTROL)
		{
		if (!menu_active)
			{
			Rotate_Piece(0,false);
			}
		}
	else if (wParam == VK_RCONTROL)
		{
		if (!menu_active)
			{
			Rotate_Piece(1,false);
			}
		}
	else if (wParam == VK_SHIFT)
		{
		if (MapVirtualKey((UINT)((lParam & 0x00ff0000) >> 16), MAPVK_VSC_TO_VK_EX)==VK_LSHIFT) //Left shift
			{
			if (!menu_active)
				{
				Rotate_Piece(0,true);
				}
			}
		else //Right shift
			{
			if (!menu_active)
				{
				Rotate_Piece(1,true);
				}
			}
		}
	else if (wParam == VK_LSHIFT)
		{
		if (!menu_active)
			{
			Rotate_Piece(0,true);
			}
		}
	else if (wParam == VK_RSHIFT)
		{
		if (!menu_active)
			{
			Rotate_Piece(1,true);
			}
		}

}


/*DirectX Tick method, fixed time frame*/
/*Must use WaitableTimer to sleep instead of sleeping until VSync in DirectX*/
/*Will actually have lower FPS*/
void Tick(HWND hwnd)
{
	BOOL Game_is_Over;
	uint64_t lines_removed;

	if(!menu_active)
		{
		if (frames_since_move>=current_period)
			{
			Game_is_Over=Push_Piece(0); //Move Player 0 (top going down) Piece first
			if(Game_is_Over)
				{
				Game_Over();
				}
			else
				{
				Game_is_Over=Push_Piece(1); //Move Player 1 (bottom going up) Piece first
				if(Game_is_Over)
					{
					Game_Over();
					}
				}
			frames_since_move=0;
			}
		else
			{
			frames_since_move++;
			}
		frames_in_game++;

		lines_removed=Delete_Lines();
		if (lines_removed>0)
			{
			i_Player_score+=score_function(lines_removed);
			Draw_Score();
			}

		/*Increase game speed*/
		if ((frames_in_game % (current_period*20))==0 && current_period>7)
			{
			current_period--;
			}
		}

	Render_frame(hwnd);

	/*Timing, sleep until next frame*/
	HANDLE hTimer = NULL;
	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	TIME_last=TIME_current;
	QueryPerformanceCounter(&TIME_current);
	TIME_remaining=TIME_target_frame-static_cast<uint64_t>(TIME_current.QuadPart-TIME_last.QuadPart);
	TIME_remaining=std::min(std::max(TIME_remaining,(uint64_t)0),TIME_target_frame);
	TIME_wait.QuadPart=-TIME_remaining;
	SetWaitableTimer(hTimer, &TIME_wait, 0, NULL, NULL, 0);
	WaitForSingleObject(hTimer, INFINITE);

}




