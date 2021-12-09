#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
//#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "constants.h"
#include "ScriptDefs.h"
#include "enums.h"
#include "SpriteSheet.h"
#include "Tile.h"
#include "TileSet.h"
#include "Map.h"
#include "Entity.h"
#include "ActiveEntity.h"
#include "TextBox.h"
#include "Script.h"
#include "SoundHandler.h"
#include "Character.h"
#include "BattleChoice.h"
#include "BattleManager.h"
#include "Globals.h"

//using namespace std;

bool active = true;

bool upkey = false;
bool downkey = false;
bool leftkey = false;
bool rightkey = false;

bool wkey = false;
bool skey = false;
bool akey = false;
bool dkey = false;

bool hkey = false;
bool lkey = false;
bool pkey = false;
bool sckey = false;

bool hidelayer1 = false;
bool setmovementpermissions = false;

bool camerafollowplayer = false;

bool renderentities;

bool inbattle;

int camposx;
int camposy;

int mouseposx;
int mouseposy;

//pmode programmode;

bool clickaction = false;
//bool buttonaction = false;
//bool caninteract;
//bool canmove;

//Script *currentscript = NULL;

int currentlayer = 0;
int currenttile = 0;
int currentsubtile = 0;
movper currentmovper = MOVEMENT_DEFAULT;

SDL_Window *window = NULL;
//SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Surface *surface = NULL;

SDL_Event e;

SpriteSheet *basic;
SpriteSheet *characters;
//SpriteSheet *font;
TileSet *tileset;
//Map *map;
//std::vector<Entity> entities;
//SDL_Texture *textboxtexture = NULL;
SDL_Texture *movpertex = NULL;

int *visrange;

SDL_Rect srcrecttiles;
SDL_Rect dstrecttiles;
SDL_Rect srcrectsubtiles;
SDL_Rect dstrectsubtiles;
SDL_Rect srcrecttext;
SDL_Rect dstrecttext;

std::string basicspritesheetname;
std::string characterspritesheetname;
std::string fontspritesheetname;
std::string textboxspritesheetname;
std::string tilesetname;
std::string mapname;

SDL_Thread *scriptthread;

int animcycle = 0;

bool setanimproperties = false;

std::vector<Character*> party;

//uint8_t *heap;
//TextBox *maintextbox;

void quit(){
	//texture->free();
	//letterA->free();
	//TTF_CloseFont(font);
	//font = NULL;
	soundhandler->quit();
	SDL_DetachThread(scriptthread);
	SDL_FreeSurface(surface);
	surface = NULL;
	SDL_DestroyRenderer(renderer);
	renderer = NULL;
	SDL_DestroyWindow(window);
	window = NULL;
	//TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
	active = false;
}

/*void createCharTex(std::string a, SDL_Color colour){
	SDL_Surface *tempsurface = TTF_RenderText_Solid(font, a.c_str(), colour);
	if(tempsurface == NULL){
		printf("Fail\n");
	} else {
		letterA = SDL_CreateTextureFromSurface(renderer, tempsurface);
		if(letterA == NULL){
			printf("Failed to load texture.\n");
		}
		ww = tempsurface->w;
		hh = tempsurface->h;
		printf("%d, %d\n", ww, hh);
	}
	SDL_FreeSurface(tempsurface);
}*/		

void switchLayer(){
	int temp;
	std::cout << "Enter a layer number:" << std::endl;
	std::cin >> temp;
	if(temp >= 0 && temp <= 1){
		currentlayer = temp;
		printf("Switched to layer %d\n", temp);
	} else {
		printf("Number out of range.\n");
	}
}

void switchTile(TileSet *tileset){
	int temp;
	std::cout << "Enter a tile number:" << std::endl;
	std::cin >> temp;
	if(temp >= 0 && temp < tileset->getLength()){
		currenttile = temp;
		printf("Current tile: %d\n", temp);
	} else {
		printf("Number out of range.\n");
	}
}

void switchSubtile(SpriteSheet *spritesheet){
	int temp;
	std::cout << "Enter a subtile number:" << std::endl;
	std::cin >> temp;
	if(temp >= 0 && temp < spritesheet->getSubtileCount()){
		currentsubtile = temp;
		printf("Current subtile: %d\n", temp);
	} else {
		printf("Number out of range.\n");
	}
}

