#ifndef _TEXT_BOX_
#define _TEXT_BOX_

class TextBox{
	private:
		int xpos;
		int ypos;
		int width;
		int height;
		int pixwidth;
		int pixheight;
		int timer;
		int timerval;
		bool visible;
		bool active;
		bool fileloaded;
		bool waitforinput;
		std::string filename;
		std::ifstream textfile;
		std::vector<int*> lines;
		std::vector<int> linelength;
		int linesshown;
		int scrollpos;
		int *charsshown;
		SDL_Rect srcrect;
		SDL_Rect dstrect;
		void setRect(SDL_Rect *rect, int x, int y, int w, int h);
		void advanceText();
		void advanceLine();
	public:
		TextBox(int x, int y, int w, int h);
		~TextBox();
		void renderBox();
		void renderText();
		bool getVisible();
		void setVisible(bool u);
		bool getWaitForInput();
		bool isActive();
		void input();
		void loadFile(std::string f);
		void reset();
		void tick();
		bool fileLoaded();
};

#endif
