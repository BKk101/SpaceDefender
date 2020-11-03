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

void Draw_BackGround(int a, int z, int x, int y, const unsigned short int *fp)
{
	register int width = fp[0]; //height = fp[1];
	register int xx, yy;

	for (yy = 0; yy < 240-y; yy++) {
		for (xx = 0; xx < width; xx++) {
			if (xx+x<0 || xx+x>=WIDTH || yy+y<20 || yy+y>=HEIGHT) continue;
			if (map[yy+y][xx+x] == 0)
				Lcd_Put_Pixel(xx + x, yy + y, (int)fp[(z + yy)*width + (xx + a) + 2]);
		}
	}
}

void Erase_obj(Obj obj, int z)
{
	int xx, yy;

	for (yy = obj.pos_old[1]-obj.size[1]/2; yy <= obj.pos_old[1]+obj.size[1]/2; yy++) {
		for (xx = obj.pos_old[0]-obj.size[0]/2; xx <= obj.pos_old[0]+obj.size[0]/2; xx++) {
			if (xx<0 || xx>=WIDTH || yy<20 || yy>=HEIGHT) continue;
			if (map[yy][xx] == 0) {
				Lcd_Put_Pixel(xx, yy, (int)space2[(z + yy)*320 + (xx) + 2]);
			}
		}
	}
}

void Draw_player()
{
	int i;
	static int time = 0;

	if (player.move_flag == 1) Erase_obj(player, zzz);
	Lcd_Set_Shape_Mode(1, BGC);
	for (i=0;i<Life;i++) Lcd_Draw_BMP(10*(i+1)+13*i,4,heart);
	if (player.flag == 1) {
		Lcd_Draw_BMP2(player.pos[0] - player.size[0]/2, player.pos[1] - player.size[1]/2, player.fp);
		player.move_flag = 0;
	}
	else { //�÷��̾� ����� �Ѿ˿� �¾��� �� ����ȿ�� ǥ��
		++time;
		Lcd_Draw_BMP2(player.pos[0] - player.size[0]/2, player.pos[1] - player.size[1]/2, player.fp);
		for (i=0;i<3;i++) Lcd_Draw_BMP2(player.pos[0]-15, player.pos[1]-15, expld[i]);
		for (i=0;i<3;i++) Lcd_Draw_BMP2(player.pos[0]+5, player.pos[1]+5, expld[i]);
		Clear_func(player); //������ ǥ��
		if (time > 8) {
			time = 0;
			Player_init();
		}
	}
	Lcd_Set_Shape_Mode(0,0);
}

void Draw_boss()
{
	int i;

	if (boss.move_flag == 1) Erase_obj(boss, zzz);
	Lcd_Set_Shape_Mode(1, BGC);
	if (boss.flag == 1) {
		Lcd_Draw_Bar(248,4,302,16,WHITE);
		Lcd_Draw_Bar(249,5,249+boss.life,15,BLUE);
		Lcd_Draw_Bar(250+boss.life,5,301,15,RED);
		if (boss.hit == 1) {
			for (i=0;i<3;i++) Lcd_Draw_BMP2(boss.pos[0]+10, boss.pos[1]+15, expld[i]);
			for (i=0;i<3;i++) Lcd_Draw_BMP2(boss.pos[0]-15, boss.pos[1], expld[i]);
			for (i=0;i<3;i++) Lcd_Draw_BMP2(boss.pos[0]+5, boss.pos[1]-25, expld[i]);
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
	Draw_func(item, &item_num);
	Draw_func(enem, &enem_num);
	Draw_func(bullet_e, &bul_num_e);
	Draw_func(bullet_b, &bul_num_b);
	Draw_func(bullet_p, &bul_num_p);
	Lcd_Set_Shape_Mode(0, 0);
}

void Draw_func(Obj obj[], int *obj_num)
{
	int i,j,num;
	static int time = 0;

	num = *obj_num;
	for (i=0;i<num;i++) {
		if (obj[i].hit == 1) {//��ü�� �������
			Clear_func(obj[i]);
			if (obj[i].idx == 3) { //�� ����Ⱑ �Ѿ˿� �¾��� �� ����ȿ�� ǥ��
				++time;
				for (j=0;j<3;j++) Lcd_Draw_BMP2(obj[i].pos[0]-7, obj[i].pos[1]-7, expld[j]);
				if (time > 5) {
					Del_obj(obj, i, obj_num); obj[i].hit = 0; time = 0;
				}
			}
			else { //�� ����� �̿��� ��ü
				Del_obj(obj, i, obj_num); obj[i].hit = 0;
			}
		}
		if ((obj[i].flag == 1) && (obj[i].move_flag == 1)) { //���� ���� ���
			Erase_obj(obj[i], zzz);
			obj[i].move_flag = 0;
			Lcd_Draw_BMP2(obj[i].pos[0] - obj[i].size[0]/2, obj[i].pos[1] - obj[i].size[1]/2, obj[i].fp);
		}
	}
}
