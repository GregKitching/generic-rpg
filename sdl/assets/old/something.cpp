#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

//using namespace std;

bool active = true;

bool moveup = false;
bool movedown = false;
bool moveleft = false;
bool moveright = false;

int tilesize = 16;
int blocksize = 32;

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

class TileSet{
	private:
		SDL_Texture *tex = NULL;
		int width;
		int height;
		int tilecount;
	public:
		TileSet(std::string filename, int w, int h);
		SDL_Rect getTile(int tilenum);
		int getTileCount();
};

TileSet::TileSet(std::string filename, int w, int h){
	tex = loadTexture(filename.c_str());
	width = w;
	height = h;
	tilecount = width * height;
}

SDL_Rect TileSet::getTile(int tilenum){
	SDL_Rect rect = {0, 0, 0, 0};
	if(tilenum >= 0 && tilenum < tilecount){
		int x = tilenum % width;
		int y = tilenum / width;
		x = x * tilesize;
		y = y * tilesize;
		rect.x = x;
		rect.y = y;
		rect.w = tilesize;
		rect.h = tilesize;
	}
	return rect;
}

int TileSet::getTileCount(){
	return tilecount;
}

enum movper{MOVEMENT_WALKABLE, MOVEMENT_BLOCKED};

class Map{
	private:
		int width;
		int height;
		int pixwidth;
		int pixheight;
		int *layer0;
		int *layer1;
		int outsidetile0;
		int outsidetile1;
		movper *movementpermissions;
	public:
		Map(int w, int h);
		Map(std::string filaname);
		int getWidth();
		int getHeight();
		int getPixWidth();
		int getPixHeight();
		void setOutsideTile0(int u);
		void setOutsideTile1(int u);
		int getOutsideTile0();
		int getOutsideTile1();
		int getLayer(int layer, int x, int y);
		movper getMovementPermissions(int x, int y);
		void changeTile();
		void save(std::string filename);
};

