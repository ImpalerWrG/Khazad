#include <MainGameScreen.h>

#include <GUI.h>
#include <InputManager.h>
#include <ImageManager.h>
#include <Renderer.h>
#include <Camera.h>
#include <Zone.h>
#include <Game.h>
#include <Map.h>

#include <PathManager.h>

MainGameScreen::MainGameScreen()
{

}

bool MainGameScreen::Init()
{
    RootWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout("MainGameScreen.layout");

    try
    {
        CEGUI::Window* GameOptionsButton = GUI->getWindowManager()->getWindow("MainGameScreen/TopBar/GameOptionsButton");
        if (GameOptionsButton != NULL)
        {
            GameOptionsButton->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::GameOptionsOpen, this));
        }

        CEGUI::Window* GameOptionsWindow = GUI->getWindowManager()->getWindow("MainGameScreen/GameOptionsWindow");
        if (GameOptionsWindow != NULL)
        {
            GameOptionsWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&MainGameScreen::GameOptionsClose, this));
        }

        CEGUI::Window* ZoneButton = GUI->getWindowManager()->getWindow("MainGameScreen/TopBar/ZoneButton");
        if (ZoneButton != NULL)
        {
            ZoneButton->subscribeEvent(CEGUI::FrameWindow::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::ZoneButtonClick, this));
        }

        CEGUI::Window* ExitButton = GUI->getWindowManager()->getWindow("MainGameScreen/GameOptionsWindow/ExitButton");
        if (ExitButton != NULL)
        {
            ExitButton->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::ExitGame, this));
        }

        CEGUI::Scrollbar* DepthSliderTop = static_cast<CEGUI::Scrollbar*> (GUI->getWindowManager()->getWindow("MainGameScreen/DepthScrollerTop"));
        if (DepthSliderTop != NULL)
        {
            DepthSliderTop->setDocumentSize(1000);
            DepthSliderTop->setScrollPosition(0);
            DepthSliderTop->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MainGameScreen::DepthSliderMoved, this));
        }

        CEGUI::Scrollbar* DepthSliderBottom = static_cast<CEGUI::Scrollbar*> (GUI->getWindowManager()->getWindow("MainGameScreen/DepthScrollerBottom"));
        if (DepthSliderBottom != NULL)
        {
            DepthSliderBottom->setDocumentSize(1000);
            DepthSliderBottom->setScrollPosition(1000);
            DepthSliderBottom->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MainGameScreen::DepthSliderMoved, this));
        }

        CEGUI::Window* PauseButton = GUI->getWindowManager()->getWindow("MainGameScreen/TopBar/PauseButton");
        if (PauseButton != NULL)
        {
            PauseButton->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::MainGameScreen::PausePressed, this));
        }

        CEGUI::Window* PlayButton = GUI->getWindowManager()->getWindow("MainGameScreen/TopBar/PlayButton");
        if (PauseButton != NULL)
        {
            PlayButton->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::MainGameScreen::PlayPressed, this));
        }

        CEGUI::Window* DigButton = GUI->getWindowManager()->getWindow("MainGameScreen/BottomWindow/DigButton");
        if (DigButton != NULL)
        {
            DigButton->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::MainGameScreen::DigPressed, this));
        }

    }

    catch(CEGUI::Exception &e)
    {
    }
}

MainGameScreen::~MainGameScreen()
{

}