void selectTile(dir direction, TileSet *tileset){
	int temp;
	int max = tileset->getLength();
	if (direction == DIR_DOWN){
		temp = currenttile + 20;
		if(temp < max){
			currenttile = temp;
		}
	} else if (direction == DIR_LEFT){
		temp = currenttile - 1;
		if(temp >= 0){
			currenttile = temp;
		}
	} else if (direction == DIR_RIGHT){
		temp = currenttile + 1;
		if(temp < max){
			currenttile = temp;
		}
	} else if (direction == DIR_UP){
		temp = currenttile - 20;
		if(temp >= 0){
			currenttile = temp;
		}
	}
	printf("Current tile: %d\n", currenttile);
}

void changeSubtile(Map *map, TileSet *tileset){
	int *tilepos = map->getClickedTile(mouseposx, mouseposy, camposx, camposy);
	int subtilepos = map->getClickedSubtile(mouseposx, mouseposy, camposx, camposy);
	int u = map->getTile(tilepos[0], tilepos[1]);
	printf("%d, %d, %d, %d\n", tilepos[0], tilepos[1], u, subtilepos);
	Tile *tile = tileset->getTileObject(u);
	tile->changeSubtile(currentlayer, subtilepos, currentsubtile);
}

void changeTileAnimated(Map *map, TileSet *tileset){
	int *tilepos = map->getClickedTile(mouseposx, mouseposy, camposx, camposy);
	bool u = tileset->getTileObject(map->getTile(tilepos[0], tilepos[1]))->isAnimated();
	tileset->getTileObject(map->getTile(tilepos[0], tilepos[1]))->setAnimated(!u);
	if(u){
		printf("Tile not animated.\n");
	} else {
		printf("Tile animated.\n");
	}
}

void selectSubtile(dir direction, SpriteSheet *spritesheet){
	int temp;
	int max = spritesheet->getSubtileCount();
	if (direction == DIR_DOWN){
		temp = currentsubtile + 16;
		if(temp < max){
			currentsubtile = temp;
		}
	} else if (direction == DIR_LEFT){
		temp = currentsubtile - 1;
		if(temp >= 0){
			currentsubtile = temp;
		}
	} else if(direction == DIR_RIGHT){
		temp = currentsubtile + 1;
		if(temp < max){
			currentsubtile = temp;
		}
	} else if (direction == DIR_UP){
		temp = currentsubtile - 16;
		if(temp >= DIR_UP){
			currentsubtile = temp;
		}
	}
	printf("Current subtile: %d\n", currentsubtile);
}

void setOutsideTile(Map *map, TileSet *tileset){
	int temp;
	std::cout << "Enter outside tile:" << std::endl;
	std::cin >> temp;
	if(temp >= 0 && temp < tileset->getLength()){
		map->setOutsideTile(temp);
		printf("Outside tile set to %d.\n", temp);
	} else {
		printf("Number out of range.\n");
	}
}

void setMapScript(){
	std::string name;
	std::cout << "Enter map run-on-load script:" << std::endl;
	std::cin >> name;
	map->setScript(name);
	printf("Map run-on-load script set to %s.\n", name.c_str());
}

bool parseBoolString(){//std::string temp){
	bool repeat = true;
	bool u;
	std::string temp;
	do{
		std::cin >> temp;
		if(temp.compare("true") == 0){
			u = true;
			repeat = false;
		} else if (temp.compare("false") == 0){
			u = false;
			repeat = false;
		} else {
			printf("Please enter a valid boolean value.\n");
		}
	} while (repeat);
	return u;
}

