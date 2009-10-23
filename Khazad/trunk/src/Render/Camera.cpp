#include <stdafx.h>

#include <Camera.h>
#include <Plane.h>
#include <Renderer.h>
#include <ConfigManager.h>
#include <Map.h>
#include <Cube.h>
#include <Gui.h>


Camera::Camera()
{
    DisplayMode = DISPLAY_MODE_THICK_WALLS;

    SlidingMode = false;
    ZoomingMode = false;
    AngleLock = false;

    IsoMode = false;
    AllFacesDrawing = false;

    LevelSeperation = 1;
    CursorLevel = 0;

    IsoScalar = CONFIG->ZoomStart();
    MaxScalar = CONFIG->ZoomMax();
    MinScalar = CONFIG->ZoomMin();

    SliceTop = 1;
    SliceBottom = 0;
}

bool Camera::ReInit(bool Isometric)
{
    if (Isometric)
    {
        setIsometricProj(RENDERER->getWidth(), RENDERER->getHeight(), 1000000.0);
        IsoMode = true;
    }
    else
    {
        IsoMode = false;
        setPerspectiveProj(RENDERER->getWidth() / RENDERER->getHeight(), 0.01, 1000);
    }
    return true;
}

bool Camera::Init(bool Isometric)
{
    SetDefaultView();

    if (Isometric)
    {
        setIsometricProj(RENDERER->getWidth(), RENDERER->getHeight(), 1000000.0);
        IsoMode = true;
        Orientation = CAMERA_NORTH_WEST;
    }
    else
    {
        setPerspectiveProj(45.0, 1.0, 10000000.0);
        IsoMode = false;
        Orientation = CAMERA_NORTH_WEST;
        setPerspectiveProj(RENDERER->getWidth() / RENDERER->getHeight(), 0.01, 1000);
    }
    return true;
}

Camera::~Camera()
{

}

void Camera::setPerspectiveProj( float fAspect, float Zmin, float Zmax )
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(90.0f, fAspect, Zmin, Zmax);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Camera::setIsometricProj( float Width, float Hight, float Depth )
{
    ViewWidth = Width;
    ViewHight = Hight;
    ViewDepth = Depth;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-Width, Width, Hight, -Hight, -Depth, Depth);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    generateViewFrustum();
}

void Camera::setViewMatrix(Vector3& vecEye, Vector3& vecLookAt, Vector3& vecUp)
{
    EyePosition = vecEye;
    UpVector = vecUp;
    LookPosition = vecLookAt;
}

void Camera::UpdateView()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (IsoMode)
    {
        glScalef(IsoScalar, IsoScalar, IsoScalar);
    }

    gluLookAt(EyePosition.x, EyePosition.y, EyePosition.z, LookPosition.x, LookPosition.y, LookPosition.z, UpVector.x, UpVector.y, UpVector.z);
    generateViewFrustum();
}

void Camera::onMousePoll()
{
    int RealX;
    int RealY;
    Uint8 MouseButtonState = SDL_GetMouseState(&RealX, &RealY);

    /*
    if(RealX <= 10)
    {
        SlideView(-CONFIG->SlideSpeed() / 5, 0);
    }
    if(RealX >= SCREEN->getWidth() - 10)
    {
        SlideView(CONFIG->SlideSpeed() / 5, 0);
    }

    if(RealY <= 10)
    {
        SlideView(0, -CONFIG->SlideSpeed() / 5);
    }
    if(RealY >= SCREEN->getHight() - 10)
    {
        SlideView(0, CONFIG->SlideSpeed() / 5);
    }
    */

    //UnProjectPoint(RealX, RealY);
    //DetermineMouseIntersection(LookZ());
}

void Camera::setCursor(Vector3 NewPoint)
{
    Cursor.x = NewPoint.x;
    Cursor.y = NewPoint.y;
    Cursor.z = NewPoint.z;
}

