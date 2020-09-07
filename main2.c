
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

#include "./Image/img01.h"
#include "./Image/img02.h"
#include "./Image/img03.h"
#include "./Image/img04.h"
#include "./Image/img05.h"
#include "./Image/shape.h"
#include "./Image/back4.h"
#include "./Image/kfx.h"
#include "./Image/plane.h"

const unsigned short * Disp[] = { img01, img02, img03, img04 };

/* 5:5:5:I Color Definition */

#define BLACK	0x0000
#define WHITE	0xfffe
#define BLUE	0x003e
#define GREEN	0x07c0
#define RED		0xf800
#define BGC		0xffff

#define WIDTH	320
#define HEIGHT	240
#define START_X	160
#define START_Y	200
#define SIZE_X	36
#define SIZE_Y	51
#define PLANE	kfx

typedef struct obj
{
	int flag;
	int pos[2];
	int pos_old[2];
	int delta[2];
	int size[2];
	unsigned short color;
	int speed;
}Obj;

void Lcd_Draw_BackGround(int z, int x, int y, const unsigned short int *fp);
void Lcd_Draw_BMP_Excl(int x, int y, const unsigned short int *fp, unsigned short color);
void Player_init();
void Move_player(int key);
void Bullet_init(int idx);
void Move_bullet(int i);

int map[320][240];
Obj player;
Obj bullet[20];
int bul_num = 0;
int Pos1[2], Pos2[2], Pos3[2];

void User_Main(void)
{
	int i, j, time = 0;
	int xtmp, ytmp;
	int zz;
	int key;

	Timer0_Repeat(20);
	zz = back4[1] - 240;
	Player_init();
	for (;;)
	{
		Lcd_Control_Brightness(5);
		Lcd_Select_Buffer_Mode(LAYER_MODE);
		Lcd_Select_Draw_Frame_Buffer(0);
		Lcd_Select_Display_Frame_Buffer(0);
		if (time == 0xFFFFFFFF) time = 0;
		time++;
		Pos1[0] = player.pos[0]; Pos1[1] = player.pos[1] - SIZE_Y/2 - 3;

		//플레이어 비행기 그리는 부분
		key = Key_Get_Pressed();
		if (Timer0_Check_Expired()) {
			if (key) Move_player(key);
			Lcd_Get_Info_BMP(&xtmp, &ytmp, PLANE);
			Lcd_Draw_BMP_Excl(player.pos[0] - SIZE_X / 2, player.pos[1] - SIZE_Y / 2, PLANE, BGC);
			//for (i=-SIZE_X;i<SIZE_X;i++)
			//	Uart_Printf("%d ", map[player.pos[1]-SIZE_Y/2][player.pos[0]+i]);
			//Uart_Printf("\n");
		}


		//총알 그리는 부분
		if (time % 50 == 0) {
			bul_num++;
			for (i = 0; i < bul_num; i++) {
				if (bullet[i].flag == 0) {
					Bullet_init(i);
					break; //한번에 하나씩?
				}
			}
		}
		for (i = 0; i < bul_num; i++) {
			if ((bullet[i].flag == 1) && (time % bullet[i].speed == 0)) {
				Move_bullet(i);
				Lcd_Draw_Bar(bullet[i].pos[0] - bullet[i].size[0] / 2, bullet[i].pos[1] - bullet[i].size[1] / 2,
					bullet[i].pos[0] + bullet[i].size[0] / 2, bullet[i].pos[1] + bullet[i].size[1] / 2, bullet[i].color);
			}
		}
		/*
				Uart_Printf("bul num %d\n", bul_num);
				for (i=0;i<bul_num;i++) {
					if (bullet[i].flag == 1) Uart_Printf("i: %d x: %d y :%d\n", i, bullet[i].pos[0], bullet[i].pos[1]);
				}
		*/
		//배경 그리는 부분
		if (time % 10 == 0) {
			//Lcd_Get_Info_BMP(&xtmp, &ytmp, PLANE);
			//Lcd_Draw_BMP_Excl(player.pos[0] - SIZE_X/2, player.pos[1] - SIZE_Y/2, PLANE, BGC);
			Lcd_Get_Info_BMP(&xtmp, &ytmp, back4);
			Lcd_Draw_BackGround(zz, 0, 0, back4);
			if (zz > 0) zz--;
		}
	}
}

