#ifndef _ACTIVE_ENTITY_
#define _ACTIVE_ENTITY_

class ActiveEntity: public Entity{
	private:
		int spritexpos;//In terms of pixels
		int spriteypos;
		int basesprite;
		int walkcycle;
		dir facing;
		dir movedir;
		bool animated;
		bool rendered;
		bool solid;
		bool busy;
		int movetimer;
		bool oddwalkcycle;
		entstate state;
		bool canMove(int newx, int newy, dir direction);//, Map *map, std::vector<Entity> *entities);
	public:
		ActiveEntity(enttype t, int x, int y, bool i, std::string sc, int e, int s, bool a, bool r, bool sl, dir d);
		int getSpriteXPos();
		int getSpriteYPos();
		int getSprite();
		dir getFacingDir();
		void setFacingDir(dir direction);
		void move(dir direction);//, Map *map, std::vector<Entity> *entities);
		void animateMove();
		int getMoveTimer();
		bool isAnimated();
		bool isRendered();
		void setRendered(bool u);
		bool isSolid();
		//int* getAdjacentTile(dir direction, Map *map);
		void tick();
		entstate getState();
		bool isBusy();
		void setBusy();
		void updateSpriteLocation();
};

#endif
