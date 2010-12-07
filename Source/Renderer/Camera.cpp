#include <Camera.h>

#include <Ogre.h>
#include <Renderer.h>


Camera::Camera()
{
    SliceTop = 1;
    SliceBottom = 0;

    RayQuery = NULL;
}

Camera::~Camera()
{
    RENDERER->getSceneManager()->destroyQuery(RayQuery);
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



            //Ogre::Entity* Marker = RENDERER->getSceneManager()->createEntity("AxialMarker");
            //Marker->setCastShadows(false);
            //TargetNode->attachObject(Marker);



	OgreCamera->setNearClipDistance(100);
	OgreCamera->setFarClipDistance(100000);

    OgreCamera->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z);

	ZoomFactor = OgreCamera->getOrthoWindowWidth();
	TranslationFactor = ZoomFactor / RENDERER->getWindow()->getWidth();

	PitchLock = false;

	RayQuery = RENDERER->getSceneManager()->createRayQuery(Ogre::Ray());

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
    Ogre::Vector3 TranslationVector = ConvertMouseToVector(X, Y);

	TargetNode->translate(TranslationVector, Ogre::Node::TS_WORLD);
}

void Camera::ChangeViewLevel(int32_t Change)
{
    if (Change != 0)
    {
        /*
        int ZMax = MAP->getMapSizeZ();
        ///FIXME: possible off-by-one errors?
        if(SliceTop + Change > ZMax)
        {
            Change = ZMax - SliceTop;
        }
        if(SliceBottom + Change < 0)
        {
            Change = -SliceBottom;
        }
        SliceTop += Change;
        SliceBottom += Change;
        ViewLevels = SliceTop - SliceBottom;

        UI->setZSliders(SliceTop,SliceBottom);
        //generateViewFrustum();
        */
    }
}

void Camera::SetSliceTop(int16_t newValue)
{
    SliceTop = newValue;
    if(SliceBottom >= SliceTop)
    {
        SliceBottom = SliceTop - 1;
    }
    ViewLevels = SliceTop - SliceBottom;
    //generateViewFrustum();
}

void Camera::SetSliceBottom(int16_t newValue)
{
    SliceBottom = newValue;
    if(SliceTop <= SliceBottom)
    {
        SliceTop = SliceBottom + 1;
    }
    ViewLevels = SliceTop - SliceBottom;
    //generateViewFrustum();
}

bool Camera::InSlice(int16_t Zlevel)
{
    if (Zlevel <= SliceTop)
    {
        float Depth = SliceTop - Zlevel;
        if (Depth < ViewLevels)
        {
            return true;
        }
        return false;
    }
    return false;
}

float Camera::getShading(int16_t Zlevel)
{
    static float Minimum = 0.3;

    if (Zlevel <= SliceTop)
    {
        float Depth = SliceTop - Zlevel;
        if (Depth < ViewLevels)
        {
            float Shading = 1.0;
            if (Depth > 0) // Below look level
            {
                Shading -= Depth / (float) ViewLevels;

                Shading = ((1.0 - Minimum) * Shading) + (Minimum);
                return Shading;
            }
            return Shading;
        }
        return Minimum;
    }
    return Minimum;
}

Ogre::Vector3 Camera::ConvertMouseToVector(float X, float Y)
{
    Ogre::Vector3 LookVector = TargetNode->_getDerivedPosition() - CameraNode->_getDerivedPosition();

    LookVector.normalise();
    float zComp = LookVector.z;
    LookVector.z = 0;
    LookVector.normalise();

    Ogre::Vector3 TempUpVector = Ogre::Vector3::UNIT_Z;

    Ogre::Vector3 CrossProduct = TempUpVector.crossProduct(LookVector);
    CrossProduct.normalise();

    return (CrossProduct * X * TranslationFactor) + (LookVector * -Y * TranslationFactor / zComp);
}

Ogre::Vector3 Camera::getMouseRayIntersection(float MouseX, float MouseY, float TestPlaneZ)
{
    Ogre::Plane FloorLevel;
    FloorLevel.normal = Ogre::Vector3::UNIT_Z;
    FloorLevel.d = -TestPlaneZ;

    // get a ray from viewport mouse position
    Ogre::Ray selectRay = OgreCamera->getCameraToViewportRay(MouseX, MouseY);
    std::pair<bool, Ogre::Real> result = selectRay.intersects(FloorLevel);

    if (result.first) // Was an intersection found
    {
        return selectRay.getPoint(result.second);  // Convert Vector3 to MapCoordinates
    }
}

Ogre::Vector3 Camera::getMouseRayIntersection(float MouseX, float MouseY)
{
    RayQuery->setRay(OgreCamera->getCameraToViewportRay(MouseX, MouseY));
    RayQuery->setSortByDistance(true);

    //RayQuery->setQueryTypeMask(SceneManager::FX_TYPE_MASK);  // Select Billboards
    RayQuery->setQueryTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);  // Select Terrain (made of Entities)
    //RayQuery->setQueryMask();

    Ogre::RaySceneQueryResult &result = RayQuery->execute();

    for (Ogre::RaySceneQueryResult::iterator it = result.begin(); it != result.end(); it++)
    {
        if (it->movable)
        {
            return it->movable->getParentSceneNode()->_getDerivedPosition();
        }
    }
    return Ogre::Vector3(0, 0, 0);
}

Ogre::Ray Camera::getMouseRay(float MouseX, float MouseY)
{
    return OgreCamera->getCameraToViewportRay(MouseX, MouseY);
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
