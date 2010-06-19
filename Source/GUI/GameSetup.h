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

private:

    bool BeginGamePressed(const CEGUI::EventArgs& pEventArgs);
};

#endif  // GAMESETUP__HEADER
