#ifndef _TILE_
#define _TILE_

class Tile{
	private:
		int *layer0;
		int *layer1;
		//flipstate *fl0;
		//flipstate *fl1;
		bool animated;
	public:
		Tile(int *subtiles, bool a);
		int getSubtile(int layer, int subtile);
		void changeSubtile(int layer, int subtile, int u);
		//void renderLayer(bool layer1, SDL_Texture *tex, SDL_Rect *srcrect, SDL_Rect *dstrect
		bool isAnimated();
		void setAnimated(bool u);
};

#endif
