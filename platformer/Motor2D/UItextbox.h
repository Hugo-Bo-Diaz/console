#ifndef __UI_TEXTBOX__
#define __UI_TEXTBOX__

#include "p2Point.h"
#include "p2SString.h"
#include "UIelement.h"

class UITextbox : public UIelement
{
public:
	p2SString string = "";
	int title_w = 0;
	int title_h = 0;
	int text_w = 0;
	int text_h = 0;
	SDL_Texture* tex = nullptr;
	SDL_Texture* default_text = nullptr;

	p2SString text = "";
	bool texting = false;
	int text_pos = 0;

public:
	UITextbox() {};
	UITextbox(iPoint pos, const char* name, const char* default_text);
	~UITextbox();


	void Draw(); 

	void OnClick();

	bool OnRelease();

	//functions for the buttons
};

#endif 