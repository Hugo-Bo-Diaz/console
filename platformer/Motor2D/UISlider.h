#ifndef __UI_SLIDER__
#define __UI_SLIDER__

#include "p2SString.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1App.h"
#include "UIelement.h"


class UIslider : public UIelement
{
public:
	SDL_Texture* title_text = nullptr;
	int* variable = 0;
	SDL_Color color = { 50, 50, 50, 255 };
	SDL_Rect start = { 0, 0, 0, 0 };
	SDL_Rect end = { 0, 0, 0, 0 };
	SDL_Rect middle = { 0, 0, 0, 0 };
	SDL_Rect button = { 0, 0, 0, 0 };
	iPoint mouse_stored = { 0,0 };
	int position_x = 0;
	int movement_x = 0;
	float max_value = 0.0;
	p2SString title_string = "";

public:
	UIslider() {};
	UIslider(iPoint pos, int* _variable,SDL_Rect portion,SDL_Rect start,SDL_Rect end, SDL_Rect middle,SDL_Rect button, int _max_value, const char* title = "");
	~UIslider();

	void Start();

	void OnClick();

	bool OnRelease();

	void Draw();
};

#endif 