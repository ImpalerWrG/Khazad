#ifndef CAMERA_HEADER
#define CAMERA_HEADER

#include <stdafx.h>

#include <Ogre.h>

class Camera
{

public:

    Camera();

    bool Init();

    ~Camera();

    void ZoomCamera(float ZoomChange);
    void RotateCamera(float RotationFactor);
    void PitchCamera(float PitchFactor);
    void TranslateCamera(float X, float Y);

    void setPitchLock(bool NewLock)                 { PitchLock = NewLock; }
    bool getPitchLock()                             { return PitchLock; }


private:

    bool PitchLock;

    Ogre::SceneNode* CameraNode;
    Ogre::SceneNode* TargetNode;

    Ogre::Camera* OgreCamera;
    Ogre::Viewport* OgreViewPort;

    Ogre::Real ZoomFactor;
    float TranslationFactor;
};

#endif  // CAMERA_HEADER
