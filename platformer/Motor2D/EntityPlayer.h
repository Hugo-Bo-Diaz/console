#ifndef __EntityPlayer_H__
#define __EntityPlayer_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "EntityInteractive.h"
struct object;
struct SDL_Texture;
struct Collider;

class EntityPlayer : public EntityInteractive
{
public:
	EntityPlayer();
	~EntityPlayer() {};

	void Start();
	bool PreUpdate(float dt);
	bool Update(float dt,bool logic);
	void OnCollision(Collider* c1, Collider* c2);
	void Awake();

	void SetPosOrigin();// sets player to the start of the map
	
	void Draw();

	void PlayerDeath();

	void InitialPos()
	{
		position.x = 0;
		position.y = 0;
	}

public:

	Animation idle;
	Animation left;
	Animation right;
	Animation airborneRight;
	Animation airborneLeft;
	Animation jumpsquatRight;
	Animation jumpsquatLeft;

	//default values are added if everything that sets em up is going horrible
	int lifes = 3;
	int width = 200;
	int height = 200;
	float jump_speed = -5;
	float acceleration = 0.1;
	float max_speed = 1;
	float gravity = 0.01;

	int initial_x = 0;
	int initial_y = 0;

	float hability = 0.5;
	float hability_stored;
	// if this is true, the player will change its position to the original one at the start of the update
	bool set_to_start_pos = false;
	j1Timer death;
	bool death_timer_on = false;

	float normalize_factor;
	bool ended = false;
};
#endif