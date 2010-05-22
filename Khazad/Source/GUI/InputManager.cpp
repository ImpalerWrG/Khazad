#include <InputManager.h>

#include <Renderer.h>

//using namespace Ogre;


bool BufferedInputHandler::Init(Ogre::Root* OgreRoot, GUI* TargetGUI)
{
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
    OIS::ParamList Parameters;
    Ogre::RenderWindow *win = OgreRoot->getAutoCreatedWindow();

    win->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    Parameters.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    InputManagerObject = OIS::InputManager::createInputSystem(Parameters);

    try
    {
        KeyboardObject = static_cast<OIS::Keyboard*>(InputManagerObject->createInputObject(OIS::OISKeyboard, true));
        KeyboardObject->setEventCallback(this);

        MouseObject = static_cast<OIS::Mouse*>(InputManagerObject->createInputObject(OIS::OISMouse, true));
        MouseObject->setEventCallback(this);

        //JoyStickObject = static_cast<OIS::JoyStick*>(InputManagerObject->createInputObject(OIS::OISJoyStick, true));
        //JoyStickObject->setEventCallback(this);
    }
    catch (const OIS::Exception &e)
    {
        throw Ogre::Exception(42, e.eText, "Application::setupInputSystem");
    }

    const OIS::MouseState &mouseState = MouseObject->getMouseState();
    mouseState.width = win->getWidth();
    mouseState.height = win->getHeight();

    GuiManager = TargetGUI;

    OgreRoot->addFrameListener(this);
    return true;
}

BufferedInputHandler::~BufferedInputHandler()
{
    InputManagerObject->destroyInputObject(KeyboardObject);
    InputManagerObject->destroyInputObject(MouseObject);

    OIS::InputManager::destroyInputSystem(InputManagerObject);
}

bool BufferedInputHandler::frameStarted()
{
    KeyboardObject->capture();
    MouseObject->capture();

    return GuiManager->isContinueRunning();
}


// KeyListener
bool BufferedInputHandler::keyPressed(const OIS::KeyEvent &arg)
{
    GuiManager->injectKeyPress(arg);

    return true;
}

bool BufferedInputHandler::keyReleased(const OIS::KeyEvent &arg)
{
    GuiManager->injectKeyRelease(arg);

    return true;
}


// MouseListener
bool BufferedInputHandler::mouseMoved(const OIS::MouseEvent &arg)
{
    GuiManager->injectMouseMove(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);

    if (arg.state.Z.rel != 0)
    {
        RENDERER->getActiveCamera()->ZoomCamera((-arg.state.Z.rel / 1000.0) + 1);
    }

    if (MouseObject->getMouseState().buttonDown(OIS::MB_Right))
    {
        if (arg.state.X.rel != 0)
        {
            RENDERER->getActiveCamera()->RotateCamera(-arg.state.X.rel / 300.0);
        }

        if (arg.state.Y.rel != 0)
        {
            RENDERER->getActiveCamera()->PitchCamera(arg.state.Y.rel / 500.0);
        }
    }
    if (MouseObject->getMouseState().buttonDown(OIS::MB_Left))
    {
        RENDERER->getActiveCamera()->TranslateCamera(arg.state.X.rel / 1200.0, arg.state.Y.rel / 1200.0);
    }

    return true;
}

bool BufferedInputHandler::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    GuiManager->injectMousePress(arg.state.X.abs, arg.state.Y.abs, id);

    return true;
}

bool BufferedInputHandler::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    GuiManager->injectMouseRelease(arg.state.X.abs, arg.state.Y.abs, id);

    return true;
}


// JoystickListener
bool BufferedInputHandler::buttonPressed(const OIS::JoyStickEvent &arg, int button)
{
    return true;
}

bool BufferedInputHandler::buttonReleased(const OIS::JoyStickEvent &arg, int button)
{
    return true;
}

bool BufferedInputHandler::axisMoved(const OIS::JoyStickEvent &arg, int axis)
{
    return true;
}

