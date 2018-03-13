#include "j1EntityManager.h"
#include "j1Collision.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1Audio.h"
#include "EntityPlayer.h"
#include "EntityEnemyBat.h"
#include "EntityEnemyZombie.h"
#include "EntityPlane.h"
#include "EntityCoin.h"
#include "Brofiler\Brofiler.h"

j1EntityManager::j1EntityManager()
{
	name.create("Entities");
	//here we initialise all object to nullptr
	active_in_pause = false;
}

j1EntityManager::~j1EntityManager()
{
	p2List_item<entity_property*>* item_1 = properties.start;
	while (item_1 != nullptr)
	{
		RELEASE(item_1->data);
		item_1 = item_1->next;
	}
	properties.clear();

	p2List_item<texture_struct*>* item_2 = textures.start;
	while (item_2 != NULL)
	{
		App->tex->UnLoad(item_2->data->texture);
		RELEASE(item_2->data);
		item_2 = item_2->next;
	}
	textures.clear();

	p2List_item<entity_saved*>* item_3 = entities_saved.start;
	while (item_3 != NULL)
	{
		RELEASE(item_3->data);
		item_3 = item_3->next;
	}
	entities_saved.clear();
};

bool j1EntityManager::Awake(pugi::xml_node& config)
{
	pugi::xml_node node_entities = config.child("properties");

	pugi::xml_node entity;
	int i = 0;
	for (entity = node_entities.first_child(); entity; entity = entity.next_sibling())// there are more entities with properties
	{
		pugi::xml_node propertie = entity.first_child();
		i = entity.attribute("type").as_int(-1);
		for (propertie = entity.first_child(); propertie; propertie = propertie.next_sibling())//there are still properties left
		{
			entity_property* iterator_property = new entity_property;
			iterator_property->name.create(propertie.attribute("name").as_string("failed_loading"));
			iterator_property->value = propertie.attribute("value").as_float(-1);
			iterator_property->type = i;
			properties.add(iterator_property);
		}
	}
	pugi::xml_node node_textures = config.child("textures");

	pugi::xml_node texture;
	int j = 0;
	for (texture = node_textures.first_child(); texture; texture = texture.next_sibling())// there are more entities with properties
	{
		texture_struct* tex = new texture_struct;
		tex->path = texture.attribute("path").as_string();
		textures.add(tex);
	}

	pugi::xml_node node_audio = config.child("sounds");

	pugi::xml_node audio;
	j = 0;
	for (audio = node_audio.first_child(); audio; audio = audio.next_sibling())// there are more entities with properties
	{
		texture_struct* tex = new texture_struct;
		sfx.add(App->audio->LoadFx(audio.attribute("path").as_string()));
	}


	return true;
}

bool j1EntityManager::Start()
{
	p2List_item<texture_struct*>* item = textures.start;
	while (item != NULL)
	{
		item->data->texture = App->tex->Load(item->data->path.GetString());
		item = item->next;
	}
	
	logic_timer.Start();

	return true;
}

Entity* j1EntityManager::AddEntity(int _x, int _y, ENTITY_TYPE type)
{
	BROFILER_CATEGORY("AddEntity_EntityManager", Profiler::Color::DarkOliveGreen);

	Entity* ret;
	switch (type)
	{
	case ENTITY_TYPE::PLAYER:
	{
		 ret = new EntityPlayer;
		 break;
	}
	case ENTITY_TYPE::BAT:
	{
		ret = new EntityEnemyBat;
		break;
	}
	case ENTITY_TYPE::ZOMBIE:
	{
		ret = new EntityEnemyZombie;
		break;
	}
	case ENTITY_TYPE::PLANE:
	{
		ret = new EntityPlane;
		break;
	}
	case ENTITY_TYPE::COIN:
	{
		ret = new EntityCoin;
		break;
	}
	default:
		ret = new EntityEnemyZombie;
		break;
	}

	//assign all the entity properties
	ret->SetPos(iPoint(_x,_y));
	ret->type = type;
	
	ret->Awake();
	ret->Start();

	entities.add(ret);
	return ret;

}

bool j1EntityManager::CleanUp()
{
	BROFILER_CATEGORY("CleanUp_EntityManager", Profiler::Color::DarkOliveGreen);

	LOG("Freeing all objects");
	p2List_item<Entity*>* item = entities.start;
	while(item != NULL)
	{
		item->data->CleanUp();
		if (item->data->obj != nullptr){App->physics->destroy_object(item->data->obj);}
		RELEASE(item->data);
		item = item->next;
	}
	entities.clear();

	return true;
}

bool j1EntityManager::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate_EntityManager", Profiler::Color::DarkOliveGreen);

	p2List_item<Entity*>* item = entities.start;
	while (item != nullptr)
	{
		item->data->PreUpdate(dt);
		item = item->next;
	}
	return true;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("Update_EntityManager", Profiler::Color::DarkOliveGreen);
	if (logic_timer.ReadSec() > 0.2)
	{
		update_logic = true;
		logic_timer.Start();
	}

	p2List_item<Entity*>* item = entities.start;
	while (item != nullptr)
	{
		item->data->Update(dt,update_logic);
		item = item->next;

	}

	if (update_logic) 
	{update_logic = false;}

	p2List_item<Entity*>* item_1 = entities.start;
	while (item_1 != nullptr)
	{
		item_1->data->Draw();
		item_1 = item_1->next;
	}
	return true;
}

