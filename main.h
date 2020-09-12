
#ifndef MAIN_H_
#define MAIN_H_

#include "device_driver.h"
#include <stdlib.h>
#include <string.h>
#include "2440addr.h"
#define printf 	Uart_Printf

void User_Main(void);
void Main(void);
int RTC_Get_Time(void);

#define BLACK	0x0000
#define WHITE	0xfffe
#define BLUE	0x003e
#define GREEN	0x07c0
#define RED		0xf800
#define ORANGE	0xf980
#define BGC		0x7d1

#define WIDTH	320
#define HEIGHT	240
#define START_X	160
#define START_Y	200
#define PLANE	plane4

typedef struct obj
{
	char flag;
	char move_flag;
	int idx;
	int pos[2];
	int pos_old[2];
	int delta[2];
	int size[2];
	int speed;
	int time;
	int hit;
	int pow;
	int life;
	const unsigned short int *fp;
}Obj;

void Draw_BackGround(int a, int z, int x, int y, const unsigned short int *fp);
void Lcd_Draw_BMP2(int x, int y, const unsigned short int *fp);

void Opening();
void Ending();
void Write_rect(int x1, int y1, int x2, int y2, int n);

void Var_init();
void Pos_init();
void Time_inc();
void Player_init();
void Boss_init(int time);
void Item_init(int time);
void Bullet_init(int time);
void Enemy_init(int time);

void Draw_player();
void Draw_boss();
void Draw_obj();
void Draw_func(Obj obj[], int *obj_num);

void Move_boss(int time);
void Move_player();
void Move_obj();
void Move_func(Obj obj[], int *obj_num);
void Write_func(Obj obj);
void Clear_func(Obj obj);
void Erase_obj(Obj obj, int z);

void Check_crush();
void Player_hit();
void Player_crush(Obj obj, int range);
int Check_Range(int x, int y, int range, Obj *obj, int num);

extern const unsigned short heart[];
extern const unsigned short plane4[];
extern const unsigned short space2[];
extern const unsigned short * expld[];
extern const unsigned short * enemy_img[];

extern Obj player;
extern Obj boss;
extern Obj enem[20];
extern Obj bullet_p[20];
extern Obj bullet_e[50];
extern Obj bullet_b[50];
extern Obj item[10];
extern int map[240][320];
extern int enem_num;
extern int item_num;
extern int bul_num_p;
extern int bul_num_e;
extern int bul_num_b;
extern int Life;
extern int last_pos;
extern int Item_Lock;
extern int Pos_x[5], Pos_y[5];
extern int Pos2_x[5], Pos2_y[5];
extern int zzz;

#endif /* MAIN_H_ */
