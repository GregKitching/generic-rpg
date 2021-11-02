#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
//#include <vector>

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

const int subtilesize = 8;
const int tilesize = 16;
int renderedsubtilesize = 8;
int renderedtilesize = 16;

int camposx;
int camposy;

int mouseposx;
int mouseposy;

int screenwidth = 800;
int screenheight = 600;

enum pmode{NORMAL_GAMEPLAY = 0, MAP_EDITOR = 1, TILESET_EDITOR = 2};
enum movper{MOVEMENT_WALKABLE = 0, MOVEMENT_BLOCKED = 1, MOVEMENT_LAYER0 = 2, MOVEMENT_LAYER1 = 3};
enum dir{DIR_DOWN = 0, DIR_LEFT = 1, DIR_RIGHT = 2, DIR_UP = 3};

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

void quit(){
	SDL_FreeSurface(surface);
	surface = NULL;
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
	active = false;
}

SDL_Surface* loadPng(std::string name){
	SDL_Surface *s = NULL;
	SDL_Surface *tempsurface = NULL;
	SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
	tempsurface = IMG_Load(name.c_str());
	if(tempsurface == NULL){
		printf("aaa%s\n", SDL_GetError());
	} else {
		s = SDL_ConvertSurface(tempsurface, format, 0);
		if(s == NULL){
			printf("%s\n", SDL_GetError());
		}
	}
	SDL_FreeSurface(tempsurface);
	SDL_FreeFormat(format);
	return s;
}

SDL_Texture* loadTexture(std::string name){
	SDL_Texture *tex = NULL;
	SDL_Surface *s = loadPng(name);
	tex = SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	return tex;
}

class SpriteSheet{
	private:
		SDL_Texture *tex = NULL;
		int width;
		int height;
		int tilecount;
		int subtilewidth;
		int subtileheight;
		int subtilecount;
	public:
		SpriteSheet(std::string filename, int w, int h);
		SDL_Texture* getTexture();
		int getTileX(int tilenum);
		int getTileY(int tilenum);
		int getSubtileX(int subtilenum);
		int getSubtileY(int subtilenum);
		int getTileCount();
		int getSubtileCount();
};

SpriteSheet::SpriteSheet(std::string filename, int w, int h){
	tex = loadTexture(filename.c_str());
	width = w;
	height = h;
	subtilewidth = width * 2;
	subtileheight = height * 2;
	tilecount = width * height;
	subtilecount = tilecount * 4;
}

SDL_Texture* SpriteSheet::getTexture(){
	return tex;
}

int SpriteSheet::getTileX(int tilenum){
	int x = 0;
	if(tilenum >= 0 && tilenum < tilecount){
		x = tilenum % width;
		x = x * tilesize;
	}
	return x;
}

int SpriteSheet::getTileY(int tilenum){
	int y = 0;
	if(tilenum >= 0 && tilenum < tilecount){
		y = tilenum / width;
		y = y * tilesize;
	}
	return y;
}

int SpriteSheet::getSubtileX(int subtilenum){
	int x = 0;
	if(subtilenum >= 0 && subtilenum < subtilecount){
		x = subtilenum % subtilewidth;
		x = x * subtilesize;
	}
	return x;
}

int SpriteSheet::getSubtileY(int subtilenum){
	int y = 0;
	if(subtilenum >= 0 && subtilenum < subtilecount){
		y = subtilenum / subtilewidth;
		y = y * subtilesize;
	}
	return y;
}

int SpriteSheet::getTileCount(){
	return tilecount;
}

int SpriteSheet::getSubtileCount(){
	return subtilecount;
}

class Tile{
	private:
		int *layer0;
		int *layer1;
	public:
		Tile(int *subtiles);
		int getSubtile(int layer, int subtile);
		void changeSubtile(int layer, int subtile, int u);
};

Tile::Tile(int *subtiles){
	layer0 = new int[4];
	layer1 = new int[4];
	layer0[0] = subtiles[0];
	layer0[1] = subtiles[1];
	layer0[2] = subtiles[2];
	layer0[3] = subtiles[3];
	layer1[0] = subtiles[4];
	layer1[1] = subtiles[5];
	layer1[2] = subtiles[6];
	layer1[3] = subtiles[7];
}

int Tile::getSubtile(int layer, int subtile){
	int u;
	switch(layer){
		case 0:
		u = layer0[subtile];
		break;
		
		case 1:
		u = layer1[subtile];
		break;
	}
	//printf("%d\n", u);
	return u;
}

