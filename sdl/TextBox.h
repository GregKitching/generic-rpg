#ifndef _TEXT_BOX_
#define _TEXT_BOX_

class TextBox{
	protected:
		int xpos, ypos, width, height, fieldxpos, fieldypos, fieldwidth, fieldheight, charwidth, charheight, scrollpos;
		textboxtype type;
		/*int ypos;
		int fieldxpos;
		int width;
		int height;
		int pixwidth;
		int pixheight;
		int timer;
		int timerval;
		int cursorpos;
		int xofs;*/
		bool visible, active, fileloaded;//, waitforinput;
		/*bool haschoice;
		bool isbook;
		bool shownextarrow;
		bool firstpage;
		bool lastpage;*/
		std::string filename;
		//std::ifstream textfile;
		std::vector<int*> *lines;
		std::vector<int> linelength;
		std::vector<std::string> l;
		/*int linesshown;
		int scrollpos;
		int *charsshown;*/
		SDL_Rect srcrect;
		SDL_Rect dstrect;
		void setRect(SDL_Rect *rect, int x, int y, int w, int h);
		virtual void initTextBox() = 0;
		/*void advanceText();
		void advanceLine();
		void prevLine();*/
	public:
		TextBox(int x, int y, int w, int h, int fx, int fy, int fw, int fh);
		~TextBox();
		void renderBox();
		virtual void renderText() = 0;
		//virtual void renderCursor();
		virtual void renderArrows() = 0;
		//void renderPageArrows();
		textboxtype getType();
		bool getVisible();
		void setVisible(bool u);
		bool isActive();
		bool fileLoaded();
		/*bool hasChoice();
		bool isBook();
		bool isSpeech();
		void advance();*/
		virtual void next() = 0;
		virtual void prev() = 0;
		/*void nextPage();
		void prevPage();*/
		void loadFile(std::string f);
		void loadText();//std::vector<std::string> *l);
		virtual void tick() = 0;
		virtual void reset() = 0;
		virtual int close() = 0;
};

#endif
