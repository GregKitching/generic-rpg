#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "Globals.h"
#include "SpriteSheet.h"
#include "TextBox.h"
#include "BookTextBox.h"

BookTextBox::BookTextBox(int x, int y, int w, int h, int fx, int fy, int fw, int fh): TextBox(x, y, w, h, fx, fy, fw, fh){
	pages = 0;
	type = TEXTBOX_BOOK;
}

void BookTextBox::initTextBox(){
	pages = (lines->size() / charheight) + 1;
	//printf("Pages: %d\n", pages);
	currentpage = 0;
	if(pages == 1){
		linesshown = lines->size();
	} else {
		linesshown = charheight;
	}
}

void BookTextBox::renderText(){
	if(fileloaded){
		setRect(&srcrect, 0, 0, 8, 16);
		setRect(&dstrect, xpos + fieldxpos, ypos + fieldypos, 8, 16);
		for(int i = 0; i < linesshown; i++){
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

void BookTextBox::renderArrows(){
	if(currentpage != 0){
		setRect(&srcrect, 8, 0, 8, 8);
		setRect(&dstrect, xpos + width - 24, ypos + height - 8, 8, 8);
		SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	}
	if(currentpage != pages - 1){
		setRect(&srcrect, 16, 0, 8, 8);
		setRect(&dstrect, xpos + width - 16, ypos + height - 8, 8, 8);
		SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	}
}

void BookTextBox::next(){
	if(currentpage < pages - 1){
		currentpage++;
		scrollpos += charheight;
		if(currentpage == pages - 1){
			linesshown = lines->size() - scrollpos;
		}
	}
}

void BookTextBox::prev(){
	if(currentpage > 0){
		currentpage--;
		scrollpos -= charheight;
		linesshown = charheight;
	}
}

void BookTextBox::tick(){
	
}

void BookTextBox::reset(){
	l.erase(l.begin(), l.end());
	lines->erase(lines->begin(), lines->end());
	linelength.erase(linelength.begin(), linelength.end());
	pages = 0;
	currentpage = 0;
	linesshown = 0;
}

int BookTextBox::close(){
	active = false;
	return currentpage;
}
