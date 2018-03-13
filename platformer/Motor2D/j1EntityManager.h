#ifndef __J1ENTITIES_H__
#define __J1ENTITIES_H__

#include "j1Module.h"
#include "p2Point.h"
#include "j1Collision.h"
#include "Entity.h"
#include "p2List.h"
#include "p2DynArray.h"

struct texture_struct//this is for loading textures from a xml file
{
	p2SString path;
	SDL_Texture* texture = nullptr;
};

struct entity_saved //this is to load the entities from the xml, then the map will create them
{
	float x;
	float y;
	ENTITY_TYPE type;
};

class j1EntityManager : public j1Module
{
public:
	j1EntityManager();
	~j1EntityManager();

	bool Start();

	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);

	bool Awake(pugi::xml_node& config);

	Entity* AddEntity(int x, int y,ENTITY_TYPE type);

	void OnCollision(Collider* c1, Collider* c2);

	void destroy_entity(p2List_item<Entity*>* ent);
	//void Draw();

	bool CleanUp();

	bool Save(pugi::xml_node& node)const;
	bool Load(pugi::xml_node& node);

	p2List<Entity*> entities;
	p2List<entity_property*> properties;

	p2List<texture_struct*> textures;
	p2List<uint> sfx;

	void Playfx(int sound, int repeat = 0);

	SDL_Texture* GetTex(int index)
	{
		SDL_Texture* ret;
		ret = textures[index]->texture;
		return ret;
	}

	p2List<entity_saved*> entities_saved;
	bool Load_entites();

	bool update_logic = true;
	j1Timer logic_timer;
};
#endif // !__J1ENTITIES_H__