void createEntity(){
	enttype t;
	int x, y, s, w;
	bool a, r, sl, i;
	std::string scr;
	dir d;
	std::string temp;
	std::cout << "Enter entity type:" << std::endl;
	std::cin >> temp;
	t = (enttype) std::stoi(temp);
	std::cout << "X position:" << std::endl;
	std::cin >> temp;
	x = std::stoi(temp);
	std::cout << "Y position:" << std::endl;
	std::cin >> temp;
	y = std::stoi(temp);
	std::cout << "Is interactable:" << std::endl;
	//std::cin >> temp;
	i = parseBoolString();
	std::cout << "Script name:" << std::endl;
	std::cin.ignore();
	std::getline(std::cin, scr);
	printf("a\n");
	if(t < ENTTYPE_SIGN){
		std::cout << "Default sprite:" << std::endl;
		std::cin >> temp;
		s = std::stoi(temp);
		std::cout << "Is animated:" << std::endl;
		//std::cin >> temp;
		a = parseBoolString();
		std::cout << "Is rendered:" << std::endl;
		//std::cin >> temp;
		r = parseBoolString();
		std::cout << "Is solid:" << std::endl;
		//std::cin >> temp;
		sl = parseBoolString();
		std::cout << "Direction facing:" << std::endl;
		std::cin >> temp;
		d = (dir) std::stoi(temp);
		ActiveEntity *ae = new ActiveEntity(t, x, y, i, scr, entities.size(), s, a, r, sl, d);
		entities.push_back(ae);
	} else {
		Entity *ent = new Entity(t, x, y, i, scr, entities.size());
		if(t == ENTTYPE_WARP){
			std::cout << "Warp num:" << std::endl;
			std::cin >> temp;
			w = std::stoi(temp);
			ent->setWarpNum(w);
		}
		entities.push_back(ent);
	}
}
	

int getInteractableEntityAtPosition(int x, int y){
	for(int i = 0; i < entities.size(); i++){
		if(entities.at(i)->getXPos() == x && entities.at(i)->getYPos() == y && entities.at(i)->isInteractable()){
			return i;
		}
	}
	return -1;
}

void visibleTiles(int *range, int x, int y){//, Map *map){
	range[0] = (x / tilesize) - 1;
	range[1] = range[0] + vistilesxoffset;
	/*if(range[0] < 0){	values outside bounds of map are now used to render outside tiles
		range[0] = 0;
	}
	if (range[1] >= map->getWidth()){
		range[1] = map->getWidth() - 1;
	}*/
	range[2] = (y / tilesize) - 1;
	range[3] = range[2] + vistilesyoffset;
	/*if(range[2] < 0){
		range[2] = 0;
	}
	if (range[3] >= map->getHeight()){
		range[3] = map->getHeight() - 1;
	}*/
	//printf("Range: x = %d - %d, y = %d - %d\n", range[0], range[1], range[2], range[3]);
}

/*Tiles are rendered as two layers of four subtiles each; layer 1 is rendered
  after all of layer 0 to include the possibility of rendering entities
  between them to create occlusion effects (not yet implemented)*/

void renderLayer(int layer, Map *map, TileSet *tileset, SpriteSheet *spritesheet, int *range, SDL_Rect *srcrect, SDL_Rect *dstrect){
	int temp;
	int temp2;
	for(int i = range[2]; i <= range[3]; i++){
		dstrect->y = i * tilesize - camposy;
		for(int j = range[0]; j <= range[1]; j++){
			dstrect->x = j * tilesize - camposx;
			temp = map->getTile(j, i);
			if(tileset->getTileObject(temp)->isAnimated() & animcycle > 63){
				temp += 20;
			}
			temp2 = tileset->getSubtileFromTile(temp, layer, 0);
			srcrect->x = spritesheet->getSubtileX(temp2);
			srcrect->y = spritesheet->getSubtileY(temp2);
			SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
			dstrect->x += subtilesize;
			
			temp2 = tileset->getSubtileFromTile(temp, layer, 1);
			srcrect->x = spritesheet->getSubtileX(temp2);
			srcrect->y = spritesheet->getSubtileY(temp2);
			SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
			dstrect->y += subtilesize;
			
			temp2 = tileset->getSubtileFromTile(temp, layer, 3);
			srcrect->x = spritesheet->getSubtileX(temp2);
			srcrect->y = spritesheet->getSubtileY(temp2);
			SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
			dstrect->x -= subtilesize;
			
			temp2 = tileset->getSubtileFromTile(temp, layer, 2);
			srcrect->x = spritesheet->getSubtileX(temp2);
			srcrect->y = spritesheet->getSubtileY(temp2);
			SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
			dstrect->y -= subtilesize;
		}
	}
}

