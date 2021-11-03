#ifndef _SPRITESHEET_
#define _SPRITESHEET_

class SpriteSheet{
	private:
		SDL_Texture *tex = NULL;
		int width;
		int height;
		int tilecount;
		int subtilewidth;
		int subtileheight;
		int subtilecount;
		SDL_Surface* loadPng(std::string name);
		SDL_Texture* loadTexture(std::string name, SDL_Renderer *renderer);
	public:
		SpriteSheet(std::string filename, int w, int h, SDL_Renderer *renderer);
		SDL_Texture* getTexture();
		int getTileX(int tilenum);
		int getTileY(int tilenum);
		int getSubtileX(int subtilenum);
		int getSubtileY(int subtilenum);
		int getTileCount();
		int getSubtileCount();
};

#endif
