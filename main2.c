
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

#include "./Image/space2.h"
#include "./Image/bul01.h"
#include "./Image/bul02.h"
#include "./Image/plane3.h"
#include "./Image/plane4.h"
#include "./Image/enemy1.h"
#include "./Image/enemy2.h"
#include "./Image/expld1.h"
#include "./Image/expld2.h"
#include "./Image/expld3.h"
#include "./Image/pow.h"

const unsigned short * expld[] = {expld1, expld2, expld3};
const unsigned short * enemy_img[] = {enemy1,enemy2};

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
#define SIZE_X	25
#define SIZE_Y	25
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
	const unsigned short int *fp;
}Obj;

typedef struct img
{
	const unsigned short int *fp;
	int wid;
	int hei;
}Img;

void Draw_BackGround(int z, int a, int x, int y, const unsigned short int *fp);
void Player_init();
void Move_player();
void Bullet_init(int idx);
void Move_Obj();
void Draw_Obj();
void Draw_Player();
int Check_Range(int x, int y, int idx, int range);
void Enemy_init(int time);
void Move_func(Obj obj[], int *obj_num);
void Draw_func(Obj obj[], int *obj_num, char c);
int Get_idx(int x, int y, Obj obj[]);
void Lcd_Draw_BMP2(int x, int y, const unsigned short int *fp);
void Check_crush();
void Write_func(Obj obj);
void Clear_func(Obj obj);
void Item_init(int time);

int map[240][320];
Obj player;
Obj enem[20];
Obj bullet_p[20];
Obj bullet_e[50];
Obj item[10];
int enem_num;
int bul_num_p;
int bul_num_e;
int item_num;
int Pos_x[5], Pos_y[5];;
int last_pos;

/*
	Uart_Printf("bul num %d\n", bul_num_p);
				for (i=0;i<bul_num_p;i++) {
					if (bullet_p[i].flag == 1) Uart_Printf("i: %d x: %d y :%d\n", i, bullet_p[i].pos[0], bullet_p[i].pos[1]);
				}
*/
void User_Main(void)
{
	int time = 0;
	int i,j;
	int zz;

	Timer0_Repeat(10);
	srand(RTC_Get_Time());
	Lcd_Draw_Bar(0,0,WIDTH,20,0x6319);
	Player_init();
	zz = space2[1] - 220;
	for (;;)
	{
		Pos_x[0] = player.pos[0]; Pos_y[0] = player.pos[1] - SIZE_Y / 2 - 3;
		Pos_x[1] = player.pos[0] - 10; Pos_y[1] = player.pos[1] - SIZE_Y / 2 - 3;
		Pos_x[2] = player.pos[0] + 10; Pos_y[2] = player.pos[1] - SIZE_Y / 2 - 3;
		if (Key_Get_Pressed() == 5) player.pow = 3;
		if (Timer0_Check_Expired()) {
			if (time == 0xFFFFFFFF) time = 0;
			time++;
			for (i=0;i<bul_num_p;i++) bullet_p[i].time++;
			for (i=0;i<bul_num_e;i++) bullet_e[i].time++;
			for (i=0;i<enem_num;i++) enem[i].time++;
			for (i=0;i<item_num;i++) item[i].time++;
			Enemy_init(time);
			Bullet_init(time);
			Item_init(time);
			Check_crush();
			Move_player();
			Move_Obj();
		}
		Draw_Obj();
		Draw_Player();
		Draw_BackGround(0,zz,0,20,space2);
		if (zz > 0) zz--;
		if (zz <= 0) zz = space2[1] - 220;
	}
}

void Player_init()
{
	player.flag = 1;
	player.idx = 1;
	player.pos[0] = START_X; player.pos[1] = START_Y;
	player.size[0] = SIZE_X; player.size[1] = SIZE_Y;
	player.delta[0] = 10; player.delta[1] = 10;
	player.move_flag = 1;
	player.hit = 0;
	player.pow = 1;
	player.fp = PLANE;
	Write_func(player);
}

