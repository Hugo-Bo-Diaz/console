#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "j1Audio.h"
#include "j1EntityManager.h"
#include "SDL/include/SDL_timer.h"
#include "Brofiler\Brofiler.h"

#include "EntityPlane.h"

EntityPlane::EntityPlane()
{
	name.create("Plane");
}

void EntityPlane::CleanUp()
{
	if (col != nullptr)
	{
		col->to_delete = true;
		col = nullptr;
	}
}

void EntityPlane::Start()
{
	BROFILER_CATEGORY("Start_EntityBackground", Profiler::Color::Purple);

	LOG("Loading background entity");
	col = App->collision->AddCollider({position.x,position.y,300,164},COLLIDER_PLANE);
	interactive = false;
}

void EntityPlane::Draw()
{
	BROFILER_CATEGORY("Draw_EntityBackground", Profiler::Color::Purple);

	App->render->Blit(App->entities->GetTex(3), position.x, position.y);
}