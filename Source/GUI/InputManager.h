#ifndef INPUT__HEADER
#define INPUT__HEADER

#include <stdafx.h>
#include <Singleton.h>

#include <Ogre.h>
#include <OIS.h>

class VolumeSelection;
class MapCoordinates;

class InputManager :  public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener, public Ogre::FrameListener
{
DECLARE_SINGLETON_CLASS(InputManager)

public:

    bool Init();

    ~InputManager();

    bool frameStarted();
    void ProccessDownKeys();

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

	void formZone();
	void setSelectionMode(bool isSelectionMode)       { SelectionMode = isSelectionMode; }

    void ResetMouse();

    void windowResized(Ogre::RenderWindow* rw);

private:

    uint32_t DoubleClickTime;

    OIS::Keyboard* KeyboardObject;
    OIS::Mouse* MouseObject;
    OIS::JoyStick* JoyStickObject;

    OIS::InputManager* InputManagerObject;

	VolumeSelection* ActiveVolumeSelection;
	std::vector< VolumeSelection* > InactiveVolumeSelections;

    bool SelectionMode;
};

#define INPUT (InputManager::GetInstance())

#endif  // INPUT__HEADER