void Tile::changeSubtile(int layer, int subtile, int u){
	//printf("%d\n", layer0[subtile]);
	switch(layer){
		case 0:
		layer0[subtile] = u;
		break;
		
		case 1:
		layer1[subtile] = u;
		break;
	}
}

class TileSet{
	private:
		int length;
		Tile **tiles;
	public:
		TileSet(int u);
		TileSet(std::string filename);
		int getSubtileFromTile(int tile, int layer, int subtile);
		int getLength();
		Tile* getTileObject(int u);
		void saveTileset(std::string filename);
};

TileSet::TileSet(int u){
	int *temp = new int[8];
	for(int i = 0; i < 8; i++){
		temp[i] = 479;
	}
	length = u;
	printf("%d\n", u);
	tiles = new Tile*[u];
	for(int i = 0; i < u; i++){
		tiles[i] = new Tile(temp);
	}
	delete temp;
}

TileSet::TileSet(std::string filename){
	std::ifstream tilesetfile;
	std::string line;
	tilesetfile.open(filename);
	if(!tilesetfile.is_open()){
		printf("Failed to open tileset file\n");
	} else {
		int *temp = new int[8];//{0, 0, 0, 0, 0, 0, 0, 0};
		getline(tilesetfile, line);
		length = std::stoi(line);
		tiles = new Tile*[length];
		for(int i = 0; i < length; i++){
			for(int j = 0; j < 8; j++){
				getline(tilesetfile, line);
				temp[j] = std::stoi(line);
			}
			tiles[i] = new Tile(temp);
		}
		delete temp;
		//printf("%d\n", temp[0]);
	}
	tilesetfile.close();
}

int TileSet::getSubtileFromTile(int tile, int layer, int subtile){
	int u = tiles[tile]->getSubtile(layer, subtile);
	//printf("%d, %d, %d, %d\n", tile, layer, subtile, u);
	return u;
}

int TileSet::getLength(){
	return length;
}

Tile* TileSet::getTileObject(int u){
	return tiles[u];
}

void TileSet::saveTileset(std::string filename){
	std::ofstream tilesetfile;
	tilesetfile.open(filename.c_str());
	if(!tilesetfile.is_open()){
		printf("Error saving to tileset file.\n");
	} else {
		tilesetfile << std::to_string(length);
		tilesetfile << "\n";
		for(int i = 0; i < length; i++){
			for(int j = 0; j < 4; j++){
				tilesetfile << std::to_string(tiles[i]->getSubtile(0, j));
				tilesetfile << "\n";
			}
			for(int j = 0; j < 4; j++){
				tilesetfile << std::to_string(tiles[i]->getSubtile(1, j));
				tilesetfile << "\n";
			}
		}
		printf("Tileset saved as %s\n", filename.c_str());
		tilesetfile.close();
	}
}

int* getClickedTile(){
	int *tilepos = new int[2];
	tilepos[0] = ((mouseposx / 4) + camposx) / renderedtilesize;
	tilepos[1] = ((mouseposy / 4) + camposy) / renderedtilesize;
	return tilepos;
}

class Map{
	private:
		int width;//width in 16x16 tiles
		int height;
		int *tiles;
		int outsidetile;//Tile to be rendered in areas ouside the map so it's not just surrounded by black
		movper *movementpermissions;
	public:
		Map(int w, int h);
		Map(int w, int h, int tilesetlength);
		Map(std::string filaname);
		int getWidth();
		int getHeight();
		int getPixWidth();
		int getPixHeight();
		int getTile(int x, int y);
		void setOutsideTile(int u);
		int getOutsideTile();
		movper getMovementPermission(int x, int y);
		void setMovementPermission();
		void changeTile();
		void save(std::string filename);
};

Map::Map(int w, int h){
	width = w;
	height = h;
	outsidetile = 0;
	int mapsize = width * height;
	tiles = new int[mapsize];
	movementpermissions = new movper[mapsize];
	for(int i = 0; i < mapsize; i++){
		tiles[i] = 0;
		movementpermissions[i] = MOVEMENT_WALKABLE;
	}
}

Map::Map(int w, int h, int tilesetlength){
	width = w;
	height = h;
	outsidetile = 0;
	int mapsize = width * height;
	tiles = new int[mapsize];
	movementpermissions = new movper[mapsize];
	for(int i = 0; i < mapsize; i++){
		if(i < tilesetlength){
			tiles[i] = i;
		} else {
			tiles[i] = 0;
		}
		movementpermissions[i] = MOVEMENT_WALKABLE;
	}
}

