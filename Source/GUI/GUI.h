#ifndef GUI__HEADER
#define GUI__HEADER

#include <stdafx.h>
#include <Singleton.h>

#include <OIS.h>

#include <CEGUI.h>
#include <CEGUIOgreRenderer.h>

#include <ScreenBase.h>

enum ScreenType
{
    SCREEN_SPLASH,
    SCREEN_OPTIONS,
    SCREEN_CREDITS,
    SCREEN_GAME_SETUP,
    SCREEN_MAIN_GAME,

    NUM_SCREENS = 5,
    NO_SCREEN,
};

class GUIManager
{
DECLARE_SINGLETON_CLASS(GUIManager)

public:

    bool Init();

    ~GUIManager();

    bool injectMouseMove(int X, int Y, int Z);
    bool injectMousePress(int X, int Y, OIS::MouseButtonID ID);
    bool injectMouseRelease(int X, int Y, OIS::MouseButtonID ID);

    bool injectKeyPress(OIS::KeyEvent Key);
    bool injectKeyRelease(OIS::KeyEvent Key);

    bool isContinueRunning()        { return ContinueRunning; }
    void TerminateRunning()         { ContinueRunning = false; }

    void ShowScreen(ScreenType);


	CEGUI::WindowManager* getWindowManager()        { return CEGUIWindowManager; }
	CEGUI::System* getSystem()                      { return CEGUISystem; }

private:

    CEGUI::OgreRenderer* GUIRenderer;
    CEGUI::System* CEGUISystem;
    CEGUI::WindowManager* CEGUIWindowManager;

    ScreenBase* ScreenList [NUM_SCREENS];
    ScreenType ActiveScreen;

    bool ContinueRunning;
};

#define GUI (GUIManager::GetInstance())

#endif  // GUI__HEADER
