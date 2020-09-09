
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
#include "./Image/bul01.h"
#include "./Image/plane3.h"
#include "./Image/space2.h"
#include "./Image/bul02.h"
#include "./Image/plane2_r.h"
#include "./Image/plane4.h"

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
}Obj;

void Draw_BackGround(int z, int a, int x, int y, const unsigned short int *fp);
void Player_init();
void Move_player();
void Bullet_init(int idx);
void Move_Obj();
void Draw_Obj();
void Draw_Player();
int Check_Range(int x, int y, int idx, int range);
void Enemy_init(int time);
void Move_func(Obj obj[], int *obj_num, const unsigned short int *fp, int color);
void Draw_func(Obj obj[], int *obj_num, const unsigned short int *fp);
int Get_idx(int x, int y, Obj obj[]);
void Check_crush();

int map[240][320];
Obj player;
Obj enem[20];
Obj bullet_p[20];
Obj bullet_e[50];
int enem_num;
int bul_num_p;
int bul_num_e;
int Pos1[2], Pos2[2], Pos3[2];
int last_pos;

/*
	Uart_Printf("bul num %d\n", bul_num);
	for (i=0;i<bul_num;i++) {
		if (bullet[i].flag == 1) Uart_Printf("i: %d x: %d y :%d\n", i, bullet[i].pos[0], bullet[i].pos[1]);
	}
*/
void User_Main(void)
{
	int time = 0;
	int i,j;
	int zz;

	Timer0_Repeat(10);
	srand(RTC_Get_Time());
	Player_init();
	zz = space2[1] - 240;
	for (;;)
	{
		Pos1[0] = player.pos[0]; Pos1[1] = player.pos[1] - SIZE_Y / 2 - 3;
		if (Timer0_Check_Expired()) {
			if (time == 0xFFFFFFFF) time = 0;
			time++;
			for (i=0;i<bul_num_p;i++) bullet_p[i].time++;
			for (i=0;i<bul_num_e;i++) bullet_e[i].time++;
			for (i=0;i<enem_num;i++) enem[i].time++;
			//idx 값 바꿔주는 함수 만들기
			Enemy_init(time);
			Bullet_init(time);
			Uart_Printf("bul num %d\n", bul_num_p);
				for (i=0;i<bul_num_p;i++) {
					if (bullet_p[i].flag == 1) Uart_Printf("i: %d x: %d y :%d\n", i, bullet_p[i].pos[0], bullet_p[i].pos[1]);
				}
			Check_crush();
			Move_player();
			Move_Obj();
		}
		Draw_Obj();
		Draw_Player();
		Draw_BackGround(0,zz,0,0,space2);
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
	player.hit = 0;
	for (i = START_Y - SIZE_Y / 2; i < START_Y + SIZE_Y / 2; i++) {
		for (j = START_X - SIZE_X/2; j < START_X + SIZE_X / 2; j++) {
			xx = j - START_X + SIZE_X/2; yy = i - START_Y + SIZE_Y/2;
			if ((int)PLANE[yy*SIZE_X + xx + 2] != BGC) map[i][j] = 1;
		}
	}
}

void Enemy_init(int time)
{
	int i,j,k,pos;
	int xx, yy;

	if (time % 30 == 0) { //한번에 만드는 갯수 제한
		if (enem_num < 5) enem_num++;
		for (i = 0; i < enem_num; i++) {
			if (enem[i].flag == 0) {
				enem[i].flag = 1;
				enem[i].idx = 3;
				enem[i].move_flag = 1;
				enem[i].size[0] = 28; enem[i].size[1] = 21;
				while (abs((pos = rand()%280 + 20) - last_pos) < 50);
				enem[i].pos[0] = pos; enem[i].pos[1] = 20;
				enem[i].delta[0] = 0; enem[i].delta[1] = 5;
				enem[i].speed = 5;
				enem[i].time = 0;
				enem[i].hit = 0;
				last_pos = enem[i].pos[0];
				for (k = enem[i].pos[1] - enem[i].size[1]/2; k < enem[i].pos[1] + enem[i].size[1]/2; k++) {
					for (j = enem[i].pos[0] - enem[i].size[0]/2; j < enem[i].pos[0] + enem[i].size[0]/2; j++) {
						xx = j - enem[i].pos[0] + enem[i].size[0]/2; yy = k - enem[i].pos[1] + enem[i].size[1]/2;
						if ((int)plane2_r[yy*SIZE_X + xx + 2] != 0xffff) map[k][j] = 3;
					}
				}
				//map[enem[i].pos[1]][enem[i].pos[0]] = 3;
				break;
			}
		}
	}
}

void Bullet_init(int time)
{
	int i,j,k,l;

/*	if (time % 20 == 0) { //한번에 만드는 갯수 제한
		for (j=0;j<enem_num;j++) {
			if (bul_num_e < 50) bul_num_e++;
			for (i = 0; i < bul_num_e; i++) {
				if (bullet_e[i].flag == 0) {
					bullet_e[i].flag = 1;
					bullet_e[i].idx = 4;
					bullet_e[i].move_flag = 1;
					bullet_e[i].size[0] = 3; bullet_e[i].size[1] = 3;
					bullet_e[i].pos[0] = enem[j].pos[0]; bullet_e[i].pos[1] = enem[j].pos[1] + enem[j].size[1]/2 + 5;
					bullet_e[i].delta[0] = 0; bullet_e[i].delta[1] = 15;
					bullet_e[i].speed = 3;
					bullet_e[i].time = 0;
					bullet_e[i].hit = 0;
					for (k = enem[i].pos[1] - enem[i].size[1]/2; k < enem[i].pos[1] + enem[i].size[1]/2; k++) {
						for (l = enem[i].pos[0] - enem[i].size[0]/2; l < enem[i].pos[0] + enem[i].size[0]/2; l++) {
							map[k][l] = 4; //--->>> bullet pos로 수정
						}
					}
					//map[bullet_e[i].pos[1]][bullet_e[i].pos[0]] = 4;
					break;
				}
			}
		}
	}
*/	if (time % 10 == 0) { //한번에 만드는 갯수 제한
		if (bul_num_p < 20) bul_num_p++;
		for (i = 0; i < bul_num_p; i++) {
			if (bullet_p[i].flag == 0) {
				bullet_p[i].flag = 1;
				bullet_p[i].idx = 2;
				bullet_p[i].move_flag = 1;
				bullet_p[i].size[0] = 3; bullet_p[i].size[1] = 3;
				bullet_p[i].pos[0] = Pos1[0]; bullet_p[i].pos[1] = player.pos[1] - player.size[1]/2 - 5;
				bullet_p[i].delta[0] = 0; bullet_p[i].delta[1] = -15;
				bullet_p[i].speed = 2;
				bullet_p[i].time = 0;
				bullet_p[i].hit = 0;
				for (k = bullet_p[i].pos[1] - bullet_p[i].size[1]/2; k < bullet_p[i].pos[1] + bullet_p[i].size[1]/2; k++) {
					for (l = bullet_p[i].pos[0] - bullet_p[i].size[0]/2; l < bullet_p[i].pos[0] + bullet_p[i].size[0]/2; l++) {
						map[k][l] = 2;
					}
				}
				break;
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

void Draw_Player(void)
{
	if (player.flag == 1) {
		Lcd_Set_Shape_Mode(1, BGC);
		Lcd_Draw_BMP(player.pos[0] - SIZE_X/2, player.pos[1] - SIZE_Y/2, PLANE);
		Lcd_Set_Shape_Mode(0,0);
		player.move_flag = 0;
	}
}

void Draw_Obj(void)
{
	Draw_func(bullet_p, &bul_num_p, bul01);
	//Draw_func(bullet_e, &bul_num_e, bul02);
	Lcd_Set_Shape_Mode(1, 0xffff);
	Draw_func(enem, &enem_num, plane2_r);
	Lcd_Set_Shape_Mode(0, 0);
}

void Draw_func(Obj obj[], int *obj_num, const unsigned short int *fp)
{
	int i;

	for (i=0;i<(*obj_num);i++) {
		if (obj[i].hit == 1) {
			Lcd_Draw_Bar(obj[i].pos[0]-obj[i].size[0]/2, obj[i].pos[1]-obj[i].size[1]/2, obj[i].pos[0], obj[i].pos[1], WHITE);
			obj[i].hit = 0;
		}
		if ((obj[i].flag == 1) && (obj[i].move_flag == 1)) {
			obj[i].move_flag = 0;
			Lcd_Draw_BMP(obj[i].pos[0] - obj[i].size[0]/2, obj[i].pos[1] - obj[i].size[1]/2, fp);
		}
	}
}

void Move_Obj()
{
	Move_func(enem, &enem_num, plane2_r, 0xffff);
	Move_func(bullet_p, &bul_num_p,0,0);
	//Move_func(bullet_e, &bul_num_e,0,0);
}

void Move_func(Obj obj[], int *obj_num, const unsigned short int *fp, int color)
{
	int i,j,k;
	int xx,yy;

	for (i = 0; i < (*obj_num); i++) {
		if (obj[i].hit == 1) {
			(*obj_num)--;
			obj[i].flag = 0;
			for (k = obj[i].pos[1] - obj[i].size[1]/2; k <= obj[i].pos[1] + obj[i].size[1]/2; k++) {
				for (j = obj[i].pos[0] - obj[i].size[0]/2; j <= obj[i].pos[0] + obj[i].size[0]/2; j++)
					map[k][j] = 0;
				}
		}
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

/*	if (player.hit == 1) {
		player.flag = 0;
		return ;
	}
*/	key = Key_Get_Pressed();
	if (key) {
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
		if (Check_Range(bullet_e[i].pos[0], bullet_e[i].pos[1], player.idx, 5)) {
			player.hit = 1;
			bullet_e[i].hit = 1;
		}
	}
	for (i=0;i<bul_num_p;i++) {
		if (Check_Range(bullet_p[i].pos[0], bullet_p[i].pos[1], enem[0].idx, 5)) {
			idx = Get_idx(bullet_p[i].pos[0], bullet_p[i].pos[1], enem);
			enem[idx].hit = 1;
			bullet_p[i].hit = 1;
		}
	}
}


