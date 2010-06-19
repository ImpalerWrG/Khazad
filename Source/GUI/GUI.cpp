#include <GUI.h>

#include <Singleton.h>

#include <SplashScreen.h>
#include <GameSetup.h>
#include <MainGameScreen.h>

#include <FPS.h>
#include <Renderer.h>

#include <Game.h>

DECLARE_SINGLETON(GUIManager)

GUIManager::GUIManager()
{
	ContinueRunning = true;

    ActiveScreen = NO_SCREEN;
}

bool GUIManager::Init()
{
    GUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("Media/GUI/schemes", "FileSystem", "schemes");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("Media/GUI/imagesets", "FileSystem", "imagesets");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("Media/GUI/fonts", "FileSystem", "fonts");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("Media/GUI/layouts", "FileSystem", "layouts");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("Media/GUI/looknfeel", "FileSystem", "looknfeels");


    CEGUI::Imageset::setDefaultResourceGroup("imagesets");
    CEGUI::Font::setDefaultResourceGroup("fonts");
    CEGUI::Scheme::setDefaultResourceGroup("schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
    CEGUI::WindowManager::setDefaultResourceGroup("layouts");

    CEGUI::SchemeManager::getSingleton().create("VanillaSkin.scheme");

    CEGUISystem = &CEGUI::System::getSingleton();

    CEGUISystem->setDefaultMouseCursor("Vanilla-Images", "MouseArrow");
    //CEGUISystem->setDefaultFont("Blackletter-10");


    CEGUIWindowManager = &CEGUI::WindowManager::getSingleton();


    // Move mouse to center of screen??
    CEGUISystem->injectMousePosition(RENDERER->getWindow()->getWidth() / 2, RENDERER->getWindow()->getHeight() / 2);



	// Initialize Screens here matching them to their Enum

	ScreenList[SCREEN_SPLASH] = (ScreenBase*) new SplashScreen();
	ScreenList[SCREEN_SPLASH]->Init();

	ScreenList[SCREEN_GAME_SETUP] = (ScreenBase*) new GameSetupScreen();
	ScreenList[SCREEN_GAME_SETUP]->Init();

	ScreenList[SCREEN_MAIN_GAME] = (ScreenBase*) new MainGameScreen();
	ScreenList[SCREEN_MAIN_GAME]->Init();



    ShowScreen(SCREEN_SPLASH);

	return true;
}

GUIManager::~GUIManager()
{

}

void GUIManager::ShowScreen(ScreenType TargetScreen)
{
    if (TargetScreen != ActiveScreen)
    {
        ActiveScreen = TargetScreen;
        CEGUISystem->setGUISheet(ScreenList[TargetScreen]->getRootWindow());
    }
}

bool GUIManager::injectMouseMove(int X, int Y, int Z)
{
	CEGUISystem->injectMouseMove(X, Y);
	CEGUISystem->injectMouseWheelChange(Z);
}

bool GUIManager::injectMousePress(int X, int Y, OIS::MouseButtonID ID)
{
    CEGUISystem->injectMouseButtonDown((CEGUI::MouseButton)ID);
}

bool GUIManager::injectMouseRelease(int X, int Y, OIS::MouseButtonID ID)
{
    CEGUISystem->injectMouseButtonUp((CEGUI::MouseButton)ID);
}


bool GUIManager::injectKeyPress(OIS::KeyEvent Event)
{
    return CEGUISystem->injectKeyDown(Event.key);
}

bool GUIManager::injectKeyRelease(OIS::KeyEvent Event)
{
    CEGUISystem->injectChar(Event.text);

    return CEGUISystem->injectKeyUp(Event.key);
}
