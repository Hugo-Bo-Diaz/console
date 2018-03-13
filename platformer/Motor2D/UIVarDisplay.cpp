#include "UIVarDisplay.h"
#include "j1Gui.h"
#include "j1Render.h"
#include <stdlib.h>

UIVarDisplay::UIVarDisplay(iPoint pos, int* _variable, SDL_Color _color, bool _follow_camera, _TTF_Font* font)// color is a 4 dim array in this order{r g b a} this is for the default font need to adapt it better
{
	winposition = pos;
	color = _color;
	variable = _variable;
	type_of_element = VARDISPLAY;
	follow_camera = _follow_camera;
	portion.w = 150;
	portion.h = 50;
}

void UIVarDisplay::Draw()
{
	if (variable != nullptr)
	{
		int value = *variable;
		std::string s = std::to_string(value);
		p2SString pchar = s.c_str();
		SDL_Texture* tex = App->font->Print(pchar.GetString(), color, App->font->fonts.start->data);

		if (follow_camera)
			App->render->Blit(tex, -App->render->camera.x + winposition.x, -App->render->camera.y + winposition.y);
		else
			App->render->Blit(tex, winposition.x, winposition.y);

		SDL_DestroyTexture(tex);
	}
	if (mouseover)
	{
		mouseover = false;
	}
};