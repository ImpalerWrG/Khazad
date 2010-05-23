#ifndef GUI__HEADER
#define GUI__HEADER

#include <stdafx.h>
#include <Singleton.h>

#include <OIS.h>
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"

//using namespace Ogre;


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

    MyGUI::Gui* getGUI()            { return MyGUI; }

private:

    MyGUI::Gui* MyGUI;
    MyGUI::OgrePlatform* GUIPlatform;

    bool ContinueRunning;
};

#define GUI (GUIManager::GetInstance())

#endif  // GUI__HEADER
