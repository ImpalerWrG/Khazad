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
    void ElevateCamera(float ElevationFactor);
    void PitchCamera(float PitchFactor);
    void TranslateCamera(float X, float Y);

    void setPitchLock(bool NewLock)                 { PitchLock = NewLock; }
    bool getPitchLock()                             { return PitchLock; }

    Ogre::Vector3 getMouseRayIntersection(float X, float Y);
    void FocusAt(Ogre::Vector3 FocalPoint);

    void SetDefaultView();


private:

    bool PitchLock;

    Ogre::SceneNode* CameraNode;
    Ogre::SceneNode* RotationNode;
    Ogre::SceneNode* TiltNode;
    Ogre::SceneNode* TargetNode;

    Ogre::Camera* OgreCamera;
    Ogre::Viewport* OgreViewPort;

    Ogre::Real ZoomFactor;
    float TranslationFactor;
    float minpitch;
};

#endif  // CAMERA_HEADER
