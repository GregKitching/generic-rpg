#include <stdio.h>
#include <string>
#include <vector>
#include <SDL2/SDL.h>

#include "constants.h"
#include "Globals.h"
#include "enums.h"
#include "Map.h"
#include "Entity.h"
#include "TextBox.h"

uint8_t *heap;
pmode programmode;
//TextBox *maintextbox;
std::vector<Entity*> entities;
Map *map;
bool caninteract;
bool canmove;
SpriteSheet *font;
Script *currentscript;
BattleManager *battlemanager;
bool renderflag;
bool rendering;
SDL_Renderer *renderer;
std::vector<TextBox*> textboxes;
SDL_Texture *textboxtexture;
ActiveEntity *player;
SoundHandler *soundhandler;

Entity::Entity(enttype t, int x, int y, bool i, std::string sc, int e){
	type = t;
	xpos = x;
	ypos = y;
	interactable = i;
	script = sc;
	entnum = e;
	currentmovper = MOVEMENT_DEFAULT;
}

void Entity::setLocation(int x, int y){
	xpos = x;
	ypos = y;
}

enttype Entity::getType(){
	return type;
}

int Entity::getXPos(){
	return xpos;
}

int Entity::getYPos(){
	return ypos;
}

movper Entity::getMovPer(){
	return currentmovper;
}

void Entity::setMovPer(movper a){
	currentmovper = a;
}

bool Entity::isInteractable(){
	return interactable;
}

int* Entity::getAdjacentTile(dir direction, Map *map){
	int *newpos = new int[2];
	switch(direction){
		case DIR_DOWN:
		newpos[0] = xpos;
		newpos[1] = ypos + 1;
		break;
		
		case DIR_LEFT:
		newpos[0] = xpos - 1;
		newpos[1] = ypos;
		break;
		
		case DIR_RIGHT:
		newpos[0] = xpos + 1;
		newpos[1] = ypos;
		break;
		
		case DIR_UP:
		newpos[0] = xpos;
		newpos[1] = ypos - 1;
		break;
		
		default:
		newpos[0] = -1;
		newpos[1] = -1;
		break;
	}
	if(newpos[0] < 0 || newpos[0] >= map->getWidth() || newpos[1] < 0 || newpos[1] >= map->getHeight()){
		newpos[0] = -1;
		newpos[1] = -1;
	}
	return newpos;
}

std::string Entity::getScript(){
	return script;
}

int Entity::getWarpNum(){
	return warpnum;
}

void Entity::setWarpNum(int w){
	warpnum = w;
}
