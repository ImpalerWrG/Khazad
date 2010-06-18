#include <SplashScreen.h>

#include <GUI.h>

#include <Game.h>
#include <Renderer.h>


SplashScreen::SplashScreen()
{

}

bool SplashScreen::Init()
{
    CEGUI::Window* RootWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout("SplashScreen.layout");
    CEGUI::System::getSingleton().setGUISheet(RootWindow);



    GUI->getWindowManager()->getWindow("SplashScreen/ExitButton")->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&SplashScreen::ExitPressed, this));
    GUI->getWindowManager()->getWindow("SplashScreen/NewGameButton")->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&SplashScreen::NewGamePressed, this));
}

SplashScreen::~SplashScreen()
{

}

bool SplashScreen::ExitPressed(const CEGUI::EventArgs& pEventArgs)
{
    GUI->TerminateRunning();
}

bool SplashScreen::OptionsPressed(const CEGUI::EventArgs& pEventArgs)
{

}

bool SplashScreen::NewGamePressed(const CEGUI::EventArgs& pEventArgs)
{
    //LogoImage->setVisible(false);
    //PanelWidgit->setVisible(false);

    GAME->CreateInstance();
    GAME->Init();
	RENDERER->getWindow()->resetStatistics();
}
