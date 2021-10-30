#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//using namespace std;

bool active = true;

bool moveup = false;
bool movedown = false;
bool moveleft = false;
bool moveright = false;

int subtilesize = 8;
int tilesize = 16;
int renderedtilesize = 32;

int xoffset;
int yoffset;

int mouseposx;
int mouseposy;

int screenwidth = 640;
int screenheight = 480;

bool mousebuttondown = false;

int currentlayer = 0;
int currenttile = 0;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Surface *surface = NULL;
//SDL_Surface *image = NULL;

SDL_Event e;

void quit(){
	SDL_FreeSurface(surface);
	surface = NULL;
	//SDL_FreeSurface(image);
	//image = NULL;
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
	active = false;
}

/*bool loadBmp(char *name){
	printf("%s\n", name);
	bool success = true;
	image = SDL_LoadBMP(name);
	if(image == NULL){
		printf("aaa%s\n", SDL_GetError());
		success = false;
	}
	return success;
}*/

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
		int subtilecount;
	public:
		SpriteSheet(std::string filename, int w, int h);
		SDL_Texture* getTexture();
		SDL_Rect getSubtile(int subtilenum);
		int getSubtileCount();
};

SpriteSheet::SpriteSheet(std::string filename, int w, int h){
	tex = loadTexture(filename.c_str());
	width = w;
	height = h;
	subtilecount = width * height;
}

SDL_Texture* SpriteSheet::getTexture(){
	return tex;
}

SDL_Rect SpriteSheet::getSubtile(int subtilenum){
	SDL_Rect rect = {0, 0, 0, 0};
	if(subtilenum >= 0 && subtilenum < subtilecount){
		int x = subtilenum % width;
		int y = subtilenum / width;
		x = x * subtilesize;
		y = y * subtilesize;
		rect.x = x;
		rect.y = y;
		rect.w = subtilesize;
		rect.h = subtilesize;
	}
	return rect;
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
		//void render(SpriteSheet spritesheet, int x, int y);
};

