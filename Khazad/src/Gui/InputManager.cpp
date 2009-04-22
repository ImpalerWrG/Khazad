#include <stdafx.h>

#include <Game.h>
#include <InputManager.h>
#include <ScreenManager.h>
#include <Camera.h>
#include <Singleton.h>
#include <Cube.h>


DECLARE_SINGLETON(InputManager)

InputManager::InputManager()
{
	Focus = NULL;
}

InputManager::~InputManager()
{

}
bool InputManager::Init()
{

	return true;
}

bool InputManager::HandleInput()
{
	SDL_Event event;

	while(SDL_PollEvent(&event)) // Poll events
	{
		switch(event.type) // Check event type
		{
		case SDL_QUIT: //User hit the X (or equivalent)
			{
				return true;  // exit program
			}
		case SDL_KEYDOWN:
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					return true;  // exit program
				}
				else
				{
                    if (event.key.keysym.mod & KMOD_CTRL)
                    {
                        SCREEN->ToggleFullScreen();
                    }

					if (event.key.keysym.sym == SDLK_q)
					{
						SCREEN->MainCamera->SetDefaultView();
					}
					// send the key to the Games focused Actor or UI?

					if (event.key.keysym.sym == SDLK_c)
					{
						SCREEN->MainCamera->CenterView();
					}
                    if (event.key.keysym.sym == SDLK_r)
					{
					    SCREEN->MainCamera->setAllFacesDrawing(!SCREEN->MainCamera->isAllFacesDrawing());
					}

				}
				break;
			}
		case SDL_VIDEORESIZE: //User resized window
			{
				SCREEN->ReSizeScreen(event.resize.w, event.resize.h);
				break;
			}
		}

		if (true) // check if the camera has focus??
		{
			if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_KEYDOWN)
			{
				SCREEN->MainCamera->onMouseEvent(&event);
				break;
			}
		}
	}

	return false;
}
