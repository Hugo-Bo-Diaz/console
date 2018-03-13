#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "j1Audio.h"
#include "j1Physics.h"
#include "j1EntityManager.h"
#include "j1Pathfinding.h"
#include "j1Scene.h"
#include "j1Gui.h"
#include "j1Transition.h"
#include "SDL/include/SDL_timer.h"
#include "Brofiler\Brofiler.h"

#include "EntityPlayer.h"

#include<stdio.h>
#include <time.h>
#include <stdlib.h>

EntityPlayer::EntityPlayer()
{
	name.create("Player");


	// idle animation
	idle.PushBack({ 277, 3, 45, 45 });
	idle.PushBack({ 322, 3, 45, 45 });
	idle.speed = 0.04f;

	// Move Right
	left.PushBack({ 3, 3, 45, 45 });
	left.PushBack({ 48, 3, 45, 45 });
	left.PushBack({ 93, 3, 45, 45 });
	left.PushBack({ 137, 3, 45, 45 });
	left.PushBack({ 182, 3, 45, 45 });
	left.PushBack({ 227, 3, 45, 45 });
	left.speed = 0.18f;

	// Move Left
	right.PushBack({ 3, 52, 45, 45 });
	right.PushBack({ 48, 52, 45, 45 });
	right.PushBack({ 93, 52, 45, 45 });
	right.PushBack({ 137, 52, 45, 45 });
	right.PushBack({ 182, 52, 45, 45 });
	right.PushBack({ 227, 52, 45, 45 });
	right.speed = 0.18f;

	// Jumpsquat Right animation
	jumpsquatRight.PushBack({ 93, 100, 45, 45 });
	jumpsquatRight.speed = 0.0f;

	// Airborne Right animation
	airborneRight.PushBack({ 182, 149, 45, 45 });
	airborneRight.PushBack({ 227, 149, 45, 45 });
	airborneRight.speed = 0.1f;

	// Jumpsquat Left animation
	jumpsquatLeft.PushBack({ 93, 198, 45, 45 });
	jumpsquatLeft.speed = 0.0f;

	// Airborne Left animation
	airborneLeft.PushBack({ 182, 247, 45, 45 });
	airborneLeft.PushBack({ 227, 247, 45, 45 });
	airborneLeft.speed = 0.1f;

	current_animation = &idle;


}

void EntityPlayer::Start()
{
	BROFILER_CATEGORY("Start_EntityPlayer", Profiler::Color::Gold);

	LOG("Loading player");
	//create object


	SDL_Rect rect;
	rect.x = App->map->initial_player_pos.x;
	rect.y = App->map->initial_player_pos.y;
	rect.w = width;
	rect.h = height;
	obj = App->physics->Addobject(	App->map->initial_player_pos.x, App->map->initial_player_pos.y, 
									gravity, &rect, COLLIDER_PLAYER, (j1Module*)App->entities);
	position.x = obj->position.x;
	position.y = obj->position.y;
	destroyed = false;
	interactive = true;
}

void EntityPlayer::Awake()
{
	LOG("Loading player config");
	int j = 0;
	int i = 0;
	for (j = 0; j<App->entities->properties.count(); ++j)
	{
		if (App->entities->properties[j]->type == 0)
		{
			i = j;
			break;
		}
	}
		width = App->entities->properties[i++]->value;
		height = App->entities->properties[i++]->value;
		jump_speed = App->entities->properties[i++]->value;
		acceleration = App->entities->properties[i++]->value;
		max_speed = App->entities->properties[i++]->value;
		gravity = App->entities->properties[i++]->value;
		hability_stored = App->entities->properties[i++]->value;
		if (!App->scene->godmode)
		{
			hability = hability_stored;
		}
		else
		{
			hability = 7;
		}
}

