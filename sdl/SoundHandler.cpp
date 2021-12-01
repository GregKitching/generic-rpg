#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "SoundHandler.h"

SoundHandler::SoundHandler(){
	fileprefix = "assets/sound/";
	filesuffix = ".wav";
}

void SoundHandler::quit(){
	for(int i = 0; i < sfx.size(); i++){
		Mix_FreeChunk(sfx.at(i));
	}
	sfx.erase(sfx.begin(), sfx.end());
}

int SoundHandler::loadSound(std::string name){
	sfx.push_back(Mix_LoadWAV((fileprefix + name + filesuffix).c_str()));
	return sfx.size() - 1;
}

void SoundHandler::playSound(int u){
	Mix_PlayChannel(-1, sfx.at(u), 0);
}
