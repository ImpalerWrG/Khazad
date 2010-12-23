#ifndef GAMESETUP__HEADER
#define GAMESETUP__HEADER

#include <stdafx.h>

#include <ScreenBase.h>

class GameSetupScreen: ScreenBase
{

public:

    GameSetupScreen();

    bool Init();

    ~GameSetupScreen();

    void SetDirty();

private:

    bool BeginGamePressed(const CEGUI::EventArgs& pEventArgs);
    bool UpdateMapArea(const CEGUI::EventArgs& pEventArgs);
};

#endif  // GAMESETUP__HEADER
