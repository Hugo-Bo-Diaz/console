#ifndef _J1PLANE_H_
#define _J1PLANE_H_

#include "j1Physics.h"
#include "EntityBackground.h"
#include "j1Collision.h"

class EntityPlane : public EntityBackground
{
public:

	EntityPlane();
	~EntityPlane()
	{};

	void Draw();
	void Start();
	void CleanUp();

	Collider* col = nullptr;
};


#endif // !
