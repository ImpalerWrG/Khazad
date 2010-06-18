#ifndef GUI__HEADER
#define GUI__HEADER

#include <stdafx.h>
#include <Singleton.h>

#include <OIS.h>

#include "CEGUI.h"
#include <CEGUIOgreRenderer.h>

//#include "MyGUI_OgrePlatform.h"
#include <FPS.h>

#include <ScreenBase.h>


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

    void ShowScreen(ScreenBase* TargetScreen);

	FPSDisplay* NewFPSDisplay;

	CEGUI::WindowManager* getWindowManager()        { return CEGUIWindowManager; }
	CEGUI::System* getSystem()                      { return CEGUISystem; }

private:

    ScreenBase* ActiveScreen;

    CEGUI::OgreRenderer* GUIRenderer;
    CEGUI::System* CEGUISystem;
    CEGUI::WindowManager* CEGUIWindowManager;

    bool ContinueRunning;
};

#define GUI (GUIManager::GetInstance())

#endif  // GUI__HEADER
