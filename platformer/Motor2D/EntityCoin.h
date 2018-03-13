#ifndef __EntityCoin_H__
#define __EntityCoin_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "EntityInteractive.h"
struct SDL_Texture;
struct Collider;

class EntityCoin : public EntityInteractive
{
public:
	EntityCoin();
	~EntityCoin() {};

	void Start();
	void OnCollision(Collider* c1, Collider* c2);

	void CleanUp();

	void Awake();

	void Draw();

	Collider* col = nullptr;

public:

	Animation spin;

	//default values are added if everything that sets em up is going horrible
	int width = 25;
	int height = 25;

	//this is the offset they will have from the grid.
	int offset_x = 10;
	int offset_y = 10;
	
};

#endif