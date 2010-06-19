#include <GameSetup.h>

#include <GUI.h>

#include <Game.h>
#include <Renderer.h>


GameSetupScreen::GameSetupScreen()
{

}

bool GameSetupScreen::Init()
{
    RootWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout("GameSetupScreen.layout");
    //CEGUI::System::getSingleton().setGUISheet(RootWindow);


    GUI->getWindowManager()->getWindow("GameSetupScreen/BeginGameButton")->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&GameSetupScreen::BeginGamePressed, this));
}

GameSetupScreen::~GameSetupScreen()
{

}

bool GameSetupScreen::BeginGamePressed(const CEGUI::EventArgs& pEventArgs)
{
    GAME->CreateInstance();
    GAME->Init();

    GUI->ShowScreen(SCREEN_MAIN_GAME);

	//RENDERER->getWindow()->resetStatistics();
}
