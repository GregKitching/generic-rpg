#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>

#include "Globals.h"
#include "enums.h"
#include "ScriptDefs.h"
#include "Script.h"
#include "TextBox.h"
#include "SpeechTextBox.h"
#include "MenuTextBox.h"
#include "BookTextBox.h"

extern uint8_t *heap;
//extern TextBox *maintextbox;
extern std::vector<Entity*> entities;
extern Map *map;
extern bool caninteract;
extern bool canmove;
extern SpriteSheet *font;

struct stat result;
//Script recursivescript = NULL;

Script::Script(int a, std::string filename){
	scriptname = scriptpathfirst + filename + scriptpathlast;
	std::ifstream scriptfile;
	std::string line;
	if(stat(scriptname.c_str(), &result) != 0){
		printf("Fail\n");
		printf("%s\n", scriptname.c_str());
	} else {
		int size = result.st_size;
		char *temp = new char[size];
		scriptfile.open(scriptname.c_str(), std::ios::binary);
		if(!scriptfile.is_open()){
			printf("Failed to open script file \"%s\".\n", scriptname.c_str());
		} else {
			scriptfile.read(temp, size);
			//command tempcommand = CMD_NOP;
			int i = 0;
			while(i < size){
				commands.push_back((command) temp[i]);
				if ((int) temp[i] < oneintpos){//no args
					
				} else if ((int) temp[i] < oneaddresspos){//one int
					i++;
					arguments.push_back(temp[i]);
				} else if ((int) temp[i] < twointpos){//one address
					i++;
					arguments.push_back(temp[i]);
					i++;
					arguments.push_back(temp[i]);
					i++;
					arguments.push_back(temp[i]);
					i++;
					arguments.push_back(temp[i]);
				} else if ((int) temp[i] < threeintpos){//two ints
					i++;
					arguments.push_back(temp[i]);
					i++;
					arguments.push_back(temp[i]);
				} else if ((int) temp[i] < twointoneaddresspos){//three ints
					i++;
					arguments.push_back(temp[i]);
					i++;
					arguments.push_back(temp[i]);
					i++;
					arguments.push_back(temp[i]);
				} else {//two ints, one address
					i++;
					arguments.push_back(temp[i]);
					i++;
					arguments.push_back(temp[i]);
					i++;
					arguments.push_back(temp[i]);
					i++;
					arguments.push_back(temp[i]);
					i++;
					arguments.push_back(temp[i]);
					i++;
					arguments.push_back(temp[i]);
				}
				i++;
			}
		}
		/*for(int i = 0; i < commands.size(); i++){
			//printf("%d\n", (int) commands.at(i));
		}
		//printf("\n");
		/*for(int i = 0; i < arguments.size(); i++){
			//printf("%d\n", (int) arguments.at(i));
		}*/
		//printf("c %lu\n", commands.size());
		scriptfile.close();
		delete [] temp;
		entitynum = a;
		commandpos = 0;
		argumentpos = 0;
		nextscript = "";
		nextcommand = true;
		intextbox = false;
		displayingtext = false;
		waiting = false;
		//waitforinput = false;
		waittimer = 0;
		//waitfortimer = false;
		scriptend = false;
	}
}

Script::~Script(){
	//delete commands;
	//delete arguments;
}

dir Script::oppositeDir(dir direction){
	dir u;
	switch(direction){
		case DIR_DOWN:
		u = DIR_UP;
		break;
		
		case DIR_LEFT:
		u = DIR_RIGHT;
		break;
		
		case DIR_RIGHT:
		u = DIR_LEFT;
		break;
		
		case DIR_UP:
		u = DIR_DOWN;
		break;
	}
	return u;
}

int Script::getIntArg(){
	//printf("getIntArd\n");
	int u = (int) arguments.at(argumentpos);
	argumentpos++;
	return u;
}

std::string Script::getStringArg(){
	char *u = new char[5];
	for(int i = 0; i < 4; i++){
		u[i] = arguments.at(argumentpos);
		argumentpos++;
	}
	u[4] = '\0';
	std::string h = std::string(u).substr(0, 4);
	//printf("%s\n", h.c_str());
	delete [] u;
	return h;
}

/*void Script::nextCommand(){
	printf("nextCommand\n");
	commandpos++;
	executeCommand();
}*/

void Script::waitForEntity(int ent){
	ActiveEntity *ae = static_cast<ActiveEntity*>(entities.at(ent));
	while(ae->isBusy()){
		//usleep(16666);
	}
}

