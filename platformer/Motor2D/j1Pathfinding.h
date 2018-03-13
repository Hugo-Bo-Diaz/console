#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "SDL/include/SDL.h"
#include "p2PQueue.h"
#include "p2DynArray.h"
#include "j1Module.h"

#define COST_MAP 100

class j1Pathfinding : public j1Module
{
public:
	j1Pathfinding();

	~j1Pathfinding();

	bool Start();

	// Pathfinding
	bool IsWalkable(int x, int y) const;
	void ResetPath(p2DynArray<iPoint>& path_);
	void DrawPath(p2DynArray<iPoint>& path_);
	void Path(iPoint goal_, p2DynArray<iPoint>& path_);

	bool PropagateAStar(p2DynArray<iPoint>& path_, iPoint pos_org, iPoint pos_dest);
	bool PropagateBFS(p2DynArray<iPoint>& path_, iPoint pos_org, iPoint pos_dest, bool groundEnemy_ = true);

private:

	p2PQueue<iPoint>	frontier;
	p2List<iPoint>		visited;
	p2List<iPoint>		breadcrumbs;
	uint				cost_so_far[COST_MAP][COST_MAP];
	SDL_Texture*		target_tile = nullptr;
	bool				isGroundEnemy = true;
};

#endif