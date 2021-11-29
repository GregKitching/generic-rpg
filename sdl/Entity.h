#ifndef _ENTITY_
#define _ENTITY_

class Entity{
	protected:
		enttype type;
		int entnum;
		int xpos;//In terms of 16x16 tiles
		int ypos;
		int warpnum;
		bool interactable;
		movper currentmovper;
		std::string script;
	public:
		Entity(enttype t, int x, int y, bool i, std::string sc, int e);
		void setLocation(int x, int y);
		enttype getType();
		int getXPos();
		int getYPos();
		movper getMovPer();
		void setMovPer(movper a);
		bool isInteractable();
		int* getAdjacentTile(dir direction, Map *map);
		std::string getScript();
		int getWarpNum();
		void setWarpNum(int w);
};

#endif
