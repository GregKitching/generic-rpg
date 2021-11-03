#ifndef _TILE_
#define _TILE_

class Tile{
	private:
		int *layer0;
		int *layer1;
	public:
		Tile(int *subtiles);
		int getSubtile(int layer, int subtile);
		void changeSubtile(int layer, int subtile, int u);
};

#endif
