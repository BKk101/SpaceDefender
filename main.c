#include "./minilibx/mlx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#define WIDTH	320
#define HEIGHT	240
#define PI		3.141592
#define ANGLE	PI/4.0
#define G		9.81
#define GREEN	0x0000FF00
typedef struct Object
{
	unsigned short color;
	int timer;
	int pos[2];
	int pos_old[2];
	int pos_delt[2];
	int size[2];
	int speed_step;
	int move_step;
} Object;

typedef struct vars
{
	void	*mlx;
	void	*win;
	void	*img;
	clock_t	time;
	int		obj_num;
	int		pause;
	Object	bullet[20];
}	t_vars;


int render_next_frame(t_vars *vars)
{
	int i;
	long long a = clock();
	if (vars->pause == 0) {
		for (i=0;i<10;i++) {
			vars->bullet[i].timer++;
			if (vars->bullet[i].timer >= vars->bullet[i].speed_step) {
				vars->bullet[i].timer = 0;
				vars->bullet[i].pos_old[0] = vars->bullet[i].pos[0];
				vars->bullet[i].pos_old[1] = vars->bullet[i].pos[1];
				vars->bullet[i].pos[0] = vars->bullet[i].pos[0] + vars->bullet[i].pos_delt[0] * vars->bullet[i].move_step;
				vars->bullet[i].pos[1] = vars->bullet[i].pos[1] + vars->bullet[i].pos_delt[1] * vars->bullet[i].move_step; 
		
				mlx_pixel_put(vars->mlx,vars->win,vars->bullet[i].pos_old[0],vars->bullet[i].pos_old[1],0x00000000);
				mlx_pixel_put(vars->mlx,vars->win,vars->bullet[i].pos[0],vars->bullet[i].pos[1],vars->bullet[i].color);
			}
		}
	}
	return 0;
}

int	keyboard(int keycode, t_vars *vars)
{
	printf("key pressed\n");
	if (keycode == 53) {
		mlx_destroy_window(vars->mlx,vars->win);
		exit(0);
	}
	if (keycode == 49) {
		vars->pause = vars->pause ? 0 : 1;
	}
	return 0;
}

void make_bullet(t_vars *vars)
{
	int i;
	for (i=0;i<10;i++) {
		vars->bullet[i].color = GREEN;
		vars->bullet[i].pos[0] = i/5 ? 320:0;
		vars->bullet[i].pos[1] = rand() % 200;
		vars->bullet[i].pos_delt[0] = i/5 ? -1 : 1;
		vars->bullet[i].pos_delt[1] = (double)rand()/RAND_MAX * 10 - 5;
		vars->bullet[i].size[0] = 5;
		vars->bullet[i].size[1] = 5;
		vars->bullet[i].timer = 0;
		vars->bullet[i].speed_step = rand()%10 +1;
		vars->bullet[i].move_step = rand()%5 + 1;
		printf("%d  x: %d y: %d\n", i, vars->bullet[i].pos[0], vars->bullet[i].pos[1]);
	}
}

int main(void)
{
	t_vars	vars;
	srand((unsigned int)time(0));
	int wid = 50;
	int hei = 50;
	vars.mlx = mlx_init();
	vars.win = mlx_new_window(vars.mlx,WIDTH,HEIGHT,"new");
	vars.time = 0;
	vars.pause = 0;
	make_bullet(&vars);
	mlx_hook(vars.win, 2, 0, keyboard, &vars);
	mlx_loop_hook(vars.mlx, render_next_frame, &vars);
	mlx_loop(vars.mlx);
	//a
	return 0;
}	