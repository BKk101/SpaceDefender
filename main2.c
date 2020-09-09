
/***********************************************************/
// 다음은 사용자 함수를 호출하는 루틴임 지우지 말것!
/***********************************************************/

#include "device_driver.h"
#include <stdlib.h>
#include "2440addr.h"
#define printf 	Uart_Printf

typedef struct
{
	int day;
	int year;
	int mon;
	int date;
	int hour;
	int min;
	int sec;
}RTC_TIME;

void User_Main(void);

void Main(void)
{
	MMU_Init();
	Led_Init();
	Key_Poll_Init();
	Uart_Init(115200);
	Timer0_Init();
	Timer3_Buzzer_Init();
	Timer4_Init();
	Lcd_Graphic_Init();
	Lcd_Clr_Screen(0xf800);

	Uart_Printf("Welcome GBOX World!\n");
	User_Main();
	Uart_Printf("Good Bye~\n");
}

int RTC_Get_Time(void)
{
	RTC_TIME a;

	a.hour = (int)rBCDHOUR;
	a.min = (int)rBCDMIN;
	a.sec = (int)rBCDSEC;
	return ((a.hour >> 4) & 0xf) * 10 + (a.hour & 0xf) * 360 + \
		((a.min >> 4) & 0xf) * 10 + (a.min & 0xf) * 60 + \
		((a.sec >> 4) & 0xf) * 10 + (a.sec & 0xf);
}

/*===== << 여기부터 사용자 프로그램을 작성함 >> =====*/

#include "./Image/back4.h"
#include "./Image/left.h"
#include "./Image/right.h"
#include "./Image/sea.h"
#include "./Image/kfx.h"
#include "./Image/plane.h"
#include "./Image/plane2.h"
#include "./Image/bul01.h"
#include "./Image/plane3.h"
#include "./Image/space.h"
#include "./Image/bul02.h"
#include "./Image/plane2_r.h"

const unsigned short * Disp[] = {};

/* 5:5:5:I Color Definition */

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
#define SIZE_X	30
#define SIZE_Y	30
#define PLANE	plane3
#define PLANE_A plane3_arr

typedef struct bul
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
}Bul;

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
	int bul_time;
	int bul_num;
	Bul bul[10];
}Obj;

void Draw_BackGround(int z, int a, int x, int y, const unsigned short int *fp);
void Player_init();
void Move_player();
void Bullet_init(int idx);
void Move_Obj();
void Draw_Obj();
void Draw_Player();
int Check_Range(int x, int y, Obj obj);
void Enemy_init(int time);
void Move_func(Obj obj[], int *obj_num, const unsigned short int *fp, int color);
void Move_func2(Bul obj[], int *obj_num, const unsigned short int *fp, int color);
void Draw_func(Obj obj[], int *obj_num, const unsigned short int *fp);
void Draw_func2(Bul obj[], int *obj_num, const unsigned short int *fp);

int map[240][320];
Obj player;
Obj enem[20];
Obj bullet_p[20];
Obj bullet_e[50];
int enem_num;
int bul_num_p;
int bul_num_e;
int Pos1[2], Pos2[2], Pos3[2];

void User_Main(void)
{
	int time = 0;
	int i,j;
	int zz;

	Timer0_Repeat(10);
	srand(RTC_Get_Time());
	//Lcd_Control_Brightness(5);
	//Lcd_Select_Buffer_Mode(LAYER_MODE);
	//Lcd_Select_Draw_Frame_Buffer(0);
	//Lcd_Select_Display_Frame_Buffer(0);
	Player_init();
	zz = back4[1] - 240;
	for (;;)
	{
		Pos1[0] = player.pos[0]; Pos1[1] = player.pos[1] - SIZE_Y / 2 - 3;
		if (Timer0_Check_Expired()) {
			if (time == 0xFFFFFFFF) time = 0;
			time++;
			for (i=0;i<bul_num_p;i++) bullet_p[i].time++;
			for (i=0;i<bul_num_e;i++) bullet_e[i].time++;
			for (i=0;i<enem_num;i++) enem[i].time++;
			Enemy_init(time);
			Uart_Printf("bul num %d\n", bul_num_e);
				for (i=0;i<bul_num_e;i++) {
					if (bullet_e[i].flag == 1) Uart_Printf("i: %d x: %d y :%d\n", i, bullet_e[i].pos[0], bullet_e[i].pos[1]);
				}

			Bullet_init(time);
			Move_player();
			Move_Obj();
		}
		//Check_crush()

		Draw_Obj();
		Draw_Player();
		Draw_BackGround(0,zz,0,0,back4);
		if (zz > 0) zz--;
	}
}

