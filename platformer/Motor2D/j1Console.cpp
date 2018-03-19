#include "j1Console.h"
#include "j1Gui.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Audio.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Fonts.h"
#include "Brofiler\Brofiler.h"

j1Console::j1Console()
{
	name.create("Console");
}

j1Console::~j1Console()
{

};

bool j1Console::Awake(pugi::xml_node& config)
{

	return true;
}

bool j1Console::Start()
{
	function_1 = AddFunction("name1",this,1,2);
	exit = AddFunction("exit", this, 0, 0);
	help = AddFunction("help", this, 0, 0);

	return true;
}

bool j1Console::CleanUp()
{
	BROFILER_CATEGORY("CleanUp_Console", Profiler::Color::DarkOliveGreen);


	return true;
}

bool j1Console::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate_Console", Profiler::Color::DarkOliveGreen);
	
	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
	{
		switch (console_active)
		{
		case false:
		{
			LOG("console on!");
			console_active = true;
			input = (UITextbox*)App->gui->GUIAdd_textbox(0, App->win->screen_surface->h / 2,this);
		}
			break;
		case true:
		{
			LOG("console off!");
			console_active = false;
			App->gui->delete_element((UIelement*)input);
		}
			break;
		}
	}
	return true;
}

bool j1Console::Update(float dt)
{
	BROFILER_CATEGORY("Update_Console", Profiler::Color::DarkOliveGreen);
	

	return true;
}


bool j1Console::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostUpdate_Console", Profiler::Color::DarkOliveGreen);

	if (console_active)
	{
		App->render->DrawQuad({ 0,0,App->win->screen_surface->w ,App->win->screen_surface->h / 2 }, 150, 150, 150, 175);
		
		int iterations = 0;
		p2List_item<p2SString>* item = logs.end;
		while (item != NULL)
		{
			SDL_Texture* text_texture = App->tex->textures.add(App->font->Print(item->data.GetString(), { 255,255,255,255 }, App->font->default))->data;

			App->render->Blit(text_texture, 0, App->win->screen_surface->h / 2 - 15 - 20*iterations);

			App->tex->UnLoad(text_texture);

			++iterations;
			item = item->prev;
		}
	}

	return true;
}

bool j1Console::UIinteraction(UIelement* element)
{
	bool ret = true;
	if (element == input)
	{
		UITextbox* textbox = (UITextbox*)element;
		p2SString new_string = textbox->text;
		LOG(new_string.GetString());
		logs.add(new_string);
		//we gonna need to analyze this string

		std::string maybe;
		maybe = new_string.GetString();

		p2List<std::string> strings;
		std::string first = "";
		strings.add(first);
		//every space in this will be starting a new string
		
		int current_string = 0;

		for (int i = 0; i < new_string.Length(); ++i)
		{
			if (maybe[i] == 32)//space
			{
				std::string next_word = "";
				strings.add(next_word);
				++current_string;
			}
			else
			{
				strings[current_string] += maybe[i];
			}
		}

		//now we check if he inputed a function
		bool found_function = false;
		p2List_item<function*>* item_1 = functions.start;
		while (item_1 != NULL)
		{
			if (item_1->data->name.GetString() == strings[0] )
			{
				found_function = true;
				p2List <int> arguments;
				for (int i = 1; i < strings.count(); ++i)
				{
					arguments.add(std::stoi(strings[i]));
				}
				if (arguments.count() < item_1->data->min_args)
				{
					logs.add("not enough arguments for this function");
				}
				if (arguments.count() > item_1->data->max_args)
				{
					logs.add("too many arguments for this function");
				}
				if (arguments.count() <= item_1->data->max_args && arguments.count() >= item_1->data->min_args)
				{
					ret = item_1->data->callback->Console_Interaction(functions.find(item_1->data), &arguments);
				}
			}
			item_1 = item_1->next;
		}
		if (!found_function)
		{
			logs.add("function not found");
		}
		//so fukin dirty :V
		App->gui->delete_element(textbox);
		input = (UITextbox*)App->gui->GUIAdd_textbox(0, App->win->screen_surface->h / 2, this);
	}

	return ret;
}

int j1Console::AddFunction(const char* name, j1Module* callback, int min_args, int max_args)
{
	function* new_func = new function();

	new_func->name = name;
	new_func->min_args = min_args;
	new_func->max_args = max_args;
	new_func->callback = callback;

	functions.add(new_func);

	return functions.find(new_func);

}

bool j1Console::Console_Interaction(int _function, p2List<int>* arguments)
{
	if (function_1 == _function)
	{
		p2List_item<int>* item = arguments->start;
		while (item != NULL)
		{
			LOG("%d",item->data);
			item = item->next;
		}
		//this means my first_function has been called
		LOG("works");
	}
	if (_function == exit)
	{
		LOG("H E C C can't close this");
		logs.add("too bad can't do this ):");
	}
	if (_function == help)
	{
		logs.add("The avalible commands are the following:");
		p2List_item<function*>* item = functions.start;
		while (item != NULL)
		{
			logs.add(item->data->name);
			LOG("%s", item->data->name.GetString());
			item = item->next;
		}
	}
	return true;
}

bool j1Console::Save(pugi::xml_node& node) const
{
	BROFILER_CATEGORY("Save_Console", Profiler::Color::DarkOliveGreen);


	return true;
}

bool j1Console::Load(pugi::xml_node& node)
{
	BROFILER_CATEGORY("Load_Console", Profiler::Color::DarkOliveGreen);
	
	return true;
}