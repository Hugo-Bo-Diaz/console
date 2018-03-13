#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Physics.h"
#include "j1Transition.h"
#include "Brofiler\Brofiler.h"
#include "UIwindow.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
	load_at_start = true;
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	cnfg_lifes = config.child("starting_lifes").attribute("value").as_int(100);

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	BROFILER_CATEGORY("Start_Scene", Profiler::Color::LawnGreen);

	App->map->Load(App->map->current_map.GetString());

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate_Scene", Profiler::Color::LawnGreen);

	time_left = App->map->time_left();

	if (time_left < 0 && !App->map->timer.paused)
	{
		//LOG("TIME'S UP BIACH");
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		App->map->change_to_this_level = App->map->index_map;
	
	if (lifes <= 0)
	{
		LOG("LOL U BAD");
		final_score = score;
		score = 0;
		coins = 0;
		lifes = cnfg_lifes;
		
		App->map->change_to_this_level = App->map->maps.count()-1;//this goes to the last level(the highscores)
		App->transition->StartTransition();
	}

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Update_Scene", Profiler::Color::LawnGreen);

	if (App->map->player != nullptr)
	{
		if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{
			godmode = !godmode;
			if (godmode)
			{
				App->map->player->hability = 7;
			}
			else
			{
				App->map->player->hability = App->map->player->hability_stored;
			}
		}

		uint win;
		uint i;
		App->win->GetWindowSize(win, i);
		if (App->map->player != nullptr)
		{
			App->render->camera.x = -App->map->player->position.x + win / 2;
			if (App->render->camera.x < win / 2)
			{
				App->render->camera.x = 0;
			}
			if (-App->render->camera.x > ((App->map->data.width*App->map->data.tile_width) - win))
			{
				App->render->camera.x = -(App->map->data.width*App->map->data.tile_width) + win;
			}
		}
	}
	//DEBUG FEATURES
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		App->map->change_to_this_level = 0;

	if(App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->LoadGame();

	if(App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame();

	if (App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_DOWN)
		App->audio->RiseVolume();

	if (App->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_DOWN)
		App->audio->LowerVolume();

	//App->render->Blit(img, 0, 0);
	if (!App->paused_game)
		App->map->Draw();

	/*int _x = 0;
	int _y = 0;

	App->input->GetMousePosition(_x, _y);
	_x = (-App->render->camera.x + _x) / App->map->data.tile_width;
	_y = (-App->render->camera.y + _y) / App->map->data.tile_height;

	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile x mouse:%d Tile y mouse:%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),_x,_y);*/
	//p2SString title("Where's my plane?");

	//App->win->SetTitle(title.GetString());

	if (App->paused_game && pause_background != nullptr )
	{
		uint x; uint y;
		App->win->GetWindowSize(x, y);
		App->render->Blit(pause_background,-App->render->camera.x,-App->render->camera.y);
		App->render->DrawQuad({ -App->render->camera.x,-App->render->camera.y,(int)x,(int)y }, 0, 0, 0, 50);
	}
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostUpdate_Scene", Profiler::Color::LawnGreen);

	bool ret = true;
	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		bool exit = false;
		if (App->frame_cap == App->cnfgframe_cap && !exit)
		{
			App->frame_cap = 0;
			exit = true;
		}
		if (App->frame_cap == 0 && !exit)
		{
			App->frame_cap = App->cnfgframe_cap;
			exit = true;
		}
	}

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	BROFILER_CATEGORY("CleanUp_Scene", Profiler::Color::LawnGreen);

	LOG("Freeing scene");

	return true;
}

bool j1Scene::Save(pugi::xml_node& node) const
{
	BROFILER_CATEGORY("Save_Scene", Profiler::Color::DarkOliveGreen);
	
	node.append_child("coins").append_attribute("value") = coins;
	node.append_child("lifes").append_attribute("value") = lifes;
	node.append_child("score").append_attribute("value") = score;

	pugi::xml_node highscores_node = node.append_child("highscores");
	p2List_item<HighScore*>* item = highscores.start;
	while (item != NULL)
	{
		pugi::xml_node entry_node = highscores_node.append_child("entry");
		entry_node.append_attribute("name") = item->data->name.GetString();
		entry_node.append_attribute("score") = item->data->score;
		item = item->next;
	}

	return true;
}

bool j1Scene::Load(pugi::xml_node& node)
{
	BROFILER_CATEGORY("Load_Scene", Profiler::Color::DarkOliveGreen);

	//clear highscores
	p2List_item<HighScore*>* item = highscores.start;
	while (item != NULL)
	{
		delete item->data;
		item = item->next;
	}
	highscores.clear();

	bool ret = true;
	if (!App->first)
	{
		score = node.child("score").attribute("value").as_int();
		coins = node.child("coins").attribute("value").as_int();
		lifes = node.child("lifes").attribute("value").as_int();
	}
	pugi::xml_node highscores_node = node.child("highscores");
	pugi::xml_node highscore;

	for (highscore = highscores_node.child("entry"); highscore && ret; highscore = highscore.next_sibling("entry"))
	{
		HighScore* ret = new HighScore;
		ret->name = highscore.attribute("name").as_string();
		ret->score = highscore.attribute("score").as_int();
		highscores.add(ret);
	}
	return ret;
}


bool j1Scene::UIinteraction(UIelement* element)
{
	bool ret = true;
	if (element->type_of_element == BUTTON)
	{
		UIButton* button = (UIButton*)element;
		switch (button->type)
		{
		case CONTINUE:
		{App->ResumeGame();
		break; }
		case SETTINGS:
		{
			Settings_window = (UIwindow*)App->gui->GUIAdd_window(400, 150, { 494,0,210,194 }, "SETTINGS", true);
			Settings_window->Attach(App->gui->GUIAdd_slider(0, 0, { 0,0,200,45 }, { 207,116,25,43 }, { 251,116,25,43 }, { 232,134,18,9 }, { 185,112,19,42 }, 128, &App->audio->volume, "Music volume"), { 5, 60 });
			Settings_window->Attach(App->gui->GUIAdd_slider(0, 0, { 0,0,200,45 }, { 207,116,25,43 }, { 251,116,25,43 }, { 232,134,18,9 }, { 185,112,19,42 }, 128, &App->audio->fx_volume, "Sound volume"), { 5, 140 });
			LOG("WHO PUT THIS HERE?");
			break; }
		case MAIN_MENU:
		{
			App->map->change_to_this_level = 0;
			App->SaveGame();
			break; }
		case SAVE_GAME:
		{
			App->ResumeGame();
			App->SaveGame();
			break;
		}
		case LOAD_GAME:
		{
			App->ResumeGame();
			App->LoadGame();
			break;
		}
		case EXIT:
		{
			if (App->transition->StartTransition())
			{
				App->map->change_to_this_level = 0;
				App->ResumeGame();
			}
			break; }
		default:
		{LOG("ERROR");
		break; }
		}
	}
	if (element->type_of_element == TEXTBOX && element == name_highscore)//this is the submit score function may change it to some separate thing
	{
		if (final_score != 0)
		{
			UITextbox* textbox = (UITextbox*)element;
			HighScore* new_player = new HighScore;
			new_player->name = textbox->text;
			new_player->score = final_score;
			final_score = 0;
			highscores.add(new_player);

			//now we reload the level again so that the highscores are displayed correctly
			App->map->change_to_this_level = App->map->index_map;
			BubbleSort_highscore();
		}
	}
	return ret;
}

bool j1Scene::Pause()
{
	/*Continue = (UIButton*)App->gui->GUIAdd_button(200, 100, { 120, 163, 117, 52 },this,"CONTINUE",CONTINUE);//{1,143,143,71}
	Exit = (UIButton*)App->gui->GUIAdd_button(200, 200, { 0, 0, 117, 52 },this,"EXIT", EXIT);//{144,143,143,71}*/
	Pause_window= (UIwindow*)App->gui->GUIAdd_window(-App->render->camera.x + 200, 100, { 282,0,210,300 }, "PAUSE");
	Continue = (UIButton*)App->gui->GUIAdd_button(0,0, {0,0,143,71}, { 144,1,133,71 }, { 0,74,171,99 },this,"CONTINUE",CONTINUE);
	Settings = (UIButton*)App->gui->GUIAdd_button(0,0, { 0,0,143,71 }, { 144,1,133,71 }, { 0,74,171,99 }, this, "SETTINGS", SETTINGS);
	Exit = (UIButton*)App->gui->GUIAdd_button(0,0, { 0,0,143,71 }, { 144,1,133,71 }, { 0,74,171,99 }, this, "EXIT", EXIT);
	
	Save_button = (UIButton*)App->gui->GUIAdd_button(0, 0, { 0,0,143,71 }, { 144,1,133,71 }, { 0,74,171,99 }, this, "SAVE", SAVE_GAME);
	Load_button= (UIButton*)App->gui->GUIAdd_button(0, 0, { 0,0,143,71 }, { 144,1,133,71 }, { 0,74,171,99 }, this, "LOAD", LOAD_GAME);
	
	Pause_window->Attach(Save_button, {200,117});
	Pause_window->Attach(Load_button, {200,200});

	Pause_window->Attach(Continue, {35,35});
	Pause_window->Attach(Settings, { 35,117 });
	Pause_window->Attach(Exit, { 35,200 });

	SDL_Surface *sshot = SDL_CreateRGBSurface(0, 750, 420, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(App->render->renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
	pause_background = App->tex->LoadSurface(sshot);
	SDL_FreeSurface(sshot);
	sshot = nullptr;

	return true;
}
bool j1Scene::Resume()
{
	if (Continue != nullptr)
	{
		App->gui->delete_element(Continue);
		Continue = nullptr;
	}
	if (Exit != nullptr)
	{
		App->gui->delete_element(Exit);
		Exit = nullptr;
	}
	if (Settings != nullptr)
	{
		App->gui->delete_element(Settings);
		Settings = nullptr;
	}

	if (Save_button != nullptr)
	{
		App->gui->delete_element(Save_button);
		Save_button = nullptr;
	}
	if (Load_button != nullptr)
	{
		App->gui->delete_element(Load_button);
		Load_button = nullptr;
	}

	if (Pause_window != nullptr)
	{
		App->gui->delete_element(Pause_window);
		Pause_window = nullptr;
	}

	if (pause_background != nullptr)
	{
		App->tex->UnLoad(pause_background);
		pause_background = nullptr;
	}

	return true;
}

void j1Scene::BubbleSort_highscore()
{

	p2List_item<HighScore*>* item = highscores.start;
	bool swapped = true;
	while (swapped)
	{
		swapped = false;
		while (item != NULL)
		{
			if (item->next != NULL && item->data->score < item->next->data->score)//error in the order
			{
				int tmp_int = item->data->score;
				p2SString tmp_str = item->data->name;

				item->data->score = item->next->data->score;
				item->data->name = item->next->data->name;

				item->next->data->score = tmp_int;
				item->next->data->name = tmp_str;

				swapped = true;
			}
			if (item->next != NULL)
			{
				item = item->next;
			}
			else
			{
				break;
			}
		}
	}



}