void Player_init()
{
	int i,j, xx, yy;

	player.flag = 1;
	player.idx = 1;
	player.pos[0] = START_X; player.pos[1] = START_Y;
	player.size[0] = SIZE_X; player.size[1] = SIZE_Y;
	player.delta[0] = 10; player.delta[1] = 10;
	player.move_flag = 1;
	for (i = START_Y - SIZE_Y / 2; i < START_Y + SIZE_Y / 2; i++) {
		for (j = START_X - SIZE_X/2; j < START_X + SIZE_X / 2; j++) {
			xx = j - START_X + SIZE_X/2; yy = i - START_Y + SIZE_Y/2;
			if ((int)PLANE[yy*SIZE_X + xx + 2] != BGC) map[i][j] = 1;
		}
	}
}

void Enemy_init(int time)
{
	int i;

	if (time % 40 == 0) { //한번에 만드는 갯수 제한
		if (enem_num < 10) enem_num++;
		for (i = 0; i < enem_num; i++) {
			if (enem[i].flag == 0) {
				enem[i].flag = 1;
				enem[i].idx = 3;
				enem[i].move_flag = 1;
				enem[i].size[0] = 28; enem[i].size[1] = 21;
				enem[i].pos[0] = rand()%300 + 20; enem[i].pos[1] = 20;
				enem[i].delta[0] = 0; enem[i].delta[1] = 10;
				enem[i].speed = 5;
				enem[i].time = 0;
				enem[i].bul_time = 10;
				map[enem[i].pos[1]][enem[i].pos[0]] = 3;
				break;
			}
		}
	}
}

void Bullet_init(int time)
{
	int i,j;

	//한번에 만드는 갯수 제한
/*	if (enem[idx].bul_num < 10) enem[idx].bul_num++;
	for (i = 0; i < enem[idx].bul_num; i++) {
		if (enem[idx].bul[i].flag == 0) {
			enem[idx].bul[i].flag = 1;
			enem[idx].bul[i].idx = 4;
			enem[idx].bul[i].move_flag = 1;
			enem[idx].bul[i].size[0] = 3; enem[idx].bul[i].size[1] = 3;
			enem[idx].bul[i].pos[0] = enem[idx].pos[0]; enem[idx].bul[i].pos[1] = enem[idx].pos[1] + enem[idx].size[1]/2 + 5;
			enem[idx].bul[i].delta[0] = 0; enem[idx].bul[i].delta[1] = 10;
			enem[idx].bul[i].speed = 1;
			enem[idx].bul[i].time = 0;
			map[enem[idx].bul[i].pos[1]][enem[idx].bul[i].pos[0]] = 4;
			break;
		}
	}
*/

	if (time % 10 == 0) { //한번에 만드는 갯수 제한
		for (j=0;j<enem_num;j++) {
			if (bul_num_e < 50) bul_num_e++;
			for (i = 0; i < bul_num_e; i++) {
				if (bullet_e[i].flag == 0) {
					bullet_e[i].flag = 1;
					bullet_e[i].idx = 4;
					bullet_e[i].move_flag = 1;
					bullet_e[i].size[0] = 3; bullet_e[i].size[1] = 3;
					bullet_e[i].pos[0] = enem[j].pos[0]; bullet_e[i].pos[1] = enem[j].pos[1] + 20;
					bullet_e[i].delta[0] = 0; bullet_e[i].delta[1] = 15;
					bullet_e[i].speed = 3;
					bullet_e[i].time = 0;
					map[bullet_e[i].pos[1]][bullet_e[i].pos[0]] = 4;
					break;
				}
			}
		}
	}

}

