#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"
#include "SDL_mixer\include\SDL_mixer.h"
#include "p2List.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	//need update to change the volume of the audio if there is need of it
	bool Update(float dt);

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	uint LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(uint fx, int repeat = 0);

	bool Save(pugi::xml_node& node) const;
	bool Load(pugi::xml_node& node);

	bool RiseVolume();
	int volume = 56;

	int saved_volume = 128;

	int fx_volume = 128;
	int saved_fxvolume = 128;

	bool LowerVolume();
private:

	_Mix_Music*			music;
	p2List<Mix_Chunk*>	fx;
	p2SString current_music;
};

#endif // __j1AUDIO_H__