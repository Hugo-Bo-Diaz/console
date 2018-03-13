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
	if (element == input)
	{
		UITextbox* textbox = (UITextbox*)element;
		p2SString new_string = textbox->text;
		LOG(new_string.GetString());
		logs.add(new_string);

		//so fukin dirty :V
		App->gui->delete_element(textbox);
		input = (UITextbox*)App->gui->GUIAdd_textbox(0, App->win->screen_surface->h / 2, this);
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