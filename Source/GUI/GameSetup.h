#ifndef GAMESETUP__HEADER
#define GAMESETUP__HEADER

#include <stdafx.h>

//#include <MyGUI.h>


class GameSetup
{

public:

    GameSetup();

    bool Init();

    ~GameSetup();

private:

    void ExitPressed();
    void NewPressed();

    //MyGUI::Widget* PanelWidgit;
    //MyGUI::StaticImage* LogoImage;

};

#endif  // GAMESETUP__HEADER