void Lcd_Draw_BackGround(int z, int x, int y, const unsigned short int *fp)
{
	register int width = fp[0], height = fp[1];
	register int xx, yy;

	for (yy = 0; yy < 240; yy++) {
		for (xx = 0; xx < width; xx++) {
			if (map[yy][xx] == 0)
				Lcd_Put_Pixel(xx + x, yy + y, (int)fp[(z + yy)*width + xx + 2]);
		}
	}
}

void Lcd_Draw_BMP_Excl(int x, int y, const unsigned short int *fp, unsigned short color)
{
	register int width = fp[0], height = fp[1];
	register int xx, yy;

	for (yy = 0; yy < height; yy++) {
		for (xx = 0; xx < width; xx++) {
			if ((int)fp[yy*width + xx + 2] != color)
				Lcd_Put_Pixel(xx + x, yy + y, (int)fp[yy*width + xx + 2]);
		}
	}
}

void Bullet_init(int idx)
{
	int i, j;
	int size_x, size_y;
	int pos_x, pos_y;

	bullet[idx].flag = 1;
	bullet[idx].size[0] = 5; bullet[idx].size[1] = 8;
	bullet[idx].pos[0] = Pos1[0]; bullet[idx].pos[1] = Pos1[1] - bullet[idx].size[1]/2;
	bullet[idx].delta[0] = 0; bullet[idx].delta[1] = -10;
	bullet[idx].speed = 5;
	bullet[idx].color = RED;
	size_x = bullet[idx].size[0]; size_y = bullet[idx].size[1];
	pos_x = bullet[idx].pos[0]; pos_y = bullet[idx].pos[1];
	for (i = pos_y - size_y / 2; i < pos_y + size_y / 2; i++) {
		for (j = pos_x - size_x / 2; j < pos_x + size_x / 2; j++) {
			map[i][j] = 2;
		}
	}
}

void Player_init()
{
	int i, j;
	int xx, yy;

	player.flag = 0;
	player.pos[0] = START_X; player.pos[1] = START_Y;
	player.size[0] = SIZE_X; player.size[1] = SIZE_Y;
	player.delta[0] = 5; player.delta[1] = 5;
	player.color = WHITE;
	for (i = START_Y - SIZE_Y / 2; i < START_Y + SIZE_Y / 2; i++) {
		for (j = START_X - SIZE_X/2; j < START_X + SIZE_X / 2; j++) {
			xx = j - START_X + SIZE_X/2; yy = i - START_Y + SIZE_Y/2;
			if ((int)PLANE[yy*SIZE_X + xx + 2] != BGC) map[i][j] = 1;
		}
	}
}

void Move_bullet(int i)
{
	int j, k;

	bullet[i].pos_old[0] = bullet[i].pos[0]; bullet[i].pos_old[1] = bullet[i].pos[1];
	bullet[i].pos[0] += bullet[i].delta[0]; bullet[i].pos[1] += bullet[i].delta[1];
	if ((bullet[i].pos[0] < 0 || bullet[i].pos[0] >= WIDTH) || (bullet[i].pos[1] < 0 || bullet[i].pos[0] >= HEIGHT)) {
		bul_num--;
		bullet[i].flag = 0;
		return;
	}
	for (k = bullet[i].pos[0] - bullet[i].size[0] / 2; k < bullet[i].pos[0] + bullet[i].size[0] / 2; k++) {
		for (j = bullet[i].pos_old[1] + bullet[i].size[1] / 2; j >= bullet[i].pos[1] + bullet[i].size[1] / 2; j--) {
			map[j][k] = 0;
		}
	}
	for (k = bullet[i].pos[0] - bullet[i].size[0] / 2; k < bullet[i].pos[0] + bullet[i].size[0] / 2; k++) {
		for (j = bullet[i].pos[1] - bullet[i].size[1] / 2; j <= bullet[i].pos_old[1] - bullet[i].size[1] / 2; j++) {
			map[j][k] = 2;
		}
	}
}

