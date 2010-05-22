#include <Camera.h>

#include <Ogre.h>
#include <Renderer.h>

Camera::Camera()
{

}

bool Camera::Init()
{
    Ogre::SceneManager* ParentSceneManager = RENDERER->getSceneManager();
    TargetNode = ParentSceneManager->getRootSceneNode()->createChildSceneNode("MainCamera TargetNode");
    OgreCamera = ParentSceneManager->createCamera("Camera");

    Ogre::Viewport* OgreViewPort = RENDERER->getWindow()->addViewport(OgreCamera);
    OgreViewPort->setBackgroundColour(Ogre::ColourValue(0,0,0));

    OgreCamera->setAspectRatio(Ogre::Real(OgreViewPort->getActualWidth()) / Ogre::Real(OgreViewPort->getActualHeight()));
    OgreCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);

    CameraNode = TargetNode->createChildSceneNode("MainCameraNode");
    CameraNode->setPosition(Ogre::Vector3(0, 0, 10000));
    CameraNode->attachObject(OgreCamera);

    CameraNode->setAutoTracking(true, TargetNode);
    OgreCamera->setNearClipDistance(1);
    OgreCamera->setFarClipDistance(100000);

    ZoomFactor = OgreCamera->getOrthoWindowWidth();

    PitchLock = false;
    PitchCamera(45);
    RotateCamera(2.37);
}

void Camera::ZoomCamera(float ZoomChange)
{
    ZoomFactor *= ZoomChange;
    OgreCamera->setOrthoWindowWidth(ZoomFactor);
}

void Camera::RotateCamera(float RotationFactor)
{
    TargetNode->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(RotationFactor), Ogre::Node::TS_WORLD);
}

void Camera::PitchCamera(float PitchFactor)
{
    if (!PitchLock)
    {
        TargetNode->pitch(Ogre::Radian(PitchFactor));

        Ogre::Real pitchAngle = 2 * Ogre::Degree(Ogre::Math::ACos(TargetNode->getOrientation().w)).valueDegrees();
        Ogre::Real pitchAngleSign = TargetNode->getOrientation().x;

        if (pitchAngle > 90.0f)
        {
            //TargetNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f), Ogre::Math::Sqrt(0.5f), 0, 0));
        }
    }
}

void Camera::TranslateCamera(float X, float Y)
{
    TargetNode->translate(-X * ZoomFactor, Y * ZoomFactor, 0, Ogre::Node::TS_LOCAL);
}