void renderMap(Map *map, TileSet *tileset, SpriteSheet *spritesheet, int *range, SDL_Rect *srcrect, SDL_Rect *dstrect){
	renderLayer(0, map, tileset, spritesheet, range, srcrect, dstrect);
	if(!hidelayer1){
		renderLayer(1, map, tileset, spritesheet, range, srcrect, dstrect);
	}
}

void renderEntities(SpriteSheet *spritesheet, int *range, SDL_Rect *srcrect, SDL_Rect *dstrect){
	int temp;
	for(int i = 0; i < entities.size(); i++){
		if(entities.at(i)->getXPos() > range[0] & entities.at(i)->getXPos() <= range[1] & entities.at(i)->getYPos() > range[2] & entities.at(i)->getYPos() <= range[3] & entities.at(i)->getType() < ENTTYPE_SIGN){
			ActiveEntity *ae = static_cast<ActiveEntity*>(entities.at(i));
			if(ae->isRendered()){
				temp = ae->getSprite();
				srcrect->x = spritesheet->getTileX(temp);
				srcrect->y = spritesheet->getTileY(temp);
				dstrect->x = ae->getSpriteXPos() - camposx;
				dstrect->y = ae->getSpriteYPos() - camposy;
				SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
			}
		}
	}
}

void renderMovPer(int *range, SDL_Rect *srcrect, SDL_Rect *dstrect){
	int temp;
	srcrect->y = 0;
	for(int i = range[2]; i <= range[3]; i++){
		dstrect->y = i * tilesize - camposy;
		for(int j = range[0]; j <= range[1]; j++){
			dstrect->x = j * tilesize - camposx;
			temp = (int) map->getMovementPermission(j, i);
			srcrect->x = temp * tilesize;
			SDL_RenderCopy(renderer, movpertex, srcrect, dstrect);
		}
	}
}

void buttonAction(){
	if(battlemanager != NULL){
		battlemanager->buttonAction();
	}
	if(caninteract){
		//printf("1\n");
		caninteract = false;
		int *facingpos = player->getAdjacentTile(player->getFacingDir(), map);
		int u = getInteractableEntityAtPosition(facingpos[0], facingpos[1]);
		delete [] facingpos;
		if(u != -1){
			if(entities.at(u)->getType() < ENTTYPE_SCRIPT){
				currentscript = new Script(u, entities.at(u)->getScript());
			}
		}
	} else {
		//printf("2\n");
		if(currentscript != NULL){
			currentscript->advance();
		}
		//caninteract = true;
	}
}

