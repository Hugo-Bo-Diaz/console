#ifndef __J1ENTITY_H__
#define __J1ENTITY_H__

#include "j1App.h"
#include "p2Point.h"
#include "j1Textures.h"
#include "Animation.h"
#include "p2List.h"
#include "j1Physics.h"


struct entity_property
{
	float value;
	p2SString name;
	int type;
};

enum ENTITY_TYPE
{
	BACKGROUND,
	BAT,
	PLAYER,
	ZOMBIE,
	PLANE,
	COIN,
	NULLLL
};

class Entity
{
public:

	p2SString	name;

	iPoint position;
	bool interactive;
	ENTITY_TYPE type;
	Animation* current_animation = nullptr;
	object* obj = nullptr;

public:
	
	Entity() {};
	virtual ~Entity() {

	};
	
	void SetPos(iPoint pos) 
	{ 
		position = pos; 
	};


	virtual void Draw() {};
	ENTITY_TYPE Get_type()
	{
		return type;
	}
	virtual bool PreUpdate(float dt) { return true; };
	virtual bool Update(float dt, bool logic) { return true; };
	virtual bool PostUpdate(float dt) { return true; };

	virtual void Awake() {};
	virtual void Start() {};

	virtual void OnCollision(Collider* c1, Collider* c2) {};
	
	virtual void CleanUp() { obj = nullptr; };
	
	bool destroyed;

};
#endif // !__J1ENTITY_H__