void Script::executeCommand(){
	//bool u = false;
	int var1, var2, var3;
	std::string path;
	/*while(maintextbox->getVisible()){
		usleep(16666);
	}*/
	/*while(waiting){
		printf("waiting\n");
	}*/
	ActiveEntity *ae;
	switch(commands.at(commandpos)){
		case CMD_NOP:
		//nextcommand = true;
		break;
		
		case CMD_RETURN:
		scriptend = true;
		//destroy
		break;
		
		case CMD_HALT:
		//printf("halt\n");
		caninteract = false;
		canmove = false;
		//waitforinput = true;
		//nextcommand = true;
		break;
		
		case CMD_UNHALT:
		//printf("unhalt\n");
		caninteract = true;
		canmove = true;
		break;
		
		case CMD_FACEPLAYER:
		//int u = getIntArg();
		//argumentpos++;
		waitForEntity(entitynum);
		static_cast<ActiveEntity*>(entities.at(entitynum))->setFacingDir(oppositeDir(player->getFacingDir()));
		//nextCommand();
		break;
		
		case CMD_WAITFORCURRENTENTITY:
		waitForEntity(entitynum);
		break;
		
		case CMD_DESTROYTEXTBOX:
		//printf("CMD_DESTROYTEXTBOX\n");
		if(textboxes.size() > 0){
			delete textboxes.at(textboxes.size() - 1);
			textboxes.pop_back();
		} else {
			printf("The text box stack is already empty.\n");
		}
		break;
		
		case CMD_BRANCH:
		var1 = getIntArg();
		commandpos += (int) heap[var1];
		argumentpos += ((int) heap[var1]) * 4;
		//nextCommand();
		break;
		
		case CMD_WAIT:
		waitfortimer = true;
		waittimer = getIntArg();
		break;
		
		case CMD_CREATETEXTBOX:
		{
			textboxspawntype t = (textboxspawntype) getIntArg();
			switch(t){
				case TEXTBOXSPAWN_SPEECH:
				textboxes.push_back(new SpeechTextBox(4, 123, 248, 56, 4, 4, 240, 48, 2));
				break;
				
				case TEXTBOXSPAWN_YESNO:
				textboxes.push_back(new MenuTextBox(120, 59, 40, 40, 12, 4, 24, 32));
				break;
				
				case TEXTBOXSPAWN_MENU:
				textboxes.push_back(new MenuTextBox(120, 59, 40, 40, 12, 4, 24, 32));
				break;
				
				case TEXTBOXSPAWN_STORE:
				textboxes.push_back(new MenuTextBox(120, 59, 40, 40, 12, 4, 24, 32));
				break;
				
				case TEXTBOXSPAWN_BOOK:
				textboxes.push_back(new BookTextBox(4, 4, 248, 184, 4, 4, 240, 176));
				break;
				
				default:
				break;
			}
		}
		//nextCommand();
		//printf("CMD_CREATETEXTBOX\n");
		break;
		
		case CMD_FACECURRENT:
		var1 = getIntArg();
		static_cast<ActiveEntity*>(entities.at(entitynum))->setFacingDir((dir) var1);
		//waitForEntity(entitynum);
		break;
		
		case CMD_MOVECURRENT:
		var1 = getIntArg();
		static_cast<ActiveEntity*>(entities.at(entitynum))->move((dir) var1);
		waitForEntity(entitynum);
		break;
		
		case CMD_WAITFORENTITY:
		waitForEntity(getIntArg());
		break;
		
		case CMD_JUMP:
		nextscript = getStringArg();
		scriptend = true;
		break;
		
		case CMD_JUMPSUBROUTINE:
		path = scriptpathfirst + getStringArg() + scriptpathlast;
		//recursivescript = new Script(entitynum, path);
		break;
		
		case CMD_DISPLAY:
		//printf("CMD_DISPLAY\n");
		//printf("%ld\n", textboxes.size());
		path = getStringArg();
		if(textboxes.size() > 0){
			textboxes.at(textboxes.size() - 1)->reset();
			textboxes.at(textboxes.size() - 1)->loadFile(path);
		} else {
			printf("No text box to display to.\n");
		}
		//displayingtext = true;
		//printf("g\n");
		//intextbox = true;
		//waitforinput = true;
		break;
		
		case CMD_STORE:
		var1 = getIntArg();
		heap[var1] = (uint8_t) getIntArg();
		//nextCommand();
		break;
		
		case CMD_COPY:
		var1 = getIntArg();
		var2 = getIntArg();
		heap[var1] = heap[var2];
		//nextCommand();
		break;
		
		case CMD_INCREMENT:
		var1 = getIntArg();
		heap[var1] += getIntArg();
		//nextCommand();
		break;
		
		case CMD_DECREMENT:
		var1 = getIntArg();
		heap[var1] -= getIntArg();
		//nextCommand();
		break;
		
		case CMD_FACE:
		var1 = getIntArg();
		var2 = getIntArg();
		static_cast<ActiveEntity*>(entities.at(var1))->setFacingDir((dir) var2);
		//waitForEntity(var1);
		break;
		
		case CMD_MOVE:
		var1 = getIntArg();
		var2 = getIntArg();
		static_cast<ActiveEntity*>(entities.at(var1))->move((dir) var2);
		waitForEntity(var1);
		break;
		
		case CMD_ADD:
		var1 = getIntArg();
		var2 = getIntArg();
		var3 = getIntArg();
		heap[var1] = heap[var2] + heap[var3];
		break;
		
		case CMD_SUBTRACT:
		var1 = getIntArg();
		var2 = getIntArg();
		var3 = getIntArg();
		heap[var1] = heap[var2] - heap[var3];
		break;
		
		case CMD_MOVEMULTIPLE:
		var1 = getIntArg();
		var2 = getIntArg();
		var3 = getIntArg();
		//waiting = true;
		ae = static_cast<ActiveEntity*>(entities.at(var1));
		for(int i = 0; i < var3; i++){
			//entities.at(var1).setBusy();
			ae->move((dir) var2);
			waitForEntity(var1);
		}
		//waiting = false;
		break;
		
		case CMD_CHOOSE:
		//???
		break;
		
		case CMD_WARP:
		var1 = getIntArg();
		var2 = getIntArg();
		path = getStringArg();
		renderflag = false;
		while(rendering){//Wait for rendering thread to stop rendering
			usleep(16666);
		}
		delete map;
		entities.erase(entities.begin() + 1, entities.end());
		map = new Map(path, var1, (dir) var2);
		renderflag = true;
		break;
	}
	commandpos++;
	//return u;
}

