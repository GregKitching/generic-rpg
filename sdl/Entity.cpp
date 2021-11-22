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
//TextBox *maintextbox;
std::vector<Entity> entities;
Map *map;
bool caninteract;
bool canmove;
SpriteSheet *font;
Script *currentscript;
bool renderflag;
bool rendering;
SDL_Renderer *renderer;
std::vector<TextBox*> textboxes;
SDL_Texture *textboxtexture;

Entity::Entity(enttype t, int x, int y, int s, bool a, bool r, bool sl, bool i, dir d, std::string sc, int e){
	type = t;
	xpos = x;
	ypos = y;
	spritexpos = x * tilesize;
	spriteypos = y * tilesize;
	basesprite = s;
	animated = a;
	rendered = r;
	solid = sl;
	interactable = i;
	facing = d;
	script = sc;
	entnum = e;
	walkcycle = 0;
	movedir = DIR_DOWN;
	movetimer = 0;
	oddwalkcycle = false;
	busy = false;
	currentmovper = MOVEMENT_DEFAULT;
	state = ENTSTATE_NONE;
}

void Entity::setLocation(int x, int y){
	xpos = x;
	ypos = y;
	spritexpos = x * tilesize;
	spriteypos = y * tilesize;
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

int Entity::getSpriteXPos(){
	return spritexpos;
}

int Entity::getSpriteYPos(){
	return spriteypos;
}

int Entity::getSprite(){
	if(animated){
		return basesprite + ((int) facing * 12) + walkcycle;
	} else {
		return basesprite;
	}
}

dir Entity::getFacingDir(){
	return facing;
}

void Entity::setFacingDir(dir direction){
	facing = direction;
}

bool Entity::canMove(int newx, int newy, dir direction){//, Map *map, std::vector<Entity> *entities){
	if(newx == -1 || newy == -1){//Check if out of map bounds
		return false;
	}
	if(map->getMovementPermission(newx, newy) == MOVEMENT_BLOCKED){
		return false;
	}
	if(map->getMovementPermission(newx, newy) == MOVEMENT_LAYER0 && currentmovper == MOVEMENT_LAYER1){
		return false;
	}
	if(map->getMovementPermission(newx, newy) == MOVEMENT_LAYER1 && currentmovper == MOVEMENT_LAYER0){
		return false;
	}
	for(int i = 0; i < entities.size(); i++){
		if(entities.at(i).getXPos() == newx && entities.at(i).getYPos() == newy && entities.at(i).isSolid()){
			return false;
		}
		/*if(map->getMovementPermission(newx, newy) == MOVEMENT_DEFAULT){
			move2(newx, newy);
			currentmovper = MOVEMENT_DEFAULT;
		} else if (map->getMovementPermission(newx, newy) == MOVEMENT_LAYER0){
			if(currentmovper == MOVEMENT_DEFAULT || currentmovper == MOVEMENT_LAYER0){
				move2(newx, newy);
				currentmovper = MOVEMENT_LAYER0;
			}
		} else if (map->getMovementPermission(newx, newy) == MOVEMENT_LAYER1){
			if(currentmovper == MOVEMENT_DEFAULT || currentmovper == MOVEMENT_LAYER1){
				move2(newx, newy);
				currentmovper = MOVEMENT_LAYER1;
			}
		}*/
	}
	return true;
}

void Entity::move(dir direction){//, Map *map, std::vector<Entity> *entities){
	int *newpos = getAdjacentTile(direction, map);
	movedir = direction;
	facing = direction;
	if(canMove(newpos[0], newpos[1], direction)){//, map, entities)){
		busy = true;
		state = ENTSTATE_MOVING;
		xpos = newpos[0];
		ypos = newpos[1];
		currentmovper = map->getMovementPermission(xpos, ypos);
		if(oddwalkcycle){
			walkcycle = -1;
		} else {
			walkcycle = 1;
		}
		movetimer = 16;
		//printf("%d\n", movetimer);
	}
}

void Entity::animateMove(){
	switch(movedir){
		case DIR_DOWN:
		spriteypos++;
		break;
		
		case DIR_LEFT:
		spritexpos--;
		break;
		
		case DIR_RIGHT:
		spritexpos++;
		break;
		
		case DIR_UP:
		spriteypos--;
		break;
	}
	movetimer--;
	if(movetimer == 8){
		walkcycle = 0;
	} else if (movetimer == 0){
		oddwalkcycle = !oddwalkcycle;
		state = ENTSTATE_NONE;
		busy = false;
		if(entnum == 0){
			enttype en = ENTTYPE_PLAYER;
			for(int i = 1; i < entities.size(); i++){
				if(entities.at(i).getXPos() == xpos && entities.at(i).getYPos() == ypos && entities.at(i).getType() > ENTTYPE_SIGN && i <= entities.size()){
					currentscript = new Script(i, entities.at(i).getScript());
					break;
				}
			}
		}		
	}
}

int Entity::getMoveTimer(){
	return movetimer;
}

movper Entity::getMovPer(){
	return currentmovper;
}

void Entity::setMovPer(movper a){
	currentmovper = a;
}

bool Entity::isAnimated(){
	return animated;
}

bool Entity::isRendered(){
	return rendered;
}

bool Entity::isSolid(){
	return solid;
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

void Entity::tick(){
	switch(state){
		case ENTSTATE_NONE:
		break;
		
		case ENTSTATE_MOVING:
		animateMove();
		break;
		
		case ENTSTATE_TURNING:
		break;
		
		case ENTSTATE_WAITING:
		break;
	}
}

entstate Entity::getState(){
	return state;
}

bool Entity::isBusy(){
	return busy;
}

void Entity::setBusy(){
	busy = true;
}

int Entity::getWarpNum(){
	return warpnum;
}

void Entity::setWarpNum(int w){
	warpnum = w;
}
