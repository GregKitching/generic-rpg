#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "SpriteSheet.h"
#include "TextBox.h"

TextBox::TextBox(int x, int y, int w, int h, SDL_Texture *t){
	xpos = x;
	ypos = y;
	width = w;
	height = h;
	texture = t;
	visible = false;
	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = 0;
	srcrect.h = 0;
	dstrect.x = 0;
	dstrect.y = 0;
	dstrect.w = 0;
	dstrect.h = 0;
}

void TextBox::setRect(SDL_Rect *rect, int x, int y, int w, int h){
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
}

void TextBox::render(SDL_Renderer *renderer){
	//Setup for top left corner
	setRect(&srcrect, 0, 0, 4, 4);
	setRect(&dstrect, xpos, ypos, 4, 4);
	SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
	//top right
	setRect(&srcrect, 4, 0, 4, 4);
	setRect(&dstrect, xpos + width - 4, ypos, 4, 4);
	SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
	//bottom left
	setRect(&srcrect, 0, 4, 4, 4);
	setRect(&dstrect, xpos, ypos + height - 4, 4, 4);
	SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
	//bottom right
	setRect(&srcrect, 4, 4, 4, 4);
	setRect(&dstrect, xpos + width - 4, ypos + height - 4, 4, 4);
	SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
	//top bar
	setRect(&srcrect, 3, 0, 1, 4);
	setRect(&dstrect, xpos + 4, ypos, width - 8, 4);
	SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
	//bottom bar
	setRect(&srcrect, 3, 4, 1, 4);
	setRect(&dstrect, xpos + 4, ypos + height - 4, width - 8, 4);
	SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
	//left bar
	setRect(&srcrect, 0, 3, 4, 1);
	setRect(&dstrect, xpos, ypos + 4, 4, height - 8);
	SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
	//right bar
	setRect(&srcrect, 4, 3, 4, 1);
	setRect(&dstrect, xpos + width - 4, ypos + 4, 4, height - 8);
	SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
	//middle
	setRect(&srcrect, 0, 0, 1, 1);
	setRect(&dstrect, xpos + 4, ypos + 4, width - 8, height - 8);
	SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
}

void TextBox::renderText(SDL_Renderer *renderer, SpriteSheet *font){
	SDL_Rect srcrect;
	SDL_Rect dstrect;
	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = 8;
	srcrect.h = 16;
	dstrect.x = xpos + 4;
	dstrect.y = ypos + 4;
	dstrect.w = 8;
	dstrect.h = 16;
	for(int i = 0; i < text.size(); i++){
		srcrect.x = font->getSubtileX(text.at(i));
		srcrect.y = (font->getSubtileY(text.at(i))) * 2;
		//printf("%d, %d\n", srcrect.x, srcrect.y);
		SDL_RenderCopy(renderer, font->getTexture(), &srcrect, &dstrect);
		dstrect.x += 8;
	}
}

bool TextBox::getVisible(){
	return visible;
}

void TextBox::setVisible(bool u){
	visible = u;
}

/*int TextBox::getFontX(int u){
	return (u * 8) % 32;
}

int TextBox::getFontY(int u){
	return u / 4;//(u * 8) / 32
}*/

void TextBox::setText(std::string f){
	std::string file = "assets/text/" + f + ".txt";
	std::ifstream textfile;
	std::string line;
	textfile.open(file.c_str());
	if(!textfile.is_open()){
		printf("Failed to open text file \"%s\".\n", file.c_str());
	} else {
		getline(textfile, line);
		textfile.close();
		int length = line.length();
		char *chars = new char[line.length() + 1];
		strcpy(chars, line.c_str());
		for(int i = 0; i < length; i++){
			text.push_back((int) chars[i] - 32);
		}
		delete [] chars;
	}
}

void TextBox::clearText(){
	text.clear();
}
