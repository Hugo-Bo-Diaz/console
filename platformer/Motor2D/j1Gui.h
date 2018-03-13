#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "j1Fonts.h"
#include "p2List.h"
#include "j1App.h"

#define CURSOR_WIDTH 2

// TODO 1: Create your structure of classes

class UIwindow;
class UIButton;
struct SDL_Rect;//WTF IS THIS??

enum button_type
{
	NEW_GAME,
	LOAD_GAME,
	SAVE_GAME,
	CREDITS,
	HIGHSCORES,
	SETTINGS,
	QUIT,
	CONTINUE,
	EXIT,
	CLOSE_WINDOW,
	MAIN_MENU,
	NUL
};


// ---------------------------------------------------
class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate(float dt);

	// Called after all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	// TODO 2: Create the factory methods
	// Gui creation functions

	SDL_Texture* GetAtlas() const;
	
	bool UIinteraction(UIelement* element);


	UIelement* GUIAdd_text(int x, int y, const char* text, SDL_Color color = {175,175,175,255}, bool follow_camera = false, _TTF_Font* font = App->font->default);
	UIelement* GUIAdd_image(int x, int y, SDL_Rect portion, bool follow_camera = false);
	UIelement* GUIAdd_button(int x, int y, SDL_Rect portion, SDL_Rect pressed, SDL_Rect hover, j1Module* callback = nullptr, const char* text = "", button_type type = button_type::NUL);
	UIelement* GUIAdd_checkbox(int x, int y, SDL_Rect portion, j1Module* callback = nullptr, const char* text = "");
	UIelement* GUIAdd_textbox(int x, int y, j1Module* callback = nullptr, const char* title = "", const char* default_text= "");
	UIelement* GUIAdd_VarDisplay(int x, int y, int* variable = nullptr, SDL_Color color = {0,0,0,255}, _TTF_Font* font = App->font->default);
	UIelement* GUIAdd_window(int x, int y, SDL_Rect portion, const char* title, bool close_button = false);
	UIelement* GUIAdd_slider(int x, int y, SDL_Rect portion, SDL_Rect start, SDL_Rect end, SDL_Rect middle, SDL_Rect button,int max_value, int* variable = nullptr, const char* title = "");
	//portion in this functions means the part of the atlas they get to draw

	bool delete_element(UIelement* element);

	UIwindow* window_ui = nullptr;

	float  test = 100000;
	bool debug = false;

	bool MouseInside(SDL_Rect* rect);

	UIwindow* Window_menu = nullptr;

	uint sound;
private:

	SDL_Texture* atlas;
	p2SString atlas_file_name;

	p2List<UIelement*> elements;

	void UIDebugDraw();

	SDL_Texture* screen = nullptr;

};

#endif // __j1GUI_H__