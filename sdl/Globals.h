#ifndef _GLOBALS_
#define _GLOBALS_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "constants.h"
#include "ScriptDefs.h"
#include "enums.h"
#include "SpriteSheet.h"
#include "Tile.h"
#include "TileSet.h"
#include "Map.h"
#include "Entity.h"
#include "ActiveEntity.h"
#include "TextBox.h"
#include "SpeechTextBox.h"
#include "MenuTextBox.h"
#include "BookTextBox.h"
#include "Script.h"
#include "SoundHandler.h"

extern uint8_t *heap;
extern pmode programmode;
//extern TextBox *maintextbox;
extern std::vector<Entity*> entities;
extern Map *map;
extern bool caninteract;
extern bool canmove;
//extern SDL_Texture textboxtexture;
extern SpriteSheet *font;
extern Script *currentscript;
extern bool renderflag;
extern bool rendering;
extern SDL_Renderer *renderer;
extern std::vector<TextBox*> textboxes;
extern SDL_Texture *textboxtexture;
extern ActiveEntity *player;
extern SoundHandler *soundhandler;

#endif
