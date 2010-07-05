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
	OgreViewPort->update();

	CameraNode = TargetNode->createChildSceneNode("MainCameraNode");
	CameraNode->setPosition(Ogre::Vector3(0, 0, 10000));
	CameraNode->attachObject(OgreCamera);


            /*
            char buffer [50];
            sprintf(buffer, "CameraMarker");
            Ogre::Entity *ent = RENDERER->getSceneManager()->createEntity(buffer, "ColorCube");
            ent->setCastShadows(false);
            TargetNode->attachObject(ent);
            */





	CameraNode->setAutoTracking(true, TargetNode);
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
	TargetNode->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(RotationFactor), Ogre::Node::TS_WORLD);
}

void Camera::ElevateCamera(float ElevationFactor)
{
	TargetNode->translate(0, 0, ElevationFactor, Ogre::Node::TS_WORLD);
}

void Camera::PitchCamera(float PitchFactor)
{
	if (!PitchLock)
	{
        Ogre::Degree originalPitch = TargetNode->getOrientation().getPitch();

        if (originalPitch > Ogre::Degree(100) && PitchFactor > 0)
        {
            TargetNode->pitch(Ogre::Degree(PitchFactor));
        }
        else if (originalPitch < Ogre::Degree(178) && PitchFactor < 0)
        {
            TargetNode->pitch(Ogre::Degree(PitchFactor));
        }
	}
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

void Camera::SetDefaultView()
{
    TargetNode->setPosition(Ogre::Vector3(0, 0, 0));
	CameraNode->setPosition(Ogre::Vector3(0, 0, 10000));


    TargetNode->pitch(Ogre::Degree(60));
	TargetNode->rotate(Ogre::Vector3::UNIT_Z, Ogre::Degree(135), Ogre::Node::TS_WORLD);

    ZoomFactor = 100;
	TranslationFactor = ZoomFactor / RENDERER->getWindow()->getWidth();
    OgreCamera->setOrthoWindowWidth(ZoomFactor) ;
}
