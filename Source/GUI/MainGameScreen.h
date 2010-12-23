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

    bool ExitPressed(const CEGUI::EventArgs& pEventArgs);
    bool DepthSliderMoved(const CEGUI::EventArgs& pEventArgs);

};
