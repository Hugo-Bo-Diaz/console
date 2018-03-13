#ifndef __UI_CHECK__
#define __UI_CHECK__

#include "p2Point.h"
#include "p2SString.h"
#include "j1App.h"
#include "UIelement.h"

struct SDL_Texture;

enum check_type
{
	ACCOUNT_NAME,
	NULLL
};

class UICheckBox : public UIelement
{
public:
	SDL_Rect cross = {199,170,16,16};
	SDL_Rect glow = {159,170,16,16};
	SDL_Rect pressed = { 178,170,16,16 };
	check_type type = NULLL;
	p2SString string = "";
	int text_w = 0;
	int text_h = 0;
	SDL_Texture* tex = nullptr;
public:
	UICheckBox() {};
	UICheckBox(iPoint pos, const char* text, check_type _type = NULLL);// color is a 4 dim array in this order{r g b a} this is for the default font need to adapt it better

	~UICheckBox();

	void Draw();

	void OnClick() ;
	bool OnRelease() ;
};

#endif 