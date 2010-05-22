#include <GUI.h>

#include <SplashScreen.h>

using namespace MyGUI;


bool GUI::Init(Ogre::Root* OgreRoot, Ogre::SceneManager* OgreSceneManager)
{
    MyGUI = new MyGUI::Gui();
    GUIPlatform = new MyGUI::OgrePlatform();

    Ogre::RenderWindow* Window = OgreRoot->getAutoCreatedWindow();
    Ogre::SceneManager* Manager = OgreSceneManager;

    GUIPlatform->initialise(Window, Manager);
    MyGUI->initialise();

    ContinueRunning = true;


    // Initialize Screens and stuff here

    SplashScreen* NewSplashScreen = new SplashScreen(this);
    NewSplashScreen->Init();
}

GUI::~GUI()
{
    MyGUI->shutdown();
    delete MyGUI;

    GUIPlatform->shutdown();
    delete GUIPlatform;
}


bool GUI::injectMouseMove(int X, int Y, int Z)
{
    return MyGUI->injectMouseMove(X, Y, Z);
}

bool GUI::injectMousePress(int X, int Y, OIS::MouseButtonID ID)
{
    return MyGUI->injectMousePress(X, Y, MyGUI::MouseButton::Enum(ID));
}

bool GUI::injectMouseRelease(int X, int Y, OIS::MouseButtonID ID)
{
    return MyGUI->injectMouseRelease(X, Y, MyGUI::MouseButton::Enum(ID));
}


bool GUI::injectKeyPress(OIS::KeyEvent Key)
{
    if (Key.key == OIS::KC_ESCAPE)
    {
        ContinueRunning = false;
    }

    return MyGUI->injectKeyPress(MyGUI::KeyCode::Enum(Key.key), Key.text);
}

bool GUI::injectKeyRelease(OIS::KeyEvent Key)
{
    return MyGUI->injectKeyRelease(MyGUI::KeyCode::Enum(Key.key));
}
