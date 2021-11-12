#ifndef _TEXT_BOX_
#define _TEXT_BOX_

class TextBox{
	private:
		int xpos;
		int ypos;
		int width;
		int height;
		int timer;
		int timerval;
		int row1pos;
		int row2pos;
		bool secondline;
		bool visible;
		bool active;
		bool fileloaded;
		bool waitforinput;
		bool buffersswapped;
		std::string filename;
		//std::string line;
		std::ifstream textfile;
		//std::vector<int> text;
		std::vector<std::string> lines;
		int linespos;
		int *bufa;
		int *bufb;
		int **buf1;
		int **buf2;
		int linealength;
		int lineblength;
		int *line1length;
		int *line2length;
		bool lastline;
		bool oneline;
		SDL_Texture *texture;
		SpriteSheet *font;
		SDL_Renderer *renderer;
		SDL_Rect srcrect;
		SDL_Rect dstrect;
		void swapBuffers();
		void setRect(SDL_Rect *rect, int x, int y, int w, int h);
		int getLine(int *buf, bool firstline);
		void advanceText();
		void advanceLine();
	public:
		TextBox(int x, int y, int w, int h, SDL_Texture *t, SpriteSheet *f, SDL_Renderer *r);
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
};

#endif
