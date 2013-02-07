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

    void ChangeViewLevel(int32_t Change);
    void SetSlice(int16_t newTop, int16_t newBottome);

    void SetSliceTop(int16_t newValue);
    uint16_t getSliceTop()                              { return SliceTop; }
    void SetSliceBottom(int16_t newValue);
    uint16_t getSliceBottom()                           { return SliceBottom; }

    bool InSlice(int16_t Zlevel);
    float getShading(int16_t Zlevel);

    void setPitchLock(bool NewLock)                 { PitchLock = NewLock; }
    bool getPitchLock()                             { return PitchLock; }
    Ogre::Degree getPitch()							{ return TiltNode->getOrientation().getPitch(); }
	float getTranslationFactor()					{ return TranslationFactor; }

    Ogre::Vector3 getMouseRayIntersection(float X, float Y, float Z);
    Ogre::Vector3 getMouseRayIntersection(float X, float Y);
    Ogre::Vector3 ConvertMouseToVector(float X, float Y);
    Ogre::Ray getMouseRay(float MouseX, float MouseY);

    void FocusAt(Ogre::Vector3 FocalPoint);

    void SetDefaultView();


private:

    bool PitchLock;

    Ogre::SceneNode* CameraNode;
    Ogre::SceneNode* RotationNode;
    Ogre::SceneNode* TiltNode;
    Ogre::SceneNode* TargetNode;

	uint16_t SliceTop;
	uint16_t SliceBottom;
	uint16_t ViewLevels;

    Ogre::Camera* OgreCamera;
    Ogre::Viewport* OgreViewPort;

    Ogre::RaySceneQuery* RayQuery;

    Ogre::Real ZoomFactor;
    float TranslationFactor;
};

#endif  // CAMERA_HEADER
