#ifndef _SCRIPT_
#define _SCRIPT_

class Script{
	private:
		const std::string scriptpathfirst = "assets/scripts/";
		const std::string scriptpathlast = ".scr";
		const std::string textpathfirst = "assets/text/";
		const std::string textpathlast = ".txt";
		std::string scriptname;
		std::vector<command> commands;
		std::vector<char> arguments;
		int entitynum;
		int commandpos;
		int argumentpos;
		bool intextbox;
		bool waitforinput;
		int waittimer;
		bool waitfortimer;
		bool nextcommand;
		dir oppositeDir(dir direction);
		int getIntArg();
		std::string getStringArg();
	public:
		Script(int a, std::string filename);
		~Script();
		void advance();
		bool executeCommand();
		void tick();
		bool waitingForInput();
};

#endif
