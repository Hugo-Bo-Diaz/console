#include "j1Physics.h"
#include "j1Collision.h"
#include "j1App.h"
#include "p2Log.h"
#include "Brofiler\Brofiler.h"

//delete this include
#include "j1Render.h"


j1Physics::j1Physics()
{	
	name.create("physics");
	active_in_pause = false;
	//here we initialise all object to nullptr
	for (uint i = 0; i < MAX_OBJECTS; ++i)
		objects[i] = nullptr;
}

bool j1Physics::Awake(pugi::xml_node* config)
{
	/*objects = new object[MAX_OBJECTS];

	memset(objects, 0, sizeof(object)*(MAX_OBJECTS));*/

	return true;
}

object* j1Physics::Addobject(float _x, float _y, float _gravity, SDL_Rect* collision,COLLIDER_TYPE TYPE, j1Module* Callback)
{
	object* ret = nullptr;
	//looks free object slot
	for (uint i = 0; i < MAX_OBJECTS; ++i)
	{
		if (objects[i] == nullptr)
		{
			ret = objects[i] = new object;
			break;
		}
	}
	//assign all the object properties
	ret->position.x = _x;
	ret->position.y = _y;
	ret->acceleration.x = 0;
	ret->acceleration.y = _gravity;


	ret->col = App->collision->AddCollider(*collision, TYPE, Callback);
	ret->predictor = App->collision->AddCollider(*collision, COLLIDER_FUTURE, this);

	ret->velocity.x = 0;
	ret->velocity.y = 0;

	return ret;

}

bool j1Physics::CleanUp()
{
	BROFILER_CATEGORY("CleanUp_Physics", Profiler::Color::SlateGray);

	LOG("Freeing all objects");

	for (uint i = 0; i < MAX_OBJECTS; ++i)
	{
		if (objects[i] != nullptr)
		{
			if (objects[i]->col != nullptr)
				objects[i]->col->to_delete = true;
			if (objects[i]->predictor != nullptr)
				objects[i]->predictor->to_delete = true;
			delete(objects[i]);
			objects[i] = nullptr;
		}
	}

	return true;
}

bool j1Physics::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate_Physics", Profiler::Color::SlateGray);

	//advance objects
	float thousanddivdt = 1000 / dt;// ~ frame RATE

	float normalize_factor = 60/ thousanddivdt;//60 is the ideal framerate for the given speeds	

	if (normalize_factor < 1) { normalize_factor = 1; }

	/*if (dt > 1000 / App->frame_cap)
	{
		normalize_factor = 1000 / (App->frame_cap / 2);
	}*/

	for (uint i = 0; i < MAX_OBJECTS; ++i)
	{
		if (objects[i] != nullptr)
		{
			objects[i]->velocity.x += objects[i]->acceleration.x * normalize_factor;
			objects[i]->velocity.y += objects[i]->acceleration.y * normalize_factor;

			objects[i]->predictor->SetPos(objects[i]->position.x + objects[i]->velocity.x * normalize_factor,
			objects[i]->position.y + objects[i]->velocity.y * normalize_factor);
		}
	}
	//we set up the colliders that will check the collisions in the near future
	for (uint i = 0; i < MAX_OBJECTS; ++i)
	{
		if (objects[i] != nullptr)
		{
			float change_x = objects[i]->velocity.x * normalize_factor + objects[i]->acceleration.x * normalize_factor;
			float change_y = objects[i]->velocity.y * normalize_factor + objects[i]->acceleration.y * normalize_factor;
			//LOG("%f %f", change_x, change_y);
			objects[i]->predictor->SetPos(objects[i]->position.x + change_x,
				objects[i]->position.y + change_y);
		}
	}
	return true;
}



