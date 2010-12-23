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

    try
    {
        CEGUI::Spinner* XSpinner = static_cast<CEGUI::Spinner*> (GUI->getWindowManager()->getWindow("GameSetupScreenWindow/XSpinnerWindow/XSpinner"));
        if (XSpinner != NULL)
        {
            XSpinner->setMinimumValue(1.0f);
            XSpinner->setCurrentValue(5.0f);
            XSpinner->setTextInputMode(CEGUI::Spinner::Integer);
            XSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&GameSetupScreen::UpdateMapArea, this));
        }

        CEGUI::Spinner* YSpinner = static_cast<CEGUI::Spinner*> (GUI->getWindowManager()->getWindow("GameSetupScreenWindow/YSpinnerWindow/YSpinner"));
        if (YSpinner != NULL)
        {
            YSpinner->setMinimumValue(1.0f);
            YSpinner->setCurrentValue(5.0f);
            YSpinner->setTextInputMode(CEGUI::Spinner::Integer);
            YSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&GameSetupScreen::UpdateMapArea, this));
        }

        CEGUI::Editbox* SeedEditBox = static_cast<CEGUI::Editbox*> (GUI->getWindowManager()->getWindow("GameSetupScreenWindow/SeedWindow/SeedEditBox"));
        if (SeedEditBox != NULL)
        {
            SeedEditBox->setReadOnly(false);
            SeedEditBox->setTextMasked(false);
        }

        CEGUI::Window* BeginGameButton = GUI->getWindowManager()->getWindow("GameSetupScreenWindow/BeginGameButton");
        if (BeginGameButton != NULL)
        {
            BeginGameButton->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&GameSetupScreen::BeginGamePressed, this));
        }
    }

    catch(CEGUI::Exception &e)
    {
        // An exception!!!!
    }

    CEGUI::EventArgs Dummy;
    UpdateMapArea(Dummy);
}

void GameSetupScreen::SetDirty()
{

}

GameSetupScreen::~GameSetupScreen()
{

}

bool GameSetupScreen::UpdateMapArea(const CEGUI::EventArgs& pEventArgs)
{
    CEGUI::Spinner* Xspinner = static_cast<CEGUI::Spinner*> (GUI->getWindowManager()->getWindow("GameSetupScreenWindow/XSpinnerWindow/XSpinner"));
    CEGUI::Spinner* Yspinner = static_cast<CEGUI::Spinner*> (GUI->getWindowManager()->getWindow("GameSetupScreenWindow/YSpinnerWindow/YSpinner"));

    uint16_t Area = Xspinner->getCurrentValue() * Yspinner->getCurrentValue();
    char buffer[32];

    sprintf(buffer, "%i", Area);
    CEGUI::Window* AreaDisplay = GUI->getWindowManager()->getWindow("GameSetupScreenWindow/MapAreaWindow/AreaDisplayValue");
    AreaDisplay->setText(buffer);
}

bool GameSetupScreen::BeginGamePressed(const CEGUI::EventArgs& pEventArgs)
{
    CEGUI::Spinner* Xspinner = static_cast<CEGUI::Spinner*> (GUI->getWindowManager()->getWindow("GameSetupScreenWindow/XSpinnerWindow/XSpinner"));
    CEGUI::Spinner* Yspinner = static_cast<CEGUI::Spinner*> (GUI->getWindowManager()->getWindow("GameSetupScreenWindow/YSpinnerWindow/YSpinner"));

    CEGUI::Editbox* SeedEditBox = static_cast<CEGUI::Editbox*> (GUI->getWindowManager()->getWindow("GameSetupScreenWindow/SeedWindow/SeedEditBox"));


    GAME->CreateInstance();
    GAME->Init(Xspinner->getCurrentValue(), Yspinner->getCurrentValue(), SeedEditBox->getText().c_str());

    GUI->ShowScreen(SCREEN_MAIN_GAME);
}
