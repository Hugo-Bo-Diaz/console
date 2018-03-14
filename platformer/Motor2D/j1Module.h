// ----------------------------------------------------
// j1Module.h
// Interface for all engine modules
// ----------------------------------------------------

#ifndef __j1MODULE_H__
#define __j1MODULE_H__

#include "p2SString.h"
#include "PugiXml\src\pugixml.hpp"

class j1App;
struct Collider;
class UIelement;

class j1Module
{
public:

	j1Module() : active(false)
	{}

	virtual ~j1Module() {};

	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate(float dt)
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

	bool IsEnabled() const { return enabled; }

	void Enable()
	{
		if (enabled == false)
			enabled = true;
	}

	void Disable()
	{
		if (enabled == true)
			enabled = false;
	}

	virtual void OnCollision(Collider*, Collider*) {};

	virtual bool Pause()
	{
		return true;
	};

	virtual bool Resume()
	{
		return true;
	};

	virtual bool UIinteraction(UIelement* element)
	{
		return true;
	}
	
	virtual bool Console_Interaction(int function, int* array_of_parameters = nullptr)
	{
		return true;
	}

public:

	p2SString	name;
	bool		active;
	bool active_in_pause = true;
	bool load_at_start = false;

private:
	bool enabled = true;
};

#endif // __j1MODULE_H__