Vector3 Camera::DetermineMouseIntersection(float MapZ)
{
    Vector3 MouseRay = NearMouseClickPoint - FarMouseClickPoint;
    MouseRay.normalize();

    Plane TestPlane;            // Use more robust tilted planes
    Vector3 Point(0, 0, MapZ);
    Vector3 Normal(0, 0, MapZ + 1);
    TestPlane.set2Points(Point, Normal);

    Vector3 PlaneNormal = TestPlane.Point - TestPlane.Normal;
    PlaneNormal.normalize();

    float denom = PlaneNormal.DotProduct(MouseRay);
    float D = MapZ;

    Vector3 Intersection;
    if (denom != 0)
    {
        float dist = (-(PlaneNormal.DotProduct(NearMouseClickPoint)) + D) / denom;
        Intersection = NearMouseClickPoint + (dist * MouseRay);
    }

    return Intersection;
}

bool Camera::DetermineCursorIntersection()
{
    Vector3 Intersection;
    for(int i = SliceTop - 1; i >= SliceBottom && i >= 0; i--)
    {
        MouseIntersection = DetermineMouseIntersection(ZlevelSeperationAdjustment(i) + 0.5);
        MouseIntersection.x = (int) (MouseIntersection.x + 0.5);
        MouseIntersection.y = (int) (MouseIntersection.y + 0.5);
        MouseIntersection.z = i;

        if(RENDERER->isCubeDrawn((Sint32) MouseIntersection.x, (Sint32) MouseIntersection.y, i))
        {
            if(MAP->hasFace((Sint32) MouseIntersection.x, (Sint32) MouseIntersection.y, i, FACET_TOP))
            {
                return true;
            }
        }

        MouseIntersection = DetermineMouseIntersection(ZlevelSeperationAdjustment(i) - 0.5);
        MouseIntersection.x = (int) (MouseIntersection.x + 0.5);
        MouseIntersection.y = (int) (MouseIntersection.y + 0.5);
        MouseIntersection.z = i;

        if(RENDERER->isCubeDrawn((Sint32) MouseIntersection.x, (Sint32) MouseIntersection.y, i))
        {
            if(MAP->hasFace((Sint32) MouseIntersection.x, (Sint32) MouseIntersection.y, i, FACET_BOTTOM))
            {
                return true;
            }
        }

        // Find Slopes while not picking Cubes that lack facets or arnt being drawn
        if(RENDERER->isCubeDrawn((Sint32) MouseIntersection.x, (Sint32) MouseIntersection.y, i)) //&& (TargetCube->isSolid() || TargetCube->isSlope()))
        {
            return true;
        }

    }
    return false;
}

void Camera::onMouseEvent(SDL_Event* Event, Sint32 RelativeX, Sint32 RelativeY, Uint8 *Keystate, Uint8 MouseButtonState, int RealX, int RealY)
{
    float DeltaX = (float)RelativeX;
    float DeltaY = (float)RelativeY;

    if (Event->type == SDL_MOUSEBUTTONDOWN)
    {
        switch(Event->button.button)
        {
            case SDL_BUTTON_WHEELUP:
            {
                if(Keystate[SDLK_RSHIFT] || Keystate[SDLK_LSHIFT])
                {
                    ChangeViewLevel(1);
                    break;
                }

                if(Keystate[SDLK_SPACE])
                {
                    MoveViewVertical(1.0);
                    break;
                }

                ZoomView(1.0 / (CONFIG->ZoomSpeed() / (50.0 + CONFIG->ZoomSpeed())));
                generateViewFrustum();
                break;
            }
            case SDL_BUTTON_WHEELDOWN:
            {
                if(Keystate[SDLK_RSHIFT] || Keystate[SDLK_LSHIFT])
                {
                    ChangeViewLevel(-1);
                    break;
                }

                if(Keystate[SDLK_SPACE])
                {
                    MoveViewVertical(-1.0);
                    break;
                }

                ZoomView((CONFIG->ZoomSpeed() / (50.0 + CONFIG->ZoomSpeed())) / 1.0);
                generateViewFrustum();
                break;
            }
            case SDL_BUTTON_LEFT:
            {
                UnProjectPoint(RealX, RealY);

                if(DetermineCursorIntersection())
                {
                    if(MouseIntersection.x == Cursor.x && MouseIntersection.y == Cursor.y && MouseIntersection.z == Cursor.z)
                    {
                        // adjust the lookAt for level separation
                        Vector3 adjustedCursor = Cursor;
                        adjustedCursor.z = ZlevelSeperationAdjustment(adjustedCursor.z);
                        CenterView(adjustedCursor);
                    }
                    Cursor = MouseIntersection;
                }
                break;
            }
            default:
                break;
        }
    }

    else
    {
        if (Event->type == SDL_MOUSEMOTION )
        {
            if (MouseButtonState & SDL_BUTTON(SDL_BUTTON_RIGHT))
            {
                TiltView(DeltaY *  (CONFIG->TiltSpeed() / 1000.0), 0.01f, 10.0f);
                OrbitView(DeltaX * (CONFIG->OrbitSpeed() / 10000.0));

                generateViewFrustum();
            }
            if (MouseButtonState & SDL_BUTTON(SDL_BUTTON_MIDDLE))
            {
                SlideView(-DeltaX * CONFIG->SlideSpeed() / 50, -DeltaY * CONFIG->SlideSpeed() / 50);
            }
        }
    }
}

