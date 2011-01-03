#include <MainGameScreen.h>

#include <GUI.h>
#include <ImageManager.h>
#include <Renderer.h>
#include <Camera.h>
#include <Zone.h>
#include <Game.h>
#include <Map.h>


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

        CEGUI::Window* ExitButton = GUI->getWindowManager()->getWindow("MainGameScreen/GameOptionsWindow/ExitButton");
        if (ExitButton != NULL)
        {
            ExitButton->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::ExitGame, this));
        }

        CEGUI::Scrollbar* DepthSliderTop = static_cast<CEGUI::Scrollbar*> (GUI->getWindowManager()->getWindow("MainGameScreen/DepthScrollerTop"));
        if (DepthSliderTop != NULL)
        {
            DepthSliderTop->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MainGameScreen::DepthSliderMoved, this));
            DepthSliderTop->setScrollPosition(0);
        }

        CEGUI::Scrollbar* DepthSliderBottom = static_cast<CEGUI::Scrollbar*> (GUI->getWindowManager()->getWindow("MainGameScreen/DepthScrollerBottom"));
        if (DepthSliderBottom != NULL)
        {
            DepthSliderBottom->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MainGameScreen::DepthSliderMoved, this));
            DepthSliderBottom->setScrollPosition(100);
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
    Zone* ActiveZone = GAME->getMap()->getActiveZone();

    if (ActiveZone != NULL)
    {
        MapCoordinates Location = ActiveZone->getLocation();

        char buffer[32];

        sprintf(buffer, "X %i  Y %i  Z %i", Location.X, Location.Y, Location.Z);
        CEGUI::Window* LocationDisplay = GUI->getWindowManager()->getWindow("MainGameScreen/SelectionWindow/LocationWindow/LocationDisplay");
        LocationDisplay->setText(buffer);

        CubeShape Shape = GAME->getMap()->getCubeShape(Location);
        sprintf(buffer, "SW%i SE%i NW%i NE%i ", Shape.SouthWestCorner, Shape.SouthEastCorner, Shape.NorthWestCorner, Shape.NorthEastCorner);
        CEGUI::Window* TileTypeDisplay = GUI->getWindowManager()->getWindow("MainGameScreen/SelectionWindow/TileTypeWindow/TileTypeDisplay");
        TileTypeDisplay->setText(buffer);
    }

    CEGUI::Scrollbar* DepthSliderTop = static_cast<CEGUI::Scrollbar*> (GUI->getWindowManager()->getWindow("MainGameScreen/DepthScrollerTop"));
    if (DepthSliderTop != NULL)
    {
        int32_t ZMax = GAME->getMap()->getHighest() - GAME->getMap()->getLowest();
        DepthSliderTop->setDocumentSize(ZMax);
        //DepthSliderTop->setScrollPosition(0);
    }

    CEGUI::Scrollbar* DepthSliderBottom = static_cast<CEGUI::Scrollbar*> (GUI->getWindowManager()->getWindow("MainGameScreen/DepthScrollerBottom"));
    if (DepthSliderBottom != NULL)
    {
        int32_t ZMax = GAME->getMap()->getHighest() - GAME->getMap()->getLowest();
        DepthSliderBottom->setDocumentSize(ZMax);
        //DepthSliderBottom->setScrollPosition(ZMax);
    }
}

bool MainGameScreen::GameOptionsOpen(const CEGUI::EventArgs& pEventArgs)
{
    CEGUI::Window* GameOptionsWindow = GUI->getWindowManager()->getWindow("MainGameScreen/GameOptionsWindow");
    GameOptionsWindow->setVisible(true);
}

bool MainGameScreen::GameOptionsClose(const CEGUI::EventArgs& pEventArgs)
{
    CEGUI::Window* GameOptionsWindow = GUI->getWindowManager()->getWindow("MainGameScreen/GameOptionsWindow");
    GameOptionsWindow->setVisible(false);
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

        float Top = CEGUI::PropertyHelper::stringToFloat(DepthSliderTop->getProperty("ScrollPosition"));
        float Bottom = CEGUI::PropertyHelper::stringToFloat(DepthSliderBottom->getProperty("ScrollPosition"));

        RENDERER->getActiveCamera()->SetSlice(ZMax - Top, ZMax - Bottom);
        return true;
    }
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
