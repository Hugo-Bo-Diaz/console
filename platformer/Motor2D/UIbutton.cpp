#include "UIbutton.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1Textures.h"
#include "j1Gui.h"
#include "j1Audio.h"

UIButton::UIButton(iPoint pos, const char* text, button_type _type, SDL_Rect _portion, SDL_Rect _pressed, SDL_Rect _hover)
{
	winposition = pos;
	type_of_element = BUTTON;
	position = { 0,0 };
	portion = _portion;
	pressed = _pressed;
	glow = _hover;
	type = _type;
	string = text;
	if (string != "")
	{
		tex = App->tex->textures.add(App->font->Print(string.GetString(), { 0,0,0,255 }, App->font->default))->data;
		SDL_QueryTexture(tex, NULL, NULL, &text_w, &text_h);
	}

	int sx;
	int sy;
	App->input->GetMousePosition(sx, sy);
	mouse_stored = { sx,sy };

}

UIButton::~UIButton()
{
	App->tex->UnLoad(tex);
};

void UIButton::Draw()
{
	if (interactuable)
	{
		if (active)
		{
			App->render->Blit(App->gui->GetAtlas(), winposition.x, winposition.y, &pressed);
		}
		else
		{
			App->render->Blit(App->gui->GetAtlas(), winposition.x, winposition.y, &portion);
		}
		if (mouseover && !active)
		{
			App->render->Blit(App->gui->GetAtlas(), winposition.x - 11, winposition.y - 11, &glow);
			mouseover = false;
		}
	}
	if (!interactuable)
	{
		App->render->Blit(App->gui->GetAtlas(), winposition.x, winposition.y, &pressed);
	}
	if (tex != nullptr)
		App->render->Blit(tex, winposition.x + portion.w / 2 - text_w / 2, (winposition.y + portion.h / 2 - text_h / 2) - 2);

	if (deactivate_next_frame == true)
	{
		active = false;
		deactivate_next_frame = false;
	}

}

void UIButton::OnClick()
{
	if (mouseover)
	{
		active = true;
	}
};

bool UIButton::OnRelease()
{
	bool ret = true;

	if (mouseover && active && interactuable)
	{
		App->audio->PlayFx(App->gui->sound);
		ret = OnActivation();
	}
	deactivate_next_frame = true;
	
	return ret;
};