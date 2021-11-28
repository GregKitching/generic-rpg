#include <SDL2/SDL.h>

#include "constants.h"
#include "Globals.h"
#include "enums.h"
#include "ActiveEntity.h"

ActiveEntity::ActiveEntity(enttype t, int x, int y, bool i, std::string sc, int e, int s, bool a, bool r, bool sl, dir d): Entity(t, x, y, i, sc, e), basesprite(s), animated(a), rendered(r), solid(sl), facing(d){
	spritexpos = x * tilesize;
	spriteypos = y * tilesize;
	walkcycle = 0;
	movedir = DIR_DOWN;
	movetimer = 0;
	oddwalkcycle = false;
	busy = false;
	state = ENTSTATE_NONE;
}

int ActiveEntity::getSpriteXPos(){
	return spritexpos;
}

int ActiveEntity::getSpriteYPos(){
	return spriteypos;
}

int ActiveEntity::getSprite(){
	if(animated){
		return basesprite + ((int) facing * 12) + walkcycle;
	} else {
		return basesprite;
	}
}

dir ActiveEntity::getFacingDir(){
	return facing;
}

void ActiveEntity::setFacingDir(dir direction){
	facing = direction;
}

bool ActiveEntity::canMove(int newx, int newy, dir direction){//, Map *map, std::vector<Entity> *entities){
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
		if(entities.at(i)->getXPos() == newx && entities.at(i)->getYPos() == newy && entities.at(i)->getType() < ENTTYPE_SIGN){
			if(static_cast<ActiveEntity*>(entities.at(i))->isSolid()){
				return false;
			}
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

void ActiveEntity::move(dir direction){//, Map *map, std::vector<Entity> *entities){
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

void ActiveEntity::animateMove(){
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
			//enttype en = ENTTYPE_PLAYER;
			for(int i = 1; i < entities.size(); i++){
				if(entities.at(i)->getXPos() == xpos && entities.at(i)->getYPos() == ypos && entities.at(i)->getType() > ENTTYPE_SIGN){// && i <= entities.size()){
					currentscript = new Script(i, entities.at(i)->getScript());
					break;
				}
			}
		}		
	}
}

int ActiveEntity::getMoveTimer(){
	return movetimer;
}

bool ActiveEntity::isAnimated(){
	return animated;
}

bool ActiveEntity::isRendered(){
	return rendered;
}

void ActiveEntity::setRendered(bool u){
	rendered = u;
}

bool ActiveEntity::isSolid(){
	return solid;
}

void ActiveEntity::tick(){
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

entstate ActiveEntity::getState(){
	return state;
}

bool ActiveEntity::isBusy(){
	return busy;
}

void ActiveEntity::setBusy(){
	busy = true;
}

void ActiveEntity::updateSpriteLocation(){
	spritexpos = xpos * tilesize;
	spriteypos = ypos * tilesize;
}