void Enemy_init(int time)
{
	int i,pos,temp;

	if (time % 70 == 0) { //한번에 만드는 갯수 제한
		if (enem_num < 5) enem_num++;
		for (i = 0; i < enem_num; i++) {
			if (enem[i].flag == 0) {
				temp = (rand()%10 > 5) ? 1:0;
				enem[i].flag = 1;
				enem[i].idx = 3;
				enem[i].move_flag = 1;
				enem[i].fp = enemy_img[temp];
				Lcd_Get_Info_BMP(&enem[i].size[0], &enem[i].size[1], enem[i].fp);
				while (abs((pos = rand()%280 + 20) - last_pos) < 80);
				enem[i].pos[0] = pos; enem[i].pos[1] = 15;
				enem[i].delta[0] = 0; enem[i].delta[1] = 5;
				enem[i].speed = 5;
				enem[i].time = 0;
				enem[i].hit = 0;
				last_pos = enem[i].pos[0];
				Write_func(enem[i]);
				break;
			}
		}
	}
}

void Bullet_init(int time)
{
	int i,j;

	if (time % 40 == 0) { //적 총알
		for (j=0;j<enem_num;j++) {
			if (bul_num_e < 50) bul_num_e++;
			for (i = 0; i < bul_num_e; i++) {
				if (bullet_e[i].flag == 0) {
					bullet_e[i].flag = 1;
					bullet_e[i].idx = 4;
					bullet_e[i].move_flag = 1;
					bullet_e[i].fp = bul02;
					Lcd_Get_Info_BMP(&bullet_e[i].size[0], &bullet_e[i].size[1], bullet_e[i].fp);
					bullet_e[i].pos[0] = enem[j].pos[0]; bullet_e[i].pos[1] = enem[j].pos[1] + enem[j].size[1]/2 + 5;
					bullet_e[i].delta[0] = 0; bullet_e[i].delta[1] = 12;
					bullet_e[i].speed = 2;
					bullet_e[i].time = 0;
					bullet_e[i].hit = 0;
					Write_func(bullet_e[i]);
					break;
				}
			}
		}
	}
	if (time % 10 == 0 && player.flag == 1) { //플레이어 총알
		if (bul_num_p < 20) bul_num_p+=player.pow;
		j = player.pow-1;
		for (i = 0; i < bul_num_p; i++) {
			if (bullet_p[i].flag == 0) {
				bullet_p[i].flag = 1;
				bullet_p[i].idx = 2;
				bullet_p[i].move_flag = 1;
				bullet_p[i].fp = bul01;
				Lcd_Get_Info_BMP(&bullet_p[i].size[0], &bullet_p[i].size[1], bullet_p[i].fp);
				bullet_p[i].pos[0] = Pos_x[j]; bullet_p[i].pos[1] = Pos_y[j];
				bullet_p[i].delta[0] = j == 0 ? 0 : (j%2 ? -3 : 3);
				bullet_p[i].delta[1] = -15;
				bullet_p[i].speed = 1;
				bullet_p[i].time = 0;
				bullet_p[i].hit = 0;
				Write_func(bullet_p[i]);
				--j;
			}
		}
	}

}

void Item_init(int time)
{
	int i;

	if (time % 50 == 0) {
		if (item_num < 10) item_num++;
		for (i=0;i<item_num;i++) {
			if (item[i].flag == 0) {
				item[i].flag = 1;
				item[i].idx = 5;
				item[i].move_flag = 1;
				item[i].fp = pow;
				Lcd_Get_Info_BMP(&item[i].size[0], &item[i].size[1], item[i].fp);
				Uart_Printf("%d %d\n", item[i].size[0], item[i].size[1]);
				item[i].pos[0] = rand()%280+20; item[i].pos[1] = 10;
				item[i].delta[0] = 0; item[i].delta[1] = 10;
				item[i].speed = 10;
				item[i].time = 0;
				item[i].hit = 0;
				Write_func(item[i]);
			}
		}
	}
}

