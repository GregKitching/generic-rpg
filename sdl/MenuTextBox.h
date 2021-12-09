#ifndef _MENU_TEXT_BOX_
#define _MENU_TEXT_BOX_

class MenuTextBox: public TextBox{
	private:
		int cursorpos;
		void initTextBox();
	public:
		MenuTextBox(int x, int y, int w, int h, int fx, int fy, int fw, int fh);
		void renderText();
		void renderArrows();
		void next();
		void prev();
		void tick();
		void reset();
		int getValue();
		void close();
};

#endif
