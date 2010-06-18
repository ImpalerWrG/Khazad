#ifndef SCREENBASE__HEADER
#define SCREENBASE__HEADER

#include <stdafx.h>

#include <CEGUI.h>


class ScreenBase
{

public:

    ScreenBase()                         { RootWindow = NULL; };

    ~ScreenBase()                                 {};

    CEGUI::Window* getRootWindow()       { return RootWindow; }

protected:

    CEGUI::Window* RootWindow;
};

#endif  // SCREENBASE__HEADER