// Update: draw background
bool EntityPlayer::Update(float dt, bool logic)
{
	BROFILER_CATEGORY("Update_EntityPlayer", Profiler::Color::Gold);
	/*p2DynArray<iPoint> path;
	App->path->PropagateAStar(path,position,iPoint(0,0));*/

	float thousanddivdt = 1000 / dt;// ~ frame_cap

	normalize_factor = 60 / thousanddivdt;//60 is the max framerate we support	
	if (!App->paused_game)
	{
		//CONTROLS
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT /*&& obj->velocity.x <max_speed*/)
		{
			obj->velocity.x = max_speed;
			current_animation = &left;
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT /*&& obj->velocity.x >-max_speed*/)
		{
			obj->velocity.x = -max_speed;
			current_animation = &right;
		}
		if (obj->velocity.x > max_speed || obj->velocity.x < -max_speed)
		{
			obj->acceleration.x = 0.0f;
		}

		if ((App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE)
			&& (App->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE))
		{
			current_animation = &idle;
			obj->velocity.x = 0.0f; //this stops the player
			obj->acceleration.x = 0.0f;
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT
			&& App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			current_animation = &idle;
			obj->velocity.x = 0.0f; //this stops the player
			obj->acceleration.x = 0.0f;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && obj->grounded && obj->velocity.y < 0.5)
		{
			obj->velocity.y = -jump_speed;
			obj->grounded = false;
			srand(time(NULL));
			int sound = rand()% 2 + 5;
			App->entities->Playfx(sound);
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			obj->velocity.y = -hability;
		}

		//aereal animations
		if (obj->grounded == false)
		{
			if (obj->velocity.y < 0)
			{
				if (obj->velocity.x < 0)
					current_animation = &jumpsquatLeft;
				if (obj->velocity.x >= 0)
					current_animation = &jumpsquatRight;
			}
			else
			{
				if (obj->velocity.x < 0)
					current_animation = &airborneLeft;
				if (obj->velocity.x >= 0)
					current_animation = &airborneRight;
			}
		}
	}
	position.x = obj->position.x;
	position.y = obj->position.y;
	//position is an easy way of telling where it is for other objects, not actually needed but useful in 
	//references and also not all entities have objects whose position is calculated automatically

	//SDL_Texture* tex = App->font->Print("IDIOT", {255,0,0,255}, App->font->fonts.start->data);
	//App->render->Blit(tex, position.x - 5, position.y - 10);
	//SDL_DestroyTexture(tex);
	return true;
}

void EntityPlayer::Draw()
{
	BROFILER_CATEGORY("Draw_EntityPlayer", Profiler::Color::Gold);
	if (App->entities->GetTex(0) != nullptr)
	{
		App->render->Blit(App->entities->GetTex(0), (int)obj->position.x - 10, (int)obj->position.y, &(current_animation->GetCurrentFrame()));
	}
}
bool EntityPlayer::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate_EntityPlayer", Profiler::Color::Gold);

	if (set_to_start_pos == true && death_timer_on == false)
	{
		death.Start();
		death_timer_on = true;
		App->entities->Playfx(4);
		App->transition->Fade(500.0f, 150, 0, 0);
	}
	if (App->scene->time_left <= 0 && death_timer_on == false)
	{
		death.Start();
		death_timer_on = true;
		App->transition->Fade(500.0f, 255, 255, 255);
	}

	if (death.Read() > 250.0f && (set_to_start_pos|| App->scene->time_left <= 0))
	{
		PlayerDeath();
	}

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		if (!App->paused_game)
		{
			App->PauseGame();
		}
	}
	return true;
}

void EntityPlayer::OnCollision(Collider* c1, Collider* c2)
{
	BROFILER_CATEGORY("OnCollision_EntityPlayer", Profiler::Color::Gold);

	if (c1->type == COLLIDER_PLAYER &&c2->type == COLLIDER_NEXT_LEVEL)
	{
		App->map->change_to_next_level = true;
		App->transition->StartTransition();
	}
	if (c1->type == COLLIDER_PLAYER &&c2->type == COLLIDER_LAVA && !App->scene->godmode&& set_to_start_pos == false)
	{
		set_to_start_pos = true;
	}
	if (c1->type == COLLIDER_PLAYER &&c2->type == COLLIDER_ENEMY && !App->scene->godmode && set_to_start_pos == false)
	{
		SDL_Rect Intersection;
		SDL_IntersectRect(&c1->rect, &c2->rect, &Intersection);
		if (c1->rect.y > c2->rect.y || Intersection.w < Intersection.h)
		{
			set_to_start_pos = true;
		}
		else
		{
			App->map->player->obj->velocity.y = -5*normalize_factor;
		}
	}
	if (c1->type == COLLIDER_PLAYER &&c2->type == COLLIDER_PLANE && ended == false)
	{
		App->map->change_to_this_level = App->map->maps.count() - 1;
		App->transition->StartTransition();
		ended = true;
	}
}

void EntityPlayer::SetPosOrigin()
{
	obj->position.x = App->map->player_start_in_map.x;
	obj->position.y = App->map->player_start_in_map.y;
	obj->velocity.x = 0;
	obj->velocity.y = 0;
}
void EntityPlayer::PlayerDeath()
{
	App->scene->lifes -= 1;
	if (App->scene->lifes > 0)
	{
		App->scene->score = App->scene->score*2/3;
		App->scene->coins = App->scene->coins;
	}
	App->map->change_to_this_level = App->map->index_map;
	set_to_start_pos = false;
}