#ifndef _ENTITY_
#define _ENTITY_

class Entity{
	private:
		enttype type;
		int xpos;//In terms of 16x16 tiles
		int ypos;
		int spritexpos;//In terms of pixels
		int spriteypos;
		int basesprite;
		int walkcycle;
		dir facing;
		dir movedir;
		bool animated;
		bool rendered;
		bool solid;
		bool interactable;
		int movetimer;
		bool oddwalkcycle;
		movper currentmovper;
		bool canMove(int newx, int newy, dir direction, Map *map, std::vector<Entity> *entities);
	public:
		Entity(enttype t, int x, int y, int s, bool a, bool r, bool sl, bool i, dir d);
		void setLocation(int x, int y);
		enttype getType();
		int getXPos();
		int getYPos();
		int getSpriteXPos();
		int getSpriteYPos();
		int getSprite();
		dir getFacingDir();
		void setFacingDir(dir direction);
		void move(dir direction, Map *map, std::vector<Entity> *entities);
		void animate();
		int getMoveTimer();
		bool isAnimated();
		bool isRendered();
		bool isSolid();
		bool isInteractable();
		int* getAdjacentTile(dir direction, Map *map);
};

#endif
