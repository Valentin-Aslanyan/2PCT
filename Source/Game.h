#include <cstdint>


/*Graphics*/
extern uint64_t frame_width_px;
extern uint64_t frame_height_px;

#define NBYTE_PX 4	//Number of bytes per pixel, 3 for RGB, 4  for RGBA
#define NBIT_PX  32	//Number of bits per pixel, 24 for RGB, 32 for RGBA

/* Width, in pixels, of one of the four squares of a tetromino */
/* Note there is an additional bounding border 1 pixel wide*/
#define SIZE_SQUARE 10
#define SIZE_BORDER 5	//Pixels, note bounding on either side of the border 1 pixel wide
#define PAD_px_left  200	//Pad the frame
#define PAD_px_right 300
#define PAD_px_up   95
#define PAD_px_down 110


/*Level*/
extern uint64_t level_width_px;
extern uint64_t level_height_px;
#define LVL_WIDTH  10 //Level width in units of squares
#define LVL_HEIGHT 40 //Level height in units of squares, should be even (for 2 players)
#define DIGIT_WIDTH  15
#define DIGIT_HEIGHT 17
extern unsigned char *DIGITS_arr;
#define SCORE_WIDTH  91
#define SCORE_HEIGHT 27
extern unsigned char *SCORE_arr;
#define PNEXT_WIDTH  99
#define PNEXT_HEIGHT 38
extern unsigned char *PNEXT_arr;
#define NEXT_WIDTH  6 //Next piece indicator width in units of squares
#define NEXT_HEIGHT 4 //Will be padded by border 1 px wide



/*Menu*/
#define MENU_anchor_X 30
#define MENU_anchor_Y 30
/*Ensure that the following are big enough, and that the whole window is big enough*/
#define MENU_width_px 560
#define MENU_height_px 620

#define MENU_BN_Width 150
#define MENU_BN_Height 50
extern uint64_t MENU_BN_Size;
extern unsigned char *MENU_BN_arr;
extern unsigned char *MENU_Inst_arr;
extern uint64_t MENU_BN_anchor_X[2];
extern uint64_t MENU_BN_anchor_Y[2];
/*Instructions Card*/
#define MENU_Inst_Width 550
#define MENU_Inst_Height 520


/*Functions*/
void Create_Border();
void Setup(HWND hwnd);
void Cleanup();
void KeyPressed(WPARAM wParam, LPARAM lParam);
void Render_frame(HWND hwnd);
void Tick(HWND hwnd);