void Draw_BackGround(int z, int a, int x, int y, const unsigned short int *fp)
{
	register int width = fp[0], height = fp[1];
	register int xx, yy;

	for (yy = 0; yy < height; yy++) {
		for (xx = 0; xx < width; xx++) {
			if (xx+x<0 || xx+x>=WIDTH || yy+y<0 || yy+y>=HEIGHT) continue;
			if (map[yy][xx] == 0)
				Lcd_Put_Pixel(xx + x, yy + y, (int)fp[(a + yy)*width + (xx + z) + 2]);
		}
	}
}
void Draw_BackGround2(int z, int a, int x, int y, const unsigned short int *fp)
{
	register int width = 3, height = 3;
	register int xx, yy;

	for (yy = 0; yy < height; yy++) {
		for (xx = 0; xx < width; xx++) {
			if (xx+x<0 || xx+x>=WIDTH || yy+y<0 || yy+y>=HEIGHT) continue;
			Lcd_Put_Pixel(xx + x, yy + y, (int)fp[(a + yy)*width + (xx + z) + 2]);
		}
	}
}

void Draw_Player(void)
{
	if (1) {
		Lcd_Set_Shape_Mode(1, BGC);
		Lcd_Draw_BMP(player.pos[0] - SIZE_X/2, player.pos[1] - SIZE_Y/2, PLANE);
		Lcd_Set_Shape_Mode(0,0);
		player.move_flag = 0;
	}
}

void Draw_Obj(void)
{
	int i;

	//for (i=0;i<enem[i].bul_num;i++)
	//		Draw_func2(enem[i].bul, &(enem[i].bul_num), bul01);
	Draw_func(bullet_e, &bul_num_e, bul02);
	Lcd_Set_Shape_Mode(1, 0xffff);
	Draw_func(enem, &enem_num, plane2_r);
	Lcd_Set_Shape_Mode(0, 0);
}

void Draw_func(Obj obj[], int *obj_num, const unsigned short int *fp)
{
	int i;

	for (i=0;i<(*obj_num);i++) {
		if ((obj[i].flag == 1) && (obj[i].move_flag == 1)) {
			obj[i].move_flag = 0;
			Lcd_Draw_BMP(obj[i].pos[0] - obj[i].size[0]/2, obj[i].pos[1] - obj[i].size[1]/2, fp);
		}
	}
}

void Draw_func2(Bul obj[], int *obj_num, const unsigned short int *fp)
{
	int i;

	for (i=0;i<(*obj_num);i++) {
		if ((obj[i].flag == 1) && (obj[i].move_flag == 1)) {
			obj[i].move_flag = 0;
			Lcd_Draw_BMP(obj[i].pos[0] - obj[i].size[0]/2, obj[i].pos[1] - obj[i].size[1]/2, fp);
		}
	}
}

void Move_Obj()
{
	int i;
	Move_func(bullet_e, &bul_num_e,0,0);
	//for (i=0;i<enem_num;i++)
	//	Move_func2(enem[i].bul, &(enem[i].bul_num),0,0);
	Move_func(enem, &enem_num, plane2_r, 0xffff);
}

void Move_func(Obj obj[], int *obj_num, const unsigned short int *fp, int color)
{
	int i,j,k;
	int xx,yy;

	for (i = 0; i < (*obj_num); i++) {
		if ((obj[i].flag == 1) && obj[i].time >= obj[i].speed) {
			obj[i].pos_old[0] = obj[i].pos[0]; obj[i].pos_old[1] = obj[i].pos[1];
			obj[i].pos[0] += obj[i].delta[0]; obj[i].pos[1] += obj[i].delta[1];
			for (k = obj[i].pos_old[1] - obj[i].size[1]/2; k <= obj[i].pos_old[1] + obj[i].size[1]/2; k++) {
				for (j = obj[i].pos_old[0] - obj[i].size[0]/2; j <= obj[i].pos_old[0] + obj[i].size[0]/2; j++)
					map[k][j] = 0;
			}
			if ((obj[i].pos[0] < 0 || obj[i].pos[0] >= WIDTH) || (obj[i].pos[1] < 0 || obj[i].pos[1] >= HEIGHT)) {
				(*obj_num)--;
				obj[i].flag = 0;
				return;
			}
			for (k = obj[i].pos[1] - obj[i].size[1]/2; k <= obj[i].pos[1] + obj[i].size[1]/2; k++) {
				for (j = obj[i].pos[0] - obj[i].size[0]/2; j <= obj[i].pos[0] + obj[i].size[0]/2; j++)
					if (fp != 0) {
						xx = j - obj[i].pos[0] + obj[i].size[0]/2; yy = k - obj[i].pos[1] + obj[i].size[1]/2;
						if ((int)fp[yy*obj[i].size[0] + xx + 2] != color) map[k][j] = obj[i].idx;
					}
					else map[k][j] = obj[i].idx;
			}
			obj[i].move_flag = 1;
			obj[i].time = 0;
		}
	}
}

