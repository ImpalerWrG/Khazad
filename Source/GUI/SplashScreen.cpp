#include <SplashScreen.h>

#include <GUI.h>

#include <boost/filesystem.hpp>

SplashScreen::SplashScreen()
{

}

bool SplashScreen::Init()
{
    RootWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout("SplashScreen.layout");
    //CEGUI::System::getSingleton().setGUISheet(RootWindow);

	GUI->getWindowManager()->getWindow("SplashScreen/LoadGameWindow")->setVisible(false);

    GUI->getWindowManager()->getWindow("SplashScreen/ExitButton")->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&SplashScreen::ExitPressed, this));
    GUI->getWindowManager()->getWindow("SplashScreen/NewGameButton")->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&SplashScreen::NewGamePressed, this));
    GUI->getWindowManager()->getWindow("SplashScreen/LoadGameButton")->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&SplashScreen::LoadGamePressed, this));
    GUI->getWindowManager()->getWindow("SplashScreen/LoadGameWindow/LoadButton")->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&SplashScreen::LoadGame, this));
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

bool SplashScreen::LoadGamePressed(const CEGUI::EventArgs& pEventArgs)
{
	CEGUI::Window* FileListWindow = GUI->getWindowManager()->getWindow("SplashScreen/LoadGameWindow/FileList");
	CEGUI::Listbox* FileListBox = (CEGUI::Listbox*) FileListWindow;

	boost::filesystem::path savdirectiory("Saves");
	if (boost::filesystem::is_directory(savdirectiory))
	{
		vector<boost::filesystem::path> Files;
		copy(boost::filesystem::directory_iterator(savdirectiory), boost::filesystem::directory_iterator(), back_inserter(Files));

		for (vector<boost::filesystem::path>::const_iterator it = Files.begin(); it != Files.end(); it++)
		{
			CEGUI::ListboxTextItem* newItem = new CEGUI::ListboxTextItem(it->filename().string());
			FileListBox->addItem(newItem);
		}
	}

	GUI->getWindowManager()->getWindow("SplashScreen/LoadGameWindow")->setVisible(true);
}

bool SplashScreen::LoadGame(const CEGUI::EventArgs& pEventArgs)
{

}

bool SplashScreen::ProcessKeyPress(OIS::KeyEvent Event)
{
    return true;
}

bool SplashScreen::ProcessKeyRelease(OIS::KeyEvent Event)
{
    return true;
}
