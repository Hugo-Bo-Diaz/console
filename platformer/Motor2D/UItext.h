#ifndef __UI_TEXT__
#define __UI_TEXT__

#include "p2SString.h"
#include "UIelement.h"

class UItext : public UIelement
{
public:
	SDL_Texture* texture = nullptr;
	p2SString text = "";
	SDL_Color color = { 50, 50, 50, 255 };
	bool follow_camera = false;
public:
	UItext() {};
	UItext(iPoint pos, const char* _text, SDL_Color _color, bool _follow_camera, _TTF_Font* font = App->font->default);// color is a 4 dim array in this order{r g b a} this is for the default font need to adapt it better
	~UItext();

	void Draw();
};
#endif 