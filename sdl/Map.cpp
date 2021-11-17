#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "constants.h"
#include "enums.h"
#include "Globals.h"
#include "Tile.h"
#include "TileSet.h"
#include "Entity.h"
#include "Map.h"

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

Map::Map(std::string f, int warpnum, dir direction){
	std::string filename = "assets/maps/" + f + ".txt";
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
		getline(mapfile, line);
		Entity *e;
		int entnum = std::stoi(line);
		enttype t;
		int x;
		int y;
		int s;
		bool a;
		bool r;
		bool sl;
		bool i;
		dir d;
		int w = 0;
		for(int j = 0; j < entnum; j++){
			getline(mapfile, line);
			t = (enttype) std::stoi(line);
			getline(mapfile, line);
			x = std::stoi(line);
			getline(mapfile, line);
			y = std::stoi(line);
			getline(mapfile, line);
			s = std::stoi(line);
			getline(mapfile, line);
			a = (bool) std::stoi(line);
			getline(mapfile, line);
			r = (bool) std::stoi(line);
			getline(mapfile, line);
			sl = (bool) std::stoi(line);
			getline(mapfile, line);
			i = (bool) std::stoi(line);
			getline(mapfile, line);
			d = (dir) std::stoi(line);
			getline(mapfile, line);
			e = new Entity(t, x, y, s, a, r, sl, i, d, line, j + 1);
			if(t == ENTTYPE_WARP){
				e->setWarpNum(w);
				w++;
			}
			entities.push_back(*e);
		}
	}
	mapfile.close();
	int *u = new int[2];
	getWarpPos(u, warpnum);
	entities.at(0).setLocation(u[0], u[1]);
	delete u;
	entities.at(0).setFacingDir(direction);
}

Map::~Map(){
	delete [] tiles;
	delete [] movementpermissions;
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

int* Map::getClickedTile(int mouseposx, int mouseposy, int camposx, int camposy){
	int *tilepos = new int[2];
	tilepos[0] = ((mouseposx / 4) + camposx) / tilesize;
	tilepos[1] = ((mouseposy / 4) + camposy) / tilesize;
	return tilepos;
}

int Map::getClickedSubtile(int mouseposx, int mouseposy, int camposx, int camposy){
	int subtileposx = ((mouseposx / 4) + camposx) / subtilesize;
	int subtileposy = ((mouseposy / 4) + camposy) / subtilesize;
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

movper Map::getMovementPermission(int x, int y){
	int offset = y * width + x;
	return movementpermissions[offset];
}

void Map::setMovementPermission(int mouseposx, int mouseposy, int camposx, int camposy, movper currentmovper){
	int *tilepos = getClickedTile(mouseposx, mouseposy, camposx, camposy);
	if(tilepos[0] >= 0 && tilepos[0] < width && tilepos[1] >= 0 && tilepos[1] < height){
		int tilenum = tilepos[0] + (tilepos[1] * width);
		movementpermissions[tilenum] = currentmovper;
		//printf("a\n");
	}
}

void Map::changeTile(int mouseposx, int mouseposy, int camposx, int camposy, int currenttile){
	int *tilepos = getClickedTile(mouseposx, mouseposy, camposx, camposy);
	if(tilepos[0] >= 0 && tilepos[0] < width && tilepos[1] >= 0 && tilepos[1] < height){
		int tilenum = tilepos[0] + (tilepos[1] * width);
		tiles[tilenum] = currenttile;
	}
}

void Map::save(std::string f){
	std::string filename = "assets/maps/" + f + ".txt";
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

void Map::getWarpPos(int *u, int warpnum){
	int j = 0;
	//int *u = new int[2];
	Entity *e = NULL;
	for(int i = 1; i < entities.size(); i++){
		if(entities.at(i).getType() == ENTTYPE_WARP){
			if(j == warpnum){
				e = &entities.at(i);
				break;
			}
			j++;
		}
	}
	u[0] = e->getXPos();
	u[1] = e->getYPos();
}
