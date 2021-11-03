#ifndef _ENTITY_
#define _ENTITY_

class Entity{
	private:
		int xpos;//In terms of 16x16 tiles
		int ypos;
		int spritexpos;//In terms of pixels
		int spriteypos;
		int basesprite;
		int walkcycle;
		dir facing;
		dir movedir;
		int movetimer;
		bool oddwalkcycle;
		movper currentmovper;
		void move2(int newx, int newy);
	public:
		Entity(int x, int y, int s);
		void setLocation(int x, int y);
		int getXPos();
		int getYPos();
		int getSpriteXPos();
		int getSpriteYPos();
		int getSprite();
		void move(dir direction, Map *map);
		void animate();
		int getMoveTimer();
};

#endif
