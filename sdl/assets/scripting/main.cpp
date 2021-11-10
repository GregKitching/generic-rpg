#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <vector>

std::regex comment("//.*");
std::regex whitespace("\\s");

std::regex nop("nop");
std::regex rtn("return");
std::regex hlt("halt");
std::regex uhl("unhalt");
std::regex fcp("faceplayer");
std::regex bch("branch\\([0-9]+\\)");
std::regex wai("wait\\([0-9]+\\)");
std::regex stv("settextboxvisible\\([0-9]+\\)");
std::regex sti("settextboxinvisible\\([0-9]+\\)");
std::regex fcc("facecurrent\\((up|down|left|right)\\)");
std::regex mvc("movecurrent\\((up|down|left|right)\\)");
std::regex jmp("jump\\([a-z]{4}\\)");
std::regex jsr("jumpsubroutine\\([a-z]{4}\\)");
std::regex dsp("display\\([a-z]{4}\\)");
std::regex str("store\\([0-9]+,[0-9]+\\)");
std::regex cpy("copy\\([0-9]+,[0-9]+\\)");
std::regex inc("increment\\([0-9]+,[0-9]+\\)");
std::regex dec("decrement\\([0-9]+,[0-9]+\\)");
std::regex fac("face\\([0-9]+,(up|down|left|right)\\)");
std::regex mov("move\\([0-9]+,(up|down|left|right)\\)");
std::regex add("add\\([0-9]+,[0-9]+,[0-9]+\\)");
std::regex subcmd("subtract\\([0-9]+,[0-9]+,[0-9]+\\)");
std::regex chs("choose\\([0-9]+,[0-9]+,[a-z]{4}\\)");

std::regex wt("%wt");
std::regex newline("%nl");
std::regex newpage("%np");
std::regex sub("%str[0-9]+");
std::regex numarg("[0-9]+,|\\)");
std::regex refarg("[a-z]{4},|\\)");
std::regex up("up");
std::regex down("down");
std::regex left("left");
std::regex right("right");

std::string playername = "Playername";
std::string nothing = "";

std::string scriptfolder = "scripts/";
std::string textfolder = "text/";
std::string scriptfileext = ".scr";
std::string textfileext = ".txt";

//uint8_t *heap = new uint8_t[256];

std::vector<std::string> lines;
std::vector<uint8_t> bin;

void oneArg(std::string *u, std::string s){
	int pos1 = s.find("(") + 1;
	int pos2 = s.find(")");
	int l = pos2 - pos1;
	u[0] = s.substr(pos1, l);
}

void twoArgs(std::string *u, std::string s){
	int pos1 = s.find("(") + 1;
	int pos2 = s.find(",");
	int pos3 = s.find(")");
	int l1 = pos2 - pos1;
	pos2++;
	int l2 = pos3 - pos2;
	u[0] = s.substr(pos1, l1);
	u[1] = s.substr(pos2, l2);
}

void threeArgs(std::string *u, std::string s){
	int pos1 = s.find(",") + 1;
	int pos2 = s.find(")");
	twoArgs(&u[1], s.substr(pos1, pos2 - pos1));
	int pos3 = s.find("(") + 1;
	int l1 = pos2 - 1 - pos3;
	u[0] = s.substr(pos3, l1);
}

void charval(int *u, std::string s){
	int l = s.length();
	char *c = new char[l + 1];
	strcpy(c, s.c_str());
	printf("%ul\n", s.length());
	for(int i = 0; i < l; i++){
		u[i] = (int) c[i];
	}
}

uint8_t dirVal(std::string u, std::smatch *s){
	uint8_t h;
	if(regex_match(u, *s, down)){
		h = 0;
	} else if (regex_match(u, *s, left)){
		h = 1;
	} else if(regex_match(u, *s, right)){
		h = 2;
	} else if (regex_match(u, *s, up)){
		h = 3;
	}
	return h;
}

