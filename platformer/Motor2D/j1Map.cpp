#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Window.h"
#include "j1Collision.h"
#include "j1Physics.h"
#include "j1EntityManager.h"
#include "j1Gui.h"
#include "Brofiler\Brofiler.h"
#include <math.h>

#include "UIwindow.h"

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		data.colliders[i] = nullptr;
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	pugi::xml_node i;
	for (i = config.child("map"); i && ret; i = i.next_sibling("map"))
	{
		p2SString set;

		if (ret == true)
		{
			set = i.attribute("directory").as_string("error");
		}

		maps.add(set);
	}
	index_map = config.child("initial_map").attribute("value").as_uint(0);
	current_map = maps[index_map];

	//background color instead of an image can be configured in config.xml and not adding any layer(image layer) on tiled
	pugi::xml_node colors = config.child("background");
	data.background_color = { (Uint8)colors.attribute("r").as_uint(), (Uint8)colors.attribute("g").as_uint(), (Uint8)colors.attribute("b").as_uint(), 0 };
	App->render->SetBackgroundColor(data.background_color);

	time_left_config = config.child("timer").attribute("value").as_int();

	return ret;
}

void j1Map::Draw()
{
	if(map_loaded == false)
		return;
	//draws the background
	if (data.back != nullptr)
	{
		int iterations_x = 1 + (data.tile_width * data.width - 1) / data.back->width;
		int iterations_y = 1 + (data.tile_height * data.height - 1) / data.back->height;
		for (int i = 0; i < iterations_y; ++i)
		{
			for (int j = 0; j < iterations_x; ++j)
			{
				App->render->Blit(data.back->texture, j*data.back->width - App->render->camera.x * data.back->parallax, i*data.back->height);
			}
		}
	}

	SDL_Rect rect = {0,0,0,0};
	p2List_item<map_layer*>* item;
	item = data.layers.start;
	//draws the layers
		while (item != NULL)
		{	
			if (item->data->logic_layer == false && item->data->UI_layer == false)
			{
			for (int _y = 0; _y < item->data->height; ++_y)
			{
				for (int _x = 0; _x < item->data->width; ++_x)
				{
					iPoint point = MapToWorld(_x, _y);

					App->render->Blit(
						data.tilesets.start->data->texture,
						point.x - App->render->camera.x * item->data->parallax, point.y,
						&data.tilesets.start->data->GetTileRect(item->data->data[item->data->Get(_x, _y)]));
				}
			}
		}			
		item = item->next;
	}
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret;

	ret.x = x / data.tile_width;
	ret.y = y / data.tile_height;

	return ret;
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	ret.x = x * data.tile_width;
	ret.y = y * data.tile_height;

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");
	BROFILER_CATEGORY("CleanUp_Map", Profiler::Color::Sienna);

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		App->tex->UnLoad(item->data->texture);
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers
	
	p2List_item<map_layer*>* item_1;
	item_1 = data.layers.start;

	while (item_1 != NULL)
	{
		RELEASE(item_1->data)
		item_1 = item_1->next;
	}
	data.layers.clear();

	for (int i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (data.colliders[i] != nullptr)
		{
			data.colliders[i]->to_delete = true;
			data.colliders[i] = nullptr;
		}
	}

	App->tex->UnLoad(data.back->texture);
	RELEASE(data.back);

	//sets everything to new to make space
	App->physics->CleanUp();
	App->entities->CleanUp();
	App->gui->CleanUp();
	player = nullptr;
	App->render->camera.x = 0;
	App->render->camera.y = 0;
	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	BROFILER_CATEGORY("Load_Map", Profiler::Color::Sienna);

	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}
	//Load all layers info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		map_layer* set = new map_layer();

		if (ret == true)
		{
			ret = LoadLayer(layer, set);
		}

		data.layers.add(set);
	}
	//Load all background info ----------------------------------------------
	pugi::xml_node backgr = map_file.child("map").child("imagelayer");
	if (backgr)
	{
		background* set = new background();
		LoadBackground(backgr, set);
		data.back = set;
	}

	// Load layer info ----------------------------------------------
	
	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}
		
		p2List_item<map_layer*>* item_layer = data.layers.start;
		while(item_layer != NULL)
		{
			map_layer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
	}

	map_loaded = ret;

	if (coming_from_save)
	{
		App->entities->Load_entites();
		coming_from_save = false;
	}

	//PLAYER UI
	if (index_map != 0 && index_map != maps.count()-1)//we don't need timer on the main menu, needs improvement(especify in tiled??)
	{
		uint winx;
		uint winy;
		App->win->GetWindowSize(winx, winy);

		UIelement*text = App->gui->GUIAdd_text(winx - 80, 10, "TIME", { 0,0,0,255 }, true);
		text->Attach(App->gui->GUIAdd_VarDisplay(0, 0, &App->scene->time_left),{5, 15});

		UIelement* image_1 = App->gui->GUIAdd_image(260, 10, {182,84,24,26},true);
		image_1->Attach(App->gui->GUIAdd_VarDisplay(0, 0, &App->scene->coins), {30,0});

		UIelement* image_2 = App->gui->GUIAdd_image(winx - 240, 7, { 244,80,22,25 }, true);
		image_2->Attach(App->gui->GUIAdd_VarDisplay(0,0, &App->scene->lifes), {30,5});

		UIelement* text_2 = App->gui->GUIAdd_text(10, 10, "SCORE", { 0,0,0,255 }, true);
		text_2->Attach(App->gui->GUIAdd_VarDisplay(0, 0, &App->scene->score), {0,15});

	}

	if (time_left_saved != -1)
	{
		max_time = time_left_saved;
		time_left_saved = -1;
	}
	else
	{
		max_time = time_left_config;
	}

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		pugi::xml_node properties = map.child("properties");
		pugi::xml_node iterator;
		for (iterator = properties.child("property"); iterator; iterator = iterator.next_sibling("property"))
		{
			p2SString str = iterator.attribute("name").as_string();
			p2SString n1 = "music";
			if (str == n1)
			{
				App->audio->PlayMusic(iterator.attribute("value").as_string(""));
			}
		}


		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

