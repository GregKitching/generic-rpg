#ifndef _GLOBALS_
#define _GLOBALS_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <fstream>

#include "constants.h"
#include "ScriptDefs.h"
#include "enums.h"
#include "SpriteSheet.h"
#include "Tile.h"
#include "TileSet.h"
#include "Map.h"
#include "Entity.h"
#include "TextBox.h"
#include "Script.h"

extern uint8_t *heap;
//extern TextBox *maintextbox;
extern std::vector<Entity> entities;
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

#endif