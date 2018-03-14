/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <assert.h>
#include <time.h>

#include <game.h>
#include <misc.h>
#include <window.h>
#include <sprite.h>
#include <map.h>
#include <bomb.h>
#include <monster.h>

struct game {
	struct map** maps;       // the game's map
	short max_levels;        // nb maps of the game
	short current_level;
	struct player* player;
	//struct monster* monster;
	//struct bomb* bomb;
};


struct game*
game_new(void){
	sprite_load(); // load sprites into process memory

	struct game* game = malloc(sizeof(*game));
	game->maps = malloc(sizeof(struct game));
	game->maps[0] = map_get_static();
	game->max_levels = 1;
	game->current_level = 0;
    //game->bomb=NULL;
	game->player = player_init(2);
	// Set default location of the player
	player_set_position(game->player, 1, 0);
	//game->monster = monster_init();
	//monster_set_position(game->monster,7,7);
	return game;
}

void game_free(struct game* game) {
	assert(game);

	player_free(game->player);
	for (int i = 0; i < game->max_levels; i++)
		map_free(game->maps[i]);
}

struct map* game_get_current_map(struct game* game) {
	assert(game);
	return game->maps[game->current_level];
}


struct player* game_get_player(struct game* game) {
	assert(game);
	return game->player;
}

/*struct monster* game_get_monster(struct game* game) {
	assert(game);
	return game->monster;
}*/

/*struct bomb* game_get_bomb(struct game* game) {
	assert(game);
	return game->bomb;
}*/

void game_banner_display(struct game* game) {
	assert(game);

	struct map* map = game_get_current_map(game);
	struct player* player = game_get_player(game);

	int y = (map_get_height(map)) * SIZE_BLOC;
	for (int i = 0; i < map_get_width(map); i++)
		window_display_image(sprite_get_banner_line(), i * SIZE_BLOC, y);

	int white_bloc = ((map_get_width(map) * SIZE_BLOC) - 6 * SIZE_BLOC) / 6;
	int x = white_bloc;
	y = (map_get_height(map) * SIZE_BLOC) + LINE_HEIGHT;
	window_display_image(sprite_get_banner_life(), x, y);

	x = white_bloc + SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_life(player)), x, y);

	x = 2 * white_bloc + 2 * SIZE_BLOC;
	window_display_image(sprite_get_banner_bomb(), x, y);

	x = 2 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_nb_bomb(player)), x, y);

	x = 3 * white_bloc + 4 * SIZE_BLOC;
	window_display_image(sprite_get_banner_range(), x, y);

	x = 3 * white_bloc + 5 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_range(player)), x, y);
	x = 4 * white_bloc + 6 * SIZE_BLOC;
	window_display_image(sprite_get_key(), x, y);

	x = 4 * white_bloc + 7 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_nb_key(player)), x, y);
}

void game_display(struct game* game) {
	assert(game);
    struct map* map=game_get_current_map(game);
	window_clear();
	game_banner_display(game);
	map_display(map);
	player_display(game->player);
	for (int i = 0; i < map_get_width(map); i++) {
		  for (int j = 0; j < map_get_height(map); j++) {
		    int x = i * SIZE_BLOC;
		    int y = j * SIZE_BLOC;

		    unsigned char type = map_get_cell_type(map,i,j);
		    unsigned int timer = map_get_time(map,i,j);

		    switch (type & 0xf0) {
		    case CELL_BOMB:
		    	bomb_display(x,y,timer,game);
		    	break;
		    case CELL_MONSTER:
		    	monster_direction(game,i,j);
		    	break;
		    default:
		    	break;
		    }}}
	window_refresh();
}


static short input_keyboard(struct game* game) {
	SDL_Event event;
	struct player* player = game_get_player(game);
	struct map* map = game_get_current_map(game);


	while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				return 1;
			case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return 1;
			case SDLK_UP:
				player_set_current_way(player, NORTH);
				player_move(player, map);
				break;
			case SDLK_DOWN:
				player_set_current_way(player, SOUTH);
				player_move(player, map);
				break;
			case SDLK_RIGHT:
				player_set_current_way(player, EAST);
				player_move(player, map);
				break;
			case SDLK_LEFT:
				player_set_current_way(player, WEST);
				player_move(player, map);
				break;
			case SDLK_SPACE:
				if(player_get_nb_bomb(player)>0 && map_get_cell_type(map,player_get_x(player),player_get_y(player))!=CELL_DOOR){
					player_dec_nb_bomb(player);
		    	map_set_cell_type(map,player_get_x(player),player_get_y(player),CELL_BOMB);}
		    	break;

			default:
				break;
			}

			break;
		}
	}
	return 0;
}

