#include <SplashScreen.h>

#include <GUI.h>


SplashScreen::SplashScreen()
{

}

bool SplashScreen::Init()
{
    RootWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout("SplashScreen.layout");
    //CEGUI::System::getSingleton().setGUISheet(RootWindow);



    GUI->getWindowManager()->getWindow("SplashScreen/ExitButton")->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&SplashScreen::ExitPressed, this));
    GUI->getWindowManager()->getWindow("SplashScreen/NewGameButton")->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&SplashScreen::NewGamePressed, this));
}

SplashScreen::~SplashScreen()
{
    delete RootWindow;
}

void SplashScreen::SetDirty()
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
    GUI->ShowScreen(SCREEN_GAME_SETUP);
}