void Move_func2(Bul obj[], int *obj_num, const unsigned short int *fp, int color)
{
	int i,j,k;
	int xx,yy;

	for (i = 0; i < (*obj_num); i++) {
		if ((obj[i].flag == 1) && obj[i].time >= obj[i].speed) {
			obj[i].pos_old[0] = obj[i].pos[0]; obj[i].pos_old[1] = obj[i].pos[1];
			obj[i].pos[0] += obj[i].delta[0]; obj[i].pos[1] += obj[i].delta[1];
			for (k = obj[i].pos_old[1] - obj[i].size[1]/2; k <= obj[i].pos_old[1] + obj[i].size[1]/2; k++) {
				for (j = obj[i].pos_old[0] - obj[i].size[0]/2; j <= obj[i].pos_old[0] + obj[i].size[0]/2; j++)
					map[k][j] = 0;
			}
			if ((obj[i].pos[0] < 0 || obj[i].pos[0] >= WIDTH) || (obj[i].pos[1] < 0 || obj[i].pos[1] >= HEIGHT)) {
				(*obj_num)--;
				obj[i].flag = 0;
				return;
			}
			for (k = obj[i].pos[1] - obj[i].size[1]/2; k <= obj[i].pos[1] + obj[i].size[1]/2; k++) {
				for (j = obj[i].pos[0] - obj[i].size[0]/2; j <= obj[i].pos[0] + obj[i].size[0]/2; j++)
					if (fp != 0) {
						xx = j - obj[i].pos[0] + obj[i].size[0]/2; yy = k - obj[i].pos[1] + obj[i].size[1]/2;
						if ((int)fp[yy*obj[i].size[0] + xx + 2] != color) map[k][j] = obj[i].idx;
					}
					else map[k][j] = obj[i].idx;
			}
			obj[i].move_flag = 1;
			obj[i].time = 0;
		}
	}
}

void Move_player()
{
	int key;
	int i,j,xx,yy;

	key = Key_Get_Pressed();
	if (key) { //그림그리는 속도랑 비슷하게 맞추기
		player.move_flag = 1;
		player.pos_old[0] = player.pos[0];
		player.pos_old[1] = player.pos[1];
		for (i = player.pos_old[1] - SIZE_Y / 2; i <= player.pos_old[1] + SIZE_Y / 2; i++) {
			for (j = player.pos_old[0] - SIZE_X / 2; j <= player.pos_old[0] + SIZE_X/2; j++)
				map[i][j] = 0;
		}
		if (key == 2) { //left
			if (player.pos[0] - SIZE_X / 2 - player.delta[0] >= 0) player.pos[0] -= player.delta[0];
		}
		else if (key == 4) { //right
			if (player.pos[0] + SIZE_X / 2 + player.delta[0] <= WIDTH) player.pos[0] += player.delta[0];
		}
		else if (key == 1) { //up
			if (player.pos[1] - SIZE_Y / 2 - player.delta[1] >= 0) player.pos[1] -= player.delta[1];
		}
		else if (key == 3) { //down
			if (player.pos[1] + SIZE_Y / 2 + player.delta[1] <= HEIGHT) player.pos[1] += player.delta[1];
		}
		for (i = player.pos[1] - SIZE_Y / 2; i <= player.pos[1] + SIZE_Y / 2; i++) {
			for (j = player.pos[0] - SIZE_X / 2; j <= player.pos[0] + SIZE_X/2; j++) {
				xx = j - player.pos[0] + SIZE_X/2; yy = i - player.pos[1] + SIZE_Y/2;
				if ((int)PLANE[yy*SIZE_X + xx + 2] != BGC) map[i][j] = 1;
			}
		}
	}
}

int Check_Range(int x, int y, Obj obj)
{
	if (x >= obj.pos[0] - obj.size[0] / 2 && x <= obj.pos[0] + obj.size[0] / 2) {
		if (y >= obj.pos[1] - obj.size[1] / 2 && y <= obj.pos[1] + obj.size[1] / 2)
			return 1;
	}
	return 0;
}

void Check_crush()
{

}

/*
							Uart_Printf("bul num %d\n", bul_num);
							for (i=0;i<bul_num;i++) {
								if (bullet[i].flag == 1) Uart_Printf("i: %d x: %d y :%d\n", i, bullet[i].pos[0], bullet[i].pos[1]);
							}
			*/
