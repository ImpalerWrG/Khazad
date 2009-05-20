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

    gcn::ImageFont* font;
    gcn::Label* label;

    bool ProcessEvent(SDL_Event event, Sint32 RelativeX, Sint32 RelativeY);
    bool isWidgetCollision(gcn::Widget* TestWidget, Uint16 RealX, Uint16 RealY);
    bool Draw();

protected:

    void InitMainMenu();
    void InitCameraControlMenu();
    void InitDepthSlider();

    gcn::Container* TopWidget;

    gcn::Window* CameraControlWindow;
    gcn::Window* MainMenuWindow;
    gcn::Slider* DepthSlider;
};


#define UI (Ui::GetInstance())
#define GRAPHICS (Ui::GetInstance()->GraphicsImplementation)

#endif // UI__HEADER
