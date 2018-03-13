#ifndef _J1BACKGROUND_H_
#define _J1BACKGROUND_H_

#include "j1Physics.h"
#include "Entity.h"

class EntityBackground : public Entity
{
public:

	EntityBackground() {};
	~EntityBackground() {};

	virtual void Draw() {};
	virtual bool Update(float dt, bool logic) { return true; };
	virtual void Start() {};
};


#endif // !
