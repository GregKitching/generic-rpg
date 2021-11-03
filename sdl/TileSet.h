#ifndef _TILESET_
#define _TILESET_

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

#endif
