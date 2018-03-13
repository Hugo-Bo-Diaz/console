#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "Brofiler\Brofiler.h"

j1Collision::j1Collision()
{
	active_in_pause = false;
	for(uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;
	matrix[COLLIDER_WALL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WALL][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_WALL][COLLIDER_FUTURE] = true;
	matrix[COLLIDER_WALL][COLLIDER_NEXT_LEVEL] = false;
	matrix[COLLIDER_WALL][COLLIDER_LAVA] = false;
	matrix[COLLIDER_WALL][COLLIDER_SLOPE_LEFT] = false;
	matrix[COLLIDER_WALL][COLLIDER_SLOPE_RIGHT] = false;
	matrix[COLLIDER_WALL][COLLIDER_PLANE] = false;
	matrix[COLLIDER_WALL][COLLIDER_COIN] = false;

	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_FUTURE] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_NEXT_LEVEL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_LAVA] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_SLOPE_LEFT] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_SLOPE_RIGHT] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLANE] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_COIN] = true;

	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_FUTURE] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_NEXT_LEVEL] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_LAVA] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_SLOPE_LEFT] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_SLOPE_RIGHT] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_PLANE] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_COIN] = false;

	matrix[COLLIDER_FUTURE][COLLIDER_FUTURE] = false;
	matrix[COLLIDER_FUTURE][COLLIDER_WALL] = true;
	matrix[COLLIDER_FUTURE][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_FUTURE][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_FUTURE][COLLIDER_NEXT_LEVEL] = false;
	matrix[COLLIDER_FUTURE][COLLIDER_LAVA] = false;
	matrix[COLLIDER_FUTURE][COLLIDER_SLOPE_LEFT] = true;
	matrix[COLLIDER_FUTURE][COLLIDER_SLOPE_RIGHT] = true;
	matrix[COLLIDER_FUTURE][COLLIDER_PLANE] = true;
	matrix[COLLIDER_FUTURE][COLLIDER_COIN] = true;

	matrix[COLLIDER_NEXT_LEVEL][COLLIDER_FUTURE] = false;
	matrix[COLLIDER_NEXT_LEVEL][COLLIDER_WALL] = false;
	matrix[COLLIDER_NEXT_LEVEL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_NEXT_LEVEL][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_NEXT_LEVEL][COLLIDER_NEXT_LEVEL] = false;
	matrix[COLLIDER_NEXT_LEVEL][COLLIDER_LAVA] = false;
	matrix[COLLIDER_NEXT_LEVEL][COLLIDER_SLOPE_LEFT] = false;
	matrix[COLLIDER_NEXT_LEVEL][COLLIDER_SLOPE_RIGHT] = false;
	matrix[COLLIDER_NEXT_LEVEL][COLLIDER_PLANE] = false;
	matrix[COLLIDER_NEXT_LEVEL][COLLIDER_COIN] = false;

	matrix[COLLIDER_LAVA][COLLIDER_FUTURE] = false;
	matrix[COLLIDER_LAVA][COLLIDER_WALL] = false;
	matrix[COLLIDER_LAVA][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_LAVA][COLLIDER_NEXT_LEVEL] = false;
	matrix[COLLIDER_LAVA][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_LAVA][COLLIDER_LAVA] = false;
	matrix[COLLIDER_LAVA][COLLIDER_SLOPE_LEFT] = false;
	matrix[COLLIDER_LAVA][COLLIDER_SLOPE_RIGHT] = false;
	matrix[COLLIDER_LAVA][COLLIDER_PLANE] = false;
	matrix[COLLIDER_LAVA][COLLIDER_COIN] = false;

	matrix[COLLIDER_SLOPE_LEFT][COLLIDER_FUTURE] = true;
	matrix[COLLIDER_SLOPE_LEFT][COLLIDER_WALL] = true;
	matrix[COLLIDER_SLOPE_LEFT][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_SLOPE_LEFT][COLLIDER_NEXT_LEVEL] = false;
	matrix[COLLIDER_SLOPE_LEFT][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_SLOPE_LEFT][COLLIDER_LAVA] = false;
	matrix[COLLIDER_SLOPE_LEFT][COLLIDER_SLOPE_LEFT] = false;
	matrix[COLLIDER_SLOPE_LEFT][COLLIDER_SLOPE_RIGHT] = false;
	matrix[COLLIDER_SLOPE_LEFT][COLLIDER_PLANE] = false;
	matrix[COLLIDER_SLOPE_LEFT][COLLIDER_COIN] = false;

	matrix[COLLIDER_SLOPE_RIGHT][COLLIDER_FUTURE] = true;
	matrix[COLLIDER_SLOPE_RIGHT][COLLIDER_WALL] = true;
	matrix[COLLIDER_SLOPE_RIGHT][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_SLOPE_RIGHT][COLLIDER_NEXT_LEVEL] = false;
	matrix[COLLIDER_SLOPE_RIGHT][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_SLOPE_RIGHT][COLLIDER_LAVA] = false;
	matrix[COLLIDER_SLOPE_RIGHT][COLLIDER_SLOPE_LEFT] = false;
	matrix[COLLIDER_SLOPE_RIGHT][COLLIDER_SLOPE_RIGHT] = false;
	matrix[COLLIDER_SLOPE_RIGHT][COLLIDER_PLANE] = false;
	matrix[COLLIDER_SLOPE_RIGHT][COLLIDER_COIN] = false;

	matrix[COLLIDER_PLANE][COLLIDER_FUTURE] = false;
	matrix[COLLIDER_PLANE][COLLIDER_WALL] = false;
	matrix[COLLIDER_PLANE][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_PLANE][COLLIDER_NEXT_LEVEL] = false;
	matrix[COLLIDER_PLANE][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_PLANE][COLLIDER_LAVA] = false;
	matrix[COLLIDER_PLANE][COLLIDER_SLOPE_LEFT] = false;
	matrix[COLLIDER_PLANE][COLLIDER_SLOPE_RIGHT] = false;
	matrix[COLLIDER_PLANE][COLLIDER_PLANE] = false;
	matrix[COLLIDER_PLANE][COLLIDER_COIN] = false;

	matrix[COLLIDER_COIN][COLLIDER_FUTURE] = false;
	matrix[COLLIDER_COIN][COLLIDER_WALL] = false;
	matrix[COLLIDER_COIN][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_COIN][COLLIDER_NEXT_LEVEL] = false;
	matrix[COLLIDER_COIN][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_COIN][COLLIDER_LAVA] = false;
	matrix[COLLIDER_COIN][COLLIDER_SLOPE_LEFT] = false;
	matrix[COLLIDER_COIN][COLLIDER_SLOPE_RIGHT] = false;
	matrix[COLLIDER_COIN][COLLIDER_PLANE] = false;
	matrix[COLLIDER_COIN][COLLIDER_COIN] = false;



	name.create("collision");
}

