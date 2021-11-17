#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "Globals.h"
#include "SpriteSheet.h"
#include "TextBox.h"

TextBox::TextBox(int x, int y, int w, int h){
	xpos = x;
	ypos = y;
	width = w;
	height = h;
	pixwidth = (width * 8) + 8;
	pixheight = (height * 16) + 8;
	visible = true;
	active = false;
	waitforinput = false;
	fileloaded = false;
	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = 0;
	srcrect.h = 0;
	dstrect.x = 0;
	dstrect.y = 0;
	dstrect.w = 0;
	dstrect.h = 0;
	timer = 2;
	timerval = 2;
	linesshown = 0;
	scrollpos = 0;
	charsshown = new int[height];
	for(int i = 0; i < height; i++){
		charsshown[i] = 0;
	}
}

TextBox::~TextBox(){
	delete [] charsshown;
}

void TextBox::setRect(SDL_Rect *rect, int x, int y, int w, int h){
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
}

void TextBox::renderBox(){
	//Setup for top left corner
	setRect(&srcrect, 0, 0, 4, 4);
	setRect(&dstrect, xpos, ypos, 4, 4);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//top right
	setRect(&srcrect, 4, 0, 4, 4);
	setRect(&dstrect, xpos + pixwidth - 4, ypos, 4, 4);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//bottom left
	setRect(&srcrect, 0, 4, 4, 4);
	setRect(&dstrect, xpos, ypos + pixheight - 4, 4, 4);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//bottom right
	setRect(&srcrect, 4, 4, 4, 4);
	setRect(&dstrect, xpos + pixwidth - 4, ypos + pixheight - 4, 4, 4);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//top bar
	setRect(&srcrect, 3, 0, 1, 4);
	setRect(&dstrect, xpos + 4, ypos, pixwidth - 8, 4);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//bottom bar
	setRect(&srcrect, 3, 4, 1, 4);
	setRect(&dstrect, xpos + 4, ypos + pixheight - 4, pixwidth - 8, 4);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//left bar
	setRect(&srcrect, 0, 3, 4, 1);
	setRect(&dstrect, xpos, ypos + 4, 4, pixheight - 8);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//right bar
	setRect(&srcrect, 4, 3, 4, 1);
	setRect(&dstrect, xpos + pixwidth - 4, ypos + 4, 4, pixheight - 8);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//middle
	setRect(&srcrect, 0, 0, 1, 1);
	setRect(&dstrect, xpos + 4, ypos + 4, pixwidth - 8, pixheight - 8);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
}

void TextBox::renderText(){
	if(fileloaded){
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
		for(int i = 0; i < linesshown + 1; i++){
			for(int j = 0; j < charsshown[i]; j++){
				srcrect.x = font->getSubtileX(lines.at(scrollpos + i)[j]);
				srcrect.y = (font->getSubtileY(lines.at(scrollpos + i)[j])) * 2;
				SDL_RenderCopy(renderer, font->getTexture(), &srcrect, &dstrect);
				dstrect.x += 8;
			}
			dstrect.x = xpos + 4;
			dstrect.y += 16;
		}
	}
}

bool TextBox::getVisible(){
	return visible;
}

void TextBox::setVisible(bool u){
	visible = u;
}

bool TextBox::getWaitForInput(){
	return waitforinput;
}

void TextBox::advanceText(){
	if(charsshown[linesshown] >= linelength.at(scrollpos + linesshown)){
		if(linesshown >= height - 1){
			waitforinput = true;
		} else {
			linesshown++;
			if(scrollpos + linesshown >= lines.size()){
				waitforinput = true;
			}
		}
	} else {
		charsshown[linesshown]++;
	}
}

void TextBox::advanceLine(){
	for(int i = 0; i < height - 1; i++){
		charsshown[i] = charsshown[i + 1];
	}
	charsshown[height - 1] = 0;
	scrollpos++;
}

bool TextBox::isActive(){
	return active;
}

void TextBox::input(){
	if(scrollpos + linesshown >= lines.size() - 1){
		active = false;
		reset();
	} else {
		advanceLine();
	}
	waitforinput = false;
}

void TextBox::loadFile(std::string f){
	active = true;
	std::string file = "assets/text/" + f + ".txt";
	std::string line;
	std::vector<std::string> l;
	textfile.open(file.c_str());
	while(!textfile.eof()){
		getline(textfile, line);
		l.push_back(line);
	}
	textfile.close();
	l.pop_back();
	char *chars;
	int *u;
	for(int i = 0; i < l.size(); i++){
		//chars = lines.at(i).c_str();
		linelength.push_back(l.at(i).length());
		chars = new char[linelength.at(i) + 1];
		strcpy(chars, l.at(i).c_str());
		u = new int[linelength.at(i)];
		for(int j = 0; j < linelength.at(i); j++){
			u[j] = ((int) chars[j]) - 32;
		}
		lines.push_back(u);
	}
	fileloaded = true;
}

void TextBox::reset(){
	timer = 0;
	scrollpos = 0;
	fileloaded = false;
	linesshown = 0;
	scrollpos = 0;
	for(int i = 0; i < height; i++){
		charsshown[i] = 0;
	}
	lines.clear();
	linelength.clear();
}

void TextBox::tick(){
	if(fileloaded && !waitforinput){
		if(timer == 0){
			advanceText();
			timer = timerval;
		} else if (timer > 0){
			timer--;
		} else {
			timer = 0;
		}
	}/* else {
		usleep(16666);
	}*/
}

bool TextBox::fileLoaded(){
	return fileloaded;
}
