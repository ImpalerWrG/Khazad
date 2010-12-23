#ifndef SCREENBASE__HEADER
#define SCREENBASE__HEADER

#include <stdafx.h>

#include <CEGUI.h>


class ScreenBase
{

public:

    ScreenBase()                         { RootWindow = NULL; };

    ~ScreenBase()                        { };

    virtual bool Init()                  { };

    virtual void SetDirty()              { };

    CEGUI::Window* getRootWindow()       { return RootWindow; }

protected:

    CEGUI::Window* RootWindow;

    bool Dirty;
};

#endif  // SCREENBASE__HEADER
