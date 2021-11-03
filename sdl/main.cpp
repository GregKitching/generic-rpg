#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
//#include <vector>

#include "constants.h"
#include "enums.h"
#include "SpriteSheet.h"
#include "Tile.h"
#include "TileSet.h"
#include "Map.h"
#include "Entity.h"

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

bool hidelayer1 = false;
bool setmovementpermissions = false;

bool camerafollowplayer = true;

int camposx;
int camposy;

int mouseposx;
int mouseposy;

pmode programmode;

bool clickaction = false;

int currentlayer = 0;
int currenttile = 0;
int currentsubtile = 0;
movper currentmovper = MOVEMENT_WALKABLE;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Surface *surface = NULL;

SDL_Event e;

SpriteSheet *basic;
SpriteSheet *characters;
TileSet *tileset;
Map *map;
Entity *player;

SDL_Rect srcrecttiles;
SDL_Rect dstrecttiles;
SDL_Rect srcrectsubtiles;
SDL_Rect dstrectsubtiles;

std::string basicspritesheetname;
std::string characterspritesheetname;
std::string tilesetname;
std::string mapname;

void quit(){
	SDL_FreeSurface(surface);
	surface = NULL;
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
	active = false;
}

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

void visibleTiles(int *range, int x, int y){//, Map *map){
	range[0] = (x / tilesize) - 1;
	range[1] = range[0] + 14;
	/*if(range[0] < 0){	values outside bounds of map are now used to render outside tiles
		range[0] = 0;
	}
	if (range[1] >= map->getWidth()){
		range[1] = map->getWidth() - 1;
	}*/
	range[2] = (y / tilesize) - 1;
	range[3] = range[2] + 12;
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

void renderMap(Map *map, TileSet *tileset, SpriteSheet *spritesheet, SDL_Rect *srcrect, SDL_Rect *dstrect){
	int range[4] = {0, 0, 0, 0};
	visibleTiles(range, camposx, camposy);//, map);
	renderLayer(0, map, tileset, spritesheet, range, srcrect, dstrect);
	if(!hidelayer1){
		renderLayer(1, map, tileset, spritesheet, range, srcrect, dstrect);
	}
}

void renderEntity(Entity *entity, SpriteSheet *spritesheet, SDL_Rect *srcrect, SDL_Rect *dstrect){
	int temp = entity->getSprite();
	srcrect->x = spritesheet->getTileX(temp);
	srcrect->y = spritesheet->getTileY(temp);
	dstrect->x = entity->getSpriteXPos() - camposx;
	dstrect->y = entity->getSpriteYPos() - camposy;
	SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
}

Uint32 callback(Uint32 interval, void *param){
	SDL_PollEvent(&e);
	if(e.type == SDL_QUIT){
		active = false;
	} else if(e.type == SDL_KEYDOWN){
		if(e.key.repeat == 0){
			switch(e.key.keysym.sym){
				case SDLK_w:
				wkey = true;
				break;
				
				case SDLK_s:
				skey = true;
				break;
				
				case SDLK_a:
				akey = true;
				break;
				
				case SDLK_d:
				dkey = true;
				break;
				
				case SDLK_l:
				if(programmode == TILESET_EDITOR){
					switchLayer();
				}
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
					currentmovper = MOVEMENT_WALKABLE;
					printf("Movement permission set to MOVEMENT_WALKABLE.\n");
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
	if(clickaction){
		if(programmode == MAP_EDITOR){
			if(setmovementpermissions){
				map->setMovementPermission(mouseposx, mouseposy, camposx, camposy, currentmovper);
			} else {
				map->changeTile(mouseposx, mouseposy, camposx, camposy, currenttile);
			}
		} else if (programmode == TILESET_EDITOR){
			changeSubtile(map, tileset);
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
	if(player->getMoveTimer() == 0){
		if(akey && !dkey && !wkey && !skey){
			player->move(DIR_LEFT, map);
		} else if (dkey && !akey && !wkey && !skey){
			player->move(DIR_RIGHT, map);
		} else if (wkey && !skey){
			player->move(DIR_UP, map);
		} else if (skey && !wkey){
			player->move(DIR_DOWN, map);
		}
	} else if (player->getMoveTimer() > 0){
		player->animate();
	}
	if(camerafollowplayer){
		camposx = player->getSpriteXPos() - 92;
		camposy = player->getSpriteYPos() - 67;
		//printf("%d, %d\n", camposx, camposy);
	}
	//int a = player->getXPos();
	/*moveup = false;
	movedown = false;
	moveleft = false;
	moveright = false;*/
	SDL_RenderClear(renderer);
	renderMap(map, tileset, basic, &srcrectsubtiles, &dstrectsubtiles);
	if(programmode == NORMAL_GAMEPLAY){
		renderEntity(player, characters, &srcrecttiles, &dstrecttiles);
	}
	SDL_RenderPresent(renderer);
	return interval;
}

void init(int loadfromfile, int mapw, int maph, std::string fname){
	camposx = 0;
	camposy = 0;
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
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
		SDL_RenderSetScale(renderer, 4.0f, 4.0f);
	}
	int imgflags = IMG_INIT_PNG;
	if(!(IMG_Init(imgflags) & imgflags)){
		printf("Could not initialize SDL_Image: %s\n", SDL_GetError());
	} else {
		surface = SDL_GetWindowSurface(window);
	}
	player = new Entity(9, 5, 4);
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
	basic = new SpriteSheet(basicspritesheetname, 8, 15, renderer);//Defined in terms of 16x16 tiles
	characters = new SpriteSheet(characterspritesheetname, 12, 8, renderer);
	if(programmode == MAP_EDITOR){
		tileset = new TileSet(tilesetname);
		if(loadfromfile == 0){
			map = new Map(mapw, maph);
		} else {
			map = new Map(mapname);
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
		map = new Map(mapname);
	}
	SDL_TimerID timerID = SDL_AddTimer(16, callback, NULL);
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
	if(programmode == MAP_EDITOR){
		tilesetname = "assets/tileset.txt";
		mapname = argv[5];
	} else if (programmode == TILESET_EDITOR){
		tilesetname = argv[5];
		mapname = "assets/map.txt";
	} else {
		tilesetname = "assets/tileset.txt";
		mapname = "assets/map.txt";
	}
	init(loadfromfile, mapw, maph, fname);
	while(active){
		usleep(60000);
	}
	quit();
	return 0;
}
