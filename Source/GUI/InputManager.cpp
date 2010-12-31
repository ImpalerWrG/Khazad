#include <InputManager.h>

#include <Renderer.h>
#include <GUI.h>
#include <Camera.h>
#include <Game.h>
#include <Map.h>
#include <Zone.h>
#include <Coordinates.h>
#include <DataManager.h>

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

    // Forcibly Hackishly place mouse at center of Screen
    CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
    OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(State);

    mutableMouseState.X.abs = State.width / 2;
    mutableMouseState.Y.abs = State.height / 2;
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

    ProccessDownKeys();

    return GUI->isContinueRunning();
}

void InputManager::ProccessDownKeys()
{
    float DiagonalSpeed = 1.414;
    float CardinalSpeed = 2;

    if (KeyboardObject->isKeyDown(OIS::KC_NUMPAD1))
    {
        RENDERER->getActiveCamera()->TranslateCamera(DiagonalSpeed, -DiagonalSpeed);
    }
    if (KeyboardObject->isKeyDown(OIS::KC_NUMPAD2))
    {
        RENDERER->getActiveCamera()->TranslateCamera(0, -CardinalSpeed);
    }
    if (KeyboardObject->isKeyDown(OIS::KC_NUMPAD3))
    {
        RENDERER->getActiveCamera()->TranslateCamera(-DiagonalSpeed, -DiagonalSpeed);
    }
    if (KeyboardObject->isKeyDown(OIS::KC_NUMPAD4))
    {
        RENDERER->getActiveCamera()->TranslateCamera(CardinalSpeed, 0);
    }
    if (KeyboardObject->isKeyDown(OIS::KC_NUMPAD6))
    {
        RENDERER->getActiveCamera()->TranslateCamera(-CardinalSpeed, 0);
    }
    if (KeyboardObject->isKeyDown(OIS::KC_NUMPAD7))
    {
        RENDERER->getActiveCamera()->TranslateCamera(DiagonalSpeed, DiagonalSpeed);
    }
    if (KeyboardObject->isKeyDown(OIS::KC_NUMPAD8))
    {
        RENDERER->getActiveCamera()->TranslateCamera(0, CardinalSpeed);
    }
    if (KeyboardObject->isKeyDown(OIS::KC_NUMPAD9))
    {
        RENDERER->getActiveCamera()->TranslateCamera(-DiagonalSpeed, DiagonalSpeed);
    }
}

// KeyListener
bool InputManager::keyPressed(const OIS::KeyEvent &arg)
{
    GUI->injectKeyPress(arg);

    if (Game::isInstance())
    {
        switch (arg.key)
        {
            case OIS::KC_UP:
            {
                RENDERER->getActiveCamera()->ElevateCamera(1);
                break;
            }

            case OIS::KC_DOWN:
            {
                RENDERER->getActiveCamera()->ElevateCamera(-1);
                break;
            }

            case OIS::KC_D:
            {
                RENDERER->getActiveCamera()->SetDefaultView();

                //GUI->DirtyActiveScreen();

                break;
            }

            case OIS::KC_T:
            {
                const OIS::MouseState &State = MouseObject->getMouseState();

                MapCoordinates ClickCoordinates = GAME->getMap()->getRayIntersection(RENDERER->getActiveCamera()->getMouseRay(State.X.abs / float(State.width), State.Y.abs / float(State.height)));

                GAME->SpawnTree(ClickCoordinates);

                break;
            }

            case OIS::KC_P:
            {
                const OIS::MouseState &State = MouseObject->getMouseState();

                MapCoordinates ClickCoordinates = GAME->getMap()->getRayIntersection(RENDERER->getActiveCamera()->getMouseRay(State.X.abs / float(State.width), State.Y.abs / float(State.height)));

                GAME->SpawnPawn(ClickCoordinates);

                break;
            }

            case OIS::KC_R:
            {
                const OIS::MouseState &State = MouseObject->getMouseState();

                MapCoordinates ClickCoordinates = GAME->getMap()->getRayIntersection(RENDERER->getActiveCamera()->getMouseRay(State.X.abs / float(State.width), State.Y.abs / float(State.height)));

                GAME->getMap()->setFaceMaterial(ClickCoordinates, DIRECTION_NONE, DATA->getLabelIndex("MATERIAL_OBSIDIAN"));

                break;
            }

            case OIS::KC_X:
            {
                const OIS::MouseState &State = MouseObject->getMouseState();

                MapCoordinates ClickCoordinates = GAME->getMap()->getRayIntersection(RENDERER->getActiveCamera()->getMouseRay(State.X.abs / float(State.width), State.Y.abs / float(State.height)));

                GAME->getMap()->setCubeShape(ClickCoordinates, CubeShape(false));

                break;
            }

            case OIS::KC_ADD:
            {
                GAME->changeTickRate(1);
                break;
            }

            case OIS::KC_SUBTRACT:
            case OIS::KC_MINUS:
            {
                GAME->changeTickRate(-1);
                break;
            }

            case OIS::KC_SPACE:
            {
                GAME->togglePause();
                break;
            }

            default:
            break;
        }
    }

    switch (arg.key)
    {
        case OIS::KC_GRAVE:
        {
            RENDERER->ToggleOverlay();
            break;
        }

        case OIS::KC_SYSRQ:
        {
            RENDERER->TakeScreenShoot();
            break;
        }

        default:
        break;
    }

    return true;
}

