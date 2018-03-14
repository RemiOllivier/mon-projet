/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<unistd.h>
#include <game.h>
#include<math.h>
#include <map.h>
#include <constant.h>
#include <misc.h>
#include <sprite.h>
#include <window.h>
#include <bomb.h>
#include <string.h>

struct map {
	int width;
	int height;
	unsigned char* grid;
	unsigned int* time;
};

#define CELL(i,j) ( (i) + (j) * map->width)

struct map* map_new(int width, int height)
{
	assert(width > 0 && height > 0);

	struct map* map = malloc(sizeof *map);
	if (map == NULL )
		error("map_new : malloc map failed");

	map->width = width;
	map->height = height;
	map->grid = malloc(height * width);
	map->time=malloc(height*width);
	if (map->grid == NULL) {
		error("map_new : malloc grid failed");
	}

	// Grid cleaning
	int i, j;
	for (i = 0; i < width; i++)
	  for (j = 0; j < height; j++)
	    map->grid[CELL(i,j)] = CELL_EMPTY;
		map->time[CELL(i,j)] = 0;

	return map;
}

int map_is_inside(struct map* map, int x, int y)
{
	assert((map));
	if ((x<map->width)&&(y<map->height)&&(x>=0)&&(y>=0)){
		return 1;}
		else {
			return 0;}
}

void map_free(struct map *map)
{
	if (map == NULL )
		return;
	free(map->grid);
	free(map->time);
	free(map);
}

int map_get_width(struct map* map)
{
	assert(map != NULL);
	return map->width;
}

int map_get_height(struct map* map)
{
	assert(map);
	return map->height;
}

void  map_set_time(struct map* map, int x, int y, unsigned int time){
	map->time[CELL(x,y)] = time;
}



enum cell_type map_get_cell_type(struct map* map, int x, int y)
{
	assert(map /*&& map_is_inside(map, x, y)*/);
	return map->grid[CELL(x,y)] & 0xf0;
}

enum compose_type map_get_cell_type2(struct map* map, int x, int y)
{
	assert(map /*&& map_is_inside(map, x, y)*/);
	return map->grid[CELL(x,y)] & 0xff;
}


unsigned int map_get_time(struct map* map, int x, int y)
{
	assert(map /*&& map_is_inside(map, x, y)*/);
	return map->time[CELL(x,y)];
}

void map_set_cell_type(struct map* map, int x, int y, enum cell_type type)
{
	//assert(map && map_is_inside(map, x, y));
	map->grid[CELL(x,y)] = type;
	if(map->grid[CELL(x,y)]==CELL_BOMB ){

		map->time[CELL(x,y)]=SDL_GetTicks();
	}
	else{
		map->time[CELL(x,y)]=0;
	}
}

void map_change_cell_type(struct map* map, int x, int y, enum cell_type type)
{
	map->grid[CELL(x,y)] = (type & 0x0f) | 0x50;
}

void display_bonus(struct map* map, int x, int y, unsigned char type)
{
	// bonus is encoded with the 4 most significant bits
	switch (type & 0x0f) {
	case BONUS_BOMB_RANGE_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_INC), x, y);
		break;

	case BONUS_BOMB_RANGE_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_DEC), x, y);
		break;

	case BONUS_BOMB_NB_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_DEC), x, y);
		break;

	case BONUS_BOMB_NB_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_INC), x, y);
		break;
	case BONUS_LIFE:
		window_display_image(sprite_get_banner_life(),x,y);
		break;
	case BONUS_MONSTER:
		window_display_image(sprite_get_monster(SOUTH),x , y);
		map_set_cell_type(map,x/SIZE_BLOC,y/SIZE_BLOC, CELL_MONSTER);
		break;
	default:
		break;
	}
}