void Lcd_Draw_BMP2(int x, int y, const unsigned short int *fp)
{
	register int width = fp[0], height = fp[1];
	register int xx, yy;

	for(yy=0;yy<height;yy++)
	{
		for(xx=0;xx<width;xx++)
		{
			if (yy+y<20) continue;
			Lcd_Put_Pixel(xx+x,yy+y,(int)fp[yy*width+xx+2]);
		}
	}
}

void Draw_BackGround(int z, int a, int x, int y, const unsigned short int *fp)
{
	register int width = fp[0], height = fp[1];
	register int xx, yy;

	for (yy = 0; yy < 240-y; yy++) {
		for (xx = 0; xx < width; xx++) {
			if (xx+x<0 || xx+x>=WIDTH || yy+y<20 || yy+y>=HEIGHT) continue;
			if (map[yy+y][xx+x] == 0)
				Lcd_Put_Pixel(xx + x, yy + y, (int)fp[(a + yy)*width + (xx + z) + 2]);
		}
	}
}

void Draw_Player(void)
{
	int i;
	static int time = 0;

	Lcd_Set_Shape_Mode(1, BGC);
	if (player.flag == 1) {
		Lcd_Draw_BMP2(player.pos[0] - SIZE_X/2, player.pos[1] - SIZE_Y/2, PLANE);
		player.move_flag = 0;
	}
	else {
		++time;
		Lcd_Draw_BMP2(player.pos[0] - SIZE_X/2, player.pos[1] - SIZE_Y/2, PLANE);
		for (i=0;i<3;i++) Lcd_Draw_BMP2(player.pos[0]-15, player.pos[1]-15, expld[i]);
		for (i=0;i<3;i++) Lcd_Draw_BMP2(player.pos[0]+5, player.pos[1]+5, expld[i]);
		Clear_func(player);
		if (time > 15) {
			time = 0;
			Player_init();
		}
	}
	Lcd_Set_Shape_Mode(0,0);
}

void Draw_Obj(void)
{
	Lcd_Set_Shape_Mode(1, BGC);
	Draw_func(bullet_p, &bul_num_p, 'b');
	Draw_func(bullet_e, &bul_num_e, 'b');
	Draw_func(item, &item_num, 'i');
	Draw_func(enem, &enem_num, 'e');
	Lcd_Set_Shape_Mode(0, 0);
}

void Draw_func(Obj obj[], int *obj_num, char c)
{
	int i,j;
	static int time = 0;

	for (i=0;i<(*obj_num);i++) {
		if (obj[i].hit == 1) {
			Clear_func(obj[i]);
			if (c == 'e') {
				++time;
				for (j=0;j<3;j++) Lcd_Draw_BMP2(obj[i].pos[0]-7, obj[i].pos[1]-7, expld[j]);
				if (time > 5) {
					time = 0;
					(*obj_num)--;
					obj[i].flag = 0;
					obj[i].hit = 0;

				}
			}
			else {
				(*obj_num)--;
				obj[i].flag = 0;
				obj[i].hit = 0;
			}
		}
		if ((obj[i].flag == 1) && (obj[i].move_flag == 1)) {
			obj[i].move_flag = 0;
			Lcd_Draw_BMP2(obj[i].pos[0] - obj[i].size[0]/2, obj[i].pos[1] - obj[i].size[1]/2, obj[i].fp);
		}
	}
}

void Move_Obj()
{
	Move_func(enem, &enem_num);
	Move_func(bullet_p, &bul_num_p);
	Move_func(bullet_e, &bul_num_e);
	Move_func(item, &item_num);
}

void Move_func(Obj obj[], int *obj_num)
{
	int i;

	for (i = 0; i < (*obj_num); i++) {
		if (obj[i].time >= obj[i].speed && (obj[i].flag == 1) && (obj[i].hit == 0)) {
			Clear_func(obj[i]);
			obj[i].pos_old[0] = obj[i].pos[0]; obj[i].pos_old[1] = obj[i].pos[1];
			obj[i].pos[0] += obj[i].delta[0]; obj[i].pos[1] += obj[i].delta[1];
			if ((obj[i].pos[0] < 0 || obj[i].pos[0] >= WIDTH) || (obj[i].pos[1] < 20 || obj[i].pos[1] >= HEIGHT)) {
				//y경계값 적생성위치에 따라 문제될 수 있음
				(*obj_num)--;
				obj[i].flag = 0;
				return;
			}
			Write_func(obj[i]);
			obj[i].move_flag = 1;
			obj[i].time = 0;
		}
	}
}

