#include <stdafx.h>

#include <Timer.h>
#include <TextureManager.h>
#include <FontManager.h>
#include <ColorManager.h>
#include <XMLManager.h>
#include <Random.h>
#include <ScreenManager.h>
#include <ImageManager.h>
#include <ConfigManager.h>
#include <DataManager.h>
#include <InputManager.h>
#include <Extract.h>
#include <Game.h>
#include <Map.h>
#include <Gui.h>
#include <Paths.h>


// General use char buffer
char buffer[256];


bool initManagers()
{
    printf("-=XML INITIALIZING=-\n");
	XML->CreateInstance();
	XML->Init();

    printf("-=CONFIG INITIALIZING=-\n");
	CONFIG->CreateInstance();
	CONFIG->Init();

    printf("-=RANDOM INITIALIZING=-\n");
	RANDOM->CreateInstance();
	RANDOM->Init();

    printf("-=COLOR INITIALIZING=-\n");
	COLOR->CreateInstance();
	COLOR->Init();

    printf("-=DATA INITIALIZING=-\n");
	DATA->CreateInstance();
	DATA->Init();

    printf("-=SCREEN INITIALIZING=-\n");
	SCREEN->CreateInstance();
	SCREEN->Init();

    printf("-=IMAGE INITIALIZING=-\n");
	IMAGE->CreateInstance();
	IMAGE->Init();

    printf("-=TEXTURE INITIALIZING=-\n");
	TEXTURE->CreateInstance();
	TEXTURE->Init();

    printf("-=UI INITIALIZING=-\n");
    UI->CreateInstance();
    UI->Init();

    printf("-=GAME INITIALIZING=-\n");
	GAME->CreateInstance();
	GAME->Init();

    printf("-=PROCESS INITIALIZING=-\n");
    PROCESS->CreateInstance();
	PROCESS->Init();

    printf("-=EXTRACTOR INITIALIZING=-\n");
	EXTRACT->CreateInstance();
	EXTRACT->Init();

    printf("-=MAP INITIALIZING=-\n");
	MAP->CreateInstance();

    printf("-=INPUT INITIALIZING=-\n");
	INPUT->CreateInstance();
	INPUT->Init();

    printf("-=FONT INITIALIZING=-\n");
    FONT->CreateInstance();
	FONT->Init();

	FONT->loadFont(Path("Assets\\Fonts\\KNIGHT.TTF"), 30);

	return true;
}

void cleanup()
{
    XML->FreeInstance();
	CONFIG->FreeInstance();
	RANDOM->FreeInstance();
	COLOR->FreeInstance();
	FONT->FreeInstance();
	DATA->FreeInstance();
	SCREEN->FreeInstance();
	TEXTURE->FreeInstance();
    GAME->FreeInstance();
	INPUT->FreeInstance();

	TTF_Quit();
	SDL_Quit();
}

float FrameRateControl(Timer* FPSTimer)
{
	//The frames per second and regulators
	const Uint16 FRAMES_PER_SECOND = 60;
	static Uint8 FrameCounter = 0;
	float AverageFrameTime = 0;
	float ActualFramesPerSecond = 0;
	static float LastFrameValue = 0;
	static float FrameRateAcumulator = 0;
	const Uint8 FrameRateSampleSize = 30;

	if (FrameCounter >= FrameRateSampleSize)
	{
		AverageFrameTime = FrameRateAcumulator / FrameRateSampleSize;
		if (AverageFrameTime > 0)
		{
			ActualFramesPerSecond = 1000.0 / AverageFrameTime;
			FrameRateAcumulator = 0;
			FrameCounter = 0;
			LastFrameValue = ActualFramesPerSecond;
		}
	}

	FrameCounter++;
	if (FPSTimer->getElapsed() < (1000 / FRAMES_PER_SECOND))
	{
		SDL_Delay((1000 / FRAMES_PER_SECOND) - FPSTimer->getElapsed());
	}

	FrameRateAcumulator += FPSTimer->getElapsed();

	return LastFrameValue;
}

int main(int argv, char** argc)
{
	if (!initManagers())
	{
		return 0;
	}

	bool done = false;

	Timer* FPSTimer = new Timer(20);
	Timer* GameTimer = new Timer(20);
	Timer* UITimer = new Timer(20);
	Timer* RenderTimer = new Timer(20);

	Uint8 FrameCounter = 0;
	Uint32 FrameRate = 0;

    GameTimer->Start();
    RenderTimer->Start();

	SCREEN->WipeScreen();

	while(!done) // While program isn't done
	{
		FPSTimer->Start();

        done = INPUT->HandleInput();

        GameTimer->Unpause();
            GAME->Run();
        GameTimer->Pause();

        RenderTimer->Unpause();
            SCREEN->Render();
        RenderTimer->Pause();

        UI->Draw();

        if(MAP->isInitialized())
        {
            SCREEN->setDrawingFlat();  // Go into HUD-drawing mode

            if(SCREEN->isDebuggingDraw())
            {
                SDL_Rect position;
                position.x = 10;
                position.y = SCREEN->getHight() - 40;

                sprintf (buffer, "FrameRate %i", FrameRate);
                SCREEN->RenderText(buffer, 0, WHITE, &position);

                position.y -= 40;

                sprintf (buffer, "Triangles %i", SCREEN->getTriangleCount());
                SCREEN->RenderText(buffer, 0, WHITE, &position);

                SCREEN->PrintDebugging();
            }
            else
            {
                SDL_Rect position;
                position.x = 10;
                position.y = SCREEN->getHight() - 40;

                sprintf (buffer, "KHAZAD");
                SCREEN->RenderText(buffer, 0, WHITE, &position);
            }

            SCREEN->setDrawing3D(); // Come out of HUD mode
        }

        SCREEN->Flip();

		FPSTimer->Pause(); // FrameRate Captures whole loop

        FrameRate = (Uint32) FrameRateControl(FPSTimer);

	} // Program done, exited

	cleanup();

	return 0;
}
