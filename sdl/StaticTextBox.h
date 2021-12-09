#ifndef _STATIC_TEXT_BOX_
#define _STATIC_TEXT_BOX_

class StaticTextBox: public TextBox{
	private:
		/*std::string name;
		//int *buff;
		void initTextBox();
		int* stringToInts(std::string s);
		//int level, hp, maxhp, mp, maxmp;*/
		void initTextBox();
	public:
		StaticTextBox(int x, int y, int w, int h, int fx, int fy, int fw, int fh);//, std::string n, int l, int h, int mh, int m, int mm);
		void renderText();
		void renderArrows();
		void next();
		void prev();
		void tick();
		void reset();
		int getValue();
		void close();
		/*void setName(std::string n);
		void setLevel(int l);
		void setHP(int h);
		void setMaxHP(int mh);
		void setMP(int m);
		void setMaxMP(int mm);*/
};

#endif