Uint32 renderFunc(Uint32 interval, void *param){
	if(renderflag){
		rendering = true;
		animcycle++;
		if(animcycle > 127){
			animcycle = 0;
		}
		SDL_PollEvent(&e);
		if(e.type == SDL_QUIT){
			active = false;
		} else if(e.type == SDL_KEYDOWN){
			if(e.key.repeat == 0){
				switch(e.key.keysym.sym){
					case SDLK_w:
					wkey = true;
					if(currentscript != NULL){
						currentscript->dirAction(DIR_UP);
					}
					if(battlemanager != NULL){
						battlemanager->dirAction(DIR_UP);
					}
					break;
					
					case SDLK_s:
					skey = true;
					if(currentscript != NULL){
						currentscript->dirAction(DIR_DOWN);
					}
					if(battlemanager != NULL){
						battlemanager->dirAction(DIR_DOWN);
					}
					break;
					
					case SDLK_a:
					akey = true;
					if(currentscript != NULL){
						currentscript->dirAction(DIR_LEFT);
					}
					break;
					
					case SDLK_d:
					dkey = true;
					if(currentscript != NULL){
						currentscript->dirAction(DIR_RIGHT);
					}
					break;
					
					case SDLK_e:
					renderentities = !renderentities;
					break;
					
					case SDLK_l:
					lkey = true;
					/*if(programmode == TILESET_EDITOR){
						switchLayer();
					}*/
					if(battlemanager != NULL){
						battlemanager->backButtonAction();
					}
					break;
					
					case SDLK_z:
					if(programmode == MAP_EDITOR){
						setMapScript();
					} else if (programmode == TILESET_EDITOR){
						setanimproperties = !setanimproperties;
						if(setanimproperties){
							printf("Setting animation properties\n");
						} else {
							printf("No longer setting animation properties\n");
						}
					}	
					break;
					
					case SDLK_c:
					if(programmode == MAP_EDITOR){
						createEntity();
					}
					break;
					
					case SDLK_b:
					if(battlemanager == NULL){
						std::vector<Character*> enemies;
						enemies.push_back(new Character("Fish", 1, 10, 0, 3, 1, 2, false));
						battlemanager = new BattleManager(party, enemies);
					}
					break;
					
					case SDLK_SEMICOLON:
					sckey = true;
					buttonAction();
					break;
					
					case SDLK_t:
					if(programmode == MAP_EDITOR){
						switchTile(tileset);
					} else if (programmode == TILESET_EDITOR){
						switchSubtile(basic);
					}
					break;
					
					case SDLK_n:
					if(programmode == MAP_EDITOR){
						map->save(mapname);
					} else if (programmode == TILESET_EDITOR){
						tileset->saveTileset(tilesetname);
					}
					break;
					
					case SDLK_o:
					if(programmode == MAP_EDITOR){
						setOutsideTile(map, tileset);
					}	
					break;
					
					case SDLK_p:
					pkey = true;
					break;
					
					case SDLK_0:
					if(programmode == MAP_EDITOR){
						currentmovper = MOVEMENT_DEFAULT;
						printf("Movement permission set to MOVEMENT_DEFAULT.\n");
					} else if (programmode == TILESET_EDITOR){
						currentlayer = 0;
						printf("Now editing layer 0.\n");
					}
					break;
					
					case SDLK_1:
					if(programmode == MAP_EDITOR){
						currentmovper = MOVEMENT_BLOCKED;
						printf("Movement permission set to MOVEMENT_BLOCKED.\n");
					} else if (programmode == TILESET_EDITOR){
						currentlayer = 1;
						printf("Now editing layer 1.\n");
					}
					break;
					
					case SDLK_2:
					if(programmode == MAP_EDITOR){
						currentmovper = MOVEMENT_LAYER0;
						printf("Movement permission set to MOVEMENT_LAYER0.\n");
					}
					break;
					
					case SDLK_3:
					if(programmode == MAP_EDITOR){
						currentmovper = MOVEMENT_LAYER1;
						printf("Movement permission set to MOVEMENT_LAYER1.\n");
					}
					break;
					
					case SDLK_UP:
					if(!upkey){
						if(programmode == MAP_EDITOR){
							selectTile(DIR_UP, tileset);
						} else if (programmode == TILESET_EDITOR){
							selectSubtile(DIR_UP, basic);
						}
						upkey = true;
					}
					break;
					
					case SDLK_DOWN:
					if(!downkey){
						if(programmode == MAP_EDITOR){
							selectTile(DIR_DOWN, tileset);
						} else if (programmode == TILESET_EDITOR){
							selectSubtile(DIR_DOWN, basic);
						}
						downkey = true;
					}
					break;
					
					case SDLK_LEFT:
					if(!leftkey){
						if(programmode == MAP_EDITOR){
							selectTile(DIR_LEFT, tileset);
						} else if (programmode == TILESET_EDITOR){
							selectSubtile(DIR_LEFT, basic);
						}
						leftkey = true;
					}
					break;
					
					case SDLK_RIGHT:
					if(!rightkey){
						if(programmode == MAP_EDITOR){
							selectTile(DIR_RIGHT, tileset);
						} else if (programmode == TILESET_EDITOR){
							selectSubtile(DIR_RIGHT, basic);
						}
						rightkey = true;
					}
					break;
					
					default:
					break;
				}
			}
		} else if(e.type == SDL_KEYUP){
			if(e.key.repeat == 0){//Might be unnecessary
				switch(e.key.keysym.sym){
					case SDLK_w:
					wkey = false;
					break;
					
					case SDLK_s:
					skey = false;
					break;
					
					case SDLK_a:
					akey = false;
					break;
					
					case SDLK_d:
					dkey = false;
					break;
					
					case SDLK_p:
					pkey = false;
					break;
					
					case SDLK_l:
					lkey = false;
					break;
					
					case SDLK_SEMICOLON:
					sckey = false;
					//caninteract = true;
					break;
					
					case SDLK_UP:
					upkey = false;
					break;
					
					case SDLK_DOWN:
					downkey = false;
					break;
					
					case SDLK_LEFT:
					leftkey = false;
					break;
					
					case SDLK_RIGHT:
					rightkey = false;
					break;
					
					default:
					break;
				}
			}
		} else if (e.type == SDL_MOUSEMOTION){
			SDL_GetMouseState(&mouseposx, &mouseposy);
			//printf("%d, %d\n", mouseposx, mouseposy);
		} else if (e.type == SDL_MOUSEBUTTONDOWN){
			clickaction = true;
		} else if (e.type == SDL_MOUSEBUTTONUP){
			clickaction = false;
		}
		//printf("b\n");
		if(clickaction){
			if(programmode == MAP_EDITOR){
				if(setmovementpermissions){
					map->setMovementPermission(mouseposx, mouseposy, camposx, camposy, currentmovper);
				} else {
					map->changeTile(mouseposx, mouseposy, camposx, camposy, currenttile);
				}
			} else if (programmode == TILESET_EDITOR){
				if(setanimproperties){
					changeTileAnimated(map, tileset);
				} else {
					changeSubtile(map, tileset);
				}
			}
		}
		if(pkey){
			if(setmovementpermissions){
				setmovementpermissions = false;
				printf("Movement permissions are no longer being set.\n");
			} else {
				setmovementpermissions = true;
				printf("Movement permissions are being set.\n");
			}
			pkey = false;
		}
		switch(programmode){
			case NORMAL_GAMEPLAY:
			if(player->getMoveTimer() == 0 && canmove){
				if(akey && !dkey && !wkey && !skey){
					player->move(DIR_LEFT);//, map, &entities);
				} else if (dkey && !akey && !wkey && !skey){
					player->move(DIR_RIGHT);//, map, &entities);
				} else if (wkey && !skey){
					player->move(DIR_UP);//, map, &entities);
				} else if (skey && !wkey){
					player->move(DIR_DOWN);//, map, &entities);
				}
			}
			break;
			
			default:
			if(akey && !dkey){// && !wkey && !skey){
				camposx -= 2;
			} else if (dkey && !akey){// && !wkey && !skey){
				camposx += 2;
			}
			if (wkey && !skey){
				camposy -= 2;
			} else if (skey && !wkey){
				camposy += 2;
			}
			break;
		}
		for(int i = 0; i < entities.size(); i++){
			if(entities.at(i)->getType() < ENTTYPE_SIGN){
				ActiveEntity *ae = static_cast<ActiveEntity*>(entities.at(i));
				if(ae->getMoveTimer() > 0){
					ae->animateMove();
				}
			}
		}
		if(camerafollowplayer){
			camposx = player->getSpriteXPos() - playerxoffset;
			camposy = player->getSpriteYPos() - playeryoffset;
		}
		visibleTiles(visrange, camposx, camposy);
		SDL_RenderClear(renderer);
		renderMap(map, tileset, basic, visrange, &srcrectsubtiles, &dstrectsubtiles);
		if(programmode == NORMAL_GAMEPLAY | renderentities){
			renderEntities(characters, visrange, &srcrecttiles, &dstrecttiles);
		}
		if (programmode == MAP_EDITOR){
			if(setmovementpermissions){
				renderMovPer(visrange, &srcrecttiles, &dstrecttiles);
			}
		}
		//printf("Number of text boxes: %ld\n", textboxes.size());
		if(textboxes.size() > 0){
			//printf("d\n");
			for(int i = 0; i < textboxes.size(); i++){
				//printf("1\n");
				textboxes.at(i)->renderBox();
				//printf("2\n");
				textboxes.at(i)->renderText();
				//printf("3\n");
			}
			if(textboxes.at(textboxes.size() - 1)->isActive()){
				textboxes.at(textboxes.size() - 1)->renderArrows();
				textboxes.at(textboxes.size() - 1)->tick();
			}
		}
		SDL_RenderPresent(renderer);
	} else {
		rendering = false;
	}
	return interval;
}

