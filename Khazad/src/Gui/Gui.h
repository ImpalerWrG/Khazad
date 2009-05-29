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

    bool isDone()                   { return done; }
    void setDone(bool NewState)     { done = NewState; }

    void GetConfirmation(const char* Question, gcn::ActionListener* Listener);
    void HideConfirmationWindow();

    gcn::ActionListener* getExitListener()      { return ExitListener; }

protected:

    void InitMainMenu();
    void InitCameraControlMenu();
    void InitConfirmationWindow();
    void InitDepthSlider();

    gcn::Container* TopWidget;

    gcn::Window* CameraControlWindow;
    gcn::Window* MainMenuWindow;
    gcn::Window* ConfirmationWindow;

    gcn::ImageButton* YesButton;
    gcn::ImageButton* NoButton;
    gcn::ActionListener* ConfirmationAction;
    gcn::ActionListener* ExitListener;

    gcn::Slider* DepthSlider;

    bool done;  // quit program command recived by UI
};


#define UI (Ui::GetInstance())
#define GRAPHICS (Ui::GetInstance()->GraphicsImplementation)

#endif // UI__HEADER
