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
#include <PathTester.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/filtering_stream.hpp>

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

        CEGUI::Window* PathTesterWindow = GUI->getWindowManager()->getWindow("MainGameScreen/PathProfileWindow");
        if (PathTesterWindow != NULL)
        {
            PathTesterWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&MainGameScreen::PathProfilerClose, this));
        }

        CEGUI::Window* ZoneButton = GUI->getWindowManager()->getWindow("MainGameScreen/TopBar/ZoneButton");
        if (ZoneButton != NULL)
        {
            ZoneButton->subscribeEvent(CEGUI::FrameWindow::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::ZoneButtonClick, this));
        }

        CEGUI::Window* SaveButton = GUI->getWindowManager()->getWindow("MainGameScreen/GameOptionsWindow/SaveGameButton");
        if (SaveButton != NULL)
        {
            SaveButton->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::SaveGame, this));
        }

        CEGUI::Window* PathProfileButton = GUI->getWindowManager()->getWindow("MainGameScreen/GameOptionsWindow/PathProfilerButton");
        if (PathProfileButton != NULL)
        {
            PathProfileButton->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::PathProfilerOpen, this));
        }

        CEGUI::Window* ExitButton = GUI->getWindowManager()->getWindow("MainGameScreen/GameOptionsWindow/ExitButton");
        if (ExitButton != NULL)
        {
            ExitButton->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::ExitGame, this));
        }

        CEGUI::Window* TestSuiteButton = GUI->getWindowManager()->getWindow("MainGameScreen/PathProfileWindow/RunSuiteButton");
        if (TestSuiteButton != NULL)
        {
            TestSuiteButton->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MainGameScreen::RunSuitePressed, this));
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
	resetDeepthSliders();
}

void MainGameScreen::resetDeepthSliders()
{
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
    GAME->setPause(true);
    CEGUI::Window* GameOptionsWindow = GUI->getWindowManager()->getWindow("MainGameScreen/GameOptionsWindow");
    GameOptionsWindow->setVisible(true);
    GameOptionsWindow->moveToFront();
    return true;
}

bool MainGameScreen::GameOptionsClose(const CEGUI::EventArgs& pEventArgs)
{
    CEGUI::Window* GameOptionsWindow = GUI->getWindowManager()->getWindow("MainGameScreen/GameOptionsWindow");
    GameOptionsWindow->setVisible(false);
    return true;
}

bool MainGameScreen::PathProfilerOpen(const CEGUI::EventArgs& pEventArgs)
{
    GAME->setPause(true);
    CEGUI::Window* PathProfileWindow = GUI->getWindowManager()->getWindow("MainGameScreen/PathProfileWindow");
    PathProfileWindow->setVisible(true);
    PathProfileWindow->moveToFront();
    return true;
}

bool MainGameScreen::PathProfilerClose(const CEGUI::EventArgs& pEventArgs)
{
    CEGUI::Window* PathProfileWindow = GUI->getWindowManager()->getWindow("MainGameScreen/PathProfileWindow");
    PathProfileWindow->setVisible(false);
    return true;
}

bool MainGameScreen::ExitGame(const CEGUI::EventArgs& pEventArgs)
{
    GUI->TerminateRunning();
}

bool MainGameScreen::SaveGame(const CEGUI::EventArgs& pEventArgs)
{
	string filename("FirstSave.khav");

	try {
		boost::filesystem::path SaveDirectory("Saves");
		boost::filesystem::path SaveFile(SaveDirectory / filename);
		boost::filesystem::basic_ofstream<char> rawStream(SaveFile, std::ios::binary);
		if (rawStream.is_open())
		{
			//boost::iostreams::filtering_ostream Compressedstream;
			//boost::iostreams::zlib_params paramaters(6); // Compression level
			//Compressedstream.push(boost::iostreams::zlib_compressor(paramaters));
			//Compressedstream.push(rawStream);

			boost::archive::binary_oarchive SaveArchive(rawStream);
			Game* THEGAME = GAME->GetInstance();
			SaveArchive << THEGAME;

			rawStream.close();
		} else {
			// FILE not Opened?
		}
	} catch (const std::exception& e) {
		//Do something
	}
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

bool MainGameScreen::RunSuitePressed(const CEGUI::EventArgs& pEventArgs)
{

    CEGUI::Spinner* SystemSpinner = static_cast<CEGUI::Spinner*> (GUI->getWindowManager()->getWindow("MainGameScreen/PathProfileWindow/SystemSpinner"));
    CEGUI::Spinner* SeedSpinner = static_cast<CEGUI::Spinner*> (GUI->getWindowManager()->getWindow("MainGameScreen/PathProfileWindow/SeedSpinner"));
    CEGUI::Spinner* SizeSpinner = static_cast<CEGUI::Spinner*> (GUI->getWindowManager()->getWindow("MainGameScreen/PathProfileWindow/SizeSpinner"));

	vector<int> Systems;
	Systems.push_back(SystemSpinner->getCurrentValue());

	GAME->getPath()->getTester()->CreateTestSuite(SeedSpinner->getCurrentValue(), SizeSpinner->getCurrentValue());
	GAME->getPath()->getTester()->RunPathTestSuites(Systems);

	GroupProfile* TestResults = GAME->getPath()->getTester()->getCurrentGroupProfile();

    CEGUI::Window* RawWindow = GUI->getWindowManager()->getWindow("MainGameScreen/PathProfileWindow/DataList");
	CEGUI::Listbox* DataListbox = (CEGUI::Listbox*) RawWindow;
	DataListbox->resetList();

	char buffer[64] ;
	CEGUI::ListboxTextItem* newItem;

	sprintf(buffer, "TotalPathSteps     %i", TestResults->TotalPathSteps);
	newItem = new CEGUI::ListboxTextItem(buffer);
	DataListbox->addItem(newItem);

	sprintf(buffer, "TotalPathLength    %f", TestResults->TotalPathLength);
	newItem = new CEGUI::ListboxTextItem(buffer);
	DataListbox->addItem(newItem);

	sprintf(buffer, "TotalGraphReads    %i", TestResults->TotalGraphReads);
	newItem = new CEGUI::ListboxTextItem(buffer);
	DataListbox->addItem(newItem);

	sprintf(buffer, "TotalNodesExpanded %i", TestResults->TotalNodesExpanded);
	newItem = new CEGUI::ListboxTextItem(buffer);
	DataListbox->addItem(newItem);

	sprintf(buffer, "TotalCacheHits     %i", TestResults->TotalCacheHits);
	newItem = new CEGUI::ListboxTextItem(buffer);
	DataListbox->addItem(newItem);

	sprintf(buffer, "TotalTimeCost      %i", TestResults->TotalTimeCost);
	newItem = new CEGUI::ListboxTextItem(buffer);
	DataListbox->addItem(newItem);

	sprintf(buffer, "GraphReadEfficency  %f", TestResults->GraphReadEfficency);
	newItem = new CEGUI::ListboxTextItem(buffer);
	DataListbox->addItem(newItem);

	sprintf(buffer, "NodeSearchEfficency %f", TestResults->NodeSearchEfficency);
	newItem = new CEGUI::ListboxTextItem(buffer);
	DataListbox->addItem(newItem);

	sprintf(buffer, "CacheEfficency      %f", TestResults->CacheEfficency);
	newItem = new CEGUI::ListboxTextItem(buffer);
	DataListbox->addItem(newItem);

	sprintf(buffer, "TimeCostEfficency   %f", TestResults->TimeCostEfficency);
	newItem = new CEGUI::ListboxTextItem(buffer);
	DataListbox->addItem(newItem);
}

// KEY PROCESSING

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
