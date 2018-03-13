#ifndef __J1CONSOLE_H__
#define __J1CONSOLE_H__

#include "j1Module.h"
#include "p2Point.h"
#include "j1Collision.h"
#include "p2List.h"
#include "UItextbox.h"

class j1Console : public j1Module
{
public:
	j1Console();
	~j1Console();

	bool Start();

	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);

	bool Awake(pugi::xml_node& config);

	bool CleanUp();

	bool UIinteraction(UIelement* element);

	bool Save(pugi::xml_node& node)const;
	bool Load(pugi::xml_node& node);

	p2List<p2SString> logs;
	
	UITextbox* input = nullptr;

public:
	bool console_active = false;

};
#endif // !__J1ENTITIES_H__
