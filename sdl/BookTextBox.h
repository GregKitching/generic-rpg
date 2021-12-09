#ifndef _BOOK_TEXT_BOX_
#define _BOOK_TEXT_BOX_

class BookTextBox: public TextBox{
	private:
		int pages, currentpage, linesshown;
		void initTextBox();
	public:
		BookTextBox(int x, int y, int w, int h, int fx, int fy, int fw, int fh);
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
