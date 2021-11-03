#include <stdio.h>
#include <string>

#include "constants.h"
#include "enums.h"
#include "Map.h"
#include "Entity.h"

Entity::Entity(int x, int y, int s){
	xpos = x;
	ypos = y;
	spritexpos = x * tilesize;
	spriteypos = y * tilesize;
	printf("%d, %d\n", spritexpos, spriteypos);
	basesprite = s;
	facing = DIR_DOWN;
	movedir = DIR_DOWN;
	movetimer = 0;
	oddwalkcycle = false;
	currentmovper = MOVEMENT_WALKABLE;
}

void Entity::setLocation(int x, int y){
	xpos = x;
	ypos = y;
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
	return basesprite + ((int) facing * 12) + walkcycle;
}

void Entity::move2(int newx, int newy){
	xpos = newx;
	ypos = newy;
	if(oddwalkcycle){
		walkcycle = -1;
	} else {
		walkcycle = 1;
	}
	movetimer = 16;
}

void Entity::move(dir direction, Map *map){
	int newx = xpos;
	int newy = ypos;
	movedir = direction;
	facing = direction;
	switch(direction){
		case DIR_DOWN:
		newy++;
		break;
		
		case DIR_LEFT:
		newx--;
		break;
		
		case DIR_RIGHT:
		newx++;
		break;
		
		case DIR_UP:
		newy--;
		break;
	}
	if(newx < 0 || newx >= map->getWidth() || newy < 0 || newy >= map->getHeight()){
		
	} else {
		if(map->getMovementPermission(newx, newy) == MOVEMENT_WALKABLE){
			move2(newx, newy);
			currentmovper = MOVEMENT_WALKABLE;
		} else if (map->getMovementPermission(newx, newy) == MOVEMENT_LAYER0){
			if(currentmovper == MOVEMENT_WALKABLE || currentmovper == MOVEMENT_LAYER0){
				move2(newx, newy);
				currentmovper = MOVEMENT_LAYER0;
			}
		} else if (map->getMovementPermission(newx, newy) == MOVEMENT_LAYER1){
			if(currentmovper == MOVEMENT_WALKABLE || currentmovper == MOVEMENT_LAYER1){
				move2(newx, newy);
				currentmovper = MOVEMENT_LAYER1;
			}
		}
	}
}

void Entity::animate(){
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
	}
}

int Entity::getMoveTimer(){
	return movetimer;
}
