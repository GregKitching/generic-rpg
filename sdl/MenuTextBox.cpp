#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "Globals.h"
#include "SpriteSheet.h"
#include "TextBox.h"
#include "MenuTextBox.h"

MenuTextBox::MenuTextBox(int x, int y, int w, int h, int fx, int fy, int fw, int fh): TextBox(x, y, w, h, fx, fy, fw, fh){
	cursorpos = 0;
	type = TEXTBOX_MENU;
}

void MenuTextBox::initTextBox(){
	
}

void MenuTextBox::renderText(){
	if(fileloaded){
		setRect(&srcrect, 0, 0, 8, 16);
		setRect(&dstrect, xpos + fieldxpos, ypos + fieldypos, 8, 16);
		for(int i = 0; i < charheight; i++){
			for(int j = 0; j < linelength.at(scrollpos + i); j++){
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

void MenuTextBox::renderArrows(){
	setRect(&srcrect, 16, 0, 8, 8);
	setRect(&dstrect, xpos + 4, ypos + 8 + ((cursorpos - scrollpos) * 16), 8, 8);
	SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
}

void MenuTextBox::next(){
	if(cursorpos + scrollpos < lines->size() - 1){
		if(cursorpos - scrollpos >= charheight - 1){
			scrollpos++;
		}
		cursorpos++;
	}
}

void MenuTextBox::prev(){
	if(cursorpos > 0){
		if(cursorpos - scrollpos <= 0){
			scrollpos--;
		}
		cursorpos--;
	}
}

void MenuTextBox::tick(){
	
}

void MenuTextBox::reset(){
	l.erase(l.begin(), l.end());
	lines->erase(lines->begin(), lines->end());
	linelength.erase(linelength.begin(), linelength.end());
	scrollpos = 0;
	cursorpos = 0;
}

int MenuTextBox::close(){
	active = false;
	return cursorpos;
}
