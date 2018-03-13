#ifndef __j1TIMER_H__
#define __j1TIMER_H__

#include "p2Defs.h"

class j1Timer
{
public:

	// Constructor
	j1Timer();

	void Start();
	uint32 Read() const;
	float ReadSec() const;

	void Pause();
	void Resume();

	bool paused = false;

private:
	uint32	started_at;

	uint32 paused_at = 0;
	uint32 resumed_at = 0;
	uint32 time_paused = 0;

};

#endif //__j1TIMER_H__