void Camera::UnProjectPoint(int XPosition, int YPosition)
{
    GLint viewport[4];
    GLdouble mvmatrix[16], projmatrix[16];
    GLint realy;  //  OpenGL y coordinate position
    GLdouble wx, wy, wz;  //  returned world x, y, z coords

    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

    realy = viewport[3] - (GLint) YPosition - 1;

    gluUnProject ((GLdouble) XPosition, (GLdouble) realy, 0.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz);
    setNearMouseClickPoint(Vector3((float) wx, (float) wy, (float) wz));

    gluUnProject ((GLdouble) XPosition, (GLdouble) realy, 1.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz);
    setFarMouseClickPoint(Vector3((float) wx, (float) wy, (float) wz));
}

Sint32 Camera::ZlevelSeperationAdjustment(Sint32 Zlevel)
{
    //return LookZ() - ((LookZ() - Zlevel) * getLevelSeperation());
    return Zlevel * getLevelSeperation();
}

void Camera::setCameraOrientation(CameraOrientation NewOrientation)
{
    if (NewOrientation >= NUM_ORIENTATIONS)
    {
        return;
    }
    Orientation = NewOrientation;

    switch(Orientation)
    {
        case CAMERA_DOWN:
        {
            EyePosition.x = LookPosition.x; EyePosition.y = LookPosition.y ;
            break;
        }
        case CAMERA_NORTH_WEST:
        {
            EyePosition.x = LookPosition.x + 1; EyePosition.y = LookPosition.y + 1;
            UpVector.x = EyePosition.x; UpVector.y = EyePosition.y;
            break;
        }
        case CAMERA_NORTH_EAST:
        {
            EyePosition.x = LookPosition.x + 1; EyePosition.y = LookPosition.y - 1;
            UpVector.x = EyePosition.x; UpVector.y = EyePosition.y;
            break;
        }
        case CAMERA_SOUTH_EAST:
        {
            EyePosition.x = LookPosition.x - 1; EyePosition.y = LookPosition.y -1;
            UpVector.x = EyePosition.x; UpVector.y = EyePosition.y;

            break;
        }
        case CAMERA_SOUTH_WEST:
        {
            EyePosition.x = LookPosition.x - 1; EyePosition.y = LookPosition.y + 1;
            UpVector.x = EyePosition.x; UpVector.y = EyePosition.y;
            break;
        }
    }
    generateViewFrustum();
}

void Camera::UpdateDirection()
{
    float X = EyePosition.x - LookPosition.x;
    float Y = EyePosition.y - LookPosition.y;

    if (X == 0 && Y == 0)
    {
        Orientation = CAMERA_DOWN;
        return;
    }

    if (X > 0)
    {
        if (Y > 0)
        {
            Orientation = CAMERA_NORTH_WEST;
        }
        else // Y < 0
        {
            Orientation = CAMERA_SOUTH_WEST;
        }
    }
    else // X < 0
    {
        if (Y > 0)
        {
            Orientation = CAMERA_NORTH_EAST;
        }
        else // Y < 0
        {
            Orientation = CAMERA_SOUTH_EAST;
        }
    }
}