Map::Map(std::string filename){
	std::ifstream mapfile;
	std::string line;
	mapfile.open(filename.c_str());
	if(!mapfile.is_open()){
		printf("Failed to open map file\n");
	} else {
		getline(mapfile, line);
		width = std::stoi(line);
		getline(mapfile, line);
		height = std::stoi(line);
		getline(mapfile, line);
		outsidetile = std::stoi(line);
		int mapsize = width * height;
		tiles = new int[mapsize];
		movementpermissions = new movper[mapsize];
		for(int i = 0; i < mapsize; i++){
			getline(mapfile, line);
			tiles[i] = std::stoi(line);
			getline(mapfile, line);
			movementpermissions[i] = (movper) std::stoi(line);
		}
	}
	mapfile.close();
}

int Map::getWidth(){
	return width;
}

int Map::getHeight(){
	return height;
}

void Map::setOutsideTile(int u){
	outsidetile = u;
}

int Map::getOutsideTile(){
	return outsidetile;
}

int Map::getTile(int x, int y){
	int result;
	if(x < 0 || x >= width || y < 0 || y >= height){
		result = outsidetile;
	} else {
		int offset = y * width + x;
		result = tiles[offset];
	}
	//printf("%d\n", result);
	return result;
}

movper Map::getMovementPermission(int x, int y){
	int offset = y * width + x;
	return movementpermissions[offset];
}

void Map::setMovementPermission(){
	int *tilepos = getClickedTile();
	if(tilepos[0] >= 0 && tilepos[0] < width && tilepos[1] >= 0 && tilepos[1] < height){
		int tilenum = tilepos[0] + (tilepos[1] * width);
		movementpermissions[tilenum] = currentmovper;
		printf("a\n");
	}
}

void Map::changeTile(){
	int *tilepos = getClickedTile();
	if(tilepos[0] >= 0 && tilepos[0] < width && tilepos[1] >= 0 && tilepos[1] < height){
		int tilenum = tilepos[0] + (tilepos[1] * width);
		tiles[tilenum] = currenttile;
	}
}

void Map::save(std::string filename){
	std::ofstream mapfile;
	mapfile.open(filename.c_str());
	if(!mapfile.is_open()){
		printf("Error saving to file");
	} else {
		mapfile << std::to_string(width);
		mapfile << "\n";
		mapfile << std::to_string(height);
		mapfile << "\n";
		mapfile << std::to_string(outsidetile);
		mapfile << "\n";
		for(int i = 0; i < width * height; i++){
			mapfile << std::to_string(tiles[i]);
			mapfile << "\n";
			mapfile << std::to_string((int) movementpermissions[i]);
			mapfile << "\n";
		}
		printf("Map saved as %s\n", filename.c_str());
	}
	mapfile.close();
}

Map *map;

class Entity{
	private:
		int xpos;//In terms of 16x16 tiles
		int ypos;
		int spritexpos;//In terms of pixels
		int spriteypos;
		int basesprite;
		int walkcycle;
		dir facing;
		dir movedir;
		int movetimer;
		bool oddwalkcycle;
		movper currentmovper;
		void move2(int newx, int newy);
	public:
		Entity(int x, int y, int s);
		void setLocation(int x, int y);
		int getXPos();
		int getYPos();
		int getSpriteXPos();
		int getSpriteYPos();
		int getSprite();
		void move(dir direction);
		void animate();
		int getMoveTimer();
};

