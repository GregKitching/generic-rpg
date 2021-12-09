#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "Globals.h"
#include "SpriteSheet.h"
#include "TextBox.h"
#include "SpeechTextBox.h"

SpeechTextBox::SpeechTextBox(int x, int y, int w, int h, int fx, int fy, int fw, int fh, int tm): TextBox(x, y, w, h, fx, fy, fw, fh), timer(tm){
	timerval = timer;
	scrollpos = 0;
	currentline = 0;
	drawingtext = false;
	endflag = false;
	charsshown = new int[charheight];
	type = TEXTBOX_SPEECH;
}

void SpeechTextBox::initTextBox(){
	for(int i = 0; i < charheight; i++){
		charsshown[i] = 0;
	}
	drawingtext = true;
}

void SpeechTextBox::renderText(){
	if(fileloaded){
		setRect(&srcrect, 0, 0, 8, 16);
		setRect(&dstrect, xpos + fieldxpos, ypos + fieldypos, 8, 16);
		for(int i = 0; i < currentline + 1; i++){
			for(int j = 0; j < charsshown[i]; j++){
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

void SpeechTextBox::renderArrows(){
	if(!drawingtext){
		setRect(&srcrect, 32, 0, 8, 8);
		setRect(&dstrect, xpos + width - 16, ypos + height - 8, 8, 8);
		SDL_RenderCopy(renderer, textboxtexture, &srcrect, &dstrect);
	}
}

void SpeechTextBox::next(){
	if(!drawingtext){
		if(endflag){
			active = false;
		} else {
			scrollpos++;
			for(int i = 0; i < charheight - 1; i++){
				charsshown[i] = charsshown[i + 1];
			}
			charsshown[charheight - 1] = 0;
			drawingtext = true;
		}
	}
}

void SpeechTextBox::prev(){
	if(!drawingtext){
		if(endflag){
			active = false;
		} else {
			scrollpos--;
			for(int i = 1; i < charheight; i++){
				charsshown[i] = charsshown[i - 1];
			}
			charsshown[0] = 0;
			drawingtext = true;
		}
	}
}

void SpeechTextBox::tick(){
	if(drawingtext){
		timer--;
		if(timer <= 0){
			if(charsshown[currentline] >= linelength.at(scrollpos + currentline)){//all characters on the current line have been drawn
				if(scrollpos + currentline >= lines->size() - 1){//no more lines
					endflag = true;
					drawingtext = false;
				} else {
					if(currentline < charheight - 1){//have not reached final row of text in text box
						currentline++;
					} else {
						drawingtext = false;
					} 
				}
			} else {
				charsshown[currentline]++;
			}
			timer = timerval;
		}
	}
}

void SpeechTextBox::reset(){
	l.erase(l.begin(), l.end());
	lines->erase(lines->begin(), lines->end());
	linelength.erase(linelength.begin(), linelength.end());
	timer = timerval;
	scrollpos = 0;
	currentline = 0;
	drawingtext = false;
	endflag = false;
}

int SpeechTextBox::getValue(){
	return 0;
}

void SpeechTextBox::close(){
	active = false;
}
