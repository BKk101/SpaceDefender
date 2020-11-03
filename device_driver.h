
#ifndef DEVICE_DRIVER_H_
#define DEVICE_DRIVER_H_

typedef unsigned int 		U32;
typedef signed int			S32;
typedef unsigned short		U16;
typedef short int			S16;
typedef unsigned char		U8;
typedef signed char 		S8;
typedef unsigned long long 	ULL;

// Heap.c
extern void * Get_Heap_Limit(void);
extern void * Get_Heap_Base(void);
extern unsigned int Get_Heap_Size(void);

// Led.c
extern void Led_Init(void);
extern void Led_Display(int disp);

// Uart.c
extern void Uart_Init(int baud);
extern void Uart_Fifo_Init(void);
extern int Uart_Printf(char *fmt,...);
extern char Uart_Get_Char(void);
extern char Uart_Get_Pressed(void);
extern int Uart_GetIntNum(void);
extern void Uart_GetString(char *string);

// MMU.c
extern void MMU_Init(void);

// Key.c
extern void Key_Poll_Init(void);
extern int Key_Get_Pressed(void);
extern void Key_Wait_Key_Released(void);
extern int Key_Wait_Key_Pressed(void);
extern int Key_Get_Pressed_Multi(void);
extern void Key_Wait_Key_Released_Multi(void);
extern int Key_Wait_Key_Pressed_Multi(void);

// Timer.c
extern void Timer0_Init(void);
extern void Timer0_Repeat(int time);
extern int Timer0_Check_Expired(void);
extern void Timer0_Stop(void);
extern void Timer0_Change_Delay(int msec);
extern void Timer1_Stopwatch_Run(void);
extern int Timer1_Stopwatch_Stop(void);
extern void Timer4_Init(void);
extern void Timer4_Delay(unsigned int msec);

// Buzzer.c
extern void Timer3_Buzzer_Beep(int tone, int duration);
extern void Timer3_Buzzer_Init(void);

// Graphics.c
#define LCD_XSIZE 		(960)
#define LCD_YSIZE 		(240)
#define LCD_SIZE 		(LCD_YSIZE*LCD_XSIZE*4)

#define LAYER_MODE		0
#define VIRTUAL_MODE	1

extern unsigned int Virtual_X;
extern unsigned int Virtual_Y;

#define BPP_16	0
#define BPP_24	1

extern unsigned int Bit_per_pixel;
extern unsigned int Selected_frame;
extern unsigned int Display_frame;

extern unsigned int Trans_mode;
extern unsigned int Shape_mode;
extern unsigned int Shape_mode_color;

extern void Lcd_Control_Brightness(int bright);

extern void Lcd_Set_Trans_Mode(int mode);
extern void Lcd_Set_Shape_Mode(int mode, int color);

extern void Lcd_Wait_Blank(void);
extern void Lcd_Select_Buffer_Mode(unsigned int mode);
extern void Lcd_Set_Virtual_Display_Start(int x, int y);
extern void Lcd_Select_Draw_Frame_Buffer(unsigned int id);
extern void Lcd_Select_Display_Frame_Buffer(unsigned int id);

extern void Lcd_Graphic_Init(void);
extern void Lcd_Put_Pixel(int x,int y,int color);
extern void Lcd_Clr_Screen(int color);
extern void Lcd_Draw_BMP(int x, int y, const unsigned short int *fp);
extern void Lcd_Draw_Image(int x, int y, const unsigned short int *fp, int width, int height);
extern void Lcd_Get_Info_BMP(int * x, int  * y, const unsigned short int *fp);
extern void Lcd_Draw_Cam_Image(int x, int y, const unsigned short *fp, int width, int height);

extern void Lcd_Draw_Line(int x1,int y1,int x2,int y2,int color);
extern void Lcd_Draw_Hline(int y, int x1, int x2, int color);
extern void Lcd_Draw_Vline(int x, int y1, int y2, int color);
extern void Lcd_Draw_Rect(int x1, int y1, int x2, int y2, int color);
extern void Lcd_Draw_Bar(int x1, int y1, int x2, int y2, int color);

extern void Lcd_Draw_BMP_File_24bpp(int x, int y, void *fp);
extern void Lcd_Puts(int x, int y, int color, int bkcolor, char *str, int zx, int zy);
extern void Lcd_Printf(int x, int y, int color, int bkcolor, int zx, int zy, char *fmt,...);

#endif



