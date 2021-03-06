#ifndef _SCRIPT_
#define _SCRIPT_

class Script{
	private:
		const std::string scriptpathfirst = "assets/scripts/";
		const std::string scriptpathlast = ".scr";
		const std::string textpathfirst = "assets/text/";
		const std::string textpathlast = ".txt";
		std::string scriptname;
		std::string nextscript;
		std::vector<command> commands;
		std::vector<char> arguments;
		int entitynum;
		int commandpos;
		int argumentpos;
		bool intextbox;
		bool displayingtext;
		//bool waitforinput;
		int waittimer;
		bool waitfortimer;
		bool scriptend;
		bool nextcommand;
		bool waiting;
		dir oppositeDir(dir direction);
		int getIntArg();
		std::string getStringArg();
		void waitForEntity(int ent);
	public:
		Script(int a, std::string filename);
		~Script();
		void executeCommand();
		void advance();
		void dirAction(dir direction);
		std::string run();
		bool waitingForInput();
		int getEntityNum();
};

#endif