void monster_direction(struct game* game, int x, int y) {
	struct map* map = game_get_current_map(game);
	int a= rand()%4;//a is a random variable
	unsigned int actual= SDL_GetTicks();
	unsigned int instant = map_get_time(map,x,y);

	if((actual-instant)>500) {


			switch (a) {

			case 0:
				monster_move(game,x,y,NORTH);
				break;
			case 1:
				monster_move(game,x,y,SOUTH);
				break;
			case 2:
				monster_move(game,x,y,EAST);
				break;
			case 3:
				monster_move(game,x,y,WEST);
				break;

				default:

				break;
			}
		}
}

int game_update(struct game* game) {
	if (input_keyboard(game)){
	return 1;}
			 	 				//exit game
	return 0;
}

void bomb_display(int x,int y,unsigned int timer,struct game* game) {
	unsigned int actual=SDL_GetTicks();
	struct player* player=game_get_player(game);
	struct map* map= game_get_current_map(game);
	int size_explosion=player_get_range(player);
	int i;
	if (actual-timer>1000 && actual-timer <2000)
	{
	window_display_image(sprite_get_banner_bomb3(),x , y);
	}
	else if (actual-timer>2000 && actual-timer<3000 )
	{
	window_display_image(sprite_get_banner_bomb2(),x , y);
	}
	else if (actual-timer>3000 && actual-timer <4000)
	{
	window_display_image(sprite_get_banner_bomb1(),x , y);
	}
	else if (actual-timer>0 && actual-timer<1000)
	{
	window_display_image(sprite_get_banner_bomb4(),x , y);
	}
	else if (actual-timer>4000 && actual-timer<4300)
	{
		window_display_image(sprite_get_explosion(),x , y);
		for(i=1; i<=size_explosion; i++){
			i=game_bomb_explosion(map,i+x/SIZE_BLOC, y/SIZE_BLOC,i);}
		for(i=1; i<=size_explosion; i++){
			i=game_bomb_explosion(map,x/SIZE_BLOC-i, y/SIZE_BLOC,i);}
		for(i=1; i<=size_explosion; i++){
			i=game_bomb_explosion(map,x/SIZE_BLOC, y/SIZE_BLOC-i,i);}
		for(i=1; i<=size_explosion; i++){
			if(y/SIZE_BLOC+i< STATIC_MAP_HEIGHT){
			i=game_bomb_explosion(map ,x/SIZE_BLOC, y/SIZE_BLOC+i,i);}}
	}
		else if (actual-timer>4300){
			map_set_cell_type(map,x/SIZE_BLOC, y/SIZE_BLOC,CELL_EMPTY);
			player_inc_nb_bomb(player);
			for(i=1; i<=size_explosion; i++){
				if(x/SIZE_BLOC+i<STATIC_MAP_WIDTH){
						i=game_bomb_destruction(map,player,i+x/SIZE_BLOC, y/SIZE_BLOC,i);}}
					for(i=1; i<=size_explosion; i++){
						if(x/SIZE_BLOC-i>=0){
						i=game_bomb_destruction(map,player,x/SIZE_BLOC-i, y/SIZE_BLOC,i);}}
					for(i=1; i<=size_explosion; i++){
						if(y/SIZE_BLOC-i>=0){
						i=game_bomb_destruction(map,player, x/SIZE_BLOC, y/SIZE_BLOC-i,i);}}
					for(i=1; i<=size_explosion; i++){
						if(y/SIZE_BLOC+i< STATIC_MAP_HEIGHT){
						i=game_bomb_destruction(map, player, x/SIZE_BLOC, y/SIZE_BLOC+i,i);}}
	}
}
int game_bomb_explosion(struct map* map, int x, int y, int i){
	switch (map_get_cell_type(map, x, y)) {
					case CELL_BOX:
						window_display_image(sprite_get_explosion(),x*SIZE_BLOC , y*SIZE_BLOC);
						i=11;
						return i;
						break;
					case CELL_BONUS:
						window_display_image(sprite_get_explosion(),x*SIZE_BLOC, y*SIZE_BLOC);
						return i;
						break;
					case CELL_MONSTER:
						window_display_image(sprite_get_explosion(),x*SIZE_BLOC , y*SIZE_BLOC);
						return i;
						break;
					case CELL_EMPTY:
					window_display_image(sprite_get_explosion(),x*SIZE_BLOC , y*SIZE_BLOC);
				    return i;
					break;
					default:
						i=11;
						return i;
						break;
				}}