//TODO 3: Create the definition for a function that loads a single layer
bool j1Map::LoadLayer(pugi::xml_node& node, map_layer* layer)
{
	bool ret= true;
	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_uint();
	layer->height = node.attribute("height").as_uint();
	layer->size = layer->width*layer->height;


	layer->data = new uint[layer->size];

	memset(layer->data, 0, sizeof(uint)*(layer->size));
	pugi::xml_node data_node = node.child("data").first_child();

	for (uint i = 0; i<layer->size; i++)
	{
		layer->data[i] = data_node.attribute("gid").as_uint();
		data_node = data_node.next_sibling();
		//LOG("item # %d , number %d", i,layer->data[i]);
	}
	pugi::xml_node properties = node.child("properties");
	pugi::xml_node iterator;
	for (iterator = properties.child("property"); iterator ; iterator = iterator.next_sibling("property"))
	{
		p2SString str = iterator.attribute("name").as_string();
		p2SString n1 = "collider_layer";
		if ( str == n1)
		{
		layer->logic_layer = node.child("properties").child("property").attribute("value").as_bool(false);
		}
		p2SString n2 = "parallax";
		if (str == n2)
		{	
			layer->parallax = node.child("properties").child("property").attribute("value").as_float(0.0);
		}
		p2SString n3 = "UI_layer";
		if (str==n3)
		{
			layer->UI_layer = node.child("properties").child("property").attribute("value").as_bool(false);
		}
	}

	if (layer->logic_layer)
	{
		CreateColliders(layer);
	}

	if (layer->UI_layer)
	{
		CreateUI(layer);
	}
	return ret;
}

