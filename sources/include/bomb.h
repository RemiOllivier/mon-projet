#ifndef BOMB_H_
#define BOMB_H_
#include <map.h>
#include <constant.h>
#include <game.h>
struct bomb;
struct bomb* bomb_init(unsigned int number,int x, int y);
void   bomb_free(struct bomb* bomb);
int get_time_bomb(struct bomb* bomb);
int get_posy_bomb(struct bomb* bomb);
int get_posx_bomb(struct bomb* bomb);
void set_time_bomb(unsigned int time, struct bomb* bomb);
void set_posy_bomb(int posy, struct bomb* bomb);
void set_posx_bomb(int posx, struct bomb* bomb);
//void bomb_display(int x,int y,unsigned int timer,struct game* game);

#endif

