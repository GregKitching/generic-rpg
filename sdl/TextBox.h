#ifndef _TEXT_BOX_
#define _TEXT_BOX_

class TextBox{
	private:
		int xpos;
		int ypos;
		int width;
		int height;
		bool visible;
		std::vector<int> text;
		SDL_Texture *texture;
		SDL_Rect srcrect;
		SDL_Rect dstrect;
		void setRect(SDL_Rect *rect, int x, int y, int w, int h);
	public:
		TextBox(int x, int y, int w, int h, SDL_Texture *t);
		void render(SDL_Renderer *renderer);
		void renderText(SDL_Renderer * renderer, SpriteSheet *font);
		bool getVisible();
		void setVisible(bool u);
		void setText(std::string f);
		void clearText();
};

#endif