int game_bomb_destruction(struct map* map, struct player* player,int x, int y, int i){
	if(player_get_x(player)==x && player_get_y(player)==y){
				player_dec_life(player);
			}
	switch (map_get_cell_type(map, x, y)) {
					case CELL_BOX:
						map_change_cell_type(map,x,y,map_get_cell_type2(map,x,y));
						i=11;
						return i;
						break;
					case CELL_BONUS:
						map_set_cell_type(map,x,y,CELL_EMPTY);
						return i;
						break;
					case CELL_MONSTER:
						map_set_cell_type(map,x,y,CELL_EMPTY);
						map_set_time(map,x,y,0);
						return i;
					break;
					case CELL_EMPTY:
						map_set_cell_type(map,x,y,CELL_EMPTY);
						return i;
					default:
						i=11;
						return i;
						break;
				}

}

int monster_move_aux(struct game* game, int x, int y, int z, int t) {
struct map* map=game_get_current_map(game);
struct player* player=game_get_player(game);
if (!map_is_inside(map, z, t)){
		return 0;}
if(player_get_x(player)==z && player_get_y(player)==t){
	player_dec_life(player);
}

	switch (map_get_cell_type(map, z, t)) {
	case CELL_SCENERY:
		return 0;
		break;

	case CELL_BOX:
		return 0;
		break;

	case CELL_BONUS:
		return 0;
		break;

	case CELL_BOMB:
			return 0;
			break;

	case CELL_KEY:
			return 0;
			break;

	case CELL_DOOR:
				return 0;
				break;

	case CELL_MONSTER:
		return 0;
		break;

	default:
		return 1;
		break;
	}

	// monster has moved
}

void monster_move(struct game* game, int x, int y, char current_direction) {
	int z = x-1;
	int t =y-1;
	int u =x+1;
	int v =y+1;
struct map* map=game_get_current_map(game);
	switch (current_direction) {
	case NORTH:

		if (monster_move_aux( game, x, y ,x,t)) {

			map_set_cell_type(map, x, t, CELL_MONSTER);
			window_display_image(sprite_get_monster(NORTH),x*SIZE_BLOC , t*SIZE_BLOC);
			map_set_time(map,x,t,SDL_GetTicks());
			map_set_time(map,x,y,0);

			map_set_cell_type(map, x, y, CELL_EMPTY);
		}
		break;

	case SOUTH:

		if (monster_move_aux(game, x, y,x,v)) {

			map_set_cell_type(map, x, v, CELL_MONSTER);
			window_display_image(sprite_get_monster(SOUTH),x*SIZE_BLOC , v*SIZE_BLOC);
			map_set_time(map,x,v,SDL_GetTicks());
			map_set_time(map,x,y,0);

			map_set_cell_type(map, x, y, CELL_EMPTY);
		}
		break;

	case WEST:

		if (monster_move_aux(game, x, y,z,y)) {

			map_set_cell_type(map, z, y, CELL_MONSTER);
			window_display_image(sprite_get_monster(WEST),z*SIZE_BLOC , y*SIZE_BLOC);
			map_set_time(map,z,y,SDL_GetTicks());
			map_set_time(map,x,y,0);
			map_set_cell_type(map, x, y, CELL_EMPTY);
		}
		break;

	case EAST:
		if (monster_move_aux(game, x, y,u,y)) {
			map_set_cell_type(map, u, y, CELL_MONSTER);
			window_display_image(sprite_get_monster(EAST),u*SIZE_BLOC , y*SIZE_BLOC);
			map_set_time(map,u,y,SDL_GetTicks());
			map_set_time(map,x,y,0);
			map_set_cell_type(map, x, y, CELL_EMPTY);
		}
		break;
	default:
		break;
	}
}
