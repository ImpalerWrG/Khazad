//#include <windows.h>
#include <stdafx.h>

#include <Timer.h>


#include <TextureManager.h>
#include <FontManager.h>
#include <ColorManager.h>
#include <XMLManager.h>
#include <Random.h>
#include <ScreenManager.h>
#include <ConfigManager.h>
#include <DataManager.h>
#include <InputManager.h>
#include <Game.h>
#include <Map.h>
#include <Gui.h>


// General use char buffer
char buffer[256];


bool initManagers()
{
    printf("XML Initializing \n");
	XML->CreateInstance();
	XML->Init();

    printf("Config Initializing \n");
	CONFIG->CreateInstance();
	CONFIG->Init();

    printf("Random Initializing \n");
	RANDOM->CreateInstance();
	RANDOM->Init();

    printf("Color Initializing \n");
	COLOR->CreateInstance();
	COLOR->Init();

    printf("Data Initializing \n");
	DATA->CreateInstance();
	DATA->Init();

    printf("Screen Initializing \n");
	SCREEN->CreateInstance();
	SCREEN->Init();

    printf("Texture Initializing \n");
	TEXTURE->CreateInstance();
	TEXTURE->Init();

	TEXTURE->loadClippedSurface("Assets\\Textures\\dots.png", 100,100, 2, 2, true);

	TEXTURE->loadTextureSingular("Assets\\Textures\\floor_detailed.png", false, false);

	TEXTURE->loadTextureSingular("Assets\\Textures\\stone.png", false, false);

	TEXTURE->loadTextureSingular("Assets\\Textures\\grass.png", false, false);

    printf("Ui Initializing \n");
    UI->CreateInstance();
    UI->Init();

    printf("Game Initializing \n");
	GAME->CreateInstance();
	GAME->Init();

    printf("Map Initializing \n");
	MAP->CreateInstance();
	MAP->Init();
	MAP->Generate(0);

    printf("Input Initializing \n");
	INPUT->CreateInstance();
	INPUT->Init();

    printf("Font Initializing \n");
    FONT->CreateInstance();
	FONT->Init();

	FONT->loadFont("Assets\\Fonts\\KNIGHT.TTF", 30);

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
	const Uint16 FRAMES_PER_SECOND = 10000;
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
    Timer* InputTimer = new Timer(20);

	Uint8 FrameCounter = 0;
	Uint32 FrameRate = 0;

    InputTimer->Start();
    GameTimer->Start();
    RenderTimer->Start();
    UITimer->Start();

	SCREEN->ClearDevice();

	while(!done) // While program isn't done
	{
		FPSTimer->Start();

		InputTimer->Unpause();
            done = INPUT->HandleInput();
		InputTimer->Pause();


		GameTimer->Unpause();
            GAME->Run();
		GameTimer->Pause();


		RenderTimer->Unpause();
            SCREEN->Render();
        RenderTimer->Pause();


		UITimer->Unpause();
            UI->Draw();

            /* Go in HUD-drawing mode */
            SCREEN->Enable2D();

            SDL_Rect position;

            position.x = 10;
            position.y = SCREEN->getHight() - 80;

            sprintf (buffer, "FrameRate %i", FrameRate);
            FONT->SDL_GL_RenderText(buffer, FONT->FontLibrary[0], WHITE, &position);

            position.y -= 40;

            //sprintf (buffer, "InputTime %3.2f", InputTimer->getAverage());
            //FONT->SDL_GL_RenderText(buffer, FONT->FontLibrary[0], WHITE, &position);

            sprintf (buffer, "Triangles %i", SCREEN->getTriangleCount());
            FONT->SDL_GL_RenderText(buffer, FONT->FontLibrary[0], WHITE, &position);

            position.y -= 40;

            sprintf (buffer, "GameTime %3.2f", GameTimer->getAverage());
            FONT->SDL_GL_RenderText(buffer, FONT->FontLibrary[0], WHITE, &position);

            position.y -= 40;

            sprintf (buffer, "RenderTime %3.2f", RenderTimer->getAverage());
            FONT->SDL_GL_RenderText(buffer, FONT->FontLibrary[0], WHITE, &position);

            position.y -= 40;

            sprintf (buffer, "UITime %3.2f", UITimer->getAverage());
            FONT->SDL_GL_RenderText(buffer, FONT->FontLibrary[0], WHITE, &position);

            /* Come out of HUD mode */
            SCREEN->Disable2D();

		UITimer->Pause();

		SDL_GL_SwapBuffers();

		FPSTimer->Pause(); // FrameRate Captures whole loop

        FrameRate = (Uint32) FrameRateControl(FPSTimer);

	} // Program done, exited

	cleanup();

	return 0;
}
