#include <InputManager.h>

#include <Renderer.h>
#include <GUI.h>

DECLARE_SINGLETON(InputManager)

InputManager::InputManager()
{

}

bool InputManager::Init()
{
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
    OIS::ParamList Parameters;

    RENDERER->getWindow()->getCustomAttribute("WINDOW", &windowHnd);
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

    ResetMouse();

    RENDERER->getRoot()->addFrameListener(this);
    return true;
}

void InputManager::ResetMouse()
{
    const OIS::MouseState &State = MouseObject->getMouseState();

    State.width = RENDERER->getWindow()->getWidth();
    State.height = RENDERER->getWindow()->getHeight();

    ModifedMouseX = State.width / 2;
    ModifedMouseY = State.height / 2;

    GUI->injectMouseMove(ModifedMouseX, ModifedMouseY, 0);
}

InputManager::~InputManager()
{
    InputManagerObject->destroyInputObject(KeyboardObject);
    InputManagerObject->destroyInputObject(MouseObject);

    OIS::InputManager::destroyInputSystem(InputManagerObject);
}

bool InputManager::frameStarted()
{
    KeyboardObject->capture();
    MouseObject->capture();

    return GUI->isContinueRunning();
}


// KeyListener
bool InputManager::keyPressed(const OIS::KeyEvent &arg)
{
    GUI->injectKeyPress(arg);

    return true;
}

bool InputManager::keyReleased(const OIS::KeyEvent &arg)
{
    GUI->injectKeyRelease(arg);

    return true;
}


// MouseListener
bool InputManager::mouseMoved(const OIS::MouseEvent &arg)
{
    ModifedMouseX += arg.state.X.rel;
    ModifedMouseY += arg.state.Y.rel;

    GUI->injectMouseMove(ModifedMouseX, ModifedMouseY, 0);

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
        RENDERER->getActiveCamera()->TranslateCamera(arg.state.X.rel, arg.state.Y.rel);
    }

    return true;
}

bool InputManager::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    ModifedMouseX += arg.state.X.rel;
    ModifedMouseY += arg.state.Y.rel;

    GUI->injectMousePress(ModifedMouseX, ModifedMouseY, id);

    return true;
}

bool InputManager::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    ModifedMouseX += arg.state.X.rel;
    ModifedMouseY += arg.state.Y.rel;

    GUI->injectMouseRelease(ModifedMouseX, ModifedMouseY, id);

    return true;
}


// JoystickListener
bool InputManager::buttonPressed(const OIS::JoyStickEvent &arg, int button)
{
    return true;
}

bool InputManager::buttonReleased(const OIS::JoyStickEvent &arg, int button)
{
    return true;
}

bool InputManager::axisMoved(const OIS::JoyStickEvent &arg, int axis)
{
    return true;
}

void InputManager::windowResized(Ogre::RenderWindow* rw)
{
}