int scriptThread(void *param){
	std::string nextscript;
	//bool u = false;
	int entnum;
	while(active){
		if(currentscript == NULL){
			usleep(16666);
		} else {
			do{
				nextscript = currentscript->run();//To script loop
				entnum = currentscript->getEntityNum();
				delete currentscript;
				if(nextscript.compare("") != 0){
					currentscript = new Script(entnum, nextscript);
					//u = true;
				} else {
					currentscript = NULL;
					//u = false;
				}
			} while (currentscript != NULL);
		}
		if(battlemanager == NULL){
			usleep(16666);
		} else {
			caninteract = false;
			canmove = false;
			battlemanager->run();
			delete battlemanager;
			battlemanager = NULL;
			caninteract = true;
			canmove = true;
		}
	}
	return 0;
}

void init(int loadfromfile, int mapw, int maph, std::string fname){
	camposx = 0;
	camposy = 0;
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0){
		printf("Could not initialize SDL: %s\n", SDL_GetError());
	}
	window = SDL_CreateWindow("Something", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenwidth, screenheight, SDL_WINDOW_SHOWN);
	if(window == NULL){
		printf("Could not create window: %s\n", SDL_GetError());
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(renderer == NULL){
		printf("Could not create renderer: %s\n", SDL_GetError());
	} else {
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderSetScale(renderer, screenscale, screenscale);
	}
	int imgflags = IMG_INIT_PNG;
	if(!(IMG_Init(imgflags) & imgflags)){
		printf("Could not initialize SDL_Image: %s\n", SDL_GetError());
	} else {
		surface = SDL_GetWindowSurface(window);
	}
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
		printf("SDL_Mixer failed to initialize.\n");
	}
	/*if(TTF_Init() == -1){
		printf("Could not initialize SDL_TTF.\n");
	}*/
	if(programmode == NORMAL_GAMEPLAY){
		camerafollowplayer = true;
		renderentities = true;
	} else {
		renderentities = false;
	}
	caninteract = true;
	canmove = true;
	player = new ActiveEntity(ENTTYPE_PLAYER, 0, 0, true, "null", 0, 4, true, true, true, DIR_DOWN);
	//en = new Entity(ENTTYPE_PLAYER, 0, 0, 4, true, true, true, true, DIR_DOWN, "null", 0);
	entities.push_back(player);
	if(programmode == MAP_EDITOR){
		player->setRendered(false);
	}
	Character *playeru = new Character("Bob", 1, 20, 10, 5, 2, 3, true);
	party.push_back(playeru);//new Character("Bob", 1, 20, 10, 5, 2, 3, true));
	/*TextBox *b = new BattleTextBox(4, 4, 96, 80, 8, 8, 80, 72);
	textboxes.push_back(b);
	std::vector<std::string> s;
	s.push_back("      ");
	s.push_back("Lv. 0");
	//s.push_back("");
	s.push_back("HP: 000/000");
	s.push_back("MP: 000/000");
	b->loadText(s);
	BattleTextBox *b2 = static_cast<BattleTextBox*>(b);
	b2->setName("Bob");
	b2->setLevel(1);
	b2->setHP(20);
	b2->setMaxHP(20);
	b2->setMP(10);
	b2->setMaxMP(10);*/
	//en = new Entity(ENTTYPE_NPC, 12, 10, 7, true, true, true, true, DIR_DOWN, "aaab");
	//entities.push_back(*en);
	srcrecttiles.x = 0;
	srcrecttiles.y = 0;
	srcrecttiles.w = tilesize;
	srcrecttiles.h = tilesize;
	dstrecttiles.x = 0;
	dstrecttiles.y = 0;
	dstrecttiles.w = tilesize;
	dstrecttiles.h = tilesize;
	srcrectsubtiles.x = 0;
	srcrectsubtiles.y = 0;
	srcrectsubtiles.w = subtilesize;
	srcrectsubtiles.h = subtilesize;
	dstrectsubtiles.x = 0;
	dstrectsubtiles.y = 0;
	dstrectsubtiles.w = subtilesize;
	dstrectsubtiles.h = subtilesize;
	srcrecttext.x = 0;
	srcrecttext.y = 0;
	srcrecttext.w = subtilesize;
	srcrecttext.h = subtilesize;
	dstrecttext.x = 0;
	dstrecttext.y = 0;
	dstrecttext.w = subtilesize;
	dstrecttext.h = subtilesize;
	soundhandler = new SoundHandler();
	int uu = soundhandler->loadSound("beep1");
	inbattle = false;
	basic = new SpriteSheet(basicspritesheetname, 8, 15, renderer);//Defined in terms of 16x16 tiles
	characters = new SpriteSheet(characterspritesheetname, 12, 8, renderer);
	font = new SpriteSheet(fontspritesheetname, 16, 3, renderer);
	SpriteSheet *textboxspritesheet = new SpriteSheet(textboxspritesheetname, 1, 1, renderer);
	textboxtexture = textboxspritesheet->getTexture();
	delete textboxspritesheet;
	if(programmode == MAP_EDITOR){
		SpriteSheet *movperspritesheet = new SpriteSheet("assets/images/MovementPermissions.png", 4, 1, renderer);
		movpertex = movperspritesheet->getTexture();
		delete movperspritesheet;
		tileset = new TileSet(tilesetname);
		if(loadfromfile == 0){
			map = new Map(mapw, maph);
		} else {
			map = new Map(mapname, 0, DIR_DOWN);
		}
	} else if (programmode == TILESET_EDITOR){
		if(loadfromfile == 0){
			tileset = new TileSet(mapw);
			map = new Map(20, (mapw / 20) + 1, mapw);
		} else {
			tileset = new TileSet(tilesetname);
			map = new Map(20, (tileset->getLength() / 20) + 1, tileset->getLength());
		}
	} else {
		tileset = new TileSet(tilesetname);
		map = new Map(mapname, 0, DIR_DOWN);
	}
	currentscript = NULL;
	visrange = new int[4];
	//maintextbox = new TextBox(4, 99, 23, 2, textboxtexture, font);//, renderer);//23x2 character display
	heap = new uint8_t[256];
	renderflag = true;
	rendering = true;
	//Person *f = new Person(ENTTYPE_NPC, 2, 2, 1, true, true, true, true, DIR_DOWN, "null", 0, 1, 2);
	//printf("%d, %d\n", f->getXPos(), f->getH1());
	SDL_TimerID timer1 = SDL_AddTimer(16, renderFunc, NULL);
	scriptthread = SDL_CreateThread(scriptThread, "scriptThread", (void*) NULL);
	SDL_DetachThread(scriptthread);
}

