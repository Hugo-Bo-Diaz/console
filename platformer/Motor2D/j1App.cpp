#include <iostream> 

#include "p2Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1App.h"
#include "j1Collision.h"
#include "j1Physics.h"
#include "j1EntityManager.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "j1Transition.h"
#include "j1Console.h"

#include "Brofiler\Brofiler.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(ptimer);


	frames = 0;
	want_to_save = want_to_load = false;

	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	scene = new j1Scene();
	map = new j1Map();

	console = new j1Console();

	transition = new j1Transition();
	gui = new j1Gui();
	font = new j1Fonts();
	path = new j1Pathfinding();
	entities = new j1EntityManager();
	collision= new j1Collision();
	physics = new j1Physics();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp //needed on pause
	AddModule(input);//1 true
	AddModule(win);//2 true
	AddModule(tex);//3 true
	AddModule(audio);//4 true
	AddModule(scene);//6 true



	AddModule(gui);//7 true
	AddModule(font);//8 true
	AddModule(path);//9 true
	AddModule(collision);//10 false
	AddModule(map);//5 true
	AddModule(entities);//11 false
	AddModule(physics);//12 false

	AddModule(console);
	AddModule(transition);//last to overcome all the other blits

	// render last to swap buffer
	AddModule(render);//13

	PERF_PEEK(ptimer);

}

// Destructor
j1App::~j1App()
{
	// release modules
	p2List_item<j1Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.add(module);
}

// Called before render is available
bool j1App::Awake()
{
	PERF_START(ptimer);

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	bool ret = false;
		
	config = LoadConfig(config_file);

	if(config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title.create(app_config.child("title").child_value());
		organization.create(app_config.child("organization").child_value());

		cnfgframe_cap = app_config.attribute("framerate_cap").as_uint(100);
		frame_cap = cnfgframe_cap;
	}
	save_game = "save_game.xml";
	load_game = "save_game.xml";

	if(ret == true)
	{
		p2List_item<j1Module*>* item;
		item = modules.start;

		while(item != NULL && ret == true)
		{
			ret = item->data->Awake(config.child(item->data->name.GetString()));
			item = item->next;
		}
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	PERF_START(ptimer);

	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}
	startup_time.Start();

	PERF_PEEK(ptimer);

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	BROFILER_CATEGORY("Update", Profiler::Color::LimeGreen);

	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	BROFILER_CATEGORY("PrepareUpdate", Profiler::Color::Turquoise);
	frame_count++;
	last_sec_frame_count++;

	float frame_time_read = frame_time.Read();
	dt = frame_time_read;

	if (frame_cap != 0 && dt > 1000 / App->frame_cap )
	{
		dt = 1000 / App->frame_cap;
	}

	frame_time.Start();
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	BROFILER_CATEGORY("FinishUpdate", Profiler::Color::Sienna);

	if (want_to_pause == true)
		PauseGameNow();

	if (want_to_resume == true)
		ResumeGameNow();

	if (App->map->change_to_next_level)
	{
		App->map->next_level();
		App->map->change_to_next_level = false;
	}

	if (App->map->change_to_previous_level)
	{
		App->map->previous_level();
		App->map->change_to_previous_level = false;
	}
	if (App->map->change_to_this_level != -1)
	{
		App->map->change_map(App->map->change_to_this_level);
		App->map->change_to_this_level = -1;
	}

	if (want_to_save == true)
		SavegameNow();

	if (want_to_load == true||first)
		LoadGameNow(first);


	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;


	static char title[256];
	sprintf_s(title, 256, "Av.FPS: %.2f Last Frame Ms: %02u Last sec frames: %i  Time since startup: %.3f Frame Count: %lu ",
		avg_fps, last_frame_ms, frames_on_last_update, seconds_since_startup, frame_count);
	App->win->SetTitle(title);

	// TODO 2: Use SDL_Delay to make sure you get your capped framerate

	j1PerfTimer* time_stopped = new j1PerfTimer();
	if (frame_cap != 0)
	{
		int framecapminuslastframesms = 1000 / frame_cap - last_frame_ms;

		if (framecapminuslastframesms > 0)
		{
			SDL_Delay(framecapminuslastframesms);
		}
	}
	// TODO3: Measure accurately the amount of time it SDL_Delay actually waits compared to what was expected
	int print = time_stopped->ReadMs();
//	LOG("we waited for %d ms", print);
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	BROFILER_CATEGORY("PreUpdate", Profiler::Color::DarkBlue);

	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PreUpdate(dt);
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	BROFILER_CATEGORY("DoUpdate", Profiler::Color::Violet);

	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	BROFILER_CATEGORY("PostUpdate", Profiler::Color::Orchid);

	bool ret = true;
	p2List_item<j1Module*>* item;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PostUpdate(dt);
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	PERF_START(ptimer);

	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}
	PERF_PEEK(ptimer);

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.GetString();
}

// Load / Save
void j1App::LoadGame()
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
}

// ---------------------------------------
void j1App::SaveGame() const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	want_to_save = true;
}

// ---------------------------------------

bool j1App::LoadGameNow(bool first_time)
{
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.load_file(load_game.GetString());

	if(result != NULL)
	{
		LOG("Loading new Game State from %s...", load_game.GetString());

		root = data.child("game_state");

		p2List_item<j1Module*>* item = modules.start;
		ret = true;
		if (first_time)
		{
			while (item != NULL && ret == true )
			{
				if (item->data->load_at_start)
					ret = item->data->Load(root.child(item->data->name.GetString()));
				item = item->next;
			}

		}
		else
		{
			while (item != NULL && ret == true)
			{
				ret = item->data->Load(root.child(item->data->name.GetString()));
				item = item->next;
			}
		}
		data.reset();
		if(ret == true)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", (item != NULL) ? item->data->name.GetString() : "unknown");
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", load_game.GetString(), result.description());
	
	first = false;
	want_to_load = false;
	return ret;
}

bool j1App::SavegameNow() const
{
	bool ret = true;

	LOG("Saving Game State to %s...", save_game.GetString());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;
	
	root = data.append_child("game_state");

	p2List_item<j1Module*>* item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Save(root.append_child(item->data->name.GetString()));
		item = item->next;
	}

	if(ret == true)
	{
		data.save_file(save_game.GetString());
		LOG("... finished saving", );
	}
	else
		LOG("Save process halted from an error in module %s", (item != NULL) ? item->data->name.GetString() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
}

void j1App::PauseGame()
{
	want_to_pause = true;
}

void j1App::ResumeGame()
{
	want_to_resume = true;
}

bool j1App::PauseGameNow()
{
	bool ret = true;
	want_to_pause = false;
	
	//call pause_methods
	p2List_item<j1Module*>* item = modules.start;
	while (item != NULL && ret == true)
	{
		ret = item->data->Pause();
		if (!item->data->active_in_pause)
		{
			item->data->active = false;
		}
		item = item->next;
	}
	
	if (ret)
	{paused_game = true;
	LOG("paused game");}
	return ret;
}

bool j1App::ResumeGameNow()
{
	bool ret = true;
	want_to_resume = false;

	//call resume_methods
	p2List_item<j1Module*>* item = modules.start;
	while (item != NULL && ret == true)
	{
		ret = item->data->Resume();
		item->data->active = true;
		item = item->next;
	}

	if (ret)
	{paused_game = false;
	LOG("resumed game");}
	return ret;
}