//this loads the colliders/logic elements on the layer that are logics
bool j1Map::CreateColliders(map_layer* layer)
{
	BROFILER_CATEGORY("CreateColliders_Map", Profiler::Color::Sienna);

	int j = 0;
	data.colliders[j] = App->collision->AddCollider({-31,0,35,(int)layer->height*35}, COLLIDER_WALL); ++j;
	data.colliders[j] = App->collision->AddCollider({ (int)layer->width*35-4, 0, 35, (int)layer->height * 35 }, COLLIDER_WALL); ++j;
	data.colliders[j] = App->collision->AddCollider({ 0,(int)layer->height * 35 ,(int)layer->width * 35,10 }, COLLIDER_WALL); ++j;

	for (int _y = 0; _y < layer->height; ++_y)
	{
		for (int _x = 0; _x < layer->width; ++_x)
		{
			int i =layer->Get(_x, _y);
			iPoint point = MapToWorld(_x, _y);

			SDL_Rect rect;
			rect.x = point.x;
			rect.y = point.y;
			rect.w = 35;
			rect.h = 35;
			SDL_Rect recthalf;
			recthalf.x = point.x;
			recthalf.y = point.y;
			recthalf.w = 35;
			recthalf.h = 20;
				switch (layer->data[i])
			{
			case 27:
				if (data.colliders[j] == nullptr)
					data.colliders[j] = App->collision->AddCollider(rect, COLLIDER_NEXT_LEVEL);
				j++;
				break;
			case 28:
				if (data.colliders[j] == nullptr)
					data.colliders[j] = App->collision->AddCollider(rect, COLLIDER_WALL);
				j++;
				break;
			case 29:
				if (data.colliders[j] == nullptr)
					data.colliders[j] = App->collision->AddCollider(recthalf, COLLIDER_WALL);
				j++;
				break;
			case 30:
				if (data.colliders[j] == nullptr)
					data.colliders[j] = App->collision->AddCollider(rect,COLLIDER_SLOPE_RIGHT);
				++j;
				break;
			case 42:
				if (!coming_from_save)//player spawned from map point
				{
					initial_player_pos = point;
					player = (EntityPlayer*)App->entities->AddEntity(0,0,ENTITY_TYPE::PLAYER);
				}
				player_start_in_map = point;
				initial_player_pos.x = -1;
				initial_player_pos.y = -1;
				break;
			case 43:
				if (data.colliders[j] == nullptr)
					data.colliders[j] = App->collision->AddCollider(rect, COLLIDER_LAVA);
				j++;
				break;
			case 44:
				if (data.colliders[j] == nullptr)
					data.colliders[j] = App->collision->AddCollider(rect, COLLIDER_SLOPE_LEFT);
				++j;
				break;
			case 45:
				if (!coming_from_save)
				{
					App->entities->AddEntity(point.x, point.y, ENTITY_TYPE::BAT);
				}
				break;
			case 60:
				if (!coming_from_save)
				{
					App->entities->AddEntity(point.x, point.y, ENTITY_TYPE::ZOMBIE);
				}
				break;
			case 90:
				if (!coming_from_save)
				{
					App->entities->AddEntity(point.x, point.y, ENTITY_TYPE::PLANE);
				}
				break;
			case 105:
				if (!coming_from_save)
				{
					App->entities->AddEntity(point.x, point.y, ENTITY_TYPE::COIN);
				}
				break;
			default:
				break;
			}
		}
	}

	return true;
}