void Move_player(int key)
{
	int i, j;
	int xx, yy;
	if (key == 2) {
		if (player.pos[0] - SIZE_X / 2 - player.delta[0] < 0) return;
		player.pos_old[0] = player.pos[0];
		player.pos[0] -= player.delta[0];
		for (i = player.pos[1] - SIZE_Y / 2; i <= player.pos[1] + SIZE_Y / 2; i++) {
			for (j = player.pos_old[0] + SIZE_X / 2; j > player.pos[0] + SIZE_X/2; j--) map[i][j] = 0;
			for (j = player.pos[0] + SIZE_X / 2; j > player.pos[0]; j--) {
				xx = j-player.pos[0] + SIZE_X/2; yy = i - player.pos[1] + SIZE_Y/2;
				if ((int)PLANE[yy*SIZE_X + xx + 2] == BGC) map[i][j] = 0;
			}
		}
		for (i = player.pos[1] - SIZE_Y / 2; i <= player.pos[1] + SIZE_Y / 2; i++) {
			for (j = player.pos[0] - SIZE_X / 2; j <= player.pos[0]; j++) {
				xx = j-player.pos[0] + SIZE_X/2; yy = i - player.pos[1] + SIZE_Y/2;
				if ((int)PLANE[yy*SIZE_X + xx + 2] != BGC) map[i][j] = 1;
			}
		}
	}
	else if (key == 4) {
		if (player.pos[0] + SIZE_X / 2 + player.delta[0] > WIDTH) return;
		player.pos_old[0] = player.pos[0];
		player.pos[0] += player.delta[0];
		for (i = player.pos[1] - SIZE_Y / 2; i <= player.pos[1] + SIZE_Y / 2; i++) {
			for (j = player.pos_old[0] - SIZE_X / 2; j < player.pos[0] - SIZE_X / 2; j++) map[i][j] = 0;
			for (j = player.pos[0] - SIZE_X/2; j < player.pos[0]; j++) {
				xx = j-player.pos[0] + SIZE_X/2; yy = i - player.pos[1] + SIZE_Y/2;
				if ((int)PLANE[yy*SIZE_X + xx + 2] == BGC) map[i][j] = 0;
			}
		}
		for (i = player.pos[1] - SIZE_Y / 2; i <= player.pos[1] + SIZE_Y / 2; i++) {
			for (j = player.pos[0] + SIZE_X / 2; j >= player.pos[0]; j--) {
				xx = j-player.pos[0] + SIZE_X/2; yy = i - player.pos[1] + SIZE_Y/2;
				if ((int)PLANE[yy*SIZE_X + xx + 2] != BGC) map[i][j] = 1;
			}
		}
	}
	else if (key == 1) {
		if (player.pos[1] - SIZE_Y / 2 - player.delta[1] < 0) return;
		player.pos_old[1] = player.pos[1];
		player.pos[1] -= player.delta[1];
		for (i = player.pos[0] - SIZE_X / 2; i <= player.pos[0] + SIZE_X / 2; i++) {
			for (j = player.pos_old[1] + SIZE_Y / 2; j > player.pos[1] + SIZE_Y / 2; j--) map[j][i] = 0;
			for (j = player.pos[1] + SIZE_Y / 2; j > player.pos[1]; j--) {
				xx = i - player.pos[0] + SIZE_X/2; yy = j - player.pos[1] + SIZE_Y/2;
				if ((int)PLANE[yy*SIZE_X + xx + 2] == BGC) map[j][i] = 0;
			}
		}
		for (i = player.pos[0] - SIZE_X / 2; i <= player.pos[0] + SIZE_X / 2; i++) {
			for (j = player.pos[1] - SIZE_Y/2; j <= player.pos[1]; j++) {
				xx = i - player.pos[0] + SIZE_X/2; yy = j - player.pos[1] + SIZE_Y/2;
				if ((int)PLANE[yy*SIZE_X + xx + 2] != BGC) map[j][i] = 1;
			}
		}
	}
	else if (key == 3) {
		if (player.pos[1] + SIZE_Y / 2 + player.delta[1] > HEIGHT) return;
		player.pos_old[1] = player.pos[1];
		player.pos[1] += player.delta[1];
		for (i = player.pos[0] - SIZE_X / 2; i <= player.pos[0] + SIZE_X / 2; i++) {
			for (j = player.pos_old[1] - SIZE_Y / 2; j < player.pos[1] - SIZE_Y / 2; j++) map[j][i] = 0;
			for (j = player.pos[1] - SIZE_Y / 2; j < player.pos[1]; j++) {
				xx = i - player.pos[0] + SIZE_X/2; yy = j - player.pos[1] + SIZE_Y/2;
				if ((int)PLANE[yy*SIZE_X + xx + 2] == BGC) map[j][i] = 0;
			}
		}
		for (i = player.pos[0] - SIZE_X / 2; i <= player.pos[0] + SIZE_X / 2; i++) {
			for (j = player.pos[1] + SIZE_Y / 2; j >= player.pos[1]; j--) {
				xx = i - player.pos[0] + SIZE_X/2; yy = j - player.pos[1] + SIZE_Y/2;
				if ((int)PLANE[yy*SIZE_X + xx + 2] != BGC) map[j][i] = 1;
			}
		}
	}
}
