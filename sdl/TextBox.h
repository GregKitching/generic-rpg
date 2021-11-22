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
		int cursorpos;
		int xofs;
		bool visible;
		bool active;
		bool fileloaded;
		bool waitforinput;
		bool haschoice;
		bool isbook;
		bool firstpage;
		bool lastpage;
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
		void prevLine();
	public:
		TextBox(int x, int y, int w, int h, bool c, bool b);
		~TextBox();
		void renderBox();
		void renderText();
		void renderCursor();
		void renderPageArrows();
		bool getVisible();
		void setVisible(bool u);
		bool getWaitForInput();
		bool isActive();
		bool hasChoice();
		bool isBook();
		bool isSpeech();
		void advance();
		void next();
		void prev();
		void nextPage();
		void prevPage();
		void loadFile(std::string f);
		void reset();
		void tick();
		bool fileLoaded();
		int getChoice();
};

#endif
