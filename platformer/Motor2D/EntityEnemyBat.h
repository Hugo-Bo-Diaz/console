#ifndef __EntityEnemyBat_H__
#define __EntityEnemyBat_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "EntityInteractive.h"
struct object;
struct SDL_Texture;
struct Collider;

class EntityEnemyBat : public EntityInteractive
{
public:
	EntityEnemyBat();
	~EntityEnemyBat() {};

	void Start();
	bool Update(float dt, bool logic);
	//	bool Update(float dt);
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

	iPoint original_pos;
};

#endif