#include "./main.h"

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
			if (player.pos[0] - player.size[0]/2 - player.delta[0] > 0) player.pos[0] -= player.delta[0];
		}
		else if (key == 4) { //right
			if (player.pos[0] + player.size[0]/2 + player.delta[0] < WIDTH) player.pos[0] += player.delta[0];
		}
		else if (key == 1) { //up
			if (player.pos[1] - player.size[1]/2 - player.delta[1] > 20) player.pos[1] -= player.delta[1];
		}
		else if (key == 3) { //down
			if (player.pos[1] + player.size[1]/2 + player.delta[1] < HEIGHT) player.pos[1] += player.delta[1];
		}
		Write_func(player);
	}
}

void Move_boss(int time)
{
	static int i = 1;
	static int j = 1;

	if (boss.hit == 1) {
		boss.life--;
		if (boss.life < 25) {boss.pow = 5; freq_b--;}
		if (boss.life < 10) boss.speed = boss.life/3;
	}
	if (time % 15 == 0) {i = rand()%3 - 1; j = rand()%3 - 1;}
	if ((boss.flag == 1) && boss.time >= boss.speed) {
		Clear_func(boss);
		boss.pos_old[0] = boss.pos[0]; boss.pos_old[1] = boss.pos[1];
		if (boss.pos[1] > 80) Boss_flag = 1;
		if (Boss_flag == 0)
			boss.pos[1] += boss.delta[1];
		else if (Boss_flag == 1) {
			boss.pos[0] += i * boss.delta[0]; boss.pos[1] += j * boss.delta[1];
			if (boss.pos[0]-boss.size[0]/2 < 0 || boss.pos[0]+boss.size[0]/2 >= WIDTH) boss.pos[0] = boss.pos_old[0];
			if (boss.pos[1]-boss.size[1]/2 < 20 || boss.pos[1]+-boss.size[0]/2 >= 100)	boss.pos[1] = boss.pos_old[1];
		}
		Write_func(boss);
		boss.move_flag = 1;
		boss.time = 0;
	}
}

void Move_obj()
{
	Move_func(item, &item_num);
	Move_func(enem, &enem_num);
	Move_func(bullet_e, &bul_num_e);
	Move_func(bullet_b, &bul_num_b);
	Move_func(bullet_p, &bul_num_p);
}

void Move_func(Obj obj[], int *obj_num)
{
	int i,temp = -1;

	for (i = 0; i < (*obj_num); i++) {
		if (obj[i].time >= obj[i].speed && (obj[i].flag == 1) && (obj[i].hit == 0)) {
			obj[i].pos_old[0] = obj[i].pos[0]; obj[i].pos_old[1] = obj[i].pos[1];
			Clear_func(obj[i]);
			obj[i].pos[0] += obj[i].delta[0]; obj[i].pos[1] += obj[i].delta[1];
			if ((obj[i].pos[0] < 0 || obj[i].pos[0] >= WIDTH) || (obj[i].pos[1] < 10 || obj[i].pos[1] >= HEIGHT)) {
				//y경계값 적 생성위치에 따라 문제될 수 있음
				temp = i;
				continue;
			}
			Write_func(obj[i]);
			obj[i].move_flag = 1;
			obj[i].time = 0;
		}
	}
	if (temp != -1) Del_obj(obj, temp, obj_num);
}

void Del_obj(Obj obj[], int i, int *obj_num)
{
	(*obj_num)--;
	obj[i] = obj[(*obj_num)];
	obj[(*obj_num)].flag = 0;
}

void Clear_func(Obj obj)
{
	int i,j;

	for (i = obj.pos[1] - obj.size[1]/2; i <= obj.pos[1] + obj.size[1]/2; i++) {
		for (j = obj.pos[0] - obj.size[0]/2; j <= obj.pos[0] + obj.size[0]/2; j++) {
			if (i < 0 || i >= HEIGHT || j < 0 || j >= WIDTH) continue;
			if (map[i][j] == obj.idx) map[i][j] = 0;
		}
	}
}

void Write_func(Obj obj)
{
	int i,j,xx,yy;

	for (i = obj.pos[1] - obj.size[1]/2; i <= obj.pos[1] + obj.size[1]/2; i++) {
		for (j = obj.pos[0] - obj.size[0]/2; j <= obj.pos[0] + obj.size[0]/2; j++) {
			xx = j - obj.pos[0] + obj.size[0]/2; yy = i - obj.pos[1] + obj.size[1]/2;
			if (i < 0 || i >= HEIGHT || j < 0 || j >= WIDTH) continue;
			if ((int)obj.fp[yy*obj.size[0] + xx + 2] != BGC) map[i][j] = obj.idx;
		}
	}
}
