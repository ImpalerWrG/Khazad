#include <Camera.h>

#include <Ogre.h>
#include <Renderer.h>

Camera::Camera()
{

}

bool Camera::Init()
{
	Ogre::SceneManager* ParentSceneManager = RENDERER->getSceneManager();
	TargetNode = ParentSceneManager->getRootSceneNode()->createChildSceneNode("CameraTargetNode");

	RotationNode = TargetNode->createChildSceneNode("RotationCameraNode");
	TiltNode = RotationNode->createChildSceneNode("TiltCameraNode");

	OgreCamera = ParentSceneManager->createCamera("Camera");
	Ogre::Viewport* OgreViewPort = RENDERER->getWindow()->addViewport(OgreCamera);
	OgreViewPort->setBackgroundColour(Ogre::ColourValue(0,0,0));

	OgreCamera->setAspectRatio(Ogre::Real(OgreViewPort->getActualWidth()) / Ogre::Real(OgreViewPort->getActualHeight()));
	OgreCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	OgreViewPort->update();

	CameraNode = TiltNode->createChildSceneNode("MainCameraNode");
	CameraNode->setPosition(Ogre::Vector3(0, 0, 10000));
	CameraNode->attachObject(OgreCamera);



            char buffer [50];
            sprintf(buffer, "CameraMarker1");
            Ogre::Entity *ent1 = RENDERER->getSceneManager()->createEntity(buffer, "ColorCube");
            ent1->setCastShadows(false);
            TargetNode->attachObject(ent1);



	OgreCamera->setNearClipDistance(100);
	OgreCamera->setFarClipDistance(100000);

    OgreCamera->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z);

	ZoomFactor = OgreCamera->getOrthoWindowWidth();
	TranslationFactor = ZoomFactor / RENDERER->getWindow()->getWidth();

	PitchLock = false;
	SetDefaultView();

	return true;
}

void Camera::ZoomCamera(float ZoomChange)
{
    if (ZoomFactor < 200 && ZoomChange > 1)
    {
        ZoomFactor *= ZoomChange;
        TranslationFactor = ZoomFactor / RENDERER->getWindow()->getWidth();

        OgreCamera->setOrthoWindowWidth(ZoomFactor);
    }
    else if (ZoomFactor > 10 && ZoomChange < 1)
    {
        ZoomFactor *= ZoomChange;
        TranslationFactor = ZoomFactor / RENDERER->getWindow()->getWidth();

        OgreCamera->setOrthoWindowWidth(ZoomFactor);
    }
}

void Camera::RotateCamera(float RotationFactor)
{
	RotationNode->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(RotationFactor), Ogre::Node::TS_LOCAL);
	OgreCamera->lookAt(TargetNode->getPosition());
}

void Camera::ElevateCamera(float ElevationFactor)
{
	TargetNode->translate(0, 0, ElevationFactor, Ogre::Node::TS_WORLD);
	OgreCamera->lookAt(TargetNode->getPosition());
}

void Camera::PitchCamera(float PitchFactor)
{
	if (!PitchLock)
	{
        Ogre::Degree originalPitch = TiltNode->getOrientation().getPitch();

        if (originalPitch < Ogre::Degree(80) && PitchFactor > 0)  // Allow Pitch to increese if not above maximum
        {
            TiltNode->pitch(Ogre::Degree(PitchFactor));
        }
        else if (originalPitch > Ogre::Degree(1) && PitchFactor < 0) // Allow Pitch to decreese if not below minimum
        {
            TiltNode->pitch(Ogre::Degree(PitchFactor));
        }
	}
	OgreCamera->lookAt(TargetNode->getPosition());
}

void Camera::TranslateCamera(float X, float Y)
{
    Ogre::Vector3 LookVector = TargetNode->_getDerivedPosition() - CameraNode->_getDerivedPosition();

    LookVector.normalise();
    float zComp = LookVector.z;
    LookVector.z = 0;
    LookVector.normalise();

    Ogre::Vector3 TempUpVector = Ogre::Vector3::UNIT_Z;

    Ogre::Vector3 CrossProduct = TempUpVector.crossProduct(LookVector);
    CrossProduct.normalise();


	TargetNode->translate(LookVector * -Y * TranslationFactor / zComp, Ogre::Node::TS_WORLD);
	TargetNode->translate(CrossProduct * X * TranslationFactor, Ogre::Node::TS_WORLD);
}

Ogre::Vector3 Camera::getMouseRayIntersection(float X, float Y)
{
    Ogre::Plane FloorLevel;
    FloorLevel.normal = Ogre::Vector3::UNIT_Z;
    FloorLevel.d = 0.0f;

       // get selection ray from viewport mouse position
    Ogre::Ray selectRay = OgreCamera->getCameraToViewportRay(X, Y);
    std::pair<bool, Ogre::Real> result = selectRay.intersects(FloorLevel);

    if (result.first)
    {
        return selectRay.getPoint(result.second);
    }
}

void Camera::FocusAt(Ogre::Vector3 FocalPoint)
{
    TargetNode->setPosition(FocalPoint);
}

void Camera::SetDefaultView()
{
    TargetNode->setPosition(Ogre::Vector3(0, 0, 0));
	CameraNode->setPosition(Ogre::Vector3(0, 0, 10000));

    TiltNode->pitch(Ogre::Degree(45));
	RotationNode->rotate(Ogre::Vector3::UNIT_Z, Ogre::Degree(135), Ogre::Node::TS_LOCAL);

    ZoomFactor = 100;
	TranslationFactor = ZoomFactor / RENDERER->getWindow()->getWidth();
    OgreCamera->setOrthoWindowWidth(ZoomFactor) ;

    OgreCamera->lookAt(TargetNode->getPosition());
}
