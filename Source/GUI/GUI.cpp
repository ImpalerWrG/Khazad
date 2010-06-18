#include <GUI.h>

#include <Singleton.h>

#include <SplashScreen.h>
#include <FPS.h>
#include <Renderer.h>

#include <Game.h>

DECLARE_SINGLETON(GUIManager)

GUIManager::GUIManager()
{
	ContinueRunning = true;

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

	// Initialize Screens and stuff here

	SplashScreen* NewSplashScreen = new SplashScreen();
	NewSplashScreen->Init();

	NewFPSDisplay = new FPSDisplay();
	NewFPSDisplay->Init();

	return true;
}

GUIManager::~GUIManager()
{
	//MyGUI->shutdown();
	//delete MyGUI;

	//GUIPlatform->shutdown();
	//delete GUIPlatform;
}

void GUIManager::ShowScreen(ScreenBase* TargetScreen)
{
    ActiveScreen = TargetScreen;
    CEGUISystem->setGUISheet(TargetScreen->getRootWindow());
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


bool GUIManager::injectKeyPress(OIS::KeyEvent Key)
{
	if (Key.key == OIS::KC_ESCAPE)
	{
		ContinueRunning = false;
	}

    return CEGUISystem->injectKeyDown(Key.key);
}

bool GUIManager::injectKeyRelease(OIS::KeyEvent Key)
{
    return CEGUISystem->injectKeyUp(Key.key);
}
