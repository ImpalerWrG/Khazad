#include <GUI.h>

#include <Singleton.h>

#include <SplashScreen.h>
#include <Renderer.h>

using namespace MyGUI;

DECLARE_SINGLETON(GUIManager)

GUIManager::GUIManager()
{
    ContinueRunning = true;
}

bool GUIManager::Init()
{
    MyGUI = new MyGUI::Gui();
    GUIPlatform = new MyGUI::OgrePlatform();

    GUIPlatform->initialise(RENDERER->getWindow(), RENDERER->getSceneManager());
    MyGUI->initialise();

    // Initialize Screens and stuff here

    SplashScreen* NewSplashScreen = new SplashScreen(this);
    NewSplashScreen->Init();
}

GUIManager::~GUIManager()
{
    MyGUI->shutdown();
    delete MyGUI;

    GUIPlatform->shutdown();
    delete GUIPlatform;
}


bool GUIManager::injectMouseMove(int X, int Y, int Z)
{
    return MyGUI->injectMouseMove(X, Y, Z);
}

bool GUIManager::injectMousePress(int X, int Y, OIS::MouseButtonID ID)
{
    return MyGUI->injectMousePress(X, Y, MyGUI::MouseButton::Enum(ID));
}

bool GUIManager::injectMouseRelease(int X, int Y, OIS::MouseButtonID ID)
{
    return MyGUI->injectMouseRelease(X, Y, MyGUI::MouseButton::Enum(ID));
}


bool GUIManager::injectKeyPress(OIS::KeyEvent Key)
{
    if (Key.key == OIS::KC_ESCAPE)
    {
        ContinueRunning = false;
    }

    return MyGUI->injectKeyPress(MyGUI::KeyCode::Enum(Key.key), Key.text);
}

bool GUIManager::injectKeyRelease(OIS::KeyEvent Key)
{
    return MyGUI->injectKeyRelease(MyGUI::KeyCode::Enum(Key.key));
}
