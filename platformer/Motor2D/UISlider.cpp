#include "UISlider.h"
#include "j1Gui.h"
#include "j1Render.h"

UIslider::UIslider(iPoint pos, int* _variable, SDL_Rect _portion, SDL_Rect _start, SDL_Rect _end, SDL_Rect _middle, SDL_Rect _button,int _max_value,const char* _title)// color is a 4 dim array in this order{r g b a} this is for the default font need to adapt it better
{
	winposition = pos;
	variable = _variable;
	type_of_element = SLIDER;
	portion = _portion;
	start = _start;
	end = _end;
	middle = _middle;
	button = _button;
	title_string = _title;

	movement_x = portion.w - start.w - end.w;//all the moves are here
	position_x = winposition.x + start.w;//+ movement_x;// here its his position
	max_value = _max_value;//trying to do this to the volume, will adapt later


}
UIslider::~UIslider()
{
	App->tex->UnLoad(title_text);
}
void UIslider::Start()
{
	title_text = App->tex->textures.add(App->font->Print(title_string.GetString(), { 50,50,50,255 }, App->font->default))->data;
}

void UIslider::Draw()
{
	if (position_x < winposition.x + start.w)
	{
		position_x = winposition.x + start.w;
	}

	//blit title
	App->render->Blit(title_text,winposition.x,winposition.y-20);

	//blit middle
	int number_of_middle_pieces = (portion.w-start.w) / middle.w;
	for (int i = 0; i < number_of_middle_pieces; ++i)
	{
		int position_y = winposition.y + (portion.h / 2 - middle.h / 2);
		App->render->Blit(App->gui->GetAtlas(), winposition.x +start.w + (middle.w*i), position_y, &middle);
	}

	//blit start
	App->render->Blit(App->gui->GetAtlas(),winposition.x,winposition.y,&start);
	
	//blit end
	App->render->Blit(App->gui->GetAtlas(),winposition.x + (portion.w - end.w),winposition.y, &end);
	

	//blit and calculate variable piece
	//first we need to know where to draw it we need its position in % we need a max value(i'll asume 0 is min)

	float value = *variable;
	float percent = (value / max_value);//value = percent*max_value
	//now we need it to draw it accordingly

	if (active)
	{
		int mouse_mx;
		int my;
		App->input->GetMouseMotion(mouse_mx, my);
		if (mouse_stored.x != mouse_mx 
			&& position_x >= (winposition.x + start.w) 
			&& position_x <= (winposition.x)+(portion.w-end.w-button.w))
		{
			position_x += mouse_mx;
			mouse_stored.x = mouse_mx;
			float distance = position_x - (winposition.x + start.w);
			float max_distance = movement_x;
			if (position_x < (winposition.x + start.w))
			{
				position_x = winposition.x + start.w;
			}
			if (position_x > (winposition.x) + (portion.w - end.w - button.w))
			{
				position_x = (winposition.x) + (portion.w - end.w - button.w);
			}
			float percent_rised = distance / max_distance;
			float new_value = max_value*percent_rised;
			*variable = new_value;
			LOG("%d %d",position_x,winposition.x+portion.w);
		}
	}

	App->render->Blit(App->gui->GetAtlas(), position_x , winposition.y, &button);

	if (mouseover)
	{
		mouseover = false;
	}
};

void UIslider::OnClick()
{
	SDL_Rect b = { position_x,winposition.y,button.w,button.h };
	if (App->gui->MouseInside(&b))
	{
		active = true;
	}

}

bool UIslider::OnRelease()
{
	active = false;
	return true;
}