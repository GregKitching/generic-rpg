#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "Globals.h"
#include "SpriteSheet.h"
#include "TextBox.h"

TextBox::TextBox(int x, int y, int w, int h, int fx, int fy, int fw, int fh){
	xpos = x;
	ypos = y;
	width = w;
	height = h;
	fieldxpos = fx;
	fieldypos = fy;
	fieldwidth = fw;
	fieldheight = fh;
	charwidth = fieldwidth / 8;
	charheight = fieldheight / 16;
	visible = true;
	active = false;
	fileloaded = false;
	lines = new std::vector<int*>();
	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = 0;
	srcrect.h = 0;
	dstrect.x = 0;
	dstrect.y = 0;
	dstrect.w = 0;
	dstrect.h = 0;
}

TextBox::~TextBox(){
	/*lines->clear();
	//delete lines;
	std::vector<int> u = {};
	std::vector<std::string> u2 = {};
	linelength.clear();
	linelength.swap(u);
	l.clear();
	l.swap(u2);*/
}

void TextBox::setRect(SDL_Rect *rect, int x, int y, int w, int h){
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
}

void TextBox::renderBox(){
	//printf("renderbox\n");
	//Setup for top left corner
	setRect(&srcrect, 0, 0, 4, 4);
	setRect(&dstrect, xpos, ypos, 4, 4);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//top right
	setRect(&srcrect, 4, 0, 4, 4);
	setRect(&dstrect, xpos + width - 4, ypos, 4, 4);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//bottom left
	setRect(&srcrect, 0, 4, 4, 4);
	setRect(&dstrect, xpos, ypos + height - 4, 4, 4);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//bottom right
	setRect(&srcrect, 4, 4, 4, 4);
	setRect(&dstrect, xpos + width - 4, ypos + height - 4, 4, 4);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//top bar
	setRect(&srcrect, 3, 0, 1, 4);
	setRect(&dstrect, xpos + 4, ypos, width - 8, 4);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//bottom bar
	setRect(&srcrect, 3, 4, 1, 4);
	setRect(&dstrect, xpos + 4, ypos + height - 4, width - 8, 4);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//left bar
	setRect(&srcrect, 0, 3, 4, 1);
	setRect(&dstrect, xpos, ypos + 4, 4, height - 8);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//right bar
	setRect(&srcrect, 4, 3, 4, 1);
	setRect(&dstrect, xpos + width - 4, ypos + 4, 4, height - 8);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	//middle
	setRect(&srcrect, 0, 0, 1, 1);
	setRect(&dstrect, xpos + 4, ypos + 4, width - 8, height - 8);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
}

int* TextBox::stringToInts(std::string s){
	char *chars = new char[s.size() + 1];
	strcpy(chars, s.c_str());
	int *u = new int[s.size()];
	for(int i = 0; i < s.size(); i++){
		u[i] = (int) chars[i] - 32;
	}
	delete [] chars;
	return u;
}

textboxtype TextBox::getType(){
	return type;
}

bool TextBox::getVisible(){
	return visible;
}

void TextBox::setVisible(bool u){
	visible = u;
}

bool TextBox::isActive(){
	return active;
}

void TextBox::loadFile(std::string f){
	//active = true;
	std::ifstream textfile;
	std::string file = "assets/text/" + f + ".txt";
	std::string line;
	//std::vector<std::string> *l = new std::vector<std::string>();
	textfile.open(file.c_str());
	if(!textfile.is_open()){
		printf("Fail\n");
	} else {
		while(!textfile.eof()){
			getline(textfile, line);
			l.push_back(line);
		}
		textfile.close();
		l.pop_back();//remove extra blank line
		loadText(l);
	}
	//delete l;
}

void TextBox::loadText(std::vector<std::string> l){
	char *chars;
	int *u;
	for(int i = 0; i < l.size(); i++){
		//chars = lines.at(i).c_str();
		linelength.push_back(l.at(i).length());
		/*if(haschoice || isbook){
			charsshown[i] = linelength.at(i);
		}*/
		chars = new char[linelength.at(i) + 1];
		strcpy(chars, l.at(i).c_str());
		u = new int[linelength.at(i)];
		for(int j = 0; j < linelength.at(i); j++){
			u[j] = ((int) chars[j]) - 32;
		}
		lines->push_back(u);
	}
	active = true;
	fileloaded = true;
	delete [] chars;
	//delete [] u;
	//printf("load text\n");
	initTextBox();
}

bool TextBox::fileLoaded(){
	return fileloaded;
}

void TextBox::changeText(int linenum, int start, int length, std::string s){
	int end = start + s.size();
	if(linenum >= lines->size()){
		printf("TextBox: Attemted to edit nonexistant line\n");
	} else if (start < 0 | start >= linelength.at(linenum) | end < 0 | end > linelength.at(linenum) | start > end){
		printf("TextBox: Malformed start and end values when attempting to edit text\n");
		printf("%d, %d, %d\n", start, end, linelength.at(linenum));
		for(int i = 0; i < linelength.at(linenum); i++){
			printf("%c, ", (char) lines->at(linenum)[i] + 32);
		}
		printf("\n");
	} else {
		int *u = stringToInts(s);
		for(int i = 0; i < s.size(); i++){
			lines->at(linenum)[i + start] = u[i];
		}
		/*if(length != -1){
			linelength.at(linenum) = length;
		}*/
		/*int *u = stringToInts(s);
		lines->erase(linenum, linenum + 1);
		lines->insert(u);*/
	}
}

void TextBox::setText(std::vector<int*> *u){
	lines = u;
}

std::vector<int*>* TextBox::getLines(){
	return lines;
}