void Camera::RotateView(float X, float Y, float Z)
{
    Vector3 vVector;

    // Get our view vVector (The direction we are facing)
    vVector.x = LookPosition.x - EyePosition.x;        // This gets the direction of the X
    vVector.y =	LookPosition.y - EyePosition.y;        // This gets the direction of the Y
    vVector.z = LookPosition.z - EyePosition.z;        // This gets the direction of the Z

    // Rotate the view along the desired axis
    if(X)
    {
        // Rotate the view vVector up or down, then add it to our position
        LookPosition.z = (float)(EyePosition.z + sin(X) * vVector.y + cos(X) * vVector.z);
        LookPosition.y = (float)(EyePosition.y + cos(X) * vVector.y - sin(X) * vVector.z);
    }
    if(Y)
    {
        // Rotate the view vVector right or left, then add it to our position
        LookPosition.z = (float)(EyePosition.z + sin(Y) * vVector.x + cos(Y) * vVector.z);
        LookPosition.x = (float)(EyePosition.x + cos(Y) * vVector.x - sin(Y) * vVector.z);
    }
    if(Z)
    {
        // Rotate the view vVector diagonally right or diagonally down, then add it to our position
        LookPosition.x = (float)(EyePosition.x + sin(Z) * vVector.y + cos(Z) * vVector.x);
        LookPosition.y = (float)(EyePosition.y + cos(Z) * vVector.y - sin(Z) * vVector.x);
    }

    generateViewFrustum();
}

void Camera::OrbitView(float Rotation)
{
    if(Orientation == CAMERA_DOWN)
    {
        float xRelative = UpVector.x;
        float yRelative = UpVector.y;

        UpVector.x = (xRelative * cos(Rotation)) - (yRelative * sin(Rotation));
        UpVector.y = (xRelative * sin(Rotation)) + (yRelative * cos(Rotation));

        UpVector.normalize();
        UpdateDirection();
    }
    else
    {
        float xRelative = EyePosition.x - LookPosition.x;
        float yRelative = EyePosition.y - LookPosition.y;

        float x = (xRelative * cos(Rotation)) - (yRelative * sin(Rotation));
        float y = (xRelative * sin(Rotation)) + (yRelative * cos(Rotation));

        EyePosition.x += x - xRelative;
        EyePosition.y += y - yRelative;

        UpVector.x = EyePosition.x - LookPosition.x;
        UpVector.y = EyePosition.y - LookPosition.y;

        UpVector.normalize();
        UpdateDirection();
    }
}

void Camera::TiltView(float Movement, float Min, float Max)
{
    float Distance = 0;
    Vector3 LookVector;

    if(AngleLock)
    {
        return;
    }

    if((Orientation == CAMERA_DOWN) && (Movement < 0)) // Break out of vertical using Up Vector
    {
        Distance = Min;

        LookVector = UpVector;
        LookVector.z = 0;

        float EyeHight = EyePosition.z;
        LookVector.normalize();

        EyePosition = LookPosition;
        EyePosition += LookVector * (Distance);
        EyePosition.z = EyeHight;

        UpdateDirection();
    }
    else
    {
        LookVector = EyePosition - LookPosition;
        LookVector.z = 0;

        Distance = LookVector.length() - Movement;

        if (Distance > Max)
        {
            Distance = Max;
        }
        if (Distance < Min)
        {
            setCameraOrientation(CAMERA_DOWN); // Camera goes to perfect vertical rendering
            return;
        }

        float EyeHight = EyePosition.z;
        LookVector.normalize();

        EyePosition = LookPosition;
        EyePosition += LookVector * (Distance);
        EyePosition.z = EyeHight;
    }
}

void Camera::SlideView(float X, float Y)
{
    if (IsoMode)
    {
        Vector3 LookVector = EyePosition - LookPosition;
        Vector3 TempUpVector;
        TempUpVector = UpVector;

        Vector3 CrossProduct = UpVector.crossProduct(LookVector);

        EyePosition += TempUpVector * Y * (1 / IsoScalar);
        LookPosition += TempUpVector * Y * (1 / IsoScalar);

        EyePosition += CrossProduct * X * (1 / IsoScalar);
        LookPosition += CrossProduct * X * (1 / IsoScalar);

        ConfineLookPosition();
        generateViewFrustum();
    }
    else // Perspective Mode
    {
        EyePosition += UpVector * Y;
        LookPosition += UpVector * Y;

        Vector3 CrossProduct = UpVector.crossProduct(EyePosition);
        EyePosition += CrossProduct * X;
        LookPosition += CrossProduct * X;

        generateViewFrustum();
    }
}

