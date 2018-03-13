#ifndef __UI_VARDISPLAY__
#define __UI_VARDISPLAY__

#include "p2SString.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1App.h"
#include "UIelement.h"


class UIVarDisplay : public UIelement
{
public:
	SDL_Texture* texture = nullptr;
	int* variable = 0;
	SDL_Color color = { 50, 50, 50, 255 };
	bool follow_camera = false;
public:
	UIVarDisplay() {};
	UIVarDisplay(iPoint pos, int* _variable, SDL_Color _color, bool _follow_camera, _TTF_Font* font = App->font->default);
	~UIVarDisplay() {};

	void Draw();
};

#endif 