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


/*
    CEGUI::Window* wnd = (CEGUI::DefaultWindow*) GUI->getWindowManager()->createWindow("Vanilla/StaticText", (CEGUI::utf8*)"MyWindow");

    wnd->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0.0f), CEGUI::UDim(0.0f, 0.0f)));
    wnd->setSize(CEGUI::UVector2(CEGUI::UDim(0.4f, 10.0f), CEGUI::UDim(0.4f, 10.0f)));
    //CEGUI::DefaultWindow* staticText = static_cast<CEGUI::DefaultWindow*> (GUI->getWindowManager()->getWindow("StaticText"));
    wnd->setText("Red Static Text");
    wnd->show();
    */

}

MainGameScreen::~MainGameScreen()
{

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
