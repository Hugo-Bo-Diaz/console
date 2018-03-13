#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include <math.h>

j1Pathfinding::j1Pathfinding()
{
	name.create("pathfinding");
}


j1Pathfinding::~j1Pathfinding()
{}

bool j1Pathfinding::Start()
{
	target_tile = App->tex->Load("maps/path_target.png");

	return true;
}

void j1Pathfinding::ResetPath(p2DynArray<iPoint>& path_)
{
	frontier.Clear();
	visited.clear();
	breadcrumbs.clear();
	path_.Clear();

	memset(cost_so_far, 0, sizeof(uint) * COST_MAP * COST_MAP);
}

bool j1Pathfinding::IsWalkable(int x, int y) const
{
	
	bool ret = false;
	int id = -1;
	if (x >= 0 && x < App->map->data.width && y >= 0)
	{
		//return true;
		/*p2List_item<map_layer*>* item = nullptr;
		item = App->map->data.layers.start;
		
		while (item != NULL)
		{
			if (item->data->logic_layer)
			{
				int id = item->data->Get(x, y);
			}
			item = item->next;
		}
		item = nullptr;*/

		int tile = App->map->data.width * y + x;
		if (!isGroundEnemy)
		{
			id = App->map->data.layers.start->next->next->data->data[tile];
			if (id != 28 && id != 29)
				ret = true;
		}
		else
		{
			id = App->map->data.layers.start->next->next->next->data->data[tile];
			if (id == 75)
				ret = true;
		}
	}

	return ret;
}

bool j1Pathfinding::PropagateAStar(p2DynArray<iPoint>& path_, iPoint pos_org, iPoint pos_dest)
{
	bool ret = false;

	ResetPath(path_);

	iPoint goal = App->map->WorldToMap(pos_dest.x, pos_dest.y);
	iPoint origin = App->map->WorldToMap(pos_org.x, pos_org.y);

	frontier.Push(origin, 0);
	visited.add(origin);
	breadcrumbs.add(origin);

	while (visited.find(goal) == -1)
	{
		iPoint curr;
		uint new_cost;

		if (frontier.Pop(curr))
		{
			iPoint neighbors[4];
			neighbors[0].create(curr.x + 1, curr.y + 0);
			neighbors[1].create(curr.x + 0, curr.y + 1);
			neighbors[2].create(curr.x - 1, curr.y + 0);
			neighbors[3].create(curr.x + 0, curr.y - 1);

			for (uint i = 0; i < 4; ++i)
			{
				int distance = neighbors[i].DistanceNoSqrt(goal);

				if (IsWalkable(neighbors[i].x, neighbors[i].y))
				{
					new_cost = cost_so_far[curr.x][curr.y] + 1;

					if (cost_so_far[neighbors[i].x][neighbors[i].y] == 0 || new_cost < cost_so_far[neighbors[i].x][neighbors[i].y])
					{
						cost_so_far[neighbors[i].x][neighbors[i].y] = new_cost;

						if (visited.find(neighbors[i]) == -1)
						{
							if (neighbors[i] != goal)
								frontier.Push(neighbors[i], new_cost + distance * 10);

							visited.add(neighbors[i]);
							breadcrumbs.add(curr);
						}
						else
							breadcrumbs.At(visited.find(neighbors[i]))->data = curr;
					}
				}
			}
		}
		ret = true;
	}

	if (visited.find(goal) != -1)
	{
		Path(goal, path_);
		if (App->collision->debug == true)
			DrawPath(path_);
	}

	return ret;
}

void j1Pathfinding::Path(iPoint goal_, p2DynArray<iPoint>& path_)
{
	iPoint current = breadcrumbs.At(visited.find(goal_))->data;

	if (visited.find(goal_) != -1)
	{
		path_.PushBack(current);

		while (current != visited.start->data)
		{
			current = breadcrumbs.At(visited.find(current))->data;
			path_.PushBack(current);
		}
		path_.Flip();
	}
}

void j1Pathfinding::DrawPath(p2DynArray<iPoint>& path_)
{
	for (uint i = 0; i < path_.Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path_[i].x, path_[i].y);
		App->render->Blit(target_tile, pos.x, pos.y);
	}
}

bool j1Pathfinding::PropagateBFS(p2DynArray<iPoint>& path_, iPoint pos_org, iPoint pos_dest, bool groundEnemy_)
{
	isGroundEnemy = groundEnemy_;

	ResetPath(path_);

	iPoint origin = App->map->WorldToMap(pos_org.x, pos_org.y);
	iPoint goal = App->map->WorldToMap(pos_dest.x, pos_dest.y);	
	
	frontier.Push(origin, 0);
	visited.add(origin);
	breadcrumbs.add(origin);
	while (visited.find(goal) == -1 && frontier.Count() > 0)
	{
		iPoint curr;
		if (frontier.Pop(curr))
		{
			iPoint neighbors[4];
			neighbors[0].create(curr.x + 1, curr.y + 0);
			neighbors[1].create(curr.x + 0, curr.y + 1);
			neighbors[2].create(curr.x - 1, curr.y + 0);
			neighbors[3].create(curr.x + 0, curr.y - 1);

			for (uint i = 0; i < 4; ++i)
			{
				if (IsWalkable(neighbors[i].x, neighbors[i].y))
				{
					if (visited.find(neighbors[i]) == -1)
					{
						frontier.Push(neighbors[i], 0);
						visited.add(neighbors[i]);

						breadcrumbs.add(curr);
					}
				}
			}
		}
	}
	if (visited.find(goal) == -1)
	{
		iPoint semigoal = { 0, 0 };
		for (int i = 0; i < visited.count(); i++)
		{
			if (abs(visited.At(i)->data.x - goal.x) < abs(semigoal.x - goal.x))
				semigoal = visited.At(i)->data;
		}
		Path(semigoal, path_);
		if (App->collision->debug == true)
			DrawPath(path_);
	}
	if (visited.find(goal) != -1)
	{
		Path(goal, path_);
		if (App->collision->debug == true)
			DrawPath(path_);
	}

	return true;
}