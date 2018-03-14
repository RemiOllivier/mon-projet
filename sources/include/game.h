/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef GAME_H_
#define GAME_H_

#include <player.h>
#include <map.h>
#include <bomb.h>
#include <monster.h>

// Abstract data type
struct game;

// Create a new game
struct game* game_new();

// Free a game
void game_free(struct game* game);

// Return the player of the current game
struct player* game_get_player(struct game* game);

struct bomb* game_get_bomb(struct game* game);
// Return the current map
struct map* game_get_current_map(struct game* game);


// Display the game on the screen
void game_display(struct game* game);
//struct monster* game_get_monster(struct game* game);
void monster_direction(struct game* game,int x, int y);

// update
int game_update(struct game* game);
void bomb_display(int x,int y,unsigned int timer,struct game* game);
int game_bomb_destruction(struct map* map,struct player* player, int x, int y, int i);
int game_bomb_explosion(struct map* map, int x, int y, int i);
void monster_move(struct game* game, int x, int y, char current_direction);
int monster_move_aux(struct game* game, int x, int y, int z, int t);

#endif /* GAME_H_ */