void Camera::ZoomView(float ZoomFactor)
{
    if (IsoMode)
    {
        IsoScalar *= ZoomFactor;
        if (IsoScalar < MinScalar)
        {
            IsoScalar = MinScalar;
        }
        if (IsoScalar > MaxScalar)
        {
            IsoScalar = MaxScalar;
        }
    }
    else
    {
        // ??? move eye position away from look point
    }
}

void Camera::MoveViewHorizontal(float X, float Y)
{
    EyePosition.x += X;
    EyePosition.y += Y;

    LookPosition.x += X;
    LookPosition.y += Y;

    if(true) // confine within map toggle?
    {
        ConfineLookPosition();
    }

    generateViewFrustum();
}

void Camera::ConfineCursor()
{
    if(Cursor.x < 0)
    {
        Cursor.x = 0;
    }
    if(Cursor.x >= MAP->getMapSizeX())
    {
        Cursor.x = MAP->getMapSizeX() - 1;
    }
    if(Cursor.y < 0)
    {
        Cursor.y = 0;
    }
    if(Cursor.y >= MAP->getMapSizeY())
    {
        Cursor.y = MAP->getMapSizeY() - 1;
    }
    if(Cursor.z < 0)
    {
        Cursor.z = 0;
    }
    if(Cursor.z >= MAP->getMapSizeZ())
    {
        Cursor.z = MAP->getMapSizeZ() - 1;
    }
}

void Camera::ConfineLookPosition()
{
    float DifferenceX = LookPosition.x - EyePosition.x;
    float DifferenceY = LookPosition.y - EyePosition.y;
    float DifferenceZ = LookPosition.z - EyePosition.z;

    bool CorrectionNeeded = false;

    // Combined X & Y
    if(LookPosition.x >= MAP->getMapSizeX() - 1)
    {
        LookPosition.x = MAP->getMapSizeX() - 1;
        CorrectionNeeded = true;
    }

    if(LookPosition.x < 0)
    {
        LookPosition.x = 0;
        CorrectionNeeded = true;
    }

    if(LookPosition.y >= MAP->getMapSizeY() - 1)
    {
        LookPosition.y = MAP->getMapSizeY() - 1;
        CorrectionNeeded = true;
    }

    if(LookPosition.y < 0)
    {
        LookPosition.y = 0;
        CorrectionNeeded = true;
    }

    if(CorrectionNeeded)
    {
        EyePosition.x = LookPosition.x - DifferenceX;
        EyePosition.y = LookPosition.y - DifferenceY;
    }

    // Z Axis
    int MaxZ = LevelSeperation * (MAP->getMapSizeZ() - 1);
//    int MinZ = 0;

    if(LookPosition.z < 0)
    {
        LookPosition.z = 0;
        EyePosition.z = LookPosition.z - DifferenceZ;
    }
    if(LookPosition.z >= MaxZ)
    {
        LookPosition.z = MaxZ;
        EyePosition.z = LookPosition.z - DifferenceZ;
    }
}

void Camera::MoveViewVertical(float Z)
{
    EyePosition.z += Z * LevelSeperation;
    LookPosition.z += Z * LevelSeperation;

    Cursor.z += Z;
    CursorLevel += Z;

    ConfineCursor();

    if(true) // confine within map toggle?
    {
        ConfineLookPosition();
    }

    generateViewFrustum();
}

void Camera::ChangeViewLevel(Sint32 Change)
{
    if (Change != 0)
    {
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
        generateViewFrustum();
    }
}

void Camera::changeLevelSeperation(Sint8 Change)
{
    Vector3 adjustedLookAt = LookPosition;
    adjustedLookAt.z /= LevelSeperation;
    LevelSeperation += Change;

    if(LevelSeperation < 1)
    {
        LevelSeperation = 1;
    }
    adjustedLookAt.z *= LevelSeperation;
    CenterView(adjustedLookAt);
}

void Camera::setLevelSeperation(Sint8 NewValue)
{
    LevelSeperation = NewValue;

    if(LevelSeperation < 1)
    {
        LevelSeperation = 1;
    }
}

