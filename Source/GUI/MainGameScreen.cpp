#include <MainGameScreen.h>

#include <GUI.h>
#include <ImageManager.h>
#include <Renderer.h>
#include <Camera.h>


MainGameScreen::MainGameScreen()
{

}

bool MainGameScreen::Init()
{
    RootWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout("MainGameScreen.layout");

    try
    {
        CEGUI::Window* ExitButton = GUI->getWindowManager()->getWindow("MainGameScreen/ExitButton");
        if (ExitButton != NULL)
        {
            ExitButton->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::ExitPressed, this));
        }

        CEGUI::Window* DepthSlider = GUI->getWindowManager()->getWindow("MainGameScreen/DepthScroller");
        if (DepthSlider != NULL)
        {
            DepthSlider->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MainGameScreen::DepthSliderMoved, this));
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
    CEGUI::Window* Title = GUI->getWindowManager()->getWindow("MainGameScreen/Title");
    Title->setText( "Hello World!" );

}

bool MainGameScreen::ExitPressed(const CEGUI::EventArgs& pEventArgs)
{
    GUI->TerminateRunning();
}

bool MainGameScreen::DepthSliderMoved(const CEGUI::EventArgs& pEventArgs)
{
    CEGUI::WindowEventArgs* windowArgs = (CEGUI::WindowEventArgs*) &pEventArgs;
    CEGUI::Scrollbar* Source = (CEGUI::Scrollbar*) windowArgs->window;


    //float position = Source->getScrollPosition();

    //RENDERER->getActiveCamera()->ElevateCamera(1);

}
