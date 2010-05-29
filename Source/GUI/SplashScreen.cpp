#include <SplashScreen.h>

#include <GUI.h>

#include <Game.h>
#include <Renderer.h>
using namespace MyGUI;


SplashScreen::SplashScreen()
{

}

bool SplashScreen::Init()
{
    const MyGUI::IntSize & view = MyGUI::Gui::getInstance().getViewSize();


    PanelWidgit = GUI->getGUI()->createWidget<MyGUI::Widget>("Button", MyGUI::IntCoord((view.width - 200) / 2, (view.height - 100) * 2 / 3, 200, 100), MyGUI::Align::HCenter, "Main");


    MyGUI::ButtonPtr Newbutton = PanelWidgit->createWidgetReal<MyGUI::Button>("Button", .10, 1.0/7, .80, 2.0/7, MyGUI::Align::HCenter, "Main");
    Newbutton->setCaption("NEW GAME");
    Newbutton->eventMouseButtonClick = MyGUI::newDelegate(this, &SplashScreen::NewPressed);

    MyGUI::ButtonPtr Exitbutton = PanelWidgit->createWidgetReal<MyGUI::Button>("Button", .10, 4.0/7, .80, 2.0/7, MyGUI::Align::HCenter, "Main");
    Exitbutton->setCaption("EXIT");
    Exitbutton->eventMouseButtonClick = MyGUI::newDelegate(this, &SplashScreen::ExitPressed);

    const MyGUI::IntSize size(200, 200);
    LogoImage = GUI->getGUI()->createWidget<MyGUI::StaticImage>("StaticImage", MyGUI::IntCoord((view.width - size.width) / 2, (view.height - size.height) / 4, size.width, size.height), MyGUI::Align::Default, "Main");
    LogoImage->setImageTexture("KhazadLogo.png");
	return true;
}

SplashScreen::~SplashScreen()
{

}

void SplashScreen::ExitPressed(MyGUI::WidgetPtr SourceWidget)
{
    GUI->TerminateRunning();
}

void SplashScreen::NewPressed(MyGUI::WidgetPtr SourceWidget)
{
    LogoImage->setVisible(false);
    PanelWidgit->setVisible(false);

    GAME->CreateInstance();
    GAME->Init();
	RENDERER->getWindow()->resetStatistics();
}
