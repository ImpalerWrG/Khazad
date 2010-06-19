#include <MainGameScreen.h>

#include <GUI.h>


MainGameScreen::MainGameScreen()
{

}

bool MainGameScreen::Init()
{
    RootWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout("MainGameScreen.layout");


    GUI->getWindowManager()->getWindow("MainGameScreen/ExitButton")->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::ExitPressed, this));
}

MainGameScreen::~MainGameScreen()
{

}

bool MainGameScreen::ExitPressed(const CEGUI::EventArgs& pEventArgs)
{
    GUI->TerminateRunning();
}
