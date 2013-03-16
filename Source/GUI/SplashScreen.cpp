#include <SplashScreen.h>

#include <GUI.h>
#include <Game.h>
#include <Map.h>
#include <Face.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/archive/binary_iarchive.hpp>

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

	GUI->getWindowManager()->getWindow("SplashScreen/LoadGameWindow/FileList")->subscribeEvent(CEGUI::Listbox::EventSelectionChanged, CEGUI::Event::Subscriber(&SplashScreen::SaveFileSelected, this));
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

	CEGUI::colour* ListItemColour = new CEGUI::colour(0, 0, 0);
	CEGUI::colour* ListItemSelectColor = new CEGUI::colour(0, 0, 0);
	CEGUI::colour* ListItemSelectedBackgroundColour = new CEGUI::colour(0, 0, 0, 0);

	boost::filesystem::path savdirectiory("Saves");
	if (boost::filesystem::is_directory(savdirectiory))
	{
		vector<boost::filesystem::path> Files;
		copy(boost::filesystem::directory_iterator(savdirectiory), boost::filesystem::directory_iterator(), back_inserter(Files));

		for (vector<boost::filesystem::path>::const_iterator it = Files.begin(); it != Files.end(); it++)
		{
			if (true)  // Test if Khazad save extension Khav (Khazad-Save)
			{
				CEGUI::ListboxTextItem* newItem = new CEGUI::ListboxTextItem(it->filename().string());
				boost::filesystem::path PathPointer = it->filename();
				boost::filesystem::path* SavedPath = new boost::filesystem::path(savdirectiory / PathPointer);
				// TODO harmless memory lead of paths here, remove?
				newItem->setUserData(SavedPath);

				newItem->setTextColours(ListItemColour->getARGB());
				newItem->setSelectionColours(ListItemSelectedBackgroundColour->getARGB());
				newItem->setSelectionBrushImage("ArkanaLook", "ListSelectionBrush");
				FileListBox->addItem(newItem);
			}
		}
	}

	CEGUI::Window* LoadWindow = GUI->getWindowManager()->getWindow("SplashScreen/LoadGameWindow");
	LoadWindow->setVisible(true);
	LoadWindow->setAlwaysOnTop(true);
}

bool SplashScreen::LoadGame(const CEGUI::EventArgs& pEventArgs)
{
	CEGUI::Window* FileListWindow = GUI->getWindowManager()->getWindow("SplashScreen/LoadGameWindow/FileList");
	CEGUI::Listbox* FileListBox = (CEGUI::Listbox*) FileListWindow;

	CEGUI::ListboxItem* selectedItem = FileListBox->getFirstSelectedItem();
	CEGUI::ListboxTextItem* TextItem = (CEGUI::ListboxTextItem*) selectedItem;
	boost::filesystem::path* LoadPath = (boost::filesystem::path*) TextItem->getUserData();
	boost::filesystem::path testpath = *LoadPath;

	try {
		boost::filesystem::basic_ifstream<char> rawStream(*LoadPath, std::ios::binary);
		if (rawStream.is_open())
		{
			boost::archive::binary_iarchive LoadArchive(rawStream);
			Game* THEGAME = GAME->GetInstance();
			LoadArchive >> THEGAME;

			GUI->ShowScreen(SCREEN_MAIN_GAME);
			GAME->getMap()->RegisterWithRendering();
		}
	} catch (const std::exception& e) {
		// Report Exception
	}
}

bool SplashScreen::SaveFileSelected(const CEGUI::EventArgs& pEventArgs)
{
	CEGUI::colour* ListItemSelectColor = new CEGUI::colour(1, 0, 0, 0.8);

	CEGUI::Window* FileListWindow = GUI->getWindowManager()->getWindow("SplashScreen/LoadGameWindow/FileList");
	CEGUI::Listbox* FileListBox = (CEGUI::Listbox*) FileListWindow;

	CEGUI::ListboxTextItem* selectedItem = (CEGUI::ListboxTextItem*) FileListBox->getFirstSelectedItem();
	selectedItem->setTextColours(ListItemSelectColor->getARGB());


	// TELL US SOME INFO ABOUT THE SAVE GAME, Last date played, name of game etc etc
	return true;
}

bool SplashScreen::ProcessKeyPress(OIS::KeyEvent Event)
{
    return true;
}

bool SplashScreen::ProcessKeyRelease(OIS::KeyEvent Event)
{
    return true;
}

