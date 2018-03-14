#ifndef __J1CONSOLE_H__
#define __J1CONSOLE_H__

#include "j1Module.h"
#include "p2Point.h"
#include "j1Collision.h"
#include "p2List.h"
#include "UItextbox.h"

struct function
{
	p2SString name;
	j1Module* callback;
	int min_args;
	int max_args;
};

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

	p2List<function*> functions;
	int AddFunction(const char*, j1Module* ,int, int);

	int first_function=0;

	bool Console_Interaction(int function, int* array_of_parameters);

public:
	bool console_active = false;

};
#endif // !__J1ENTITIES_H__
