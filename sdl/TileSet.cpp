#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include "Globals.h"
#include "SpriteSheet.h"
#include "Tile.h"
#include "TileSet.h"

TileSet::TileSet(int u){
	int *temp = new int[8];
	for(int i = 0; i < 8; i++){
		temp[i] = 479;//The last subtile in basictilesmodified.png, completely transparent
	}
	length = u;
	printf("%d\n", u);
	tiles = new Tile*[u];
	for(int i = 0; i < u; i++){
		tiles[i] = new Tile(temp, false);
	}
	delete [] temp;
}

TileSet::TileSet(std::string f){
	std::string filename = "assets/tilesets/" + f + ".txt";
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
			getline(tilesetfile, line);
			tiles[i] = new Tile(temp, (bool) std::stoi(line));
		}
		delete [] temp;
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

void TileSet::saveTileset(std::string f){
	std::string filename = "assets/tilesets/" + f + ".txt";
	std::ofstream tilesetfile;
	tilesetfile.open(filename.c_str());
	if(!tilesetfile.is_open()){
		printf("Error saving to tileset file.\n");
	} else {
		tilesetfile << std::to_string(length);
		tilesetfile << std::endl;
		for(int i = 0; i < length; i++){
			for(int j = 0; j < 4; j++){
				tilesetfile << std::to_string(tiles[i]->getSubtile(0, j));
				tilesetfile << std::endl;
			}
			for(int j = 0; j < 4; j++){
				tilesetfile << std::to_string(tiles[i]->getSubtile(1, j));
				tilesetfile << std::endl;
			}
			tilesetfile << std::to_string((int) tiles[i]->isAnimated());
			tilesetfile << std::endl;
		}
		printf("Tileset saved as %s\n", filename.c_str());
		tilesetfile.close();
	}
}
