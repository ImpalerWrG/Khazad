#ifndef SPLASH__HEADER
#define SPLASH__HEADER

#include <stdafx.h>

#include "MyGUI.h"


class SplashScreen
{

public:

    SplashScreen(GUIManager* GUISystem);

    bool Init();

    ~SplashScreen();

private:

    void ExitPressed(MyGUI::WidgetPtr SourceWidget);
    void NewPressed(MyGUI::WidgetPtr SourceWidget);

    MyGUI::Widget* PanelWidgit;
    MyGUI::StaticImage* LogoImage;

    GUIManager* GUISystem;
};

//#define RENDERER (Renderer::GetInstance())

#endif  // GUI__HEADER
