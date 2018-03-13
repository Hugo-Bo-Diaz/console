#include "UItext.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Render.h"
#include "j1App.h"

UItext::UItext(iPoint pos, const char* _text, SDL_Color _color, bool _follow_camera, _TTF_Font* font)// color is a 4 dim array in this order{r g b a} this is for the default font need to adapt it better
{
	winposition = pos;
	//text.create(_text);
	texture = App->tex->textures.add(App->font->Print(_text, _color, font))->data;
	//color = _color;
	type_of_element = TEXT;
	follow_camera = _follow_camera;
	portion.w = 150;
	portion.h = 50;
}

UItext::~UItext() 
{ 
	App->tex->UnLoad(texture); 
};

void UItext::Draw()
{
	if (follow_camera)
		App->render->Blit(texture, -App->render->camera.x + winposition.x, -App->render->camera.y + winposition.y);
	else
		App->render->Blit(texture, winposition.x, winposition.y);

	if (mouseover)
	{
		mouseover = false;
	}
};