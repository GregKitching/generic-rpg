#ifndef _CONSTANTS_
#define _CONSTANTS_

const int subtilesize = 8;
const int tilesize = 16;

const int screenwidth = 1024;
const int screenheight = 768;

const float screenscale = 4.0f;

const int playerxoffset = (screenwidth / (int) screenscale) / 2 - subtilesize;
const int playeryoffset = (screenheight / (int) screenscale) / 2 - subtilesize;

const int vistilesxoffset = (screenwidth / (int) screenscale) / tilesize + 2;
const int vistilesyoffset = (screenheight / (int) screenscale) / tilesize + 2;

const int uixoffset = 4;
const int uiyoffset = 6;

#endif
