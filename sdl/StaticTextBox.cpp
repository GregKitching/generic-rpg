#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "Globals.h"
#include "SpriteSheet.h"
#include "TextBox.h"
#include "StaticTextBox.h"

StaticTextBox::StaticTextBox(int x, int y, int w, int h, int fx, int fy, int fw, int fh): TextBox(x, y, w, h, fx, fy, fw, fh){
	scrollpos = 0;
	type = TEXTBOX_STATIC;
}

void StaticTextBox::initTextBox(){
	
}

void StaticTextBox::renderText(){
	if(fileloaded){
		setRect(&srcrect, 0, 0, 8, 16);
		setRect(&dstrect, xpos + fieldxpos, ypos + fieldypos, 8, 16);
		for(int i = 0; i < lines->size(); i++){
			for(int j = 0; j < linelength[i]; j++){
				srcrect.x = font->getSubtileX(lines->at(scrollpos + i)[j]);
				srcrect.y = (font->getSubtileY(lines->at(scrollpos + i)[j])) * 2;
				SDL_RenderCopy(renderer, font->getTexture(), &srcrect, &dstrect);
				dstrect.x += 8;
			}
			dstrect.x = xpos + fieldxpos;
			dstrect.y += 16;
		}
	}
}

void StaticTextBox::renderArrows(){
	
}

void StaticTextBox::next(){
	
}

void StaticTextBox::prev(){
	
}

void StaticTextBox::tick(){
	
}

void StaticTextBox::reset(){
	l.erase(l.begin(), l.end());
	lines->erase(lines->begin(), lines->end());
	linelength.erase(linelength.begin(), linelength.end());
}

int StaticTextBox::getValue(){
	return 0;
}

void StaticTextBox::close(){
	active = false;
}

/*int* StaticTextBox::stringToInts(std::string s){
	char *chars = new char[s.size() + 1];
	strcpy(chars, s.c_str());
	int *u = new int[s.size()];
	for(int i = 0; i < s.size(); i++){
		u[i] = (int) chars[i] - 32;
	}
	delete [] chars;
	return u;
}

void StaticTextBox::setName(std::string name){//******
	if(name.size() < 7){
		int *u = stringToInts(name);
		for(int i = 0; i < name.size(); i++){
			lines->at(0)[i] = u[i];
		}
		linelength.at(0) = name.size();
	}
}

void StaticTextBox::setLevel(int lv){//Lv. 0**
	std::string s = std::to_string(lv);
	int n;
	if(lv > 99){
		n = 3;
	} else if (lv > 9){
		n = 2;
	} else {
		n = 1;
	}
	int *u = stringToInts(s);
	for(int i = 0; i < n; i++){
		lines->at(1)[i + 4] = u[i];
	}
	linelength.at(1) = n + 4;
}

void StaticTextBox::setHP(int h){//HP: **0/0**
	std::string s = std::to_string(h);
	int *u = stringToInts(s);
	if(h > 99){
		lines->at(2)[4] = u[0];
		lines->at(2)[5] = u[1];
		lines->at(2)[6] = u[2];
	} else if (h > 9){
		lines->at(2)[4] = 0;
		lines->at(2)[5] = u[0];
		lines->at(2)[6] = u[1];
	} else {
		lines->at(2)[4] = 0;
		lines->at(2)[5] = 0;
		lines->at(2)[6] = u[0];
	}
}

void StaticTextBox::setMaxHP(int mh){//HP: **0/0**
	std::string s = std::to_string(mh);
	int *u = stringToInts(s);
	int n;
	if(mh > 99){
		n = 3;
	} else if (mh > 9){
		n = 2;
	} else {
		n = 1;
	}
	linelength.at(2) = n + 8;
	for(int i = 0; i < n; i++){
		lines->at(2)[i + 8] = u[i];
	}
}

void StaticTextBox::setMP(int m){//MP: **0/0**
	std::string s = std::to_string(m);
	int *u = stringToInts(s);
	if(m > 99){
		lines->at(3)[4] = u[0];
		lines->at(3)[5] = u[1];
		lines->at(3)[6] = u[2];
	} else if (m > 9){
		lines->at(3)[4] = 0;
		lines->at(3)[5] = u[0];
		lines->at(3)[6] = u[1];
	} else {
		lines->at(3)[4] = 0;
		lines->at(3)[5] = 0;
		lines->at(3)[6] = u[0];
	}
}

void StaticTextBox::setMaxMP(int mm){//MP: **0/0**
	std::string s = std::to_string(mm);
	int *u = stringToInts(s);
	int n;
	if(mm > 99){
		n = 3;
	} else if (mm > 9){
		n = 2;
	} else {
		n = 1;
	}
	linelength.at(3) = n + 8;
	for(int i = 0; i < n; i++){
		lines->at(3)[i + 8] = u[i];
	}
}*/
