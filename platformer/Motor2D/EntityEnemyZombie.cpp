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

#include "EntityEnemyZombie.h"

EntityEnemyZombie::EntityEnemyZombie()
{
	name.create("Zombie");

	// Idle animation
	idle.PushBack({ 0, 82, 30, 38 });
	idle.PushBack({ 32, 82, 30, 38 });
	idle.PushBack({ 64, 82, 30, 38 });
	idle.speed = 0.12f;
	//animations.add(idle);
	//Moving left animation
	left.PushBack({ 0, 42, 30, 38 });
	left.PushBack({ 32, 42, 30, 38 });
	left.PushBack({ 64, 42, 30, 38 });
	left.speed = 0.15f;
	//animations.add(left);
	//Moving right animation
	right.PushBack({ 0, 122, 30, 38 });
	right.PushBack({ 32, 122, 30, 38 });
	right.PushBack({ 64, 122, 30, 38 });
	right.speed = 0.15f;
	//animations.add(right);
	
	current_animation = &idle;
}

void EntityEnemyZombie::Start()
{
	BROFILER_CATEGORY("Start_EntityEnemyZombie", Profiler::Color::Purple);

	LOG("Loading enemy Zombie");
	//create object
	original_pos = position;
	SDL_Rect rect;
	rect.x = position.x;
	rect.y = position.y;
	rect.w = width;
	rect.h = height;
	obj = App->physics->Addobject(position.x, position.y,
									gravity, &rect, COLLIDER_ENEMY, (j1Module*)App->entities);
	destroyed = false;
	interactive = true;
}

void EntityEnemyZombie::Awake()
{
	LOG("Loading bat config");
	int j = 0;
	int i = 0;
	for (j=0;j<App->entities->properties.count();++j)
	{
		if (App->entities->properties[j]->type == 2)
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

bool EntityEnemyZombie::Update(float dt, bool logic)
{
	BROFILER_CATEGORY("Update_EntityEnemyZombie", Profiler::Color::Purple);

	//CONTROLS
	//obj->velocity.x = 0;
	position.x = obj->position.x;
	position.y = obj->position.y;
	//position is an easy way of telling where it is for other objects, not actually needed but useful in 
	//references and also not all entities have objects whose position is calculated automatically
	if (abs(position.x - App->map->player->position.x) < App->map->data.tile_width * 5 && App->map->player->position.y > 6)
	{
		if (logic == true)
		{
			iPoint player_center = { App->map->player->position.x + App->map->player->width / 2, App->map->player->position.y + App->map->player->height / 2 };
			App->path->PropagateBFS(path, { position.x + width / 2, position.y + height / 2 }, player_center);
			path.Pop(step);
		}
		/*iPoint worldStep = App->map->MapToWorld(step.x, step.y);
		if (obj->position.x < worldStep.x - 40)
			obj->velocity.x = max_speed;
		else if (obj->position.x > worldStep.x + 60)
			obj->velocity.x = -max_speed;*/
		
		iPoint pos = App->map->WorldToMap(obj->position.x, obj->position.y);
		if (pos.x < step.x)
			obj->velocity.x = max_speed;
		if (pos.x > step.x)
			obj->velocity.x = -max_speed;
		/*else
			obj->velocity.x = 0*/
	}
	else if (position.x != original_pos.x)
	{
		if (logic == true)
		{
			App->path->PropagateBFS(path, { position.x + width / 2, position.y + height / 2 }, original_pos);
			path.Pop(step);			
		}
		iPoint pos = App->map->WorldToMap(obj->position.x, obj->position.y);
		if (pos.x < step.x)
			obj->velocity.x = max_speed;
		else if (pos.x > step.x)
			obj->velocity.x = -max_speed;
		else
			obj->velocity.x = 0;
	}

	if (obj->velocity.x > 0)
	{
		current_animation = &right;
	}
	else if (obj->velocity.x < 0)
	{
		current_animation = &left;
	}
	else
	{
		current_animation = &idle;
	}

	/*if (obj->velocity.x < -max_speed)
		obj->velocity.x = -max_speed;
	if (obj->velocity.x > max_speed)
		obj->velocity.x = max_speed;*/
	return true;
}

void EntityEnemyZombie::Draw()
{
	BROFILER_CATEGORY("Draw_EntityEnemyZombie", Profiler::Color::Purple);

	App->render->Blit(App->entities->GetTex(2), (int)obj->position.x, (int)obj->position.y+1, &(current_animation->GetCurrentFrame()));
}

void EntityEnemyZombie::OnCollision(Collider* c1, Collider* c2)
{
	BROFILER_CATEGORY("OnCollision_EntityEnemyZombie", Profiler::Color::Purple);

	if (c1->type == COLLIDER_ENEMY && c2->type == COLLIDER_PLAYER)
	{
		SDL_Rect Intersection;
		SDL_IntersectRect(&c1->rect, &c2->rect, &Intersection);
		if (c1->rect.y > c2->rect.y && Intersection.w > Intersection.h)
		{
			LOG("A zombie was crushed!");
			App->scene->score += 100;
			destroyed = true;
			App->entities->Playfx(3);
		}
	}
	if (c1->type == COLLIDER_ENEMY && c2->type == COLLIDER_LAVA)
	{
		LOG("A zombie was barbecued!");
		destroyed = true;
		App->entities->Playfx(3);
	}
}