#include <stdafx.h>

#include <Gui.h>
//#include <ScreenManager.h>
#include <Singleton.h>
#include <ActionListeners.h>


DECLARE_SINGLETON(Ui)

bool Ui::Init()
{
    // We first create a container to be used as the top widget.
    // The top widget in Guichan can be any kind of widget, but
    // in order to make the Gui contain more than one widget we
    // make the top widget a container.

    // Create a Top level Container
    top = new gcn::Container();
    top->setDimension(gcn::Rectangle(SCREEN->getWidth() - 30, 0, 30, SCREEN->getHight()));
    GuiChanMainObject->setTop(top);

/*
    label = new gcn::Label("Hello World, Goodbye World");
    top->add(label);
    label->setPosition(10, 10);
    label->adjustSize();
*/


    gcn::Slider* DepthSlider = new gcn::Slider(0.0, 1.0);
    DepthSlider->setOrientation(gcn::Slider::VERTICAL);
    top->add(DepthSlider);

    DepthSlider->setSize(20, SCREEN->getHight());
    DepthSlider->setPosition(5, 0);
    DepthSlider->setValue(0.5);

    gcn::ActionListener* actionListener = new DepthChangeActionListener();
    DepthSlider->addActionListener(actionListener);

	return true;
}

Ui::Ui()
{
    // Now we create the Gui object to be used with this OpenGL
    // and SDL application.
    // The Gui object needs a Graphics to be able to draw itself and an Input
    // object to be able to check for user input. In this case we provide the
    // Gui object with OpenGL and SDL implementations of these objects hence
    // making Guichan able to utilise OpenGL and SDL.

    GraphicsImplementation = new gcn::OpenGLGraphics();
    GraphicsImplementation->setTargetPlane(SCREEN->getWidth(), SCREEN->getHight());

    Input = new gcn::SDLInput();
    GuiChanMainObject = new gcn::Gui();

    GuiChanMainObject->setGraphics(GraphicsImplementation);
    GuiChanMainObject->setInput(Input);
}

Ui::~Ui()
{
    delete GuiChanMainObject;
    delete Input;
    delete GraphicsImplementation;
}

bool Ui::ProcessEvent(SDL_Event event)
{
    Input->pushInput(event);

    if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN)
    {
        return false; // Not consumed in the UI
    }

    if(event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
    {
        int RealX;
        int RealY;
        Uint8 MouseState = SDL_GetMouseState(&RealX, &RealY);

        if(RealX >= top->getX() && RealX < (top->getX() + top->getWidth()))
        {
            if(RealY >= top->getY() && RealY < (top->getY() + top->getHeight()))
            {
               return true;  // Mouse events and movment on the UI are consumed and not sent to the Camera
            }
        }
    }

    return false; // Default not consumed
}

bool Ui::Draw()
{
    GuiChanMainObject->logic();
    GuiChanMainObject->draw();

    return true;
}



