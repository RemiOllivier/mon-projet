#include <assert.h>
#include <time.h>
#include <game.h>
#include <misc.h>
#include <window.h>
#include <sprite.h>
#include <map.h>

struct bomb {
	int posx;
	int posy;
	unsigned int time;
};

struct bomb* bomb_init(unsigned int number,int x, int y) {
	struct bomb* bomb = malloc(sizeof(*bomb));
	bomb->time=number;
	bomb->posx=x;
	bomb->posy=y;
	if (!bomb)
		error("Memory error");

	return bomb;
}

void bomb_free(struct bomb* bomb) {
	assert(bomb);
	free(bomb);
}


int get_time_bomb(struct bomb* bomb){
	return bomb->time;
}

int get_posx_bomb(struct bomb* bomb){
	return bomb->posx;
}

int get_posy_bomb(struct bomb* bomb){
	return bomb->posy;
}

void set_time_bomb(unsigned int time, struct bomb* bomb){
	bomb->time=time;
}

void set_posx_bomb(int posx, struct bomb* bomb){
	bomb->posx=posx;
}

void set_posy_bomb(int posy, struct bomb* bomb){
	bomb->posy=posy;
}

