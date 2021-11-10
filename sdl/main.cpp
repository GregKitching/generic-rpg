#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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
#include "TextBox.h"
#include "Script.h"
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

bool camerafollowplayer = true;

int camposx;
int camposy;

int mouseposx;
int mouseposy;

int test = 0;

pmode programmode;

bool clickaction = false;
//bool buttonaction = false;
//bool caninteract;
//bool canmove;

Script *currentscript = NULL;

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
SpriteSheet *font;
TileSet *tileset;
//Map *map;
//std::vector<Entity> entities;
SDL_Texture *textboxtexture = NULL;
SDL_Texture *letterA = NULL;

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

//uint8_t *heap;
//TextBox *maintextbox;

void quit(){
	//texture->free();
	//letterA->free();
	//TTF_CloseFont(font);
	//font = NULL;
	SDL_FreeSurface(surface);
	surface = NULL;
	SDL_DestroyRenderer(renderer);
	renderer = NULL;
	SDL_DestroyWindow(window);
	window = NULL;
	//TTF_Quit();
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

void createScript(int u){
	currentscript = new Script(u, entities.at(u).getScript());
	//currentscript->executeCommand();
	/*std::ifstream scriptfile;
	std::string line;
	scriptfile.open(script.c_str());
	if(!scriptfile.is_open()){
		printf("Failed to open script file \"%s\".\n", script.c_str());
	} else {
		std::vector<command> commands;
		std::vector<std::string> arguments;
		command tempcommand = CMD_NOP;
		while(tempcommand != CMD_ENDSCRIPT){
			getline(scriptfile, line);
			tempcommand = (command) std::stoi(line);
			commands.push_back(tempcommand);
			if(tempcommand == CMD_DISPLAYTEXT || tempcommand == CMD_NPCFACEPLAYER){
				getline(scriptfile, line);
				arguments.push_back(line);
			}
		}
		scriptfile.close();
		int argumentpos = 0;
		for(int i = 0; i < commands.size(); i++){
			switch(commands.at(i)){
				case CMD_DISPLAYTEXT:
				maintextbox->setVisible(true);
				maintextbox->setText(arguments.at(argumentpos));
				argumentpos++;
				break;
				
				case CMD_DISABLEMOVE:
				canmove = false;
				caninteract = false;
				break;
				
				case CMD_ENABLEMOVE:
				canmove = true;
				caninteract = true;
				break;
				
				case CMD_NPCFACEPLAYER:
				int u = std::stoi(arguments.at(argumentpos));
				argumentpos++;
				entities.at(u).setFacingDir(oppositeDir(entities.at(0).getFacingDir()));
				break;
			}
		}
		commands.clear();
		arguments.clear();
	}*/
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

int getInteractableEntityAtPosition(int x, int y){
	for(int i = 0; i < entities.size(); i++){
		if(entities.at(i).getXPos() == x && entities.at(i).getYPos() == y && entities.at(i).isInteractable()){
			return i;
		}
	}
	return -1;
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

void renderMap(Map *map, TileSet *tileset, SpriteSheet *spritesheet, int *range, SDL_Rect *srcrect, SDL_Rect *dstrect){
	renderLayer(0, map, tileset, spritesheet, range, srcrect, dstrect);
	if(!hidelayer1){
		renderLayer(1, map, tileset, spritesheet, range, srcrect, dstrect);
	}
}

void renderEntities(std::vector<Entity> *entities, SpriteSheet *spritesheet, int *range, SDL_Rect *srcrect, SDL_Rect *dstrect){
	int temp;
	for(int i = 0; i < entities->size(); i++){
		if(entities->at(i).getXPos() > range[0] && entities->at(i).getXPos() <= range[1] && entities->at(i).getYPos() > range[2] && entities->at(i).getYPos() <= range[3] && entities->at(i).isRendered()){// == ENTTYPE_PLAYER || entities->at(i).getType() == ENTTYPE_NPC){
			temp = entities->at(i).getSprite();
			srcrect->x = spritesheet->getTileX(temp);
			srcrect->y = spritesheet->getTileY(temp);
			dstrect->x = entities->at(i).getSpriteXPos() - camposx;
			dstrect->y = entities->at(i).getSpriteYPos() - camposy;
			SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
		}
	}
}

/*void renderTextBox(TextBox *t){
	t->render(renderer);
}

void renderText(SDL_Rect *srcrect, SDL_Rect *dstrect){
	SDL_RenderCopy(renderer, letterA, srcrect, dstrect);
}*/

void buttonAction(){
	test++;
	printf("%d\n", test);
	if(caninteract){
		printf("1\n");
		caninteract = false;
		int *facingpos = entities.at(0).getAdjacentTile(entities.at(0).getFacingDir(), map);
		int u = getInteractableEntityAtPosition(facingpos[0], facingpos[1]);
		delete facingpos;
		if(u != -1){
			if(entities.at(u).getType() == ENTTYPE_NPC){
				createScript(u);
				//dir direction = oppositeDir(entities.at(0).getFacingDir());
				//entities.at(u).setFacingDir(direction);
			}
		}
	} else {
		printf("2\n");
		if(currentscript != NULL){
			if(currentscript->waitingForInput()){
				currentscript->advance();
				printf("h\n");
			}
		}
		//caninteract = true;
	}
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
				lkey = true;
				/*if(programmode == TILESET_EDITOR){
					switchLayer();
				}*/
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
	/*if(sckey){
		if(caninteract){
			int *facingpos = entities.at(0).getAdjacentTile(entities.at(0).getFacingDir(), map);
			int u = getInteractableEntityAtPosition(facingpos[0], facingpos[1]);
			delete facingpos;
			if(u != -1){
				if(entities.at(u).getType() == ENTTYPE_NPC){
					createScript(u);
					//dir direction = oppositeDir(entities.at(0).getFacingDir());
					//entities.at(u).setFacingDir(direction);
				}
			}
			caninteract = false;
		} else {
			if(currentscript != NULL){
				currentscript->advance();
				printf("h\n");
			}
			caninteract = true;
		}
		sckey = false;
	}*/
	if(entities.at(0).getMoveTimer() == 0 && canmove){
		if(akey && !dkey && !wkey && !skey){
			entities.at(0).move(DIR_LEFT);//, map, &entities);
		} else if (dkey && !akey && !wkey && !skey){
			entities.at(0).move(DIR_RIGHT);//, map, &entities);
		} else if (wkey && !skey){
			entities.at(0).move(DIR_UP);//, map, &entities);
		} else if (skey && !wkey){
			entities.at(0).move(DIR_DOWN);//, map, &entities);
		}
	}
	for(int i = 0; i < entities.size(); i++){
		if (entities.at(i).getMoveTimer() > 0){
			entities.at(i).animate();
		}
	}
	if(camerafollowplayer){
		camposx = entities.at(0).getSpriteXPos() - 92;
		camposy = entities.at(0).getSpriteYPos() - 67;
	}
	/*if(currentscript != NULL){
		currentscript->tick();
	}*/
	visibleTiles(visrange, camposx, camposy);
	SDL_RenderClear(renderer);
	renderMap(map, tileset, basic, visrange, &srcrectsubtiles, &dstrectsubtiles);
	if(programmode == NORMAL_GAMEPLAY){
		renderEntities(&entities, characters, visrange, &srcrecttiles, &dstrecttiles);
	}
	if(maintextbox->getVisible()){
		maintextbox->render(renderer);
		maintextbox->renderText(renderer, font);
	}
	/*if(currentscript != NULL){
		currentscript->tick();
	}*/
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
	/*if(TTF_Init() == -1){
		printf("Could not initialize SDL_TTF.\n");
	}*/
	caninteract = true;
	canmove = true;
	Entity *en;
	en = new Entity(ENTTYPE_PLAYER, 9, 5, 4, true, true, true, true, DIR_DOWN, "");
	entities.push_back(*en);
	en = new Entity(ENTTYPE_NPC, 12, 10, 7, true, true, true, true, DIR_DOWN, "aaac");
	entities.push_back(*en);
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
	basic = new SpriteSheet(basicspritesheetname, 8, 15, renderer);//Defined in terms of 16x16 tiles
	characters = new SpriteSheet(characterspritesheetname, 12, 8, renderer);
	font = new SpriteSheet(fontspritesheetname, 16, 3, renderer);
	SpriteSheet *textboxspritesheet = new SpriteSheet(textboxspritesheetname, 1, 1, renderer);
	textboxtexture = textboxspritesheet->getTexture();
	textboxspritesheet = NULL;
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
	visrange = new int[4];
	maintextbox = new TextBox(0, 83, 200, 67, textboxtexture);
	/*white.r = 255;
	white.g = 255;
	white.b = 255;
	white.a = 255;
	font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 28);
	if(font == NULL){
		printf("Failed to load font.\n");
	}*/
	//createCharTex("A", white);
	heap = new uint8_t[256];
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
	fontspritesheetname = "assets/images/font.png";
	textboxspritesheetname = "assets/images/textbox.png";
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
		if(currentscript == NULL){
			usleep(16666);
		} else {
			if(currentscript->waitingForInput()){
				usleep(16666);
			} else {
				if(currentscript->executeCommand()){
					delete currentscript;
					currentscript = NULL;
				}
			}
		}
	}
	quit();
	return 0;
}
