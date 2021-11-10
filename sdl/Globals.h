#ifndef _GLOBALS_
#define _GLOBALS_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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
extern TextBox *maintextbox;
extern std::vector<Entity> entities;
extern Map *map;
extern bool caninteract;
extern bool canmove;

#endif
