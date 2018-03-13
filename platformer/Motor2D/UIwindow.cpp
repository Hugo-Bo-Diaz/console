#include "UIwindow.h"
#include "UIbutton.h"
#include "j1Input.h"
#include "j1Fonts.h"
#include "j1App.h"
#include "j1Render.h"

UIwindow::UIwindow(iPoint pos, const char* text, SDL_Rect _portion, bool _close_button)// color is a 4 dim array in this order{r g b a} this is for the default font need to adapt it better
{
	type_of_element = WINDOW;
	winposition = pos;
	portion = _portion;
	title = text;
	if (title != "")
	{
		tex = App->tex->textures.add(App->font->Print(title.GetString(), { 0,0,0,255 }, App->font->default))->data;
		SDL_QueryTexture(tex, NULL, NULL, &text_w, &text_h);
	}
	close_button = _close_button;
}

UIwindow::~UIwindow()
{
	p2List_item<UIelement*>* item = attached_elements.start;
	while (item != NULL)
	{
		App->gui->delete_element(item->data);
		item = item->next;

	}
	App->tex->UnLoad(tex);

	if (this == App->gui->Window_menu)//only needed for the window menu because you are permitted only one window at a time
	{
		App->gui->Window_menu = nullptr;
	}
}

void UIwindow::Start()
{
	if (close_button)
	{
		close = (UIButton*)App->gui->GUIAdd_button(0, 0, { 282,300,25,25 }, { 307,300,25,25 }, { 0,0,0,0 }, nullptr, "", button_type::CLOSE_WINDOW);
		Attach(close, { portion.w - 25,0 });
	}
}

void UIwindow::Draw()
{
	App->render->Blit(App->gui->GetAtlas(), winposition.x, winposition.y, &portion);
	if (mouseover)
	{
		mouseover = false;
	}
	if (tex != nullptr)
		App->render->Blit(tex, winposition.x + portion.w / 2 - text_w / 2, winposition.y + 10);
}

bool UIwindow::OnRelease()
{
	bool ret = true;

	active = false;
	if (close != nullptr &&close->mouseover && close->active)
	{
		LOG("DESTROY THIS WINDOW");
		App->gui->delete_element(this);
	}

	return ret;
};