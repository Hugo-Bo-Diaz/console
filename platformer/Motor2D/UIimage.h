#ifndef __UI_IMAGE__
#define __UI_IMAGE__

#include "p2Point.h"
#include "p2SString.h"
#include "j1App.h"
#include "UIelement.h"


class UIimage : public UIelement
{
public:
	bool follow_camera = false;
public:
	UIimage() {};
	UIimage(iPoint pos, SDL_Rect _rect, bool _follow_camera = false);
	//color is a 4 dim array in this order{r g b a} this is for the default font need to adapt it better
	~UIimage() {};
	void Draw();
};

#endif 