int main(int argc, char **argv){
	if(argc != 6){
		printf("Not enough arguements.\n");
		return 0;
	}
	std::string tempstring = std::string(argv[1]);
	int u = std::stoi(tempstring);
	if(u < 0 || u > 2){
		printf("Unknown program mode.\n");
		return 0;
	}
	programmode = (pmode) u;
	tempstring = std::string(argv[2]);
	int loadfromfile = std::stoi(tempstring);
	tempstring = std::string(argv[3]);
	int mapw = std::stoi(tempstring);
	tempstring = std::string(argv[4]);
	int maph = std::stoi(tempstring);
	std::string fname = argv[5];
	basicspritesheetname = "assets/images/basictilesmodified.png";
	characterspritesheetname = "assets/images/characters.png";
	fontspritesheetname = "assets/images/font.png";
	textboxspritesheetname = "assets/images/textbox.png";
	if(programmode == MAP_EDITOR){
		tilesetname = "aaaa";
		mapname = argv[5];
	} else if (programmode == TILESET_EDITOR){
		tilesetname = argv[5];
		mapname = "aaaa";
	} else {
		tilesetname = "aaaa";
		mapname = "aaaa";
	}
	init(loadfromfile, mapw, maph, fname);
	while(active){
		usleep(16666);
	}
	quit();
	return 0;
}
