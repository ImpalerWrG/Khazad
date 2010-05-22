#ifndef INPUT__HEADER
#define INPUT__HEADER

#include <stdafx.h>

#include <Ogre.h>
#include <OIS.h>
#include <GUI.h>


//using namespace Ogre;


class BufferedInputHandler :  public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener, public Ogre::FrameListener
{

public:

    bool Init(Ogre::Root* OgreRoot, GUI* GuiManager);

    ~BufferedInputHandler();

    bool frameStarted();

    // KeyListener
    virtual bool keyPressed(const OIS::KeyEvent &arg);
    virtual bool keyReleased(const OIS::KeyEvent &arg);

    // MouseListener
    virtual bool mouseMoved(const OIS::MouseEvent &arg);
    virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

    // JoystickListener
    virtual bool buttonPressed(const OIS::JoyStickEvent &arg, int button);
    virtual bool buttonReleased(const OIS::JoyStickEvent &arg, int button);
    virtual bool axisMoved(const OIS::JoyStickEvent &arg, int axis);

private:

    OIS::Keyboard* KeyboardObject;
    OIS::Mouse* MouseObject;
    OIS::JoyStick* JoyStickObject;

    OIS::InputManager* InputManagerObject;

    GUI* GuiManager;
};

//#define RENDERER (Renderer::GetInstance())

#endif  // INPUT__HEADER
