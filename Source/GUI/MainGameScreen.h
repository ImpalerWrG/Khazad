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
    bool DepthSliderMoved(const CEGUI::EventArgs& pEventArgs);

};
