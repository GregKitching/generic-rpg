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

extern uint8_t *heap;
extern TextBox *maintextbox;
extern std::vector<Entity> entities;
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
		for(int i = 0; i < commands.size(); i++){
			//printf("%d\n", (int) commands.at(i));
		}
		printf("\n");
		for(int i = 0; i < arguments.size(); i++){
			//printf("%d\n", (int) arguments.at(i));
		}
		//printf("c %lu\n", commands.size());
		entitynum = a;
		scriptfile.close();
		commandpos = 0;
		argumentpos = 0;
		nextcommand = true;
		intextbox = false;
		displayingtext = false;
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
	u[5] = '\0';
	std::string h = std::string(u).substr(0, 4);
	//printf("%s\n", h.c_str());
	delete u;
	return h;
}

/*void Script::nextCommand(){
	printf("nextCommand\n");
	commandpos++;
	executeCommand();
}*/

void Script::waitForEntity(int ent){
	while(entities.at(entitynum).getMoveTimer() != 0){
		usleep(16666);
	}
}

void Script::executeCommand(){
	//bool u = false;
	int var1, var2, var3;
	std::string path;
	/*while(maintextbox->getVisible()){
		usleep(16666);
	}*/
	switch(commands.at(commandpos)){
		case CMD_NOP:
		//nextcommand = true;
		break;
		
		case CMD_RETURN:
		scriptend = true;
		//destroy
		break;
		
		case CMD_HALT:
		printf("halt\n");
		caninteract = false;
		canmove = false;
		//waitforinput = true;
		//nextcommand = true;
		break;
		
		case CMD_UNHALT:
		printf("unhalt\n");
		caninteract = true;
		canmove = true;
		break;
		
		case CMD_FACEPLAYER:
		//int u = getIntArg();
		//argumentpos++;
		waitForEntity(entitynum);
		entities.at(entitynum).setFacingDir(oppositeDir(entities.at(0).getFacingDir()));
		//nextCommand();
		break;
		
		case CMD_WAITFORCURRENTENTITY:
		waitForEntity(entitynum);
		break;
		
		case CMD_BRANCH:
		commandpos += getIntArg();
		//nextCommand();
		break;
		
		case CMD_WAIT:
		waitfortimer = true;
		waittimer = getIntArg();
		break;
		
		case CMD_SETTEXTBOXVISIBLE:
		var1 = getIntArg();
		switch(var1){
			case 0:
			maintextbox->setVisible(true);
			intextbox = true;
			break;
		}
		//nextCommand();
		printf("CMD_SETTEXTBOXVISIBLE\n");
		break;
		
		case CMD_SETTEXTBOXINVISIBLE:
		var1 = getIntArg();
		switch(var1){
			case 0:
			maintextbox->setVisible(false);
			intextbox = false;
			displayingtext = false;
			break;
		}
		//nextCommand();
		printf("CMD_SETTEXTBOXINVISIBLE\n");
		break;
		
		case CMD_FACECURRENT:
		var1 = getIntArg();
		waitForEntity(entitynum);
		entities.at(entitynum).setFacingDir((dir) var1);
		//nextCommand();
		break;
		
		case CMD_MOVECURRENT:
		var1 = getIntArg();
		waitForEntity(entitynum);
		entities.at(entitynum).move((dir) var1);
		break;
		
		case CMD_WAITFORENTITY:
		waitForEntity(getIntArg());
		break;
		
		case CMD_JUMP:
		//somehow break out of this script
		path = scriptpathfirst + getStringArg() + scriptpathlast;
		//???
		break;
		
		case CMD_JUMPSUBROUTINE:
		path = scriptpathfirst + getStringArg() + scriptpathlast;
		//recursivescript = new Script(entitynum, path);
		break;
		
		case CMD_DISPLAY:
		printf("CMD_DISPLAY\n");
		path = getStringArg();
		//printf("UUUU %s\n", path.c_str());
		printf("e\n");
		maintextbox->reset();
		printf("f\n");
		maintextbox->loadFile(path);
		displayingtext = true;
		printf("g\n");
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
		waitForEntity(var1);
		entities.at(var1).setFacingDir((dir) var2);
		break;
		
		case CMD_MOVE:
		var1 = getIntArg();
		var2 = getIntArg();
		waitForEntity(var1);
		entities.at(var1).move((dir) var2);
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
		
		case CMD_CHOOSE:
		//???
		break;
	}
	commandpos++;
	//return u;
}

void Script::advance(){
	if(maintextbox->getWaitForInput()){
		maintextbox->input();
		printf("advance\n");
	}	
}

void Script::run(){
	//printf("a\n");
	while(!scriptend){
		if(displayingtext){
			while(maintextbox->isActive()){
				usleep(16666);
			}
			displayingtext = false;
		}
		executeCommand();
	}
}

bool Script::waitingForInput(){
	return 0;//waitforinput;
}
