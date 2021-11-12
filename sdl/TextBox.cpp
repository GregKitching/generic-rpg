#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "SpriteSheet.h"
#include "TextBox.h"

TextBox::TextBox(int x, int y, int w, int h, SDL_Texture *t, SpriteSheet *f, SDL_Renderer *r){
	xpos = x;
	ypos = y;
	width = w;
	height = h;
	texture = t;
	font = f;
	renderer = r;
	visible = false;
	lastline = false;
	active = false;
	waitforinput = false;
	buffersswapped = false;
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
	row1pos = 0;
	row2pos = 0;
	linespos = 0;
	secondline = false;
	bufa = new int[23];
	bufb = new int[23];
	buf1 = &bufa;
	buf2 = &bufb;
	linealength = 0;
	lineblength = 0;
	line1length = &linealength;
	line2length = &lineblength;
	lastline = false;
	oneline = false;
}

void TextBox::swapBuffers(){
	if(buffersswapped){
		buf1 = &bufa;
		buf2 = &bufb;
		line1length = &linealength;
		line2length = &lineblength;
		buffersswapped = false;
	} else {
		buf1 = &bufb;
		buf2 = &bufa;
		line1length = &lineblength;
		line2length = &linealength;
		buffersswapped = true;
	}
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

void TextBox::renderText(){
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
	for(int i = 0; i < row1pos; i++){
		srcrect.x = font->getSubtileX(buf1[0][i]);
		srcrect.y = (font->getSubtileY(buf1[0][i])) * 2;
		//printf("1 %d, %d, %d\n", buf1[0][i], srcrect.x, srcrect.y);
		SDL_RenderCopy(renderer, font->getTexture(), &srcrect, &dstrect);
		dstrect.x += 8;
	}
	dstrect.x = xpos + 4;
	dstrect.y = ypos + 20;
	printf("row2pos: %d\n", row2pos);
	for(int i = 0; i < row2pos; i++){
		printf("i: %d\n", i);
		srcrect.x = font->getSubtileX(buf2[0][i]);
		srcrect.y = (font->getSubtileY(buf2[0][i])) * 2;
		//printf("2 %d, %d, %d\n", buf2[0][i], srcrect.x, srcrect.y);
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

bool TextBox::getWaitForInput(){
	return waitforinput;
}

int TextBox::getLine(int *buf, bool firstline){
	char *chars = new char[lines.at(linespos).length() + 1];
	strcpy(chars, lines.at(linespos).c_str());
	for(int i = 0; i < lines.at(linespos).length(); i++){
		buf[i] = ((int) chars[i]) - 32;
	}
	delete [] chars;
	int u = lines.at(linespos).length();
	linespos++;
	printf("uuuu %d, %ld\n", linespos, lines.size() - 1);
	if(linespos == lines.size() - 1){
		if(firstline){
			oneline = true;
		} else {
			lastline = true;
		}
		printf("last line\n");
	}
	return u;
}

void TextBox::advanceText(){
	if(secondline){
		row2pos++;
		printf("k\n");
		if(row2pos >= *line2length){
			waitforinput = true;
		}
	} else {
		row1pos++;
		if(row1pos >= *line1length){
			printf("s1\n");
			if(oneline){
				printf("s2\n");
				waitforinput = true;
			} else {
				printf("s3\n");
			}
			secondline = true;
		}
	}
}

void TextBox::advanceLine(){
	printf("1\n");
	swapBuffers();
	printf("2\n");
	row2pos = 0;
	printf("3\n");
	for(int i = 0; i < 23; i++){
		buf2[0][i] = 0;
	}
	printf("4\n");
	*line2length = getLine(buf2[0], false);
	printf("5\n");
}

/*int TextBox::getFontX(int u){
	return (u * 8) % 32;
}

int TextBox::getFontY(int u){
	return u / 4;//(u * 8) / 32
}*/

bool TextBox::isActive(){
	return active;
}

void TextBox::input(){
	if(lastline || oneline){
		reset();
		active = false;
	} else {
		advanceLine();
	}
	waitforinput = false;
}

void TextBox::loadFile(std::string f){
	active = true;
	std::string file = "assets/text/" + f + ".txt";
	std::string line;
	textfile.open(file.c_str());
	while(!textfile.eof()){
		getline(textfile, line);
		lines.push_back(line);
	}
	textfile.close();
	*line1length = getLine(buf1[0], true);
	printf("rrrr %u\n", lastline);
	if(!lastline){
		printf("aaaa\n");
		*line2length = getLine(buf2[0], false);
		printf("rrrr %u\n", lastline);
	}/* else {
		//waitforinput = true;
		//printf("last line\n");
	}*/
	fileloaded = true;
}

void TextBox::reset(){
	for(int i = 0; i < 23; i++){
		bufa[i] = 0;
		bufb[i] = 0;
	}
	//active = true;
	//waitforinput = false;
	buffersswapped = false;
	buf1 = &bufa;
	buf2 = &bufb;
	timer = 0;
	row1pos = 0;
	row2pos = 0;
	linespos = 0;
	secondline = false;
	fileloaded = false;
	lastline = false;
	oneline = false;
	lines.clear();
}

void TextBox::tick(){
	if(visible && fileloaded && !waitforinput){
		//printf("tick\n");
		if(timer == 0){
			//printf("a\n");
			advanceText();
			//printf("b\n");
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
