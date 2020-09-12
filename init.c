#include "./main.h"

#include "./Image/bul01.h"
#include "./Image/bul02.h"
#include "./Image/expld1.h"
#include "./Image/expld2.h"
#include "./Image/expld3.h"
#include "./Image/pow.h"
#include "./Image/opening.h"
#include "./Image/boss1.h"
#include "./Image/gameover.h"
#include "./Image/gameclr.h"
#include "./Image/enemy2d.h"
#include "./Image/enemy1d.h"

const unsigned short * expld[] = {expld1, expld2, expld3};
const unsigned short * enemy_img[] = {enemy1d,enemy2d};

void Opening()
{
	int zz;

	zz = space2[1] - 240;
	Lcd_Clr_Screen(BLACK);
	Write_rect(70,50,250,150,11);
	Write_rect(70,200,250,220,11);
	Lcd_Draw_BMP(70,50,opening);
	Lcd_Printf(70,200,WHITE,BLACK, 1,1,"Press Any Key to Start");
	for (;;) {
		if (Key_Get_Pressed()) break;
		Draw_BackGround(0,zz,0,0,space2);
		if (zz > 0) zz -= 3;
		if (zz <= 0) zz = space2[1] - 240;
	}
	Write_rect(70,200,250,230,0);
	Write_rect(70,50,250,150,0);
}

void Write_rect(int x1, int y1, int x2, int y2, int n)
{
	int i,j;

	for (i=y1;i<y2;i++) {
		for (j=x1;j<x2;j++) {
			map[i][j] = n;
		}
	}
}

void Ending(int n)
{
	Timer4_Delay(1000);
	Lcd_Control_Brightness(1);
	if (n == 0)
		Lcd_Draw_BMP(0,60,gameover);
	else
		Lcd_Draw_BMP(0,60,gameclr);
	Lcd_Printf(100,160,WHITE,BLACK, 1,1,"Press <6> to Exit");
	while(Key_Get_Pressed() != 6);
}

void Var_init()
{
	memset(map,0,sizeof(map));
	memset(enem,0,sizeof(enem));
	memset(bullet_e,0,sizeof(bullet_e));
	memset(bullet_p,0,sizeof(bullet_p));
	memset(bullet_b,0,sizeof(bullet_b));
	memset(&boss,0,sizeof(boss));
	memset(item,0,sizeof(item));

	bul_num_e = 0;
	bul_num_p = 0;
	item_num =  0;
	enem_num = 0;
	Life = 10;
	srand(RTC_Get_Time());
	Lcd_Draw_Bar(0,0,WIDTH,20,0x1);
}

void Pos_init()
{
	Pos_x[0] = player.pos[0]; Pos_y[0] = player.pos[1] - player.size[1]/2 - (int)bul01[1]/2 - 5;
	Pos_x[1] = player.pos[0] - 10; Pos_y[1] = player.pos[1] - player.size[1]/2 - (int)bul01[1]/2 - 5;
	Pos_x[2] = player.pos[0] + 10; Pos_y[2] = player.pos[1] - player.size[1]/2 - (int)bul01[1]/2 - 5;

	Pos2_x[0] = boss.pos[0]; Pos2_y[0] = boss.pos[1] + boss.size[1]/2 + 7;
	Pos2_x[1] = boss.pos[0] -10; Pos2_y[1] = boss.pos[1] + boss.size[1]/2 + 7;
	Pos2_x[2] = boss.pos[0] +10; Pos2_y[2] = boss.pos[1] + boss.size[1]/2 + 7;
	Pos2_x[3] = boss.pos[0] - boss.size[0]/2 -5; Pos2_y[3] = boss.pos[1] + 7;
	Pos2_x[4] = boss.pos[0] + boss.size[0]/2 +5; Pos2_y[4] = boss.pos[1] + 7;
}

void Time_inc()
{
	int i;

	if (boss.flag == 1) boss.time++;
	for (i=0;i<bul_num_p;i++) bullet_p[i].time++;
	for (i=0;i<bul_num_e;i++) bullet_e[i].time++;
	for (i=0;i<bul_num_b;i++) bullet_b[i].time++;
	for (i=0;i<enem_num;i++) enem[i].time++;
	for (i=0;i<item_num;i++) item[i].time++;
}

void Player_init()
{
	player.flag = 1;
	player.idx = 1;
	player.pos[0] = START_X; player.pos[1] = START_Y;
	player.pos_old[0] = START_X; player.pos_old[1] = START_Y;
	player.fp = PLANE;
	Lcd_Get_Info_BMP(&player.size[0], &player.size[1], player.fp);
	player.delta[0] = 10; player.delta[1] = 10;
	player.move_flag = 1;
	player.hit = 0;
	player.pow = 1;
	Write_func(player);
}

