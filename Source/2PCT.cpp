#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include "Game.h"
#include "resource.h"


HRSRC MENU_BN_Obj;
HRSRC MENU_Inst_Obj;
HRSRC DIGITS_Obj;
HRSRC SCORE_Obj;
HRSRC PNEXT_Obj;

LRESULT CALLBACK GUI_Main_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
			Setup(hwnd);
		break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
		case WM_DESTROY:
			FreeResource(MENU_BN_Obj);
			FreeResource(MENU_Inst_Obj);
			FreeResource(DIGITS_Obj);
			FreeResource(SCORE_Obj);
			FreeResource(PNEXT_Obj);
			Cleanup();
			PostQuitMessage(0);
		break;
		case WM_PAINT:
			Render_frame(hwnd);
		break;
		case WM_KEYDOWN:
			KeyPressed(wParam,lParam);
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND GUI_Main_hwnd;
	WNDCLASSEX wc;	//WNDCLASSEXW etc?

	wc.cbSize	 = sizeof(WNDCLASSEX);
	wc.style	 = 0;
	wc.lpfnWndProc	 = GUI_Main_WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon	 = LoadIcon(hInstance,"MAIN_GAME_ICON");
	wc.hCursor	 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "MainWindow";
	wc.hIconSm	 = LoadIcon(hInstance,"MAIN_GAME_ICON");

	if(!RegisterClassEx(&wc))
		{
		return 1;
		}

	/*Load menu buttons, instructions card, other graphics assets*/
	MENU_BN_Obj = FindResource(NULL,MAKEINTRESOURCE(MENU_BN),RT_RCDATA);
	HGLOBAL MENU_BN_Res = LoadResource(NULL,MENU_BN_Obj);
	MENU_BN_arr = (unsigned char*)LockResource(MENU_BN_Res);

	MENU_Inst_Obj = FindResource(NULL,MAKEINTRESOURCE(MENU_INST),RT_RCDATA);
	HGLOBAL MENU_Inst_Res = LoadResource(NULL,MENU_Inst_Obj);
	MENU_Inst_arr = (unsigned char*)LockResource(MENU_Inst_Res);

	DIGITS_Obj = FindResource(NULL,MAKEINTRESOURCE(DIGIT_GFX),RT_RCDATA);
	HGLOBAL DIGITS_Res = LoadResource(NULL,DIGITS_Obj);
	DIGITS_arr = (unsigned char*)LockResource(DIGITS_Res);

	SCORE_Obj = FindResource(NULL,MAKEINTRESOURCE(SCORE_GFX),RT_RCDATA);
	HGLOBAL SCORE_Res = LoadResource(NULL,SCORE_Obj);
	SCORE_arr = (unsigned char*)LockResource(SCORE_Res);

	PNEXT_Obj = FindResource(NULL,MAKEINTRESOURCE(PNEXT_GFX),RT_RCDATA);
	HGLOBAL PNEXT_Res = LoadResource(NULL,PNEXT_Obj);
	PNEXT_arr = (unsigned char*)LockResource(PNEXT_Res);

	/*Make main window*/
	level_width_px=(SIZE_SQUARE+2)*LVL_WIDTH;
	level_height_px=(SIZE_SQUARE+2)*LVL_HEIGHT;
	frame_width_px=(SIZE_BORDER+2)*2+level_width_px+PAD_px_left+PAD_px_right;
	frame_height_px=level_height_px+PAD_px_up+PAD_px_down;
	printf("Dims: %d  %d\n",frame_width_px,frame_height_px);
	RECT GUI_Main_RECT = {0, 0, (long int)frame_width_px, (long int)frame_height_px};
	AdjustWindowRect(&GUI_Main_RECT, WS_OVERLAPPEDWINDOW, FALSE);
	GUI_Main_hwnd = CreateWindow("MainWindow","2PCT",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, GUI_Main_RECT.right-GUI_Main_RECT.left, GUI_Main_RECT.bottom-GUI_Main_RECT.top, NULL, NULL, hInstance, NULL);

	if(GUI_Main_hwnd == NULL)
		{
		return 1;
		}

	ShowWindow(GUI_Main_hwnd, nCmdShow);
	UpdateWindow(GUI_Main_hwnd);

	/*Main game loop*/
	MSG msg = {};
	while (WM_QUIT != msg.message)
		{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        		{
            		TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
		else
			{
			Tick(GUI_Main_hwnd);
			}
		}
	return msg.wParam;
}
