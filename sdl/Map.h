#ifndef _MAP_
#define _MAP_

class Map{
	private:
		int width;//width in 16x16 tiles
		int height;
		int *tiles;
		int outsidetile;//Tile to be rendered in areas ouside the map so it's not just surrounded by black
		movper *movementpermissions;
		std::string loadscript;
	public:
		Map(int w, int h);
		Map(int w, int h, int tilesetlength);
		Map(std::string f, int warpnum, dir direction);
		~Map();
		int getWidth();
		int getHeight();
		int getPixWidth();
		int getPixHeight();
		int getTile(int x, int y);
		int* getClickedTile(int mouseposx, int mouseposy, int camposx, int camposy);
		int getClickedSubtile(int mouseposx, int mouseposy, int camposx, int camposy);
		void setOutsideTile(int u);
		int getOutsideTile();
		movper getMovementPermission(int x, int y);
		void setMovementPermission(int mouseposx, int mouseposy, int camposx, int camposy, movper currentmovper);
		void changeTile(int mouseposx, int mouseposy, int camposx, int camposy, int currenttile);
		void save(std::string f);
		void getWarpPos(int *u, int warpnum);
		void setScript(std::string u);
};

#endif
