#include <stdafx.h>

#include <Game.h>
#include <InputManager.h>
#include <Renderer.h>
#include <Map.h>
#include <Camera.h>
#include <Singleton.h>
#include <Cube.h>
#include <ConfigManager.h>
#include <ColorManager.h>
#include <DataManager.h>
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
    if(UI->isDone())
    {
        return true;  // End Program
    }
	SDL_Event event;

	// moved from camera input handler to avoid excess calls to those things
    Uint8* Keystate = SDL_GetKeyState(NULL);
    int RealX, RealY;
    Uint8 MouseButtonState = SDL_GetMouseState(&RealX, &RealY);

	while(SDL_PollEvent(&event)) // Poll events
	{
        Sint32 RelativeX = 0;
        Sint32 RelativeY = 0;
        Uint8 MouseButtonState = SDL_GetRelativeMouseState(&RelativeX, &RelativeY);

        // Pass events into the GUI, if they are consumed their skip other checks
		if(UI->ProcessEvent(event, RelativeX, RelativeY))
		{
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
                    case SDLK_F11:
                    {
                        RENDERER->ToggleFullScreen();
                        break;
                    }
                    case SDLK_RIGHT:
                    {
                        RENDERER->MainCamera->OrbitView(CONFIG->OrbitSpeed() / 1000.0);
                        break;
                    }
                    case SDLK_LEFT:
                    {
                        RENDERER->MainCamera->OrbitView(CONFIG->OrbitSpeed() / -1000.0);
                        break;
                    }
                    case SDLK_UP:
                    {
                        RENDERER->MainCamera->MoveViewVertical(1.0);
                        break;
                    }
                    case SDLK_DOWN:
                    {
                        RENDERER->MainCamera->MoveViewVertical(-1.0);
                        break;
                    }
                    case SDLK_INSERT:
                    {
                        RENDERER->MainCamera->ChangeViewLevel(1);
                        break;
                    }
                    case SDLK_DELETE:
                    {
                        RENDERER->MainCamera->ChangeViewLevel(-1);
                        break;
                    }
                    case SDLK_PAGEDOWN:
					{
					    RENDERER->MainCamera->changeLevelSeperation(1);
					    break;
					}
                    case SDLK_PAGEUP:
					{
					    RENDERER->MainCamera->changeLevelSeperation(-1);
					    break;
					}
                    case SDLK_c:
                    {
                        Vector3 adjustedCursor;
                        MapCoordinates CursorCoodinates = RENDERER->getCursor();

                        adjustedCursor.x = CursorCoodinates.X;
                        adjustedCursor.y = CursorCoodinates.Y;
                        adjustedCursor.z = CursorCoodinates.Z;

                        adjustedCursor.z = RENDERER->MainCamera->ZlevelSeperationAdjustment(adjustedCursor.z);

                        RENDERER->MainCamera->CenterView(adjustedCursor);
                        break;
                    }
                    case SDLK_PRINT:
                    {
                        RENDERER->CaptureScreenShot();
                        break;
                    }
                    case SDLK_u:
					{
					    RENDERER->setSubTerranianDraw(!RENDERER->isSubTerranianDraw());
					    break;
					}
                    case SDLK_i:
					{
					    RENDERER->setSkyViewDraw(!RENDERER->isSkyViewDraw());
					    break;
					}
                    case SDLK_o:
					{
					    RENDERER->setSunLitDraw(!RENDERER->isSunLitDraw());
					    break;
					}
                    case SDLK_b:
					{
					    RENDERER->setDebuggingDraw(!RENDERER->isDebuggingDraw());
					    break;
					}
					default:
                        break;
				}
				break;
			}

            case SDL_VIDEORESIZE: //User resized window
			{
				RENDERER->ReSizeScreen(event.resize.w, event.resize.h, 0);
				break;
			}
		}
		//if (RENDERER->MainCamera->hasFocus())
		//{
			if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
			{
				RENDERER->MainCamera->onMouseEvent(&event, RelativeX, RelativeY,Keystate, MouseButtonState,RealX,RealY);
			}
		//}
		/*else
		{
		    if (event.type == SDL_MOUSEBUTTONDOWN)
		    {
		        SCREEN->MainCamera->onMouseEvent(&event, RelativeX, RelativeY,Keystate, MouseButtonState,RealX,RealY);
		    }
		}*/
	}

	//SCREEN->MainCamera->onMousePoll(); // Check for edge scrolling even without events


	return false;
}
