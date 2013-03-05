#include <stdafx.h>

#include <ScreenBase.h>

class MainGameScreen: ScreenBase
{

public:

    MainGameScreen();

    bool Init();

    ~MainGameScreen();

    void SetDirty();

private:

    bool GameOptionsOpen(const CEGUI::EventArgs& pEventArgs);
    bool GameOptionsClose(const CEGUI::EventArgs& pEventArgs);

    bool ExitGame(const CEGUI::EventArgs& pEventArgs);
    bool SaveGame(const CEGUI::EventArgs& pEventArgs);

	void resetDeepthSliders();

    bool DepthSliderMoved(const CEGUI::EventArgs& pEventArgs);
    bool PausePressed(const CEGUI::EventArgs& pEventArgs);
    bool PlayPressed(const CEGUI::EventArgs& pEventArgs);
    bool DigPressed(const CEGUI::EventArgs& pEventArgs);
	bool ZoneButtonClick(const CEGUI::EventArgs& pEventArgs);
	bool RunSuitePressed(const CEGUI::EventArgs& pEventArgs);

    bool ProcessKeyPress(OIS::KeyEvent Event);
    bool ProcessKeyRelease(OIS::KeyEvent Event);
};
