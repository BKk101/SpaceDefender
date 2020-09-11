#include "./main.h"

void Lcd_Draw_BMP2(int x, int y, const unsigned short int *fp)
{
	register int width = fp[0], height = fp[1];
	register int xx, yy;

	for(yy=0;yy<height;yy++){
		for(xx=0;xx<width;xx++){
			if (yy+y<20) continue;
			Lcd_Put_Pixel(xx+x,yy+y,(int)fp[yy*width+xx+2]);
		}
	}
}

void Draw_BackGround(int z, int a, int x, int y, const unsigned short int *fp)
{
	register int width = fp[0]; //height = fp[1];
	register int xx, yy;

	for (yy = 0; yy < 240-y; yy++) {
		for (xx = 0; xx < width; xx++) {
			if (xx+x<0 || xx+x>=WIDTH || yy+y<0 || yy+y>=HEIGHT) continue;
			if (map[yy+y][xx+x] == 0)
				Lcd_Put_Pixel(xx + x, yy + y, (int)fp[(a + yy)*width + (xx + z) + 2]);
		}
	}
}

void Draw_player(void)
{
	int i;
	static int time = 0;

	Lcd_Set_Shape_Mode(1, BGC);
	for (i=0;i<Life;i++) Lcd_Draw_BMP(10*(i+1)+13*i,4,heart);
	if (player.flag == 1) {
		Lcd_Draw_BMP2(player.pos[0] - player.size[0]/2, player.pos[1] - player.size[1]/2, player.fp);
		player.move_flag = 0;
	}
	else { //플레이어 비행기 총알에 맞았을 때 폭발효과 표현
		++time;
		Lcd_Draw_BMP2(player.pos[0] - player.size[0]/2, player.pos[1] - player.size[1]/2, player.fp);
		for (i=0;i<3;i++) Lcd_Draw_BMP2(player.pos[0]-15, player.pos[1]-15, expld[i]);
		for (i=0;i<3;i++) Lcd_Draw_BMP2(player.pos[0]+5, player.pos[1]+5, expld[i]);
		Clear_func(player); //깜빡임 표현
		if (time > 12) {
			time = 0;
			Player_init();
		}
	}
	Lcd_Set_Shape_Mode(0,0);
}

void Draw_boss()
{
	int i;

	Lcd_Set_Shape_Mode(1, BGC);
	if (boss.flag == 1) {
		Lcd_Draw_Bar(248,4,302,16,WHITE);
		Lcd_Draw_Bar(249,5,249+boss.life,15,BLUE);
		Lcd_Draw_Bar(250+boss.life,5,301,15,RED);
		if (boss.hit == 1) {
			for (i=0;i<3;i++) Lcd_Draw_BMP2(boss.pos[0], boss.pos[1]+25, expld[i]);
			boss.hit = 0;
		}
		else
			Lcd_Draw_BMP2(boss.pos[0] - boss.size[0]/2, boss.pos[1] - boss.size[1]/2, boss.fp);
		boss.move_flag = 0;
	}
	Lcd_Set_Shape_Mode(0,0);
}

void Draw_obj(void)
{
	Lcd_Set_Shape_Mode(1, BGC);
	Draw_func(bullet_p, &bul_num_p);
	Draw_func(bullet_e, &bul_num_e);
	Draw_func(bullet_b, &bul_num_b);
	Draw_func(item, &item_num);
	Draw_func(enem, &enem_num);
	Lcd_Set_Shape_Mode(0, 0);
}

void Draw_func(Obj obj[], int *obj_num)
{
	int i,j;
	static int time = 0;

	for (i=0;i<(*obj_num);i++) {
		if (obj[i].hit == 1) {//물체가 맞은경우
			Clear_func(obj[i]);
			if (obj[i].idx == 3) { //적 비행기가 총알에 맞았을 때 폭발효과 표현
				++time;
				for (j=0;j<3;j++) Lcd_Draw_BMP2(obj[i].pos[0]-7, obj[i].pos[1]-7, expld[j]);
				if (time > 5) {
					(*obj_num)--;
					time = 0; obj[i].flag = 0; obj[i].hit = 0;
				}
			}
			else { //적 비행기 이외의 물체
				(*obj_num)--;
				obj[i].flag = 0;
				obj[i].hit = 0;
			}
		}
		if ((obj[i].flag == 1) && (obj[i].move_flag == 1)) { //맞지 않은 경우
			obj[i].move_flag = 0;
			Lcd_Draw_BMP2(obj[i].pos[0] - obj[i].size[0]/2, obj[i].pos[1] - obj[i].size[1]/2, obj[i].fp);
		}
	}
}
