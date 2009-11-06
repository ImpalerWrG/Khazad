#include <stdafx.h>

#include <Timer.h>
#include <TextureManager.h>
#include <FontManager.h>
#include <ColorManager.h>
#include <XMLManager.h>
#include <Random.h>
#include <Renderer.h>
#include <ImageManager.h>
#include <ConfigManager.h>
#include <DataManager.h>
#include <InputManager.h>
#include <ModelManager.h>
#include <Game.h>
#include <Map.h>
#include <Extractor.h>
#include <Gui.h>
#include <Paths.h>


// General use char buffer
char buffer[256];


bool initManagers()
{
    printf("-=XML INITIALIZING=- ... \n");
    XML->CreateInstance();
    XML->Init();
    printf("XML DONE\n");

    printf("-=CONFIG INITIALIZING=- ... \n");
    CONFIG->CreateInstance();
    CONFIG->Init();
    printf("CONFIG DONE\n");

    printf("-=DATA INITIALIZING=- ... \n");
    DATA->CreateInstance();
    DATA->Init();
    printf("DATA DONE\n");

    printf("-=RANDOM INITIALIZING=- ... \n");
    RANDOM->CreateInstance();
    RANDOM->Init();
    printf("RANDOM DONE\n");

    printf("-=COLOR INITIALIZING=- ... \n");
    COLOR->CreateInstance();
    COLOR->Init();
    printf("COLOR DONE\n");

    printf("-=RENDERER INITIALIZING=- ... \n");
    RENDERER->CreateInstance();
    RENDERER->Init();
    printf("RENDERER DONE\n");

    printf("-=IMAGE INITIALIZING=- ... \n");
    IMAGE->CreateInstance();
    IMAGE->Init();
    printf("IMAGE DONE\n");

    printf("-=TEXTURE INITIALIZING=- ... \n");
    TEXTURE->CreateInstance();
    TEXTURE->Init();
    printf("TEXTURE DONE\n");

    printf("-=MODEL INITIALIZING=- ... \n");
    MODEL->CreateInstance();
    MODEL->Init();
    printf("MODEL DONE\n");

    printf("-=UI INITIALIZING=- ... \n");
    UI->CreateInstance();
    UI->Init();
    printf("UI DONE\n");

    printf("-=GAME INITIALIZING=- ... \n");
    GAME->CreateInstance();
    GAME->Init();
    printf("GAME DONE\n");

    printf("-=EXTRACTOR INITIALIZING=- ... \n");
    EXTRACTOR->CreateInstance();
    EXTRACTOR->Init();
    printf("EXTRACTOR DONE\n");

    printf("-=MAP INITIALIZING=- ... \n");
    MAP->CreateInstance();
    MAP->Init();
    printf("MAP DONE\n");

    printf("-=INPUT INITIALIZING=- ... \n");
    INPUT->CreateInstance();
    INPUT->Init();
    printf("INPUT DONE\n");

    printf("-=FONT INITIALIZING=- ... \n");
    FONT->CreateInstance();
    FONT->Init();
    printf("FONT DONE\n");

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
    TEXTURE->FreeInstance();
    MODEL->FreeInstance();
    IMAGE->FreeInstance();
    GAME->FreeInstance();
    INPUT->FreeInstance();
    MAP->FreeInstance();
    UI->FreeInstance();
    RENDERER->FreeInstance();

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
    RENDERER->WipeScreen();
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
            RENDERER->Render();
        RenderTimer->Pause();

        UI->Draw();

        if(MAP->isMapLoaded())
        {
            RENDERER->setDrawingFlat();  // Go into HUD-drawing mode

            if(RENDERER->isDebuggingDraw())
            {
                SDL_Rect position;
                position.x = 10;
                position.y = RENDERER->getHeight() - 40;

                sprintf (buffer, "FrameRate %i", FrameRate);
                RENDERER->RenderText(buffer, 0, WHITE, &position);

                position.y -= 40;

                sprintf (buffer, "Triangles %i", RENDERER->getTriangleCount());
                RENDERER->RenderText(buffer, 0, WHITE, &position);

                RENDERER->PrintDebugging();
            }
            else
            {
                SDL_Rect position;
                position.x = 10;
                position.y = RENDERER->getHeight() - 40;

                sprintf (buffer, "KHAZAD");
                RENDERER->RenderText(buffer, 0, WHITE, &position);
            }

            RENDERER->setDrawing3D(); // Come out of HUD mode
        }

        RENDERER->Flip();
		FPSTimer->Pause(); // FrameRate Captures whole loop

        FrameRate = (Uint32) FrameRateControl(FPSTimer);

	} // Program done, exited

	cleanup();

	return 0;
}
