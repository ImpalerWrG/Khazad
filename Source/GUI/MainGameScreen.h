#include <stdafx.h>

#include <ScreenBase.h>

class MainGameScreen: ScreenBase
{

public:

    MainGameScreen();

    bool Init();

    ~MainGameScreen();

private:

    bool ExitPressed(const CEGUI::EventArgs& pEventArgs);
    bool DepthSliderMoved(const CEGUI::EventArgs& pEventArgs);

};
