#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "j1Audio.h"
#include "j1Physics.h"
#include "j1Pathfinding.h"
#include "j1EntityManager.h"
#include "j1Scene.h"
#include "SDL/include/SDL_timer.h"
#include "Brofiler\Brofiler.h"

#include "EntityEnemyBat.h"

EntityEnemyBat::EntityEnemyBat()
{
	name.create("Bat");

	// Idle animation
	idle.PushBack({ 0, 0, 30, 28 });
	idle.PushBack({ 32, 0, 30, 28 });
	idle.PushBack({ 64, 0, 30, 28 });
	idle.speed = 0.12f;
	//animations.add(idle);
	//Moving left animation
	left.PushBack({ 0, 30, 18, 28 });
	left.PushBack({ 20, 30, 18, 28 });
	left.PushBack({ 40, 30, 18, 28 });
	left.PushBack({ 60, 30, 18, 28 });
	left.speed = 0.15f;
	//animations.add(left);
	//Moving right animation
	right.PushBack({ 0, 60, 18, 28 });
	right.PushBack({ 20, 60, 18, 28 });
	right.PushBack({ 40, 60, 18, 28 });
	right.PushBack({ 60, 60, 18, 28 });
	right.speed = 0.15f;
	//animations.add(right);

	current_animation = &idle;
}

void EntityEnemyBat::Start()
{
	BROFILER_CATEGORY("Start_EntityEnemyBat", Profiler::Color::Purple);

	LOG("Loading enemy bat");
	//create object
	original_pos = position;
	SDL_Rect rect;
	rect.x = position.x;
	rect.y = position.y;
	rect.w = width;
	rect.h = height;
	obj = App->physics->Addobject(position.x + 8, position.y,
									gravity, &rect, COLLIDER_ENEMY, (j1Module*)App->entities);
	destroyed = false;
	interactive = true;
}

void EntityEnemyBat::Awake()
{
	LOG("Loading bat config");
	int j = 0;
	int i = 0;
	for (j=0;j<App->entities->properties.count();++j)
	{
		if (App->entities->properties[j]->type == 1)
		{
			i = j;
			break;
		}
	}
	width = App->entities->properties[i++]->value;
	height = App->entities->properties[i++]->value;
	acceleration = App->entities->properties[i++]->value;
	max_speed = App->entities->properties[i++]->value;
	gravity = App->entities->properties[i++]->value;
	
}

bool EntityEnemyBat::Update(float dt, bool logic)
{
	BROFILER_CATEGORY("Update_EntityEnemyBat", Profiler::Color::Purple);

	if (abs(position.x - App->map->player->position.x) < App->map->data.tile_width * 7 && App->map->player->position.y > 6)
	{
		if (logic == true)
		{
			iPoint player_center = { App->map->player->position.x + App->map->player->width / 2, App->map->player->position.y + App->map->player->height / 2 };
			App->path->PropagateBFS(path, { position.x + width / 2, position.y + height / 2 }, player_center, false);
			path.Pop(step);
		}
		/*iPoint worldStep = App->map->MapToWorld(step.x, step.y);
		if (obj->position.x < worldStep.x && obj->velocity.x < max_speed)
			obj->acceleration.x = max_speed;
		else if (obj->position.x > worldStep.x && obj->velocity.x > -max_speed)
			obj->acceleration.x = -max_speed;
		else if (obj->position.y < worldStep.y && obj->velocity.y < max_speed)
			obj->acceleration.y = max_speed;
		else if (obj->position.y > worldStep.y && obj->velocity.y > -max_speed)
			obj->acceleration.y = -max_speed;
		else
		{
			obj->acceleration.x = 0;
		}*/

		iPoint pos = App->map->WorldToMap(obj->position.x, obj->position.y);
		if (pos.x < step.x)
			obj->velocity.x = max_speed;
		if (pos.x > step.x)
			obj->velocity.x = -max_speed;
		if (pos.y < step.y)
			obj->velocity.y = max_speed;
		if (pos.y > step.y)
			obj->velocity.y = -max_speed;
	}
	else
	{
		if (logic == true)
		{
			App->path->PropagateBFS(path, { position.x + width / 2, position.y + height / 2 }, original_pos, false);
			path.Pop(step);

		iPoint pos = App->map->WorldToMap(obj->position.x, obj->position.y);
		if (pos.x < step.x)
			obj->velocity.x = max_speed;
		if (pos.x > step.x)
			obj->velocity.x = -max_speed;
		if (pos.y < step.y)
			obj->velocity.y = max_speed;
		if (pos.y > step.y)
			obj->velocity.y = -max_speed;
		}
	}

	/*if (obj->velocity.x < -max_speed)
		obj->velocity.x = -max_speed;
	if (obj->velocity.x > max_speed)
		obj->velocity.x = max_speed;
	if (obj->velocity.y < -max_speed)
		obj->velocity.y = -max_speed;
	if (obj->velocity.y > max_speed)
		obj->velocity.y = max_speed;*/

	position.x = obj->position.x;
	position.y = obj->position.y;
	//position is an easy way of telling where it is for other objects, not actually needed but useful in 
	//references and also not all entities have objects whose position is calculated automatically
	return true;
}

void EntityEnemyBat::Draw()
{
	BROFILER_CATEGORY("Draw_EntityEnemyBat", Profiler::Color::Purple);

	App->render->Blit(App->entities->GetTex(1), (int)obj->position.x, (int)obj->position.y, &(current_animation->GetCurrentFrame()));
}

void EntityEnemyBat::OnCollision(Collider* c1, Collider* c2)
{
	BROFILER_CATEGORY("OnCollision_EntityEnemyBat", Profiler::Color::Purple);

	if (c1->type == COLLIDER_ENEMY && c2->type == COLLIDER_PLAYER)
	{
		SDL_Rect Intersection;
		SDL_IntersectRect(&c1->rect, &c2->rect, &Intersection);
		if (c1->rect.y > c2->rect.y && Intersection.w > Intersection.h)
		{
			LOG("A bat was crushed");
			App->scene->score += 100;
			destroyed = true;
			App->entities->Playfx(2);
		}
	}
	if (c1->type == COLLIDER_ENEMY && c2->type == COLLIDER_LAVA)
	{
		LOG("A bat was barbecued!");
		destroyed = true;
		App->entities->Playfx(2);
	}
}