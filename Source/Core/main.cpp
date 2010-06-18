#include <stdafx.h>

#include <Ogre.h>

#include <DataManager.h>
#include <Renderer.h>
#include <InputManager.h>
#include <GUI.h>
#include <Game.h>



//using namespace Ogre;

bool Init()
{


    return true;
}

void CleanUp()
{

}

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
        Ogre::LogManager* logMgr = new Ogre::LogManager;
        Ogre::Log *log = Ogre::LogManager::getSingleton().createLog("Ogre.log", true, true, false);
        //Ogre::LogManager::getSingletonPtr()->logMessage("NEW LOG MESSAGE");

        Ogre::LogManager::getSingletonPtr()->logMessage("Starting Geom main routine...");

        // XML Data loading
        printf("-=DATA INITIALIZING=- ... \n");
        DATA->CreateInstance();
        DATA->Init();
        printf("DATA DONE\n");

        // OGRE Renderer
        printf("-=RENDER INITIALIZING=- ... \n");
        RENDERER->CreateInstance();
        RENDERER->Init();
        printf("RENDER DONE\n");

        // CEGUI with OGRE Platform
        printf("-=GUI INITIALIZING=- ... \n");
        GUI->CreateInstance();
        GUI->Init();
        printf("GUI DONE\n");

        // OIS injection into GUI
        printf("-=INPUT INITIALIZING=- ... \n");
        INPUT->CreateInstance();
        INPUT->Init();
        printf("INPUT DONE\n");

        //return 0; // Errors durring Initialization, Abort



    bool Continue = true;

    printf("Starting timers ...");

    /*
	Timer* FPSTimer = new Timer(20);
    Timer* GameTimer = new Timer(20);
    Timer* UITimer = new Timer(20);
    Timer* RenderTimer = new Timer(20);

    //uint8_t FrameCounter = 0;
    uint32_t FrameRate = 0;
    uint32_t GameRate = 0;

    GameTimer->Start();
    RenderTimer->Start();
	*/

    //RENDERER->WipeScreen();

    printf("Init done. Entering main loop.\n");
	while(Continue) // While program isn't done
	{
		//FPSTimer->Start();

            Continue = INPUT->frameStarted();


                // Calculate time since last frame and remember current time for next frame
                //unsigned long lTimeCurrentFrame = mRoot->getTimer()->getMilliseconds();
                //unsigned long lTimeSinceLastFrame = lTimeCurrentFrame - lTimeLastFrame;
                //lTimeLastFrame = lTimeCurrentFrame;


                if (GAME)
                {
                    GAME->Run();
                }

            //GameTimer->Unpause();
                //GAME->Run();
            //GameTimer->Pause();

            //RenderTimer->Unpause();
                //RENDERER->Render();
				RENDERER->RenderFrame();


                //RENDERER->CaptureScreenShot();
                //RENDERER->Flip();

            //RenderTimer->Pause();

		//FPSTimer->Pause(); // FrameRate Captures whole loop

        //FrameRate = (uint32_t) FrameRateControl(FPSTimer);
        //GameRate = (uint32_t) GameRateControl(FPSTimer);

	} // Program done, exiting

	CleanUp();

	return 0;
}



/*
void GeomCore::run( void )
{
   bShutdownSignal = false;
   unsigned long lTimeLastFrame = 0;

   mRoot->getRenderSystem()->_initRenderTargets();

   Ogre::LogManager::getSingletonPtr()->logMessage("Starting Geom main routine...");

    while( !bShutdownSignal )
     {
        // Calculate time since last frame and remember current time for next frame
        unsigned long lTimeCurrentFrame = mRoot->getTimer()->getMilliseconds();
        unsigned long lTimeSinceLastFrame = lTimeCurrentFrame - lTimeLastFrame;
        lTimeLastFrame = lTimeCurrentFrame;

        // Update inputmanager
        //mInputMgr->capture();

      Ogre::WindowEventUtilities::messagePump();

        // Render next frame
        if(!mRoot->_fireFrameStarted())
         fireShutdownSignal();

        mRoot->getRenderSystem()->_updateAllRenderTargets();

        if(!mRoot->_fireFrameEnded())
         fireShutdownSignal();
    }
}
*/