void Camera::SetSliceTop(int newValue)
{
    SliceTop = newValue;
    if(SliceBottom >= SliceTop)
    {
        SliceBottom = SliceTop - 1;
    }
    ViewLevels = SliceTop - SliceBottom;
    generateViewFrustum();
}
void Camera::SetSliceBottom(int newValue)
{
    SliceBottom = newValue;
    if(SliceTop <= SliceBottom)
    {
        SliceTop = SliceBottom + 1;
    }
    ViewLevels = SliceTop - SliceBottom;
    generateViewFrustum();
}

void Camera::SetDefaultView()
{
    EyePosition.x = 1.0;
    EyePosition.y = 1.0;
    EyePosition.z = 1.0;

    LookPosition.x = 0.0;
    LookPosition.y = 0.0;
    LookPosition.z = 0.0;

    UpVector.x = EyePosition.x - LookPosition.x;
    UpVector.y = EyePosition.y - LookPosition.y;
    UpVector.z = 0.0;

    IsoScalar = CONFIG->ZoomStart();
    ViewLevels = 5;

    generateViewFrustum();
}

void Camera::CenterView(Vector3 CenterPoint)
{
    if(MAP == NULL)
    {
        return;
    }

    float DifferenceX = EyePosition.x - LookPosition.x;
    float DifferenceY = EyePosition.y - LookPosition.y;
    float DifferenceZ = EyePosition.z - LookPosition.z;

    LookPosition.x = CenterPoint.x;
    LookPosition.y = CenterPoint.y;
    LookPosition.z = CenterPoint.z;

    EyePosition.x = LookPosition.x + DifferenceX;
    EyePosition.y = LookPosition.y + DifferenceY;
    EyePosition.z = LookPosition.z + DifferenceZ;

    if(!Orientation == CAMERA_DOWN)
    {
        UpVector.x = EyePosition.x - LookPosition.x;
        UpVector.y = EyePosition.y - LookPosition.y;
    }

	generateViewFrustum();
}

bool Camera::InSlice(float Zlevel)
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

float Camera::getShading(float Zlevel)
{
    float Minimum = 0.3;
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

bool Camera::sphereInFrustum(Vector3 Point, float Radius)
{
    float distance;

    if (IsoMode)
    {
        for(Uint8 i = 0; i < 4; i++)
        {
            distance = FrustumPlanes[i].distance(Point);
            if (distance < -Radius)
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        return true;  // TODO 6 plane frustrum usage for Perspective projection
    }
}

void Camera::generateViewFrustum()
{
    if (IsoMode)
    {
        float Hight = (ViewHight) * (1 / IsoScalar);
        float Width = (ViewWidth) * (1 / IsoScalar);

        Vector3 TempVector = UpVector;
        TempVector.normalize();
        Vector3 LookVector = EyePosition - LookPosition;
        LookVector.normalize();
        Vector3 Normal;

        Vector3 TempPoint = EyePosition + (TempVector * Hight);

        TempVector = TempVector.crossProduct(LookVector);
        TempVector.normalize();
        TempPoint = EyePosition + (TempVector * Width);
        Normal = -TempVector;
        FrustumPlanes[0].set2Points( Normal, TempPoint);

        TempVector = TempVector.crossProduct(LookVector);
        TempVector.normalize();
        TempPoint = EyePosition + (TempVector * Hight);
        Normal = -TempVector;
        FrustumPlanes[1].set2Points( Normal, TempPoint);

        TempVector = TempVector.crossProduct(LookVector);
        TempVector.normalize();
        TempPoint = EyePosition + (TempVector * Width);
        Normal = -TempVector;
        FrustumPlanes[2].set2Points( Normal, TempPoint);

        TempVector = TempVector.crossProduct(LookVector);
        TempVector.normalize();
        TempPoint = EyePosition + (TempVector * Hight);
        Normal = -TempVector;
        FrustumPlanes[3].set2Points( Normal, TempPoint);
    }
}

void Camera::PrintDebugging()
{
    printf("Eye Position X: %f\n", EyePosition.x);
    printf("Eye Position Y: %f\n", EyePosition.y);
    printf("Eye Position Z: %f\n", EyePosition.z);

    printf("Look Position X: %f\n", LookPosition.x);
    printf("Look Position Y: %f\n", LookPosition.y);
    printf("Look Position Z: %f\n", LookPosition.z);
}