bool InputManager::keyReleased(const OIS::KeyEvent &arg)
{
    return GUI->injectKeyRelease(arg);
}


// MouseListener
bool InputManager::mouseMoved(const OIS::MouseEvent &arg)
{
    GUI->injectMouseMove(arg.state.X.rel, arg.state.Y.rel, 0);

    {
        if (arg.state.Z.rel != 0)
        {
            RENDERER->getActiveCamera()->ZoomCamera((-arg.state.Z.rel / 1000.0) + 1);
        }

        if (MouseObject->getMouseState().buttonDown(OIS::MB_Middle))
        {
            if (arg.state.X.rel != 0)
            {
                RENDERER->getActiveCamera()->RotateCamera(-arg.state.X.rel / 300.0);
            }

            if (arg.state.Y.rel != 0)
            {
                RENDERER->getActiveCamera()->PitchCamera(arg.state.Y.rel / 10.0);
            }
        }

        if (Game::isInstance())
        {
            if (MouseObject->getMouseState().buttonDown(OIS::MB_Right))
            {
                if (GAME->getMap()->getActiveZone() != NULL)
                {
                    Zone* ActiveZone = GAME->getMap()->getActiveZone();

                    MapCoordinates Location = GAME->getMap()->getRayIntersection(RENDERER->getActiveCamera()->getMouseRay(arg.state.X.abs / float(arg.state.width), arg.state.Y.abs / float(arg.state.height)));

                    ActiveZone->MoveZone(Location);
                    GUI->DirtyActiveScreen();
                }
                else
                {
                    RENDERER->getActiveCamera()->TranslateCamera(arg.state.X.rel, arg.state.Y.rel);
                }
            }
        }
    }

    return true;
}

bool InputManager::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    GUI->injectMousePress(0, 0, id);

    const OIS::MouseState &State = MouseObject->getMouseState();

    if (id == OIS::MB_Left)
    {
        if (Game::isInstance())
        {
            MapCoordinates Location = GAME->getMap()->getRayIntersection(RENDERER->getActiveCamera()->getMouseRay(arg.state.X.abs / float(arg.state.width), arg.state.Y.abs / float(arg.state.height)));

            Zone* ClickedZone = GAME->getMap()->getZoneAt(Location);

            if ((RENDERER->getRoot()->getTimer()->getMillisecondsCPU() - DoubleClickTime) < 250)  // Left DoubleClick
            {
                bool Doubleclick = true;

                if (ClickedZone == NULL)
                {
                    Zone* NewZone = new Zone(Location);
                    GAME->getMap()->addZone(NewZone);
                    GAME->getMap()->setActiveZone(NewZone);

                    GUI->DirtyActiveScreen();
                }
            }
            else // Single Left Click
            {
                GAME->getMap()->setActiveZone(ClickedZone);
                GUI->DirtyActiveScreen();
            }
        }
    }

    return true;
}

bool InputManager::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    GUI->injectMouseRelease(0, 0, id);

    if (id == OIS::MB_Left)
    {
        DoubleClickTime = RENDERER->getRoot()->getTimer()->getMillisecondsCPU();
    }

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