void display_scenery(struct map* map, int x, int  y, unsigned char type)
{
	switch (type & 0x0f) { // sub-types are encoded with the 4 less significant bits
	case SCENERY_STONE:
		window_display_image(sprite_get_stone(), x, y);
		break;

	case SCENERY_TREE:
		window_display_image(sprite_get_tree(), x, y);
		break;
	}
}

void display_door(struct map* map, int x, int  y, unsigned char type)
{
	switch (type & 0x0f) { // sub-types are encoded with the 4 less significant bits
	case DOOR_CLOSED:
		window_display_image(sprite_get_door_closed(), x, y);
		break;
	case DOOR_OPENED:
			window_display_image(sprite_get_door_opened(), x, y);
			break;

	}
}


void map_display(struct map* map)
{
	assert(map != NULL);
	assert(map->height > 0 && map->width > 0);
	int x, y;
	for (int i = 0; i < map->width; i++) {
	  for (int j = 0; j < map->height; j++) {
	    x = i * SIZE_BLOC;
	    y = j * SIZE_BLOC;

	    unsigned char type = map->grid[CELL(i,j)];
	
	    switch (type & 0xf0) {
		case CELL_SCENERY:
		  display_scenery(map, x, y, type);
		  break;
	    case CELL_BOX:
	      window_display_image(sprite_get_box(), x, y);
	      break;
	    case CELL_BONUS:
	      display_bonus(map, x, y, type);
	      break;
	    case CELL_KEY:
	      window_display_image(sprite_get_key(), x, y);
	      break;
	    case CELL_DOOR:
	    	display_door(map, x,y,type);
	      break;
	    case CELL_BOMB:
	    	break;
	    case CELL_MONSTER:
	    	window_display_image(sprite_get_monster(EAST),x , y);
	        break;
	    }
	  }
	}
}

/*unsigned char binaire(int x){
  unsigned char b[9];
  unsigned char a[3];
  unsigned char c[3];
  int i;
  strcpy(b," ");
  strcpy(a,"0");
  strcpy(c,"1");
  while(x>0){
  if(x%2==0){
    x=x/2;
    strcat(b,a);
  }
  else {
    x=x/2;
    strcat(b,c);
  }
  for(i=0;i<strlen(b)/2;i++){
	  int k;
	  k=b[i];
	  b[i]=b[strlen(b)-1-i];
	  b[strlen(b)-1-i]=k;
  }
  }
  return b;
}

struct map* map_get(void)

{

    FILE* fichier = NULL;
    int width;
    int height;
    fichier = fopen("../map/map_1.txt", "r");
    int i;
    fscanf(fichier, "%d x %d", &width, &height);
	int tab[width*height];
	unsigned char themap[width*height];
	struct map* map=map_new(width,height);
	for(i=0; i<height*width; i++){
	    fscanf(fichier, "%d ", &tab[i]);
	    themap[i]=binaire(tab[i]);
	    for (int i = 0; i < width * height; i++)
	       		map->grid[i] = themap[i];
	    }
        fclose(fichier);
    return map;
}*/


struct map* map_get_static(void)
{
	struct map* map = map_new(STATIC_MAP_WIDTH, STATIC_MAP_HEIGHT);

	unsigned char themap[STATIC_MAP_WIDTH * STATIC_MAP_HEIGHT] = {
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
	  CELL_BOX_RANGEINC, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_BOX, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_BOX_MONSTER, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY , CELL_EMPTY, CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_TREE, CELL_BOX_LIFE, CELL_TREE, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,  CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	  CELL_KEY, CELL_TREE, CELL_TREE, CELL_TREE, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,  CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_EMPTY, CELL_MONSTER, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	  CELL_BOX, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE,  CELL_BOX_LIFE, CELL_EMPTY,
	  CELL_BOX,  CELL_EMPTY, CELL_DOOR_CLOSED, CELL_KEY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
		};

	for (int i = 0; i < STATIC_MAP_WIDTH * STATIC_MAP_HEIGHT; i++)
		map->grid[i] = themap[i];

	return map;
}
