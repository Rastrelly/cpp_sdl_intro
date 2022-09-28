#include <iostream>
#include <SDL.h>
#include <vector>

SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;
bool quit = false;
int img_op=0;
std::string texpath = "C:/ProgrammingData/kusanagi.bmp";

SDL_Texture * makeSDLTexture(std::string fn, int &errCode, SDL_Rect &imgSize);
void ApplySurface(int x, int y, int w, int h, SDL_Texture *tex);

class iButton 
{
private:
	int x, y, w, h, state;
	SDL_Texture * tex_neutral, *tex_up;
	SDL_Rect is;
	int ec;
	int button_operation;
public:
	iButton(int px, int py, int pw, int ph, int bo)
	{
		x = px; y = py; w = pw; h = ph; button_operation = bo;
		tex_neutral = makeSDLTexture("C:/ProgrammingData/btn_n.bmp",ec,is);
		tex_up = makeSDLTexture("C:/ProgrammingData/btn_up.bmp", ec, is);
	}
	void drawbutton()
	{
		if (state == 0)
		{
			ApplySurface(x, y, w, h, tex_neutral);
		}
		if (state == 1)
		{
			ApplySurface(x, y, w, h, tex_up);
		}
	}
	void checkbuttonstate(int cx, int cy)
	{
		if ((cx >= x) && (cx <= (x + w)) &&
			(cy >= y) && (cy <= (y + h)))
		{
			state = 1;
		}
		else
		{
			state = 0;
		}
	}

	void performOperation()
	{
		if (state == 1)
		{
			if (button_operation == 0)
			{
				img_op = 0;
			}
			if (button_operation == 1)
			{
				img_op = 1;
			}
		}
	}

};

std::vector<iButton> buttons;

int StartupSDL()
{
	//INIT SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	//MAKE WINDOW
	win = SDL_CreateWindow
	("Our SDL window", //window name
		100, 100,      //window cords
		640, 480,	   //widow width and heigh
		SDL_WINDOW_SHOWN); //widow type

	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	
	//MAKE RENDERER
	ren = SDL_CreateRenderer
	(
		win, //render window
		-1, //render device
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC //settings
	);
	if (ren == nullptr) {
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	return 0;
}

SDL_Texture * makeSDLTexture(std::string fn, int &errCode, SDL_Rect &imgSize)
{
	//MAKE SURFACE
	SDL_Surface *bmp = SDL_LoadBMP(fn.c_str());
	if (bmp == nullptr)
	{
		std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
		errCode = 1;
	}

	imgSize.w = bmp->w;
	imgSize.h = bmp->h;

	//MAKE TEXTURE FOM SURFACE
	SDL_Texture * tex =  SDL_CreateTextureFromSurface(ren, bmp); //make texture
	SDL_FreeSurface(bmp); //delete surface
	if (tex == nullptr)
	{
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		errCode = 1;
	}
	return tex;
}

void ApplySurface(int x, int y, int w, int h, SDL_Texture *tex)
{
	SDL_Rect src;
	SDL_QueryTexture(tex, NULL, NULL, &src.w, &src.h);
	src.x = 0;
	src.y = 0;

	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	pos.w = w;
	pos.h = h;
	
	SDL_RenderCopy(ren, tex, &src, &pos);
}



int main(int argc, char* argv[])
{
	
	if (StartupSDL() != 0) return 1;	

	//creating buttons
	buttons.push_back(iButton(10,10,40,40,0));
	buttons.push_back(iButton(60, 10, 40, 40, 1));

	SDL_Event e; //our event

	while (!quit)
	{
		
		int mx = 0; int my = 0;
		SDL_GetMouseState(&mx, &my);

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_LEFT:
					img_op = 0;
					break;

				case SDLK_RIGHT:
					img_op = 1;
					break;
				}
			} else if (e.type == SDL_MOUSEMOTION)
			{

				for (int i = 0; i < buttons.size(); i++)
				{

					buttons[i].checkbuttonstate(mx, my);
				}
			} else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (SDL_GetMouseState(NULL, NULL) &
					SDL_BUTTON(SDL_BUTTON_LEFT))
				{
					std::cout << "mb left" << std::endl;
					for (int i = 0; i < buttons.size(); i++)
					{
						buttons[i].performOperation();
					}
				}
			}


		}

		int texerr = 0;
		SDL_Rect texsize = {};

		if (img_op == 0) texpath = "C:/ProgrammingData/kusanagi.bmp";
		else texpath = "C:/ProgrammingData/duck.bmp";
		

		SDL_Texture * tex = makeSDLTexture(texpath, texerr, texsize);

		//SET WINDOW SIZE FROM BMP
		SDL_SetWindowSize(win, (texsize.w / 2), (texsize.h / 2));

		//PERFORM RENDERING
		SDL_RenderClear(ren); //clear image

		SDL_RenderCopy(ren, tex, NULL, NULL); //put texture to render

		for (int i = 0; i < buttons.size(); i++)
		{
			buttons[i].drawbutton();
		}

		SDL_RenderPresent(ren); //make the render draw its contents

		SDL_DestroyTexture(tex);
	}	

	//KILL EVERYTHING
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}

