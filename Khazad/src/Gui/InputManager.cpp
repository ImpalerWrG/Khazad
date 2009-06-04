#include <stdafx.h>

#include <Game.h>
#include <InputManager.h>
#include <ScreenManager.h>
#include <Extract.h>
#include <Map.h>
#include <Camera.h>
#include <Singleton.h>
#include <Cube.h>
#include <ConfigManager.h>
#include <ColorManager.h>
#include <Gui.h>


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
        Sint32 RelativeX = 0;
        Sint32 RelativeY = 0;
        Uint8 MouseButtonState = SDL_GetRelativeMouseState(&RelativeX, &RelativeY);

        // Pass events into the GUI, if they are consumed their skip other checks
		if(UI->ProcessEvent(event, RelativeX, RelativeY))
		{
		    if(UI->isDone())
		    {
		        return true;  // End Program
		    }
		    continue;
		}

        switch(event.type) // Check event type
        {
            case SDL_QUIT: // User hit the X (or equivalent)
            {
                return true;  // exit program
            }

            case SDL_KEYDOWN:
            {
                switch(event.key.keysym.sym)
                {
                    //case SDLK_ESCAPE: // Escape key exits program
                    //{
                    //    return true;  // exit program
                    //}
                    case SDLK_F1:
                    {
                        SCREEN->ToggleFullScreen();  //TODO dose not work??
                        break;
                    }
                    case SDLK_RIGHT:
                    {
                        SCREEN->MainCamera->OrbitView(CONFIG->OrbitSpeed() / 1000.0);
                        break;
                    }
                    case SDLK_LEFT:
                    {
                        SCREEN->MainCamera->OrbitView(CONFIG->OrbitSpeed() / -1000.0);
                        break;
                    }
                    case SDLK_UP:
                    {
                        SCREEN->MainCamera->MoveViewVertical(1.0);
                        break;
                    }
                    case SDLK_DOWN:
                    {
                        SCREEN->MainCamera->MoveViewVertical(-1.0);
                        break;
                    }
                    case SDLK_INSERT:
                    {
                        SCREEN->MainCamera->ChangeViewLevels(1);
                        break;
                    }
                    case SDLK_DELETE:
                    {
                        SCREEN->MainCamera->ChangeViewLevels(-1);
                        break;
                    }
                    case SDLK_PAGEDOWN:
					{
					    SCREEN->MainCamera->changeLevelSeperation(1);
					    break;
					}
                    case SDLK_PAGEUP:
					{
					    SCREEN->MainCamera->changeLevelSeperation(-1);
					    break;
					}
                    case SDLK_q:
                    {
                        SCREEN->MainCamera->SetDefaultView();
                        break;
                    }
                    case SDLK_c:
                    {
                        SCREEN->MainCamera->CenterView();
                        break;
                    }
                    case SDLK_r:
                    {
                        // toggle face drawing options
                        break;
                    }
                    case SDLK_PRINT:
                    {
                        SCREEN->CaptureScreenShot();
                    }
                    case SDLK_d:
                    {
                        SCREEN->WipeScreen();
                        SCREEN->setDrawingFlat();
                        SCREEN->RenderLogo();

                        SCREEN->RenderTextCentered("Dumping Memory", 0, WHITE, 0);

                        SCREEN->Flip();


                        if(!MAP->Initialized)
                        {
                            EXTRACT->dumpMemory();
                            MAP->LoadExtract();
                            SCREEN->MainCamera->CenterView();
                        }
                        else
                        {
                            EXTRACT->dumpMemory();
                            MAP->LoadExtract();
                        }
                        break;
                    }
                    case SDLK_l:
                    {
                        SCREEN->WipeScreen();
                        SCREEN->setDrawingFlat();
                        SCREEN->RenderLogo();

                        char buffer[256];
                        sprintf(buffer, "Loading from File:  %s", CONFIG->LoadPath());
                        SCREEN->RenderTextCentered(buffer, 0, WHITE, 0);
                        SCREEN->Flip();

                        if(!MAP->Initialized)
                        {
                            EXTRACT->loadMap(CONFIG->LoadPath());
                            MAP->LoadExtract();
                            SCREEN->MainCamera->CenterView();
                        }
                        else
                        {
                            EXTRACT->loadMap(CONFIG->LoadPath());
                            MAP->LoadExtract();
                        }
                        break;
                    }
                    case SDLK_w:
					{
                        SCREEN->WipeScreen();
                        SCREEN->setDrawingFlat();
                        SCREEN->RenderLogo();

                        char buffer[256];
                        sprintf(buffer, "Writing to File:  %s", CONFIG->SavePath());
                        SCREEN->RenderTextCentered(buffer, 0, WHITE, 0);
                        SCREEN->Flip();


					    EXTRACT->writeMap(CONFIG->SavePath());
					    break;
					}
                    case SDLK_f:
					{
					    SCREEN->setFrameDraw(!SCREEN->isFrameDraw());
					    break;
					}
                    case SDLK_s:
					{
					    SCREEN->setShadedDraw(!SCREEN->isShadedDraw());
					    break;
					}
                    case SDLK_h:
					{
					    SCREEN->setHiddenDraw(!SCREEN->isHiddenDraw());
					    break;
					}
                    case SDLK_u:
					{
					    SCREEN->setSubTerranianDraw(!SCREEN->isSubTerranianDraw());
					    break;
					}
                    case SDLK_i:
					{
					    SCREEN->setSkyViewDraw(!SCREEN->isSkyViewDraw());
					    break;
					}
                    case SDLK_o:
					{
					    SCREEN->setSunLitDraw(!SCREEN->isSunLitDraw());
					    break;
					}
                    case SDLK_v:
					{
					    SCREEN->MainCamera->setVerticalMode(!SCREEN->MainCamera->isVerticalMode());
					    break;
					}
                    case SDLK_b:
					{
					    SCREEN->setDebuggingDraw(!SCREEN->isDebuggingDraw());
					    break;
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
			if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
			{
				SCREEN->MainCamera->onMouseEvent(&event, RelativeX, RelativeY);
				break;
			}
		}
	}

	//SCREEN->MainCamera->onMousePoll(); // Check for edge scrolling even without events


	return false;
}
