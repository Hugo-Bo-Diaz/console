#include "UICheckBox.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Render.h"

UICheckBox::UICheckBox(iPoint pos, const char* text, check_type _type)
{
	type_of_element = CHECKBOX;
	winposition = pos;
	position = { 0, 0 };
	portion = { 219,169,16,16 };
	type = _type;
	string = text;
	tex = App->tex->textures.add(App->font->Print(string.GetString(), { 255,255,0,255 }, App->font->default))->data;
	SDL_QueryTexture(tex, NULL, NULL, &text_w, &text_h);
}

UICheckBox::~UICheckBox()
{
	App->tex->UnLoad(tex);
}
void UICheckBox::Draw()
{

	App->render->Blit(App->gui->GetAtlas(), winposition.x, winposition.y, &portion);

	if (mouseover)
	{
		App->render->Blit(App->gui->GetAtlas(), winposition.x, winposition.y, &glow);
		mouseover = false;
	}
	if (active)
	{
		App->render->Blit(App->gui->GetAtlas(), winposition.x, winposition.y, &pressed);
	}
	if (active)
	{
		App->render->Blit(App->gui->GetAtlas(), winposition.x + 1, winposition.y + 1, &cross);
	}
	App->render->Blit(tex, winposition.x + 20, (winposition.y + portion.h / 2 - text_h / 2));
}

void UICheckBox::OnClick()
{
	if (mouseover)
	{
		active = true;
	}
}

bool UICheckBox::OnRelease()
{
	bool ret = true;

	if (mouseover && active)
	{
		active = !active;
	}
	active = false;
	return ret;
}