Map::Map(int w, int h){
	width = w;
	height = h;
	pixwidth = w * blocksize;
	pixheight = h * blocksize;
	outsidetile0 = 119;
	outsidetile1 = 119;
	int mapsize = width * height;
	layer0 = new int[mapsize];
	layer1 = new int[mapsize];
	movementpermissions = new movper[mapsize];
	for(int i = 0; i < mapsize; i++){
		layer0[i] = 119;
		layer1[i] = 119;
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
		pixwidth = width * blocksize;
		pixheight = height * blocksize;
		getline(mapfile, line);
		outsidetile0 = std::stoi(line);
		getline(mapfile, line);
		outsidetile1 = std::stoi(line);
		int mapsize = width * height;
		layer0 = new int[mapsize];
		layer1 = new int[mapsize];
		movementpermissions = new movper[mapsize];
		for(int i = 0; i < mapsize; i++){
			getline(mapfile, line);
			layer0[i] = std::stoi(line);
			getline(mapfile, line);
			layer1[i] = std::stoi(line);
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

void Map::setOutsideTile0(int u){
	outsidetile0 = u;
}

void Map::setOutsideTile1(int u){
	outsidetile1 = u;
}

int Map::getOutsideTile0(){
	return outsidetile0;
}

int Map::getOutsideTile1(){
	return outsidetile1;
}

int Map::getLayer(int layer, int x, int y){
	int result;
	if(x < 0 || x >= width || y < 0 || y >= height){
		switch(layer){
			case 0:
			result = outsidetile0;
			break;
			
			case 1:
			result = outsidetile1;
			break;
		}
	} else {
		int offset = y * width + x;
		switch(layer){
			case 0:
			result = layer0[offset];
			break;
			
			case 1:
			result = layer1[offset];
			break;
		}
	}
	return result;
}

movper Map::getMovementPermissions(int x, int y){
	int offset = y * width + x;
	return movementpermissions[offset];
}

void Map::changeTile(){
	int tileposx = (mouseposx + xoffset) / blocksize;
	int tileposy = (mouseposy + yoffset) / blocksize;
	if(tileposx >= 0 && tileposx < width && tileposy >= 0 && tileposy < height){
		int tilenum = tileposx + (tileposy * width);
		printf("%d\n", tilenum);
		switch(currentlayer){
			case 0:
			layer0[tilenum] = currenttile;
			break;
			
			case 1:
			layer1[tilenum] = currenttile;
			break;
		}
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
		mapfile << std::to_string(outsidetile0);
		mapfile << "\n";
		mapfile << std::to_string(outsidetile1);
		mapfile << "\n";
		for(int i = 0; i < width * height; i++){
			mapfile << std::to_string(layer0[i]);
			mapfile << "\n";
			mapfile << std::to_string(layer1[i]);
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
	if(temp >= 0 && temp < tileset->getTileCount()){
		currenttile = temp;
		printf("Current tile: %d\n", temp);
	} else {
		printf("Number out of range.\n");
	}
}

void setOutsideTiles(Map *map, TileSet *tileset){
	int temp;
	std::cout << "Enter outside tile layer 0:" << std::endl;
	std::cin >> temp;
	if(temp >= 0 && temp < tileset->getTileCount()){
		map->setOutsideTile0(temp);
		printf("Outside tile 0 set to %d.\n", temp);
	} else {
		printf("Number out of range.\n");
	}
	std::cout << "Enter outside tile layer 1:" << std::endl;
	std::cin >> temp;
	if(temp >= 0 && temp < tileset->getTileCount()){
		map->setOutsideTile1(temp);
		printf("Outside tile 1 set to %d.\n", temp);
	} else {
		printf("Number out of range.\n");
	}
}

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
	range[0] = (x / blocksize) - 1;
	range[1] = range[0] + 21;
	/*if(range[0] < 0){
		range[0] = 0;
	}
	if (range[1] >= map->getWidth()){
		range[1] = map->getWidth() - 1;
	}*/
	range[2] = (y / blocksize) - 1;
	range[3] = range[2] + 16;
	/*if(range[2] < 0){
		range[2] = 0;
	}
	if (range[3] >= map->getHeight()){
		range[3] = map->getHeight() - 1;
	}*/
	//printf("Range: x = %d - %d, y = %d - %d\n", range[0], range[1], range[2], range[3]);
}

void renderLayer(int layer, Map *map, TileSet *tileset, int *range, SDL_Rect *srcrect, SDL_Rect *dstrect){
	int temp;
	for(int i = range[2]; i <= range[3]; i++){
		dstrect->y = i * blocksize - yoffset;
		for(int j = range[0]; j <= range[1]; j++){
			dstrect->x = j * blocksize - xoffset;
			switch(layer){
				case 0:
				temp = map->getLayer(0, j, i);
				break;
				
				case 1:
				temp = map->getLayer(1, j, i);
				break;
			}
			*srcrect = tileset->getTile(temp);
			SDL_RenderCopy(renderer, texture, srcrect, dstrect);
		}
	}
}

void renderMap(Map *map, TileSet *tileset, SDL_Rect *srcrect){
	SDL_Rect dstrect;
	dstrect.w = blocksize;
	dstrect.h = blocksize;
	SDL_RenderClear(renderer);
	int range[4] = {0, 0, 0, 0};
	visibleTiles(range, xoffset, yoffset, map);
	renderLayer(0, map, tileset, range, srcrect, &dstrect);
	renderLayer(1, map, tileset, range, srcrect, &dstrect);
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
	std::string tilesetname = "images/basictiles.png";
	std::string charactertilesetname = "images/characters.png";
	std::string mapname = argv[4];
	SDL_TimerID timerID = SDL_AddTimer(0, callback, NULL);
	texture = loadTexture(tilesetname);
	if(texture == NULL){
		printf("%s\n", SDL_GetError());
	}
	SDL_Rect srcrect;
	TileSet *tileset = new TileSet(tilesetname, 8, 15);
	Tileset *characters = new Tileset(charactertilesetname, 12, 8);
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
					
					case SDLK_l:
					switchLayer();
					break;
					
					case SDLK_t:
					switchTile(tileset);
					break;
					
					case SDLK_n:
					map->save("map.txt");
					break;
					
					case SDLK_o:
					setOutsideTiles(map, tileset);
					break;
					
					
					
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
		renderMap(map, tileset, &srcrect);
	}
	quit();
	return 0;
}
