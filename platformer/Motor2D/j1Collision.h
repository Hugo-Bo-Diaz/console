#ifndef __j1Collision_H__
#define __j1Collision_H__

#define MAX_COLLIDERS 500

#include "j1Module.h"
#include "SDL/include/SDL.h"


enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,

	COLLIDER_WALL,
	COLLIDER_SLOPE_LEFT,
	COLLIDER_SLOPE_RIGHT,

	COLLIDER_PLAYER,
	COLLIDER_ENEMY,
	COLLIDER_FUTURE,

	COLLIDER_NEXT_LEVEL,
	COLLIDER_LAVA,

	COLLIDER_PLANE,

	COLLIDER_COIN,

	COLLIDER_MAX
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Module* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const;
};

class j1Collision : public j1Module
{
public:

	j1Collision();
	~j1Collision();

	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	bool Awake(pugi::xml_node&);

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr);
	void DebugDraw();
	bool debug = true;

private:

	Collider* colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
};

#endif // __ModuleCollision_H__