bool j1Map::CreateUI(map_layer* layer)
{
	BROFILER_CATEGORY("CreateUI_Map", Profiler::Color::Sienna);

	//this type of layer is only on the menus, meaning the score will reset to 0.
	if (index_map == 0)
	{ 
		App->scene->score = 0; 
	}
	else
	{
		App->scene->final_score = App->scene->score;
	}

	for (int _y = 0; _y < layer->height; ++_y)
	{
		for (int _x = 0; _x < layer->width; ++_x)
		{
			int i = layer->Get(_x, _y);
			iPoint point = MapToWorld(_x, _y);
			int width = 143;
			int height = 71;
			switch (layer->data[i])
			{
			case 1:
			{
			UIwindow* start_menu = (UIwindow*)App->gui->GUIAdd_window(point.x - 10, point.y, { 282,0,210,300 }, "MENU");
			start_menu->Attach(App->gui->GUIAdd_button(0, 0, { 0,0,width,height }, { 144,1,133,71 }, { 0,74,171,99 }, App->gui, "NEW GAME", button_type::NEW_GAME), { 35,35 });
			
			UIButton* load_button = (UIButton*)App->gui->GUIAdd_button(0, 0, { 0,0,width,height }, { 144,1,133,71 }, { 0,74,171,99 }, App->gui, "LOAD GAME", button_type::LOAD_GAME);
			pugi::xml_document doc;
			pugi::xml_parse_result result = doc.load_file("save_game.xml");
			if (!result)
			{
				load_button->interactuable = false;
			}
			start_menu->Attach(load_button, { 35,117 });
			
			
			start_menu->Attach(App->gui->GUIAdd_button(0, 0, { 0,0,width,height }, { 144,1,133,71 }, { 0,74,171,99 }, App->gui, "SETTINGS", button_type::SETTINGS), { 35,200 });
			App->gui->GUIAdd_image(10, 10, {0,171,270,131});

			break;
			}
			case 2://credits button
				App->gui->GUIAdd_button(point.x, point.y, { 0,0,width,height }, { 144,1,133,71 }, { 0,74,171,99 }, App->gui, "CREDITS", button_type::CREDITS);
				App->gui->GUIAdd_button(point.x, point.y-100, { 0,0,width,height }, { 144,1,133,71 }, { 0,74,171,99 }, App->gui, "HIGHSCORES", button_type::HIGHSCORES);
				break;
			case 3://quit button
				App->gui->GUIAdd_button(point.x, point.y, {0,0,width,height}, { 144,1,133,71 }, { 0,74,171,99 }, App->gui,"QUIT",button_type::QUIT);
				break;
			case 4://highscore textbox
				App->scene->name_highscore = (UITextbox*)App->gui->GUIAdd_textbox(point.x, point.y, App->scene,"YOUR NAME","click here");
				break;
			case 5://highscore leaderboard
			{
				UIwindow* table = (UIwindow*)App->gui->GUIAdd_window(point.x, point.y, {0,303,281,322}, "HIGHSCORE TABLE");
				p2List_item<HighScore*>*item = App->scene->highscores.start;
				int i = 0;
				while (item != NULL)
				{
					table->Attach(App->gui->GUIAdd_text(0, 0, item->data->name.GetString(), { 0,0,0,255 }), { 10,70 + i * 50 });
					table->Attach(App->gui->GUIAdd_VarDisplay(0, 0, &item->data->score), {250,70+i*50});
					item = item->next;
					++i;
				}
				App->gui->GUIAdd_button(point.x + 330, point.y, { 0,0,width,height }, { 144,1,133,71 }, { 0,74,171,99 },App->scene,"MENU",button_type::MAIN_MENU);
			}
				break;
			case 6://highscore score
				App->gui->GUIAdd_text(point.x, point.y - 30, "YOUR SCORE", {0,0,0,255});
				App->gui->GUIAdd_VarDisplay(point.x, point.y, &App->scene->final_score, {0,0,0,255});
				break;
			default:
				break;
			}
		}
	}

	return true;
}

bool j1Map::LoadBackground(pugi::xml_node& node, background* back)
{
	bool ret = true;
	back->name = node.attribute("name").as_string();
	back->width = node.child("image").attribute("width").as_uint();
	back->height = node.child("image").attribute("height").as_uint();
	back->texture = App->tex->Load(PATH(folder.GetString(), node.child("image").attribute("source").as_string()));

	pugi::xml_node properties = node.child("properties");
	pugi::xml_node iterator;
	for (iterator = properties.child("property"); iterator; iterator = iterator.next_sibling("property"))
	{
		p2SString str = iterator.attribute("name").as_string();
		p2SString n2 = "parallax";
		if (str == n2)
		{
			back->parallax = node.child("properties").child("property").attribute("value").as_float(0.0);
		}
	}
	return ret;
}


void j1Map::change_map(uint map)
{
	if (index_map > 0 && map != index_map)
		App->scene->score += time_left() * 10;
	index_map = map;	
	CleanUp();
	Load(maps[index_map].GetString());
	timer.Start();
}
void j1Map::next_level()
{
	if (index_map > 0)
		App->scene->score += time_left() * 10;
	index_map += 1;	
	CleanUp();
	Load(maps[index_map].GetString());
	timer.Start();
}
void j1Map::previous_level()
{
	if (index_map > 0)
		App->scene->score += time_left()*10;	
	index_map += 1;	
	CleanUp();
	Load(maps[index_map].GetString());
	timer.Start();
}

bool j1Map::Save(pugi::xml_node& node) const
{
	pugi::xml_node map_node = node.append_child("current_map");

	map_node.append_attribute("value") = index_map;
	node.append_child("time_left").append_attribute("value") = App->scene->time_left;
	
	return true;
}

bool j1Map::Load(pugi::xml_node& node)
{
	change_to_this_level = node.child("current_map").attribute("value").as_int();
	time_left_saved = node.child("time_left").attribute("value").as_int();
	coming_from_save = true;
	return true;
}

bool j1Map::Pause()
{
	timer.Pause();
	return true;
}
bool j1Map::Resume()
{
	timer.Resume();
	return true;
}