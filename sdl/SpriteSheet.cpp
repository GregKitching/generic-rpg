#include <string>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "constants.h"
#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(std::string filename, int w, int h, SDL_Renderer *renderer){
	tex = loadTexture(filename.c_str(), renderer);
	width = w;
	height = h;
	subtilewidth = width * 2;
	subtileheight = height * 2;
	tilecount = width * height;
	subtilecount = tilecount * 4;
}

SDL_Surface* SpriteSheet::loadPng(std::string name){
	SDL_Surface *s = NULL;
	SDL_Surface *tempsurface = NULL;
	SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
	tempsurface = IMG_Load(name.c_str());
	if(tempsurface == NULL){
		printf("aaa%s\n", SDL_GetError());
	} else {
		s = SDL_ConvertSurface(tempsurface, format, 0);
		if(s == NULL){
			printf("%s\n", SDL_GetError());
		}
	}
	SDL_FreeSurface(tempsurface);
	SDL_FreeFormat(format);
	return s;
}

SDL_Texture* SpriteSheet::loadTexture(std::string name, SDL_Renderer *renderer){
	SDL_Texture *tex = NULL;
	SDL_Surface *s = loadPng(name);
	tex = SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);
	return tex;
}

SDL_Texture* SpriteSheet::getTexture(){
	return tex;
}

int SpriteSheet::getTileX(int tilenum){
	int x = 0;
	if(tilenum >= 0 && tilenum < tilecount){
		x = tilenum % width;
		x = x * tilesize;
	}
	return x;
}

int SpriteSheet::getTileY(int tilenum){
	int y = 0;
	if(tilenum >= 0 && tilenum < tilecount){
		y = tilenum / width;
		y = y * tilesize;
	}
	return y;
}

int SpriteSheet::getSubtileX(int subtilenum){
	int x = 0;
	if(subtilenum >= 0 && subtilenum < subtilecount){
		x = subtilenum % subtilewidth;
		x = x * subtilesize;
	}
	return x;
}

int SpriteSheet::getSubtileY(int subtilenum){
	int y = 0;
	if(subtilenum >= 0 && subtilenum < subtilecount){
		y = subtilenum / subtilewidth;
		y = y * subtilesize;
	}
	return y;
}

int SpriteSheet::getTileCount(){
	return tilecount;
}

int SpriteSheet::getSubtileCount(){
	return subtilecount;
}
