#include <stdafx.h>

#include <Gui.h>
#include <ScreenManager.h>
#include <Singleton.h>


DECLARE_SINGLETON(Ui)

bool Ui::Init()
{
    // We first create a container to be used as the top widget.
    // The top widget in Guichan can be any kind of widget, but
    // in order to make the Gui contain more than one widget we
    // make the top widget a container.
    top = new gcn::Container();
    // We set the dimension of the top container to match the screen.
    top->setDimension(gcn::Rectangle(0, 0, SCREEN->getWidth(), 30));
    // Finally we pass the top widget to the Gui object.
    GuiChanMainObject->setTop(top);

    label = new gcn::Label("Hello World, Goodbye World");

    label->adjustSize();
    // We give the label a position.
    label->setPosition(10, 10);
    // And finally we add the label to the top container.
    top->add(label);

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

    if(true)
    {
        GraphicsImplementation = new gcn::OpenGLGraphics();
        GraphicsImplementation->setTargetPlane(SCREEN->getWidth(), SCREEN->getHight());
    }
    else
    {
        //GraphicsImplementation = new gcn::SDLGraphics();
        //GraphicsImplementation->setTarget(SCREEN->MainScreen());
    }

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

bool Ui::Draw()
{
    GuiChanMainObject->logic();
    GuiChanMainObject->draw();

    return true;
}