void j1Physics::OnCollision(Collider* c1,Collider*c2)
{
	BROFILER_CATEGORY("OnCollision_Physics", Profiler::Color::SlateGray);

	//this method wil be called on an earlier update method(module collisions)
	if (c1->type == COLLIDER_FUTURE && c2->type == COLLIDER_WALL)
	{
		SDL_Rect result;//this is the rectangle that they share when they are colliding
						//from here we know how much they are colliding and know how much to push back these collisions
		SDL_bool _bool = SDL_IntersectRect(&c1->rect,&c2->rect, &result);
		if (_bool)//this means they collided 4 real
		{
			//logic operations
			object* obj= GetObjectFromRect_predictor(&c1->rect);
			 if (result.h == result.w && obj->grounded == false)
			{
				if (c1->rect.y < c2->rect.y)
				 {
					 if (c1->rect.x < c2->rect.x)
					 {
						 obj->predictor->rect.x -= result.w;
						 obj->predictor->rect.y -= result.h;
					 }
					 else
					 {
						 obj->predictor->rect.x += result.w;
						 obj->predictor->rect.y -= result.h;
					 }
				 }
				else
				{
					if (c1->rect.x <c2->rect.x)
					{
						obj->predictor->rect.x -= result.w;
						obj->predictor->rect.y += result.h;
					}
					else
					{
						obj->predictor->rect.x += result.w;
						obj->predictor->rect.y += result.h;
					}
				}
				obj->velocity.x = 0;
			}
			else if (result.h > result.w)
			{
				if (c1->rect.x <c2->rect.x)
				{obj->predictor->rect.x -= result.w;
				}
				else
				{obj->predictor->rect.x += result.w;
				}
				obj->velocity.x = 0;
			}
			else if (result.h  <= result.w)
			{
				if (c1->rect.y < c2->rect.y)
				{obj->predictor->rect.y -= result.h;
				obj->grounded = true;}
				else
				{obj->predictor->rect.y += result.h;
				}
				obj->velocity.y = 0;
			}

		}
	}
	//I have the starting x of the slope and can calculate where he should be with that
	if (c1->type == COLLIDER_FUTURE && c2->type == COLLIDER_SLOPE_LEFT)
	{
		SDL_Rect result;//this is the rectangle that they share when they are colliding
						//from here we know how much they are colliding and know how much to push back these collisions
		object* obj = GetObjectFromRect_predictor(&c1->rect);
		SDL_Rect c3;
		c3.x = c2->rect.x;
		c3.w = c2->rect.w;
		c3.y = (c2->rect.y + c2->rect.h) - (c1->rect.x+(0.6*c1->rect.w) - c2->rect.x) -5;
		c3.h = c2->rect.h;

		if (c3.y < c2->rect.y)// this makes it more consistent at the peak of the slope so the player is not moved over the floor
		{
			c3.y = c2->rect.y;
		}
		//DEBUG PURPOSES
		if (App->collision->debug)
		App->render->DrawQuad(c3, 0, 255, 255,80);

		SDL_bool _bool = SDL_IntersectRect(&c1->rect, &c3, &result);
		if (_bool)//this means they collided 4 real
		{
			//logic operations
			obj->predictor->rect.y -= result.h;
			obj->grounded = true;
			obj->velocity.y = 0;
		}
	}

	if (c1->type == COLLIDER_FUTURE && c2->type == COLLIDER_SLOPE_RIGHT)
	{
		SDL_Rect result;//this is the rectangle that they share when they are colliding
						//from here we know how much they are colliding and know how much to push back these collisions
		object* obj = GetObjectFromRect_predictor(&c1->rect);
		SDL_Rect c3;
		c3.x = c2->rect.x;
		c3.w = c2->rect.w;
		c3.y = c2->rect.y + (c1->rect.x + (0.2*c1->rect.w) - c2->rect.x)-6;//this -7 is just a little help so that its easier to get to the top
		c3.h = c2->rect.h;
		
		if (c3.y < c2->rect.y)
		{
			c3.y = c2->rect.y;
		}
		//DEBUG PURPOSES
		if (App->collision->debug)
		App->render->DrawQuad(c3, 0, 255, 255, 80);

		SDL_bool _bool = SDL_IntersectRect(&c1->rect, &c3, &result);
		if (_bool)//this means they collided 4 real
		{
			//logic operations
			obj->predictor->rect.y -= result.h;
			obj->grounded = true;
			obj->velocity.y = 0;
		}
	}

};

bool j1Physics::PostUpdate(float dt) 
{
	BROFILER_CATEGORY("PostUpdate_Physics", Profiler::Color::SlateGray);

	// here is where the colliders positions are adjusted to where they should be
	for (uint i = 0; i < MAX_OBJECTS; ++i)
	{
		if (objects[i] != nullptr)
		{
			//LOG("%f", objects[i]->velocity.x);
			objects[i]->position.x = objects[i]->predictor->rect.x;
			objects[i]->position.y = objects[i]->predictor->rect.y;
			objects[i]->col->SetPos(objects[i]->position.x, objects[i]->position.y);
		}
	}
	return true;
}

//this is to know whose collider was called from the OnCollision method
object* j1Physics::GetObjectFromRect_predictor(SDL_Rect* rectangle)
{
	for (uint i = 0; i < MAX_OBJECTS; ++i)
	{
		if (objects[i] != nullptr)
		{
			if (objects[i]->predictor->rect.x == rectangle->x&&
				objects[i]->predictor->rect.y == rectangle->y&&
				objects[i]->predictor->rect.w == rectangle->w&&
				objects[i]->predictor->rect.h == rectangle->h)
			{
				return objects[i];
			}
		}
	}
}

void j1Physics::destroy_object(object* obj)
{
	for (uint i = 0; i < MAX_OBJECTS; ++i)
	{
		if (objects[i] == obj)
		{
			if (objects[i]->col != nullptr)
				objects[i]->col->to_delete = true;
			if (objects[i]->predictor != nullptr)
				objects[i]->predictor->to_delete = true;
			delete(objects[i]);
			objects[i] = nullptr;
		}
	}

}

