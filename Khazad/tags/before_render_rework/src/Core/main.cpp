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
#include <Game.h>
#include <Map.h>
#include <Gui.h>
#include <Paths.h>


// General use char buffer
char buffer[256];


bool initManagers()
{
    printf("-=XML INITIALIZING=- ... ");
    XML->CreateInstance();
    XML->Init();
    printf("DONE\n");

    printf("-=CONFIG INITIALIZING=- ... ");
    CONFIG->CreateInstance();
    CONFIG->Init();
    printf("DONE\n");

    printf("-=RANDOM INITIALIZING=- ... ");
    RANDOM->CreateInstance();
    RANDOM->Init();
    printf("DONE\n");

    printf("-=COLOR INITIALIZING=- ... ");
    COLOR->CreateInstance();
    COLOR->Init();
    printf("DONE\n");

    printf("-=DATA INITIALIZING=- ... ");
    DATA->CreateInstance();
    DATA->Init();
    printf("DONE\n");

    printf("-=SCREEN INITIALIZING=- ... ");
    SCREEN->CreateInstance();
    SCREEN->Init();
    printf("DONE\n");

    printf("-=IMAGE INITIALIZING=- ... ");
    IMAGE->CreateInstance();
    IMAGE->Init();
    printf("DONE\n");

    printf("-=TEXTURE INITIALIZING=- ... ");
    TEXTURE->CreateInstance();
    TEXTURE->Init();
    printf("DONE\n");

    printf("-=UI INITIALIZING=- ... ");
    UI->CreateInstance();
    UI->Init();
    printf("DONE\n");

    printf("-=GAME INITIALIZING=- ... ");
    GAME->CreateInstance();
    GAME->Init();
    printf("DONE\n");

    printf("-=MAP INITIALIZING=- ... ");
    MAP->CreateInstance();
    MAP->Init();
    printf("DONE\n");

    printf("-=INPUT INITIALIZING=- ... ");
    INPUT->CreateInstance();
    INPUT->Init();
    printf("DONE\n");

    printf("-=FONT INITIALIZING=- ... ");
    FONT->CreateInstance();
    FONT->Init();
    printf("DONE\n");

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
    IMAGE->FreeInstance();
    GAME->FreeInstance();
    INPUT->FreeInstance();
    MAP->FreeInstance();
    UI->FreeInstance();

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
        float delay = (1000 / FRAMES_PER_SECOND) - FPSTimer->getElapsed();
        SDL_Delay(delay);
        if(delay > 100)
            printf("DELAY %f > 100 !\n", delay);
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
    printf("Starting timers ...");
    Timer* FPSTimer = new Timer(20);
    Timer* GameTimer = new Timer(20);
    //Timer* UITimer = new Timer(20);
    Timer* RenderTimer = new Timer(20);

    //Uint8 FrameCounter = 0;
    Uint32 FrameRate = 0;

    GameTimer->Start();
    RenderTimer->Start();
    printf("DONE\n");

    printf("Wiping the screen ...");
    SCREEN->WipeScreen();
    printf("DONE\n");

    printf("Init done. Entering main loop.\n");
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

        if(MAP->isMapLoaded())
        {
            SCREEN->setDrawingFlat();  // Go into HUD-drawing mode

            if(SCREEN->isDebuggingDraw())
            {
                SDL_Rect position;
                position.x = 10;
                position.y = SCREEN->getHeight() - 40;

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
                position.y = SCREEN->getHeight() - 40;

                sprintf (buffer, "KHAZAD");
                SCREEN->RenderText(buffer, 0, WHITE, &position);
            }

            SCREEN->setDrawing3D(); // Come out of HUD mode
        }

        SCREEN->Flip();
        glFinish();
		FPSTimer->Pause(); // FrameRate Captures whole loop

        FrameRate = (Uint32) FrameRateControl(FPSTimer);

	} // Program done, exited

	cleanup();

	return 0;
}
