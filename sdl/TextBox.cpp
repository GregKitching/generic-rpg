#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "Globals.h"
#include "SpriteSheet.h"
#include "TextBox.h"

TextBox::TextBox(int x, int y, int w, int h, bool c, bool b){
	xpos = x;
	ypos = y;
	width = w;
	height = h;
	haschoice = c;
	isbook = b;
	if(haschoice){
		xofs = 12;
		linesshown = height - 1;
		waitforinput = true;
	} else if (isbook){
		xofs = 4;
		linesshown = height - 1;
		waitforinput = true;
	} else {
		xofs = 4;
		linesshown = 0;
		waitforinput = false;
	}
	pixwidth = (width * 8) + xofs + 4;
	pixheight = (height * 16) + 8;
	visible = true;
	active = false;
	fileloaded = false;
	firstpage = true;
	lastpage = false;
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
	cursorpos = 0;
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
		setRect(&srcrect, 0, 0, 8, 16);
		setRect(&dstrect, xpos + xofs, ypos + 4, 8, 16);
		for(int i = 0; i < linesshown + 1; i++){
			//printf("u\n");
			for(int j = 0; j < charsshown[i]; j++){
				//printf("%d\n", i);
				srcrect.x = font->getSubtileX(lines.at(scrollpos + i)[j]);
				//printf("b\n");
				srcrect.y = (font->getSubtileY(lines.at(scrollpos + i)[j])) * 2;
				//printf("c\n");
				SDL_RenderCopy(renderer, font->getTexture(), &srcrect, &dstrect);
				dstrect.x += 8;
			}
			dstrect.x = xpos + xofs;
			dstrect.y += 16;
		}
	}
}

void TextBox::renderCursor(){
	setRect(&srcrect, 16, 0, 8, 8);
	setRect(&dstrect, xpos + 4, ypos + 8 + (cursorpos * 16), 8, 8);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
}

void TextBox::renderPageArrows(){
	if(firstpage & !lastpage){
		setRect(&srcrect, 8, 0, 8, 8);
		setRect(&dstrect, xpos + pixwidth - 16, ypos + pixheight - 8, 8, 8);
		SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	}
	if(lastpage & !firstpage){
		setRect(&srcrect, 16, 0, 8, 8);
		setRect(&dstrect, xpos + pixwidth - 8, ypos + pixheight - 8, 8, 8);
		SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
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
	scrollpos++;
	for(int i = 0; i < height - 1; i++){
		charsshown[i] = charsshown[i + 1];
	}
	if(haschoice || isbook){
		charsshown[height - 1] = linelength.at(scrollpos + height - 1);
	} else {
		charsshown[height - 1] = 0;
	}	
}

void TextBox::prevLine(){
	scrollpos--;
	for(int i = height - 1; i > 0; i--){
		charsshown[i] = charsshown[i - 1];
	}
	if(haschoice || isbook){
		charsshown[0] = linelength.at(scrollpos);
	} else {
		charsshown[0] = 0;
	}
}

bool TextBox::isActive(){
	return active;
}

bool TextBox::hasChoice(){
	return haschoice;
}

bool TextBox::isBook(){
	return isbook;
}

bool TextBox::isSpeech(){
	return !(haschoice || isbook);
}

void TextBox::advance(){
	if(!(haschoice || isbook)){
		if(scrollpos + linesshown >= lines.size() - 1){
			active = false;
			//reset();
		} else {
			advanceLine();
		}
	} else {
		active = false;
	}
	waitforinput = false;
}

void TextBox::next(){
	if(scrollpos + cursorpos < lines.size() - 1){
		if(cursorpos == height - 1){
			advanceLine();
		} else {
			cursorpos++;
		}
	}
}

void TextBox::prev(){
	if(scrollpos + cursorpos > 0){
		if(cursorpos == 0){
			prevLine();
		} else {
			cursorpos--;
		}
	}
}

void TextBox::nextPage(){
	if(!lastpage){
		firstpage = false;
		int u = scrollpos + height;
		if(u + height > lines.size()){
			linesshown = height - u;
			lastpage = true;
		} else if(u + height == lines.size()){
			linesshown = height - 1;
			lastpage = true;
		} else {
			linesshown = height - 1;
		}
		for(int i = 0; i < linesshown + 1; i++){
			charsshown[i] = linelength.at(scrollpos + height + i);
		}
		scrollpos += height;
	}
}

void TextBox::prevPage(){
	if(!firstpage){
		lastpage = false;
		scrollpos -= height;
		linesshown = height - 1;
		if(scrollpos == 0){
			firstpage = true;
		}
		for(int i = 0; i < height; i++){
			charsshown[i] = linelength.at(i);
		}
	}
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
		if(haschoice || isbook){
			charsshown[i] = linelength.at(i);
		}
		chars = new char[linelength.at(i) + 1];
		strcpy(chars, l.at(i).c_str());
		u = new int[linelength.at(i)];
		for(int j = 0; j < linelength.at(i); j++){
			u[j] = ((int) chars[j]) - 32;
		}
		lines.push_back(u);
	}
	if(isbook && lines.size() <= height){
		lastpage = true;
	}
	fileloaded = true;
}

void TextBox::reset(){
	timer = 0;
	scrollpos = 0;
	fileloaded = false;
	if(haschoice || isbook){
		//xofs = 12;
		linesshown = height - 1;
		//waitforinput = true;
	} else {
		//xofs = 4;
		linesshown = 0;
		//waitforinput = false;
	}
	//linesshown = 0;
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

int TextBox::getChoice(){
	return scrollpos + cursorpos;
}