bool j1EntityManager::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostUpdate_EntityManager", Profiler::Color::DarkOliveGreen);

	p2List_item<Entity*>* item = entities.start;
	while (item != nullptr)
	{
		item->data->PostUpdate(dt);
		// maybe this object is going to be destroyed so i need to get the next member before i do;
		p2List_item<Entity*>* item_next = item->next;
		if (item->data->destroyed == true)
		{
			if (item->data->obj != nullptr)
				App->physics->destroy_object(item->data->obj);
			App->entities->destroy_entity(item);
		}
		if (item != nullptr)
		{
			item = item->next;
		}
		else
		{
			item = item_next;
		}

	}

	return true;
}

void j1EntityManager::OnCollision(Collider* c1, Collider*c2)
{
	BROFILER_CATEGORY("OnCollision_EntityManager", Profiler::Color::DarkOliveGreen);

	p2List_item<Entity*>* item = entities.start;
	while (item != NULL)
	{
		if (item->data->interactive)
		{
			// this is to talk to the item as an interactive item that has the obj property;
			if (item->data->obj != nullptr)
			{// this checks the object if there is any it uses its properties
				if (c1->rect.x == item->data->obj->col->rect.x &&
					c1->rect.y == item->data->obj->col->rect.y &&
					c1->type == item->data->obj->col->type)
				{
					item->data->OnCollision(c1, c2);
				}
			}
			else
			{
				if (c1->rect.x == item->data->position.x &&//if there is no object this will still look for a collision because the object is interactive
					c1->rect.y == item->data->position.y )//we check the position because this object isn't moving(else it would have an object))
				{//sadly we can't check if they are the same type of colliders but its the 99.999% chance they are
					item->data->OnCollision(c1, c2);
				}
			}
		}
		item = item->next;
	}
};

bool j1EntityManager::Save(pugi::xml_node& node) const
{
	BROFILER_CATEGORY("Save_EntityManager", Profiler::Color::DarkOliveGreen);

	p2List_item<Entity*>* item = entities.start;
	while (item != nullptr)
	{
		pugi::xml_node item_node = node.append_child(item->data->name.GetString());
		item_node.append_attribute("x") = item->data->position.x;
		item_node.append_attribute("y") = item->data->position.y;
		item_node.append_attribute("type") = item->data->type;
		item = item->next;
	}
	return true;
}

bool j1EntityManager::Load(pugi::xml_node& node)
{
	BROFILER_CATEGORY("Load_EntityManager", Profiler::Color::DarkOliveGreen);
	
	CleanUp();

	//p2List_item<Entity*>* item = entities.start;
	pugi::xml_node item_node;
	for (item_node = node.first_child(); item_node; item_node = item_node.next_sibling())
	{
		//if(item_node.)
		entity_saved* ent = new entity_saved;
		ent->x = item_node.attribute("x").as_float();
		ent->y = item_node.attribute("y").as_float();
		ent->type = (ENTITY_TYPE)item_node.attribute("type").as_int();
		entities_saved.add(ent);
	}
	return true;
}

bool j1EntityManager::Load_entites()
{
	BROFILER_CATEGORY("Load_EntityManager", Profiler::Color::DarkOliveGreen);

	CleanUp();

	p2List_item<entity_saved*>* item = entities_saved.start;
	while(item != NULL)
	{
		switch (item->data->type)
		{
			case ENTITY_TYPE::PLAYER:
			{		
				App->map->initial_player_pos.x = item->data->x;
				App->map->initial_player_pos.y = item->data->y;
				App->map->player = (EntityPlayer*)AddEntity(item->data->x, item->data->y, item->data->type);
				break;
			}
			//case ENTITY_TYPE::BAT:
			//{
			//	AddEntity(item->data->x,item->data->y,item->data->type);
			//	break;
			//}
			//case ENTITY_TYPE::ZOMBIE:
			//{
			//	AddEntity(item->data->x, item->data->y, item->data->type);
			//	break;
			//}
			//case ENTITY_TYPE::PLANE:
			//{
			//	AddEntity(item->data->x, item->data->y, item->data->type);
			//	break;
			//}
			//case ENTITY_TYPE::COIN:
			//{
			//	AddEntity(item->data->x, item->data->y, item->data->type);
			//	break;
			//}
			default:
			{
				AddEntity(item->data->x, item->data->y, item->data->type);
				break;
			}
		}

	delete item->data;
	item = item->next;
	}
	entities_saved.clear();
	return true;
}

void j1EntityManager::destroy_entity(p2List_item<Entity*>* ent)
{
	ent->data->CleanUp();
	RELEASE(ent->data);
	entities.del(ent);
}

void j1EntityManager::Playfx(int sound, int repeat)
{
	App->audio->PlayFx(sfx[sound], repeat);
}