void Move_player()
{
	int key;

	if (player.hit == 1) {
		player.flag = 0;
		return ;
	}
	key = Key_Get_Pressed();
	if (key) {
		player.move_flag = 1;
		player.pos_old[0] = player.pos[0]; player.pos_old[1] = player.pos[1];
		Clear_func(player);
		if (key == 2) { //left
			if (player.pos[0] - SIZE_X / 2 - player.delta[0] > 0) player.pos[0] -= player.delta[0];
		}
		else if (key == 4) { //right
			if (player.pos[0] + SIZE_X / 2 + player.delta[0] < WIDTH) player.pos[0] += player.delta[0];
		}
		else if (key == 1) { //up
			if (player.pos[1] - SIZE_Y / 2 - player.delta[1] > 20) player.pos[1] -= player.delta[1];
		}
		else if (key == 3) { //down
			if (player.pos[1] + SIZE_Y / 2 + player.delta[1] < HEIGHT) player.pos[1] += player.delta[1];
		}
		Write_func(player);
	}
}

void Write_func(Obj obj)
{
	int i,j,xx,yy;

	for (i = obj.pos[1] - obj.size[1]/2; i <= obj.pos[1] + obj.size[1]/2; i++) {
		for (j = obj.pos[0] - obj.size[0]/2; j <= obj.pos[0] + obj.size[0]/2; j++) {
			xx = j - obj.pos[0] + obj.size[0]/2; yy = i - obj.pos[1] + obj.size[1]/2;
			if ((int)obj.fp[yy*obj.size[0] + xx + 2] != BGC) map[i][j] = obj.idx;
		}
	}
}

void Clear_func(Obj obj)
{
	int i,j;

	for (i = obj.pos[1] - obj.size[1]/2; i <= obj.pos[1] + obj.size[1]/2; i++) {
		for (j = obj.pos[0] - obj.size[0]/2; j <= obj.pos[0] + obj.size[0]/2; j++) {
			map[i][j] = 0;
		}
	}
}

int Check_Range(int x, int y, int idx, int range)
{
	int i,j;

	for (i=-range;i<=range;i++) {
		for (j=-range;j<=range;j++) {
			if (map[y+i][x+j] == idx) return 1;
		}
	}
	return 0;
}

int Get_idx(int x, int y, Obj obj[])
{
	int i;
	for (i=0;i<enem_num;i++) {
		if ((abs(x-obj[i].pos[0]) < obj[i].size[0]+10) && (abs(y-obj[i].pos[1]) < obj[i].size[1]+10))
			return i;
	}
	return -1;
}
void Check_crush()
{
	int i,idx;
	for (i=0;i<bul_num_e;i++) {
		if (Check_Range(bullet_e[i].pos[0], bullet_e[i].pos[1], player.idx, 3)) {
			player.hit = 1;
			bullet_e[i].hit = 1;
		}
	}
	for (i=0;i<enem_num;i++) {
		if (Check_Range(enem[i].pos[0], enem[i].pos[1], 2, 8)) {
			idx = Get_idx(enem[i].pos[0], enem[i].pos[1], bullet_p);
			bullet_p[idx].hit = 1;
			enem[i].hit = 1;
		}
		if (Check_Range(enem[i].pos[0], enem[i].pos[1], 1, 8)) {
			player.hit = 1;
			enem[i].hit = 1;
		}
	}
	for (i=0;i<item_num;i++) {
		if (Check_Range(item[i].pos[0], item[i].pos[1], player.idx, 10)) {
			player.pow = 3;
			item[i].hit = 1;
		}
	}
}