Tile::Tile(int *subtiles){
	layer0 = new int[4];//{0, 0, 0, 0};
	layer1 = new int[4];//{0, 0, 0, 0};
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

//void Tile::render(SpriteSheet

class TileSet{
	private:
		int length;
		Tile **tiles;
		//std::vector<Tile*> tiles;
	public:
		TileSet(std::string filename);
		int getSubtileFromTile(int tile, int layer, int subtile);
		int getLength();
};

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
			//tiles.push_back(new Tile(temp));
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

enum movper{MOVEMENT_WALKABLE, MOVEMENT_BLOCKED};

class Map{
	private:
		int width;
		int height;
		int pixwidth;
		int pixheight;
		int *tiles;
		int outsidetile;
		movper *movementpermissions;
	public:
		Map(int w, int h);
		Map(std::string filaname);
		int getWidth();
		int getHeight();
		int getPixWidth();
		int getPixHeight();
		int getTile(int x, int y);
		void setOutsideTile(int u);
		int getOutsideTile();
		movper getMovementPermissions(int x, int y);
		void changeTile();
		void save(std::string filename);
};

Map::Map(int w, int h){
	width = w;
	height = h;
	pixwidth = w * tilesize;
	pixheight = h * tilesize;
	outsidetile = 0;
	int mapsize = width * height;
	tiles = new int[mapsize];
	movementpermissions = new movper[mapsize];
	for(int i = 0; i < mapsize; i++){
		tiles[i] = 0;
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
		pixwidth = width * tilesize;
		pixheight = height * tilesize;
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

int Map::getPixWidth(){
	return pixwidth;
}

int Map::getPixHeight(){
	return pixheight;
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

movper Map::getMovementPermissions(int x, int y){
	int offset = y * width + x;
	return movementpermissions[offset];
}

void Map::changeTile(){
	int tileposx = (mouseposx + xoffset) / renderedtilesize;
	int tileposy = (mouseposy + yoffset) / renderedtilesize;
	if(tileposx >= 0 && tileposx < width && tileposy >= 0 && tileposy < height){
		int tilenum = tileposx + (tileposy * width);
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

/*void setOutsideTile(Map *map, int tile){
	int temp;
	std::cout << "Enter outside tile layer 0:" << std::endl;
	std::cin >> temp;
	if(temp >= 0 && temp < spritesheet->getSubtileCount()){
		map->setOutsideTile0(temp);
		printf("Outside tile 0 set to %d.\n", temp);
	} else {
		printf("Number out of range.\n");
	}
	std::cout << "Enter outside tile layer 1:" << std::endl;
	std::cin >> temp;
	if(temp >= 0 && temp < spritesheet->getSubtileCount()){
		map->setOutsideTile1(temp);
		printf("Outside tile 1 set to %d.\n", temp);
	} else {
		printf("Number out of range.\n");
	}
}*/

bool init(){
	bool success = true;
	xoffset = 0;
	yoffset = 0;
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
		printf("Could not initialize SDL: %s\n", SDL_GetError());
	}
	window = SDL_CreateWindow("Something", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
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

void visibleTiles(int *range, int x, int y, Map *map){
	range[0] = (x / renderedtilesize) - 1;
	range[1] = range[0] + 21;
	/*if(range[0] < 0){
		range[0] = 0;
	}
	if (range[1] >= map->getWidth()){
		range[1] = map->getWidth() - 1;
	}*/
	range[2] = (y / renderedtilesize) - 1;
	range[3] = range[2] + 16;
	/*if(range[2] < 0){
		range[2] = 0;
	}
	if (range[3] >= map->getHeight()){
		range[3] = map->getHeight() - 1;
	}*/
	printf("Range: x = %d - %d, y = %d - %d\n", range[0], range[1], range[2], range[3]);
}

void renderLayer(int layer, Map *map, TileSet *tileset, SpriteSheet *spritesheet, int *range, SDL_Rect *srcrect, SDL_Rect *dstrect){
	int temp;
	int temp2;
	for(int i = range[2]; i <= range[3]; i++){
		dstrect->y = i * renderedtilesize - yoffset;
		for(int j = range[0]; j <= range[1]; j++){
			dstrect->x = j * renderedtilesize - xoffset;
			temp = map->getTile(j, i);
			temp2 = tileset->getSubtileFromTile(temp, layer, 0);
			*srcrect = spritesheet->getSubtile(temp2);
			SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
			dstrect->x += tilesize;
			
			temp2 = tileset->getSubtileFromTile(temp, layer, 1);
			*srcrect = spritesheet->getSubtile(temp2);
			SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
			dstrect->y += tilesize;
			
			temp2 = tileset->getSubtileFromTile(temp, layer, 3);
			*srcrect = spritesheet->getSubtile(temp2);
			SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
			dstrect->x -= tilesize;
			
			temp2 = tileset->getSubtileFromTile(temp, layer, 2);
			*srcrect = spritesheet->getSubtile(temp2);
			SDL_RenderCopy(renderer, spritesheet->getTexture(), srcrect, dstrect);
			dstrect->y -= tilesize;
		}
	}
}

void renderMap(Map *map, TileSet *tileset, SpriteSheet *spritesheet, SDL_Rect *srcrect, SDL_Rect *dstrect){
	SDL_RenderClear(renderer);
	int range[4] = {0, 0, 0, 0};
	visibleTiles(range, xoffset, yoffset, map);
	renderLayer(0, map, tileset, spritesheet, range, srcrect, dstrect);
	renderLayer(1, map, tileset, spritesheet, range, srcrect, dstrect);
	SDL_RenderPresent(renderer);
}

Uint32 callback(Uint32 interval, void *param){
	if(moveup && !movedown){
		yoffset -= 10;
	} else if (movedown && !moveup){
		yoffset += 10;
	}
	if(moveleft && !moveright){
		xoffset -= 10;
	} else if (moveright && !moveleft){
		xoffset += 10;
	}
	return 16;
}

int main(int argc, char **argv){
	if(argc != 5){
		printf("Not enough arguements.\n");
		return 0;
	}
	std::string tempstring = std::string(argv[1]);
	int loadfromfile = std::stoi(tempstring);
	tempstring = std::string(argv[2]);
	int mapw = std::stoi(tempstring);
	tempstring = std::string(argv[3]);
	int maph = std::stoi(tempstring);
	bool a = init();
	std::string basicspritesheetname = "assets/images/basictiles.png";
	std::string characterspritesheetname = "assets/images/characters.png";
	std::string tilesetname = "tileset.txt";
	std::string mapname = argv[4];
	SDL_TimerID timerID = SDL_AddTimer(0, callback, NULL);
	/*texture = loadTexture(basicspritesheetname);
	if(texture == NULL){
		printf("%s\n", SDL_GetError());
	}*/
	SDL_Rect srcrect;
	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = subtilesize;
	srcrect.h = subtilesize;
	SDL_Rect dstrect;
	dstrect.x = 0;
	dstrect.y = 0;
	dstrect.w = tilesize;
	dstrect.h = tilesize;
	SpriteSheet *basic = new SpriteSheet(basicspritesheetname, 16, 30);
	//SpriteSheet *characters = new SpriteSheet(characterspritesheetname, 24, 16);
	TileSet *tileset = new TileSet(tilesetname);
	Map *map;
	if(loadfromfile == 0){
		map = new Map(mapw, maph);
	} else {
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
					moveup = true;
					break;
					
					case SDLK_s:
					movedown = true;
					break;
					
					case SDLK_a:
					moveleft = true;
					break;
					
					case SDLK_d:
					moveright = true;
					break;
					
					/*case SDLK_l:
					switchLayer();
					break;*/
					
					case SDLK_t:
					switchTile(tileset);
					break;
					
					case SDLK_n:
					map->save("map.txt");
					break;
					
					/*case SDLK_o:
					setOutsideTiles(map, basic);
					break;*/
					
					default:
					break;
				}
			}
		 } else if(e.type == SDL_KEYUP){
			if(e.key.repeat == 0){//Might be unnecessary
				switch(e.key.keysym.sym){
					case SDLK_w:
					moveup = false;
					break;
					
					case SDLK_s:
					movedown = false;
					break;
					
					case SDLK_a:
					moveleft = false;
					break;
					
					case SDLK_d:
					moveright = false;
					break;
					
					default:
					break;
				}
			}
		} else if (e.type == SDL_MOUSEMOTION){
			SDL_GetMouseState(&mouseposx, &mouseposy);
			//printf("%d, %d\n", mouseposx, mouseposy);
		} else if (e.type == SDL_MOUSEBUTTONDOWN){
			mousebuttondown = true;
		} else if (e.type == SDL_MOUSEBUTTONUP){
			mousebuttondown = false;
		}
		if(mousebuttondown){
			map->changeTile();
		}
		renderMap(map, tileset, basic, &srcrect, &dstrect);
	}
	quit();
	return 0;
}
