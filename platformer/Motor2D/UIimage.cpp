#include "UIimage.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"

UIimage::UIimage(iPoint pos, SDL_Rect _rect, bool _follow_camera)
{
	type_of_element = IMAGE;
	winposition = pos;
	portion = _rect;
	follow_camera = _follow_camera;
}

void UIimage::Draw() {
	if (follow_camera)
		App->render->Blit(App->gui->GetAtlas(), -App->render->camera.x + winposition.x, -App->render->camera.y + winposition.y, &portion);
	else
		App->render->Blit(App->gui->GetAtlas(), winposition.x, winposition.y, &portion);

	if (mouseover)
	{
		mouseover = false;
	}
};