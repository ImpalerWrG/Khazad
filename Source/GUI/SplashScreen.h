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

};

#endif  // SPLASH__HEADER
