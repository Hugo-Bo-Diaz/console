#ifndef _J1INTERACTIVE_H_
#define _J1INTERACTIVE_H_

#include "j1Physics.h"
#include "Entity.h"

class EntityInteractive : public Entity
{
public:

	EntityInteractive() { interactive = true; };
	EntityInteractive(float _x, float _y, float _gravity, SDL_Rect* collision, COLLIDER_TYPE TYPE)
	{
		obj->acceleration = fPoint(0, _gravity);
		obj->position = fPoint(_x, _y);
		obj->col = App->collision->AddCollider(*collision,TYPE,nullptr);
		interactive = true;
	};
	~EntityInteractive()
	{
	
	};

	virtual void Draw() {};
	virtual bool PreUpdate() { return true; };
	virtual bool Update(float dt, bool logic) { return true; };
	virtual bool PostUpdate() { return true; };
	virtual void Awake() {};
	virtual void Start() {};

	virtual void CleanUp() {};


	virtual void OnCollision(Collider* c1, Collider* c2) {};

};


#endif // !
