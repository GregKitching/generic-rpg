#ifndef _SPEECH_TEXT_BOX_
#define _SPEECH_TEXT_BOX_

class SpeechTextBox: public TextBox{
	private:
		int timer, timerval, currentline;
		int *charsshown;
		bool drawingtext, endflag;
		void initTextBox();
	public:
		SpeechTextBox(int x, int y, int w, int h, int fx, int fy, int fw, int fh, int tm);
		void renderText();
		void renderArrows();
		void next();
		void prev();
		void tick();
		void reset();
		int close();
};

#endif