int main(int argc, char **argv){
	if(argc != 3){
		printf("Not enough arguments.\n");
		return 0;
	}
	std::string srcfilename = std::string(argv[1]);
	std::string binfilename = std::string(argv[2]) + ".scr";
	std::ifstream srcfile;
	std::ofstream binfile;
	std::smatch s;
	uint8_t command;
	uint8_t arg0;
	uint8_t arg1;
	uint8_t arg2;
	srcfile.open(srcfilename.c_str());
	std::string *u = new std::string[3];
	int *u2 = new int[4];
	for(std::string line; getline(srcfile, line); ){
		line = regex_replace(line, whitespace, nothing);
		line = regex_replace(line, comment, nothing);
		printf("%s\n", line.c_str());
		if(regex_match(line, s, nop)){
			bin.push_back(0);
		} else if (regex_match(line, s, rtn)){
			bin.push_back(1);
		} else if (regex_match(line, s, hlt)){
			bin.push_back(2);
		} else if (regex_match(line, s, uhl)){
			bin.push_back(3);
		} else if (regex_match(line, s, fcp)){
			bin.push_back(4);
		} else if (regex_match(line, s, bch)){
			bin.push_back(5);
			oneArg(u, line);
			bin.push_back(stoi(u[0]));
		} else if (regex_match(line, s, wai)){
			bin.push_back(6);
			oneArg(u, line);
			bin.push_back(stoi(u[0]));
		} else if (regex_match(line, s, stv)){
			bin.push_back(7);
			oneArg(u, line);
			bin.push_back(stoi(u[0]));
		} else if (regex_match(line, s, sti)){
			bin.push_back(8);
			oneArg(u, line);
			bin.push_back(stoi(u[0]));
		} else if (regex_match(line, s, fcc)){
			bin.push_back(9);
			oneArg(u, line);
			bin.push_back(dirVal(u[0], &s));
		} else if (regex_match(line, s, mvc)){
			bin.push_back(10);
			oneArg(u, line);
			bin.push_back(dirVal(u[0], &s));
		} else if (regex_match(line, s, jmp)){
			bin.push_back(11);
			oneArg(u, line);
			charval(u2, u[0]);
			bin.push_back(u2[0]);
			bin.push_back(u2[1]);
			bin.push_back(u2[2]);
			bin.push_back(u2[3]);
		} else if (regex_match(line, s, jsr)){
			bin.push_back(12);
			oneArg(u, line);
			charval(u2, u[0]);
			bin.push_back(u2[0]);
			bin.push_back(u2[1]);
			bin.push_back(u2[2]);
			bin.push_back(u2[3]);
		} else if (regex_match(line, s, dsp)){
			bin.push_back(13);
			oneArg(u, line);
			charval(u2, u[0]);
			bin.push_back(u2[0]);
			bin.push_back(u2[1]);
			bin.push_back(u2[2]);
			bin.push_back(u2[3]);
		} else if (regex_match(line, s, str)){
			bin.push_back(14);
			twoArgs(u, line);
			bin.push_back(stoi(u[0]));
			bin.push_back(stoi(u[1]));
		} else if (regex_match(line, s, cpy)){
			bin.push_back(15);
			twoArgs(u, line);
			bin.push_back(stoi(u[0]));
			bin.push_back(stoi(u[1]));
		} else if (regex_match(line, s, inc)){
			bin.push_back(16);
			twoArgs(u, line);
			bin.push_back(stoi(u[0]));
			bin.push_back(stoi(u[1]));
		} else if (regex_match(line, s, dec)){
			bin.push_back(17);
			twoArgs(u, line);
			bin.push_back(stoi(u[0]));
			bin.push_back(stoi(u[1]));
		} else if (regex_match(line, s, fac)){
			bin.push_back(18);
			twoArgs(u, line);
			bin.push_back(stoi(u[0]));
			bin.push_back(dirVal(u[1], &s));
		} else if (regex_match(line, s, mov)){
			bin.push_back(19);
			twoArgs(u, line);
			bin.push_back(stoi(u[0]));
			bin.push_back(dirVal(u[1], &s));
		} else if (regex_match(line, s, add)){
			bin.push_back(20);
			threeArgs(u, line);
			bin.push_back(stoi(u[0]));
			bin.push_back(stoi(u[1]));
			bin.push_back(stoi(u[2]));
		} else if (regex_match(line, s, subcmd)){
			bin.push_back(21);
			threeArgs(u, line);
			bin.push_back(stoi(u[0]));
			bin.push_back(stoi(u[1]));
			bin.push_back(stoi(u[2]));
		} else if (regex_match(line, s, chs)){
			bin.push_back(22);
			threeArgs(u, line);
			charval(u2, u[2]);
			bin.push_back(stoi(u[0]));
			bin.push_back(stoi(u[1]));
			bin.push_back(u2[0]);
			bin.push_back(u2[1]);
			bin.push_back(u2[2]);
			bin.push_back(u2[3]);
		} else {
			printf("Fail\n");
		}
		printf("%s, %s, %s\n", u[0].c_str(), u[1].c_str(), u[2].c_str());
	}
	srcfile.close();
	
	char *h = new char[bin.size()];
	for(int i = 0; i < bin.size(); i++){
		h[i] = (char) bin.at(i);
	}
	binfile.open(binfilename.c_str(), std::ios::binary);
	binfile.write(h, bin.size());
	//printf("%s\n", h);
	binfile.close();
}
