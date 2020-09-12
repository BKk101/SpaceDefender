
/***********************************************************/
// 다음은 사용자 함수를 호출하는 루틴임 지우지 말것!
/***********************************************************/

#include "device_driver.h"
#include <stdlib.h>
#include <string.h>
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
	//Lcd_Clr_Screen(0xf800);

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

#include "./main.h"

Obj player;
Obj boss;
Obj enem[20];
Obj bullet_p[20];
Obj bullet_e[50];
Obj bullet_b[50];
Obj item[10];
int map[240][320];
int enem_num;
int item_num;
int bul_num_p;
int bul_num_e;
int bul_num_b;
int Life;
int last_pos;
int Item_Lock;
int Pos_x[5], Pos_y[5];
int Pos2_x[5], Pos2_y[5];
int zzz;
/*
	Uart_Printf("bul num %d\n", bul_num_p);
				for (i=0;i<bul_num_p;i++) {
					if (bullet_p[i].flag == 1) Uart_Printf("i: %d x: %d y :%d\n", i, bullet_p[i].pos[0], bullet_p[i].pos[1]);
				}
*/
void User_Main(void)
{
	int time;
	int zz;

	Lcd_Control_Brightness(5);

Start_P:
	Opening();
	Var_init();
	Player_init();
	Timer0_Repeat(20);
	time = 0;
	zz = space2[1] - 220;
	for (;;)
	{
		zzz = zz;
		Pos_init();
		if (Life <= 0) {Ending(0); goto Start_P;}
		if (boss.flag == 1 && boss.life < 1) {Ending(1); goto Start_P;}
		if (Timer0_Check_Expired()) {
			if (time == 0xFFFFFFFF) time = 0;
			time++;
			Time_inc();
			Boss_init(time);
			Enemy_init(time);
			Bullet_init(time);
			Item_init(time);
			Check_crush();
			Move_player(zz);
			Move_boss(time);
			Move_obj();
		}
		Draw_obj();
		Draw_boss();
		Draw_player(zz);
		Draw_BackGround(0,zz,0,20,space2);
		if (zz > 0) zz--;
		if (zz <= 0) zz = space2[1] - 220;
	}
}

void Check_crush()
{
	int i,idx;

	if (boss.flag == 1) {
		idx = Check_Range(boss.pos[0], boss.pos[1], boss.size[1]/2 + 5, bullet_p, bul_num_p); //플레이어 총알이 보스에 맞았는지 확인
		if (idx != -1) {
			boss.hit = 1;
			bullet_p[idx].hit = 1;
		}
		Player_crush(boss, 3);
	}
	for (i=0;i<bul_num_e;i++) Player_crush(bullet_e[i], 2); //적총알이 플레이어에 맞았는지 확인
	for (i=0;i<bul_num_b;i++) Player_crush(bullet_b[i], 2); //보스 총알이 플레이어에 맞았는지 확인
	for (i=0;i<enem_num;i++) { //적이 플레이어 총알이나 본체에 맞았는지 확인
		idx = Check_Range(enem[i].pos[0], enem[i].pos[1], enem[i].size[1]/2 + 6, bullet_p, bul_num_p);
		if (idx != -1) {
			enem[i].hit = 1;
			bullet_p[idx].hit = 1;
		}
		Player_crush(enem[i], 3);
	}
	for (i=0;i<item_num;i++) { //아이템이 플레이어에 맞았는지 확인
		idx = Check_Range(item[i].pos[0], item[i].pos[1], item[i].size[1]/2 + 5, &player, 1);
		if (idx != -1) {
			item[i].hit = 1;
			if (item[i].idx == 5 && Item_Lock == 0)
				player.pow = 3;
			else if (item[i].idx == 6 && Item_Lock == 0)
				if (Life < 6) Life++;
			Item_Lock = 1;
		}
	}
}

void Player_crush(Obj obj, int range)
{
	int idx;

	idx = Check_Range(obj.pos[0], obj.pos[1], obj.size[1]/2 + range, &player, 1);
	if (idx != -1) {
		obj.hit = 1;
		Player_hit();
	}
}

int Check_Range(int x, int y, int range, Obj *obj, int num)
{
	int i,j,k;
	int xx,yy;

	for (i=0;i<num;i++) {
		for (j=-range;j<=range;j++) {
			for (k=-range;k<=range;k++) {
				if (map[y+j][x+k] == (obj+i)->idx) {
					xx = x+k; yy = y+j;
					if ((abs(xx-(obj+i)->pos[0]) < (obj+i)->size[0]+1) && (abs(yy-(obj+i)->pos[1]) < (obj+i)->size[1]+1))
						return i;
				}
			}
		}
	}
	return -1;
}

void Player_hit()
{
	player.hit = 1;
	Lcd_Draw_Bar(0,0,WIDTH,20,0x1);
	if (Life > 0 && player.flag == 1) Life--;
};