void MainGameScreen::SetDirty()
{
	/*
    DragSelection* ActiveSelection = INPUT->getActiveZone();

    if (ActiveSelection != NULL)
    {
        MapCoordinates Location = ActiveSelection->getLocation();

        char buffer[32];

        sprintf(buffer, "X %i  Y %i  Z %i", Location.X, Location.Y, Location.Z);
        CEGUI::Window* LocationDisplay = GUI->getWindowManager()->getWindow("MainGameScreen/SelectionWindow/LocationWindow/LocationDisplay");
        LocationDisplay->setText(buffer);

        CubeShape Shape = GAME->getMap()->getCubeShape(Location);

        sprintf(buffer, "SW%i SE%i NW%i NE%i ", Shape.SouthWestCorner, Shape.SouthEastCorner, Shape.NorthWestCorner, Shape.NorthEastCorner);
        CEGUI::Window* TileTypeDisplay = GUI->getWindowManager()->getWindow("MainGameScreen/SelectionWindow/TileTypeWindow/TileTypeDisplay");
        TileTypeDisplay->setText(buffer);

        uint32_t zone = GAME->getPath()->getZoneEquivilency(Location);
        sprintf(buffer, "Zone %i", zone);
        CEGUI::Window* TileFullDisplay = GUI->getWindowManager()->getWindow("MainGameScreen/SelectionWindow/TileFullnessDisplay");
        TileFullDisplay->setText(buffer);

    }*/

    CEGUI::Scrollbar* DepthSliderTop = static_cast<CEGUI::Scrollbar*> (GUI->getWindowManager()->getWindow("MainGameScreen/DepthScrollerTop"));
    if (DepthSliderTop != NULL)
    {
        int32_t ZMax = GAME->getMap()->getHighest() - GAME->getMap()->getLowest();
        DepthSliderTop->setDocumentSize(ZMax);
    }

    CEGUI::Scrollbar* DepthSliderBottom = static_cast<CEGUI::Scrollbar*> (GUI->getWindowManager()->getWindow("MainGameScreen/DepthScrollerBottom"));
    if (DepthSliderBottom != NULL)
    {
        int32_t ZMax = GAME->getMap()->getHighest() - GAME->getMap()->getLowest();
        float OldPosition = DepthSliderBottom->getScrollPosition();
        if (OldPosition > ZMax)
        {
            DepthSliderBottom->setDocumentSize(ZMax);
            DepthSliderBottom->setScrollPosition(ZMax);
        } else {
            DepthSliderBottom->setScrollPosition(ZMax);
        }
    }
}

bool MainGameScreen::GameOptionsOpen(const CEGUI::EventArgs& pEventArgs)
{
    CEGUI::Window* GameOptionsWindow = GUI->getWindowManager()->getWindow("MainGameScreen/GameOptionsWindow");
    GameOptionsWindow->setVisible(true);
    return true;
}

bool MainGameScreen::GameOptionsClose(const CEGUI::EventArgs& pEventArgs)
{
    CEGUI::Window* GameOptionsWindow = GUI->getWindowManager()->getWindow("MainGameScreen/GameOptionsWindow");
    GameOptionsWindow->setVisible(false);
    return true;
}

bool MainGameScreen::ExitGame(const CEGUI::EventArgs& pEventArgs)
{
    GUI->TerminateRunning();
}

bool MainGameScreen::DepthSliderMoved(const CEGUI::EventArgs& pEventArgs)
{
    CEGUI::Scrollbar* DepthSliderTop = static_cast<CEGUI::Scrollbar*> (GUI->getWindowManager()->getWindow("MainGameScreen/DepthScrollerTop"));
    CEGUI::Scrollbar* DepthSliderBottom = static_cast<CEGUI::Scrollbar*> (GUI->getWindowManager()->getWindow("MainGameScreen/DepthScrollerBottom"));

    if (DepthSliderTop != NULL && DepthSliderBottom != NULL)
    {
        int32_t ZMax = GAME->getMap()->getHighest() - GAME->getMap()->getLowest();
        RENDERER->getActiveCamera()->SetSlice(ZMax - DepthSliderTop->getScrollPosition(), ZMax - DepthSliderBottom->getScrollPosition());
    }
    return true;
}

bool MainGameScreen::PlayPressed(const CEGUI::EventArgs& pEventArgs)
{
    GAME->setPause(false);
    return true;
}

bool MainGameScreen::PausePressed(const CEGUI::EventArgs& pEventArgs)
{
    GAME->setPause(true);
    return true;
}

bool MainGameScreen::DigPressed(const CEGUI::EventArgs& pEventArgs)
{
    INPUT->setSelectionMode(true);
    return true;
}

bool MainGameScreen::ZoneButtonClick(const CEGUI::EventArgs& pEventArgs)
{
	INPUT->formZone();
	return true;
}

bool MainGameScreen::ProcessKeyPress(OIS::KeyEvent Event)
{
    switch (Event.key)
    {
        case OIS::KC_ESCAPE:
        {
            GameOptionsOpen(CEGUI::EventArgs());
            break;
        }

        default:
        break;
    }

    return true;
}

bool MainGameScreen::ProcessKeyRelease(OIS::KeyEvent Event)
{
    return true;
}
