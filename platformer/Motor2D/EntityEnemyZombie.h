#ifndef __EntityEnemyZombie_H__
#define __EntityEnemyZombie_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "EntityInteractive.h"
struct object;
struct SDL_Texture;
struct Collider;

class EntityEnemyZombie : public EntityInteractive
{
public:
	EntityEnemyZombie();
	~EntityEnemyZombie() {};

	void Start();
	bool Update(float dt, bool logic);
	void OnCollision(Collider* c1, Collider* c2);

	void Awake();

	void Draw();

	void InitialPos()
	{
		position.x = 0;
		position.y = 0;
	}

public:

	Animation idle;
	Animation left;
	Animation right;

	//default values are added if everything that sets em up is going horrible
	int lifes = 1;
	int width = 200;
	int height = 200;
	float acceleration = 0.1;
	float max_speed = 1;
	float gravity = 0.0;

	int initial_x = 0;
	int initial_y = 0;

	p2DynArray<iPoint> path;
	iPoint step;
	iPoint player_center;

	iPoint original_pos;
};

#endif*/