#include <GameSetup.h>

#include <GUI.h>

#include <Game.h>


GameSetup::GameSetup()
{

}

bool GameSetup::Init()
{
    /*
	const MyGUI::IntSize & view = MyGUI::Gui::getInstance().getViewSize();


	PanelWidgit = GUI->getGUI()->createWidget<MyGUI::Widget>("Button", MyGUI::IntCoord((view.width - 200) / 2, (view.height - 100) * 2 / 3, 200, 100), MyGUI::Align::HCenter, "Main");


	MyGUI::ButtonPtr Newbutton = PanelWidgit->createWidgetReal<MyGUI::Button>("Button", .10, 1.0/7, .80, 2.0/7, MyGUI::Align::HCenter, "Main");
	Newbutton->setCaption("NEW GAME");
	Newbutton->eventMouseButtonClick = MyGUI::newDelegate(this, &GameSetup::NewPressed);

	MyGUI::ButtonPtr Exitbutton = PanelWidgit->createWidgetReal<MyGUI::Button>("Button", .10, 4.0/7, .80, 2.0/7, MyGUI::Align::HCenter, "Main");
	Exitbutton->setCaption("CANCEL");
	Exitbutton->eventMouseButtonClick = MyGUI::newDelegate(this, &GameSetup::ExitPressed);


	const MyGUI::IntSize size(200, 200);
	LogoImage = GUI->getGUI()->createWidget<MyGUI::StaticImage>("StaticImage", MyGUI::IntCoord((view.width - size.width) / 2, (view.height - size.height) / 4, size.width, size.height), MyGUI::Align::Default, "Main");
	LogoImage->setImageTexture("KhazadLogo.png");
	return true;
	*/
}

GameSetup::~GameSetup()
{

}

void GameSetup::ExitPressed()
{

}

void GameSetup::NewPressed()
{
	//LogoImage->setVisible(false);
	//PanelWidgit->setVisible(false);
}