Entity::Entity(int x, int y, int s){
	xpos = x;
	ypos = y;
	spritexpos = x * renderedtilesize;
	spriteypos = y * renderedtilesize;
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

void Entity::move(dir direction){
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

Entity *player;

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

int getClickedSubtile(){
	int subtileposx = ((mouseposx / 4) + camposx) / renderedsubtilesize;
	int subtileposy = ((mouseposy / 4) + camposy) / renderedsubtilesize;
	int u;
	bool xiseven = (subtileposx % 2 == 0);
	bool yiseven = (subtileposy % 2 == 0);
	if(xiseven && yiseven){
		u = 0;
	} else if (!xiseven && yiseven){
		u = 1;
	} else if (xiseven && !yiseven){
		u = 2;
	} else {
		u = 3;
	}
	//printf("%d, %d\n", mouseposx, mouseposy);
	return u;
}

void changeSubtile(Map *map, TileSet *tileset){
	int *tilepos = getClickedTile();
	int subtilepos = getClickedSubtile();
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

bool init(){
	bool success = true;
	camposx = 0;
	camposy = 0;
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
		printf("Could not initialize SDL: %s\n", SDL_GetError());
	}
	window = SDL_CreateWindow("Something", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenwidth, screenheight, SDL_WINDOW_SHOWN);
	if(window == NULL){
		printf("Could not create window: %s\n", SDL_GetError());
		success = false;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(renderer == NULL){
		printf("Could not create renderer: %s\n", SDL_GetError());
		success = false;
	} else {
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderSetScale(renderer, 4.0f, 4.0f);
	}
	int imgflags = IMG_INIT_PNG;
	if(!(IMG_Init(imgflags) & imgflags)){
		printf("Could not initialize SDL_Image: %s\n", SDL_GetError());
		success = false;
	} else {
		surface = SDL_GetWindowSurface(window);
	}
	return success;
}

void visibleTiles(int *range, int x, int y){//, Map *map){
	range[0] = (x / renderedtilesize) - 1;
	range[1] = range[0] + 14;
	/*if(range[0] < 0){	values outside bounds of map are now used to render outside tiles
		range[0] = 0;
	}
	if (range[1] >= map->getWidth()){
		range[1] = map->getWidth() - 1;
	}*/
	range[2] = (y / renderedtilesize) - 1;
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
		dstrect->y = i * renderedtilesize - camposy;
		for(int j = range[0]; j <= range[1]; j++){
			dstrect->x = j * renderedtilesize - camposx;
			temp = map->getTile(j, i);
			temp2 = tileset->getSubtileFromTile(temp, layer, 0);
			srcrect->x = spritesheet->getSubtileX(temp2);
			srcrect->y = spritesheet->getSubtileY(temp2);
			SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
			dstrect->x += renderedsubtilesize;
			
			temp2 = tileset->getSubtileFromTile(temp, layer, 1);
			srcrect->x = spritesheet->getSubtileX(temp2);
			srcrect->y = spritesheet->getSubtileY(temp2);
			SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
			dstrect->y += renderedsubtilesize;
			
			temp2 = tileset->getSubtileFromTile(temp, layer, 3);
			srcrect->x = spritesheet->getSubtileX(temp2);
			srcrect->y = spritesheet->getSubtileY(temp2);
			SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
			dstrect->x -= renderedsubtilesize;
			
			temp2 = tileset->getSubtileFromTile(temp, layer, 2);
			srcrect->x = spritesheet->getSubtileX(temp2);
			srcrect->y = spritesheet->getSubtileY(temp2);
			SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
			dstrect->y -= renderedsubtilesize;
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
	if(player->getMoveTimer() == 0){
		if(akey && !dkey && !wkey && !skey){
			player->move(DIR_LEFT);
		} else if (dkey && !akey && !wkey && !skey){
			player->move(DIR_RIGHT);
		} else if (wkey && !skey){
			player->move(DIR_UP);
		} else if (skey && !wkey){
			player->move(DIR_DOWN);
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
	return interval;
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
	bool a = init();
	std::string basicspritesheetname = "assets/images/basictilesmodified.png";
	std::string characterspritesheetname = "assets/images/characters.png";
	std::string tilesetname;
	std::string mapname;
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
	player = new Entity(9, 5, 4);
	SDL_TimerID timerID = SDL_AddTimer(16, callback, NULL);
	SDL_Rect srcrecttiles;
	srcrecttiles.x = 0;
	srcrecttiles.y = 0;
	srcrecttiles.w = tilesize;
	srcrecttiles.h = tilesize;
	SDL_Rect dstrecttiles;
	dstrecttiles.x = 0;
	dstrecttiles.y = 0;
	dstrecttiles.w = renderedtilesize;
	dstrecttiles.h = renderedtilesize;
	SDL_Rect srcrectsubtiles;
	srcrectsubtiles.x = 0;
	srcrectsubtiles.y = 0;
	srcrectsubtiles.w = subtilesize;
	srcrectsubtiles.h = subtilesize;
	SDL_Rect dstrectsubtiles;
	dstrectsubtiles.x = 0;
	dstrectsubtiles.y = 0;
	dstrectsubtiles.w = renderedsubtilesize;
	dstrectsubtiles.h = renderedsubtilesize;
	SpriteSheet *basic = new SpriteSheet(basicspritesheetname, 8, 15);//Defined in terms of 16x16 tiles
	SpriteSheet *characters = new SpriteSheet(characterspritesheetname, 12, 8);
	TileSet *tileset;
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
	while(active){
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
					map->setMovementPermission();
				} else {
					map->changeTile();
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
		SDL_RenderClear(renderer);
		renderMap(map, tileset, basic, &srcrectsubtiles, &dstrectsubtiles);
		if(programmode == NORMAL_GAMEPLAY){
			renderEntity(player, characters, &srcrecttiles, &dstrecttiles);
		}
		SDL_RenderPresent(renderer);
	}
	quit();
	return 0;
}
