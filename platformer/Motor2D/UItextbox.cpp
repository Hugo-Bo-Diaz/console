#include "UItextbox.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Input.h"

UITextbox::UITextbox(iPoint pos, const char* name, const char* _default_text)// color is a 4 dim array in this order{r g b a} this is for the default font need to adapt it better
{
	winposition = pos;
	string = name;
	portion = { 401,300,126,25 };
	type_of_element = TEXTBOX;
	//Attach(App->gui->GUIAdd_VarDisplay(0, 0, &text_pos), {0,50});
}

UITextbox::~UITextbox()
{
	App->tex->UnLoad(tex);
	App->tex->UnLoad(default_text);
};


void UITextbox::Draw() {
	//text box
	App->render->Blit(App->gui->GetAtlas(), winposition.x, winposition.y, &portion);

	if (App->input->GetMouseButtonDown(1) == KEY_UP && !mouseover)
	{
		active = false;
		texting = false;
	}

	//update text from the input buffer
	if (texting && text.Length() < 60)//max chars
	{
		text += App->input->buffered_text;
		text_pos += App->input->buffered_text.Length();
		App->input->buffered_text.Clear();
	}

	//backspace and delete functions
	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN && text.Length() >0 && texting && text_pos > 0)
	{
		text.DeleteChar(text_pos-1);
		text_pos -= 1;
	}
	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN && text.Length() >0 && texting && text_pos != text.Length())
	{
		text.DeleteChar(text_pos);
	}

	//move with arrows function
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN && text.Length() >0 && texting && text_pos > 0)
	{
		text_pos -= 1;
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN && text.Length() >0 && texting && text_pos < text.Length())
	{
		text_pos += 1;
	}

	//start and end functions
	if (App->input->GetKey(SDL_SCANCODE_HOME) == KEY_DOWN && text.Length() >0 && texting)
	{
		text_pos = 0;
	}

	if (App->input->GetKey(SDL_SCANCODE_END) == KEY_DOWN && text.Length() >0 && texting)
	{
		text_pos = text.Length();
	}


	//text
	if (text.Length() > 0)
	{
		SDL_Texture* text_texture = App->tex->textures.add(App->font->Print(text.GetString(), { 50,50,50,255 }, App->font->default))->data;

		SDL_QueryTexture(text_texture, NULL, NULL, &text_w, &text_h);

		App->render->Blit(text_texture, winposition.x+5, winposition.y+5);

		App->tex->UnLoad(text_texture);
	}

	//bar
	if (texting)
	{
		int width = 0;
		int height;
		p2SString yolo = text;
		if (text.Length() > 0)
		{
			char* test = (char*)yolo.GetString();
			test[text_pos] = '\0';
			App->font->CalcSize(test, width, height,App->font->default);
		}
		App->render->DrawQuad({ winposition.x + width + 5,winposition.y,2,25 }, 150, 150, 150, 255);
	}

	//we need enter to enter the text as input and call the text in whatever callback it is
	if (texting && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		OnActivation();
	}

	if (mouseover)
	{
		mouseover = false;
	}
}

void UITextbox::OnClick()
{
	if (mouseover)
	{
		active = true;
		texting = true;
		App->input->StartKeyInput();
	}
};

bool UITextbox::OnRelease()
{
	bool ret = true;
	if (active)
	{
		active = false;
	}
	return ret;
};