// Destructor
j1Collision::~j1Collision()
{}

bool j1Collision::Awake(pugi::xml_node& node)
{
	debug = node.child("debug").attribute("value").as_bool();
	return true;
}

bool j1Collision::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate_Collision", Profiler::Color::Snow);

	// Remove all colliders scheduled for deletion
	for(uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if(colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

// Called before render is available
bool j1Collision::Update(float dt)
{
	BROFILER_CATEGORY("Update_Collision", Profiler::Color::Snow);

	Collider* c1;
	Collider* c2;

	for(uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if(colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for(uint k = i+1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if(colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if(c1->CheckCollision(c2->rect) == true)
			{
				if(matrix[c1->type][c2->type] && c1->callback) 
					c1->callback->OnCollision(c1, c2);
				
				if(matrix[c2->type][c1->type] && c2->callback) 
					c2->callback->OnCollision(c2, c1);
			}
		}
	}

	return true;
}

bool j1Collision::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostUpdate_Collision", Profiler::Color::Snow);

	DebugDraw();
	return true;
}

void j1Collision::DebugDraw()
{
	BROFILER_CATEGORY("DebugDraw_Collision", Profiler::Color::Snow);

	if(App->input->GetKey(SDL_SCANCODE_F9)== KEY_DOWN)
		debug = !debug;

	if(debug == false)
		return;

	Uint8 alpha = 80;
	for(uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if(colliders[i] == nullptr)
			continue;
		
		switch(colliders[i]->type)
		{
			case COLLIDER_NONE: // white
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
			case COLLIDER_WALL: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
			case COLLIDER_PLAYER: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
			case COLLIDER_LAVA: // red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
			case COLLIDER_ENEMY: // red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
			case COLLIDER_FUTURE: // yellow
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
			break;
			case COLLIDER_SLOPE_LEFT:
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 255, alpha);
			break;
			case COLLIDER_SLOPE_RIGHT:
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 255, alpha);
			break;
			case COLLIDER_NEXT_LEVEL:
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
			break;
			case COLLIDER_PLANE: // yellow
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
			break;
			case COLLIDER_COIN:
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
			break;
		}
	}
}

// Called before quitting
bool j1Collision::CleanUp()
{
	BROFILER_CATEGORY("CleanUp_Collision", Profiler::Color::Snow);

	LOG("Freeing all colliders");

	for(uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if(colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	BROFILER_CATEGORY("AddCollider_Collision", Profiler::Color::Snow);

	Collider* ret = nullptr;

	for(uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if(colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}

	return ret;
}

// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	BROFILER_CATEGORY("CheckCollision_Collision", Profiler::Color::Snow);

	return (rect.x < r.x + r.w &&
			rect.x + rect.w > r.x &&
			rect.y < r.y + r.h &&
			rect.h + rect.y > r.y);
}