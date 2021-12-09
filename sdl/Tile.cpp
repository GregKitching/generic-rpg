#include "Tile.h"

Tile::Tile(int *subtiles, bool a){
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
	/*fl0 = new flipstate[4];
	fl1 = new flipstate[4];
	fl0[0] = FLIPSTATE_NONE;
	fl0[0] = FLIPSTATE_NONE;
	fl0[0] = FLIPSTATE_NONE;
	fl0[0] = FLIPSTATE_NONE;
	fl0[0] = FLIPSTATE_NONE;
	fl0[0] = FLIPSTATE_NONE;
	fl0[0] = FLIPSTATE_NONE;
	fl0[0] = FLIPSTATE_NONE;*/
	animated = a;
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

bool Tile::isAnimated(){
	return animated;
}

void Tile::setAnimated(bool u){
	animated = u;
}