void Boss_init(int time)
{
	if (time > 300 && boss.flag == 0) {
		boss.flag = 1;
		boss.idx = 9;
		boss.pos[0] = START_X; boss.pos[1] = 0;
		boss.fp = boss1;
		Lcd_Get_Info_BMP(&boss.size[0], &boss.size[1], boss.fp);
		boss.delta[0] = 10; boss.delta[1] = 10;
		boss.move_flag = 1;
		boss.time = 0;
		boss.speed = 5;
		boss.hit = 0;
		boss.pow = 1;
		boss.life = 50;
		boss.pow = 3;
		Write_func(boss);
	}
}

void Enemy_init(int time)
{
	int i,pos,temp;

	if (time < 200 && time % 50 == 0) {
		if (enem_num < 10) enem_num++; //
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
				enem[i].speed = 2;
				enem[i].time = 0;
				enem[i].hit = 0;
				last_pos = enem[i].pos[0];
				Write_func(enem[i]);
			}
		}
	}
}

void Bullet_init(int time)
{
	int i,j;

	if (time % 70 == 0) { //적 총알
		for (j=0;j<enem_num;j++) {
			if (bul_num_e < 50) bul_num_e++;
			for (i = 0; i < bul_num_e; i++) {
				if (bullet_e[i].flag == 0) {
					bullet_e[i].flag = 1;
					bullet_e[i].idx = 4;
					bullet_e[i].move_flag = 1;
					bullet_e[i].fp = bul02;
					Lcd_Get_Info_BMP(&bullet_e[i].size[0], &bullet_e[i].size[1], bullet_e[i].fp);
					bullet_e[i].pos[0] = enem[j].pos[0];
					bullet_e[i].pos[1] = enem[j].pos[1] + enem[j].size[1]/2 + bullet_e[i].size[1]/2 + 5;
					bullet_e[i].delta[0] = 0; bullet_e[i].delta[1] = 15;
					bullet_e[i].speed = 2;
					bullet_e[i].time = 0;
					bullet_e[i].hit = 0;
					Write_func(bullet_e[i]);
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
				bullet_p[i].delta[0] = j == 0 ? 0 : (j%2 ? -2 : 2);
				bullet_p[i].delta[1] = -15;
				bullet_p[i].speed = 1;
				bullet_p[i].time = 0;
				bullet_p[i].hit = 0;
				Write_func(bullet_p[i]);
				--j;
			}
		}
	}
	if (time % 50 == 0 && boss.flag == 1) { //보스 총알
		if (bul_num_b < 50) bul_num_b+=boss.pow;
		j = boss.pow -1;
		for (i = 0; i < bul_num_b; i++) {
			if (bullet_b[i].flag == 0) {
				bullet_b[i].flag = 1;
				bullet_b[i].idx = 10;
				bullet_b[i].move_flag = 1;
				bullet_b[i].fp = bul02;
				Lcd_Get_Info_BMP(&bullet_b[i].size[0], &bullet_b[i].size[1], bullet_b[i].fp);
				bullet_b[i].pos[0] = Pos2_x[j]; bullet_b[i].pos[1] = Pos2_y[j];
				bullet_b[i].delta[1] = 15;
				if (j == 0) bullet_b[i].delta[0] = 0;
				else if (j == 1) bullet_b[i].delta[0] = -2;
				else if (j == 2) bullet_b[i].delta[0] = 2;
				else if (j == 3) {bullet_b[i].delta[0] = -3; bullet_b[i].delta[1] = 10;}
				else if (j == 4) {bullet_b[i].delta[0] = 3; bullet_b[i].delta[1] = 10;}
				bullet_b[i].speed = 2;
				bullet_b[i].time = 0;
				bullet_b[i].hit = 0;
				Write_func(bullet_b[i]);
				--j;
			}
		}
	}
}

void Item_init(int time)
{
	int i,temp;

	if (time % 80 == 0 && boss.flag == 0) {//아이템 등장 조건 조절
		if (item_num == 0) item_num++;
		for (i=0;i<item_num;i++) {
			temp = rand()%10 > 3 ? 1 : 0;
			if (item[i].flag == 0) {
				item[i].flag = 1;
				item[i].idx = temp ? 6 : 5;
				item[i].move_flag = 1;
				item[i].fp = temp ? heart : pow;
				Lcd_Get_Info_BMP(&item[i].size[0], &item[i].size[1], item[i].fp);
				item[i].pos[0] = rand()%280+20; item[i].pos[1] = 10;
				item[i].delta[0] = 0; item[i].delta[1] = temp ? 20 : 15;
				item[i].speed = 10;
				item[i].time = 0;
				item[i].hit = 0;
				Write_func(item[i]);
			}
		}
		Item_Lock = 0;
	}
}
