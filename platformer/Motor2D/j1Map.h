#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "SDL/include/SDL.h"
#include "EntityPlayer.h"
#include "j1Timer.h"

// TODO 1: Create a struct for the map layer
// ----------------------------------------------------
struct map_layer
{
	p2SString name;
	uint* data = nullptr;
	uint size = 0;
	uint width = 0;
	uint height = 0;
	bool logic_layer = false;
	bool UI_layer = false;
	float parallax = 0.0;
	inline uint Get(int x, int y)const
	{
		return y*width +x;
	};
	~map_layer()
	{
		RELEASE_ARRAY(data);

	}
};

	// TODO 6: Short function to get the value of x,y




// ----------------------------------------------------
struct TileSet
{
	// TODO 7: Create a method that receives a tile id and returns it's Rectfind the Rect associated with a specific tile id
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

struct background
{
	p2SString name;
	uint width = 0;
	uint height = 0;
	SDL_Texture* texture;
	float parallax = 0.0;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	// TODO 2: Add a list/array of layers to the map!
	p2List<map_layer*> layers;
	Collider* colliders[500];
	p2SString			music;
	background* back = nullptr;
};




// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	// TODO 8: Create a method that translates x,y coordinates from map positions to world positions
	iPoint WorldToMap(int x, int y) const;
	iPoint MapToWorld(int x, int y) const;
	/*{
		iPoint p;
		p.x = x*;
		p.y = ;
	}*/


private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	// TODO 3: Create a method that loads a single layer
	bool LoadLayer(pugi::xml_node& node, map_layer* layer);
	bool LoadBackground(pugi::xml_node&node, background* back);
	bool CreateColliders(map_layer* layer);
	bool CreateUI(map_layer*layer);

public:

	MapData data;
	p2List<p2SString>  maps;
	p2SString current_map;
	uint index_map=0;
	
	EntityPlayer* player = nullptr;

	void change_map(uint index);
	void next_level();
	void previous_level();

	bool Pause();
	bool Resume();

	bool change_to_next_level = false;
	bool change_to_previous_level = false;
	int change_to_this_level = -1;
	
	iPoint initial_player_pos = iPoint(-1,-1);// where he will be created
	iPoint player_start_in_map = iPoint(0, 0);// where he will respawn when killed / swpawned

	j1Timer timer;//NEEDS IMPLEMENTING 174*101

	int max_time;
	int time_left() 
	{
		return max_time - timer.ReadSec();
	}

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
	int time_left_saved = -1;
	int time_left_config;

	bool Save(pugi::xml_node& node) const;
	bool Load(pugi::xml_node& node);

	bool coming_from_save = false;
};

#endif // __j1MAP_H__