#ifndef _SOUND_HANDLER_
#define _SOUND_HANDLER_

class SoundHandler{
	private:
		std::vector<Mix_Chunk*> sfx;
		std::string fileprefix;
		std::string filesuffix;
	public:
		SoundHandler();
		void quit();
		int loadSound(std::string name);
		void playSound(int u);
};

#endif
