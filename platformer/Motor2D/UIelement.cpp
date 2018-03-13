#include "UIelement.h"
#include "j1Gui.h"
#include "j1App.h"

void UIelement::Update()
{
	if (active && App->gui->debug)
	{
		int x;
		int y;
		App->input->GetMousePosition(x, y);
		if (attached == false)
		{
			if (mouse_stored.x != x)
				winposition.x += x - mouse_stored.x;
			if (mouse_stored.y != y)
				winposition.y += y - mouse_stored.y;
		}
		else
		{
			if (mouse_stored.x != x)
				position.x += x - mouse_stored.x;
			if (mouse_stored.y != y)
				position.y += y - mouse_stored.y;
		}
	}
	int sx;
	int sy;
	App->input->GetMousePosition(sx, sy);
	mouse_stored = { sx,sy };
};

void UIelement::UpdateAttached()
{
	p2List_item<UIelement*>* item = attached_elements.start;
	while (item != NULL)
	{
		item->data->winposition.x = winposition.x + item->data->position.x;
		item->data->winposition.y = winposition.y + item->data->position.y;
		item = item->next;
	}
}

void UIelement::Attach(UIelement* element, iPoint pos)
{
	element->position = pos;//position relative to the window
	element->attached = true;
	attached_elements.add(element);
}