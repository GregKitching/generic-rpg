#ifndef _TEXT_BOX_
#define _TEXT_BOX_

class TextBox{
	protected:
		int xpos, ypos, width, height, fieldxpos, fieldypos, fieldwidth, fieldheight, charwidth, charheight, scrollpos;
		textboxtype type;
		bool visible, active, fileloaded;
		std::string filename;
		std::vector<int*> *lines;
		std::vector<int> linelength;
		std::vector<std::string> l;
		SDL_Rect srcrect;
		SDL_Rect dstrect;
		void setRect(SDL_Rect *rect, int x, int y, int w, int h);
		int* stringToInts(std::string s);
		virtual void initTextBox() = 0;
	public:
		TextBox(int x, int y, int w, int h, int fx, int fy, int fw, int fh);
		~TextBox();
		void renderBox();
		virtual void renderText() = 0;
		virtual void renderArrows() = 0;
		textboxtype getType();
		bool getVisible();
		void setVisible(bool u);
		bool isActive();
		bool fileLoaded();
		virtual void next() = 0;
		virtual void prev() = 0;
		void loadFile(std::string f);
		void loadText(std::vector<std::string> l);
		virtual void tick() = 0;
		virtual void reset() = 0;
		virtual int getValue() = 0;
		virtual void close() = 0;
		void changeText(int linenum, int start, int length, std::string s);
		void setText(std::vector<int*> *u);
		std::vector<int*>* getLines();
};

#endif
