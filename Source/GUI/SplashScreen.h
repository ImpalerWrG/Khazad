#ifndef SPLASH__HEADER
#define SPLASH__HEADER

#include <stdafx.h>

#include <ScreenBase.h>

class SplashScreen: ScreenBase
{

public:

    SplashScreen();

    bool Init();

    ~SplashScreen();

    void SetDirty();

private:

    bool ExitPressed(const CEGUI::EventArgs& pEventArgs);
    bool OptionsPressed(const CEGUI::EventArgs& pEventArgs);
    bool NewGamePressed(const CEGUI::EventArgs& pEventArgs);
    bool LoadGamePressed(const CEGUI::EventArgs& pEventArgs);
    bool LoadGame(const CEGUI::EventArgs& pEventArgs);

    bool ProcessKeyPress(OIS::KeyEvent Event);
    bool ProcessKeyRelease(OIS::KeyEvent Event);
};

#endif  // SPLASH__HEADER
