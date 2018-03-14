/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>

#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>

struct player {
	int x, y;
	enum direction current_direction;
	int nb_bombs;
	int range;
	int life;
	int nb_key;
};

struct player* player_init(int bomb_number) {
	struct player* player = malloc(sizeof(*player));
	if (!player)
		error("Memory error");

	player->current_direction = SOUTH;
	player->nb_bombs = bomb_number;
	player->range = 2;
	player->life=2;
player->nb_key=0;
	return player;
}


void player_set_position(struct player *player, int x, int y) {
	assert(player);
	player->x = x;
	player->y = y;
}

int player_get_nb_key(struct player* player) {
	assert(player != NULL);
	return player->nb_key;
}

void player_free(struct player* player) {
	assert(player);
	free(player);
}

int player_get_x(struct player* player) {
	assert(player != NULL);
	return player->x;
}

int player_get_y(struct player* player) {
	assert(player != NULL);
	return player->y;
}

void player_set_current_way(struct player* player, enum direction way) {
	assert(player);
	player->current_direction = way;
}

int player_get_nb_bomb(struct player* player) {
	assert(player);
	return player->nb_bombs;
}

int player_get_range(struct player* player) {
	assert(player);
	return player->range;
}

int player_get_life(struct player* player) {
	assert(player);
	return player->life;
}

void player_inc_nb_bomb(struct player* player) {
	assert(player);
	player->nb_bombs += 1;
}

void player_inc_life(struct player* player) {
	assert(player);
	player->life += 1;
}

void player_dec_nb_bomb(struct player* player) {
	assert(player);
	player->nb_bombs -= 1;
}

void player_dec_life(struct player* player) {
	assert(player);
	player->life -= 1;
	if(player->life==0){
			SDL_Quit();
		}
}

static int player_move_aux(struct player* player, struct map* map, int x, int y, int z, int t) {

	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY:
		return 0;
		break;

	case CELL_BOX:
		if(map_get_cell_type(map, z, t)==CELL_EMPTY && map_is_inside(map,z,t)){
			map_set_cell_type(map, z, t, map_get_cell_type2(map, x, y));
			map_set_cell_type(map, x, y, CELL_EMPTY);
		return 1;
		}
		return 0;
		break;
	case CELL_KEY:
		map_set_cell_type(map, x, y, CELL_EMPTY);
		player->nb_key=player->nb_key+1;
		break;
	case CELL_DOOR:
		if(player->nb_key>0 && map_get_cell_type2(map,x,y)!=CELL_DOOR_OPENED){
		map_set_cell_type(map, x, y, CELL_DOOR_OPENED);
		player->nb_key=player->nb_key-1;
		}
		break;
	case CELL_BONUS:
		switch(map_get_cell_type2(map,x,y) & 0x0f){
		case BONUS_BOMB_RANGE_DEC:
			if(player->range>1){
			player->range=player->range-1;}
			map_set_cell_type(map, x, y, CELL_EMPTY);
			break;
		case BONUS_BOMB_RANGE_INC:
			if(player->range<9){
			player->range=player->range+1;}
			map_set_cell_type(map, x, y, CELL_EMPTY);
			break;
		case BONUS_BOMB_NB_DEC:
			if(player->nb_bombs>1){
			player_dec_nb_bomb(player);}
			map_set_cell_type(map, x, y, CELL_EMPTY);
			break;
		case BONUS_BOMB_NB_INC:
			if(player->nb_bombs<9){
			player_inc_nb_bomb(player);}
			map_set_cell_type(map, x, y, CELL_EMPTY);
			break;
		case BONUS_LIFE:
			if(player->life<9){
				player_inc_life(player);}
			map_set_cell_type(map, x, y, CELL_EMPTY);
			break;
		case BONUS_MONSTER:
			if(player->life>0){
				player_dec_life(player);}
				break;
		default:
			break;}

		break;

	case CELL_MONSTER:
		if(player->life>0){
			player_dec_life(player);}
			break;

	default:
		break;
	}

	// Player has moved
	return 1;
}

int player_move(struct player* player, struct map* map) {
	int x = player->x;
	int y = player->y;
	int move = 0;

	switch (player->current_direction) {
	case NORTH:

		if (player_move_aux(player, map, x, y - 1,x,y-2)) {
			player->y--;
			move = 1;
		}
		break;

	case SOUTH:

		if (player_move_aux(player, map, x, y + 1,x,y+2)) {
			player->y++;
			move = 1;
		}
		break;

	case WEST:

		if (player_move_aux(player, map, x - 1, y,x-2,y)) {
			player->x--;
			move = 1;
		}
		break;

	case EAST:
		if (player_move_aux(player, map, x + 1, y,x+2,y)) {
			player->x++;
			move = 1;
		}
		break;
	}
	return move;
}

void player_display(struct player* player) {
	assert(player);
	window_display_image(sprite_get_player(player->current_direction),
			player->x * SIZE_BLOC, player->y * SIZE_BLOC);
}