void Script::advance(){
	if(textboxes.size() > 0){
		textboxtype t = textboxes.at(textboxes.size() - 1)->getType();
		if(t == TEXTBOX_SPEECH){
			textboxes.at(textboxes.size() - 1)->next();
		} else {
			heap[0] = textboxes.at(textboxes.size() - 1)->close();
		}
		/*if(textboxes.at(textboxes.size() - 1)->getWaitForInput()){
			if(textboxes.at(textboxes.size() - 1)->hasChoice()){
				heap[0] = textboxes.at(textboxes.size() - 1)->getChoice();
			}
			textboxes.at(textboxes.size() - 1)->advance();
			//printf("advance\n");
		}*/
	}	
}

void Script::dirAction(dir direction){
	if(textboxes.size() > 0){
		textboxtype t = textboxes.at(textboxes.size() - 1)->getType();
		if(t == TEXTBOX_MENU){
			if(direction == DIR_UP){
				textboxes.at(textboxes.size() - 1)->prev();
			} else if (direction == DIR_DOWN){
				textboxes.at(textboxes.size() - 1)->next();
			}
		} else if (t == TEXTBOX_BOOK){
			if(direction == DIR_LEFT){
				textboxes.at(textboxes.size() - 1)->prev();
			} else if (direction == DIR_RIGHT){
				textboxes.at(textboxes.size() - 1)->next();
			}
		}
		/*if(textboxes.at(textboxes.size() - 1)->hasChoice()){// || textboxes.at(textboxes.size() - 1)->isBook()){
			if(direction == DIR_DOWN){
				textboxes.at(textboxes.size() - 1)->next();
			} else if (direction == DIR_UP){
				textboxes.at(textboxes.size() - 1)->prev();
			}
		} else if (textboxes.at(textboxes.size() - 1)->isBook()){
			if(direction == DIR_RIGHT){
				textboxes.at(textboxes.size() - 1)->nextPage();
			} else if (direction == DIR_LEFT){
				textboxes.at(textboxes.size() - 1)->prevPage();
			}
		}*/
	}
}
		

std::string Script::run(){
	while(!scriptend){
		//printf("uu, %ld\n", textboxes.size());
		if(textboxes.size() > 0){
			//printf("%d\n", (int) textboxes.at(textboxes.size() - 1)->isActive());
			while(textboxes.at(textboxes.size() - 1)->isActive()){
				//printf("scriptthread\n");
				usleep(16666);
			}
			//printf("2\n");
			//printf("u, %d\n", (int) textboxes.at(textboxes.size() - 1)->isActive());
			displayingtext = false;
		}
		executeCommand();
	}
	return nextscript;
}

bool Script::waitingForInput(){
	return 0;//waitforinput;
}

int Script::getEntityNum(){
	return entitynum;
}
