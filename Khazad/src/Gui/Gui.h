#ifndef UI__HEADER
#define UI__HEADER

#include <stdafx.h>
#include <Singleton.h>


class Ui
{
DECLARE_SINGLETON_CLASS(Ui)

public:
	~Ui();
	bool Init();
	bool Run();

    gcn::Gui* GuiChanMainObject;
    gcn::SDLInput* Input;
    gcn::OpenGLGraphics* GraphicsImplementation;

    gcn::Container* top;
    gcn::ImageFont* font;
    gcn::Label* label;

    bool ProcessEvent(SDL_Event event);
    bool Draw();

protected:


};


#define UI (Ui::GetInstance())
#define GRAPHICS (Ui::GetInstance()->GraphicsImplementation)

#endif // UI__HEADER
