#include <stdafx.h>

#include <Ogre.h>

#include <DataManager.h>
#include <Renderer.h>
#include <InputManager.h>
#include <ImageManager.h>
#include <GUI.h>
#include <Game.h>


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
        Ogre::Log *Khazadlog = Ogre::LogManager::getSingleton().createLog("Khazad.log", false, true, false);

        Ogre::LogManager::getSingletonPtr()->logMessage("Starting Initialization routines...");

        // XML Data loading
        Khazadlog->logMessage("-=DATA INITIALIZING=- ...");
        DATA->CreateInstance();
        DATA->Init();
        Khazadlog->logMessage("DATA DONE");

        Khazadlog->logMessage("-=IMAGE INITIALIZING=- ...");
        IMAGE->CreateInstance();
        IMAGE->Init();
        Khazadlog->logMessage("IMAGE DONE");

        // OGRE Renderer
        Khazadlog->logMessage("-=RENDERER INITIALIZING=- ...");
        RENDERER->CreateInstance();
        RENDERER->InitializeRenderer();
        Khazadlog->logMessage("RENDERER DONE");

        // CEGUI with OGRE Platform
        Khazadlog->logMessage("-=GUI INITIALIZING=- ...");
        GUI->CreateInstance();
        GUI->Init();
        Khazadlog->logMessage("GUI DONE");

        // OIS mouse and keyboard injection into GUI
        Khazadlog->logMessage("-=INPUT INITIALIZING=- ...");
        INPUT->CreateInstance();
        INPUT->Init();
        Khazadlog->logMessage("INPUT DONE");



    bool Continue = true;
    Khazadlog->logMessage("Starting MainLoop ...");

	while(Continue) // While program isn't done
	{
        Ogre::WindowEventUtilities::messagePump();

        Continue = INPUT->frameStarted();

        if (Game::isInstance())
        {
            GAME->Run();
        }

        RENDERER->RenderFrame();

	} // Program done, exiting

    Khazadlog->logMessage("EXITING CLEANLY");

	return 0;
}

