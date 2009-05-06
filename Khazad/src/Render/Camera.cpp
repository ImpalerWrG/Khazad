#include <stdafx.h>

#include <Camera.h>
#include <Plane.h>
#include <ScreenManager.h>
#include <ConfigManager.h>
#include <Map.h>


Camera::Camera()
{
	SlidingMode = false;
	ZoomingMode = false;
	IsoMode = false;
	AllFacesDrawing = false;

	IsoScalar = CONFIG->ZoomStart();
    MaxScalar = CONFIG->ZoomMax();
	MinScalar = CONFIG->ZoomMin();
}

bool Camera::Init(bool Isometric)
{
	SetDefaultView();

	if (Isometric)
	{
		setIsometricProj(SCREEN->getWidth(), SCREEN->getHight(), 10000.0);
		IsoMode = true;
		Orientation = CAMERA_NORTH;
		ViewLevels = 6;
	}
	else
	{
		setPerspectiveProj(45.0, 1.0, 100000.0);
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

	gluPerspective(45.0f, fAspect, Zmin, Zmax);

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

void Camera::setViewMatrix( Vector3& vecEye, Vector3& vecLookAt, Vector3& vecUp)
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

	gluLookAt( EyePosition.x, EyePosition.y, EyePosition.z, LookPosition.x, LookPosition.y, LookPosition.z, UpVector.x, UpVector.y, UpVector.z );
	generateViewFrustum();
}

void Camera::onMouseEvent(SDL_Event* Event)
{
    Uint8* Keystate = SDL_GetKeyState(NULL);

    int X, Y;
    Uint8 MouseButtonState = SDL_GetRelativeMouseState(&X, &Y);

    float DeltaX = (float)X;
    float DeltaY = (float)Y;

	if (Event->type == SDL_KEYDOWN)
	{
		switch(Event->key.keysym.sym)
		{

		case SDLK_F1:
			{
				SCREEN->ToggleFullScreen();  //TODO dose not work??
				break;
			}
		case SDLK_RIGHT:
			{
				OrbitView(CONFIG->OrbitSpeed() / 100.0);
				break;
			}
		case SDLK_LEFT:
			{
				OrbitView(CONFIG->OrbitSpeed() / -100.0);
				break;
			}
		case SDLK_UP:
			{
				MoveViewVertical(1.0);
				break;
			}
		case SDLK_DOWN:
			{
				MoveViewVertical(-1.0);
				break;
			}
		case SDLK_INSERT:
			{
				ChangeViewLevels(1);
				break;
			}
		case SDLK_DELETE:
			{
				ChangeViewLevels(-1);
				break;
			}
        default:
            break;
		}
	}

	if (Event->type == SDL_MOUSEBUTTONDOWN)
	{
		switch(Event->button.button)
		{
            case SDL_BUTTON_WHEELUP:
			{
			    if(Keystate[SDLK_RSHIFT] || Keystate[SDLK_LSHIFT])
			    {
                    ChangeViewLevels(1);
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
                    ChangeViewLevels(-1);
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
				int XPosition, YPosition;
				SDL_GetMouseState(&XPosition, &YPosition);

				/*
				GLdouble NearXCordinate, NearYCordinate, NearZCordinate = 0;
				GLdouble FarXCordinate, FarYCordinate, FarZCordinate = 0;
				GLdouble ProjectionMatrix[16];
				GLdouble ModelMatrix[16];
				GLint ViewPort[4];

				glGetDoublev(GL_PROJECTION_MATRIX, ProjectionMatrix);
				glGetDoublev(GL_MODELVIEW_MATRIX, ModelMatrix);
				glGetIntegerv(GL_VIEWPORT, ViewPort);

				gluUnProject(XPosition, YPosition, 0.0, ProjectionMatrix, ModelMatrix, ViewPort, &NearXCordinate, &NearYCordinate, &NearZCordinate);
				gluUnProject(XPosition, YPosition, 1.0, ProjectionMatrix, ModelMatrix, ViewPort, &FarXCordinate, &FarYCordinate, &FarZCordinate);

				//GLdouble RayX = (FarXCordinate - NearXCordinate);
				//GLdouble RayY = (FarYCordinate - NearYCordinate);
				//GLdouble RayZ = (FarZCordinate - NearZCordinate);

				Vector3 RayOrigin, RayDestination;

				RayOrigin.set(NearXCordinate, -NearYCordinate, NearZCordinate);
				RayDestination.set(FarXCordinate, -FarYCordinate, FarZCordinate);
				Vector3 RayVector = RayDestination - RayOrigin;

				Vector3 PlaneNormal, Plane;
				Plane = LookPosition;
				PlaneNormal.set(0.0, 0.0, 1.0);

				float distance =  (PlaneNormal.DotProduct(Plane - RayOrigin)) / (PlaneNormal.DotProduct(RayDestination - RayOrigin));

				Vector3 location;
				location = RayOrigin + ( RayVector * distance );

				//MAP->getCell((Sint32)location.x, (Sint32)location.y, (Sint32)LookPosition.z). ;


				*/





				/*

				GLuint buffer[512];
				GLint viewport[4];
				glGetIntegerv(GL_VIEWPORT, viewport);
				glSelectBuffer(512, buffer);
				GLint Hits;


				// Puts OpenGL In Selection Mode. Nothing Will Be Drawn. Object ID's and Extents Are Stored In The Buffer.
				(void) glRenderMode(GL_SELECT);

				glInitNames();

				//glPushName(0);
				// Get visible Actor ID's into the name list


				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();

				// This Creates A Matrix That Will Zoom Up To A Small Portion Of The Screen, Where The Mouse Is
				gluPickMatrix((GLdouble) XPosition, (GLdouble) viewport[3] - XPosition, 1.0f, 1.0f, viewport);


				gluPerspective(45.0f, (GLfloat) (viewport[2] - viewport[0]) / (GLfloat) (viewport[3] - viewport[1]), 0.1f, 100.0f);
				// make this ISO

				glMatrixMode(GL_MODELVIEW);
				DrawTargets();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				Hits = glRenderMode(GL_RENDER);

				if (Hits > 0)
				{
					int	choose = buffer[3];
					int	depth = buffer[1];

					for (int loop = 1; loop < Hits; loop++)
					{
						// Select closest Hit
						if (buffer[loop * 4 + 1] < GLuint(depth))
						{
							choose = buffer[loop * 4 + 3];
							depth = buffer[loop * 4 + 1];
						}
					}

					Actor* SelectedActor = ActorList[choose];
					Vector3 location = SelectedActor.Position;

					//printf("X = %f Y = %f Z = %f \n", location.x, location.y, LookPosition.z);
				}
				*/



				//GLint viewport[4];
				//GLubyte pixel[4];

				//glGetIntegerv(GL_VIEWPORT, viewport);

				//glReadPixels(XPosition, viewport[3] - YPosition, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, (void *)pixel);

				//printf("%d %d %d %d\n", pixel[0], pixel[1], pixel[2], pixel[3]);
				/*
				if (pixel[0] == 255)
				  printf ("You picked the 1st snowman on the 1st row\n");
				else if (pixel[1] == 255)
				  printf ("You picked the 1st snowman on the 2nd row\n");
				else if (pixel[2] == 255)
				  printf ("You picked the 2nd snowman on the 1st row\n");
				else if (pixel[0] == 250)
				  printf ("You picked the 2nd snowman on the 2nd row\n");
   				else
				   printf("You didn't click a snowman!\n");
				   */



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
				SlideView(DeltaX * CONFIG->SlideSpeed() / 50, DeltaY * CONFIG->SlideSpeed() / 50);
			}

			if (MouseButtonState & SDL_BUTTON(SDL_BUTTON_LEFT))
			{
				TiltView(DeltaY *  (CONFIG->TiltSpeed() / 1000.0), (float)0.01, (float)10.0);
				OrbitView(DeltaX * (CONFIG->OrbitSpeed() / 10000.0));

				generateViewFrustum();
			}
		}
	}
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
		case CAMERA_NORTH:
		{
			EyePosition.x = LookPosition.x + 1; EyePosition.y = LookPosition.y + 1;
			UpVector.x = EyePosition.x; UpVector.y = EyePosition.y;
			break;
		}
		case CAMERA_EAST:
		{
			EyePosition.x = LookPosition.x + 1; EyePosition.y = LookPosition.y - 1;
			UpVector.x = EyePosition.x; UpVector.y = EyePosition.y;
			break;
		}
		case CAMERA_SOUTH:
		{
			EyePosition.x = LookPosition.x - 1; EyePosition.y = LookPosition.y -1;
			UpVector.x = EyePosition.x; UpVector.y = EyePosition.y;

			break;
		}
		case CAMERA_WEST:
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
            Orientation = CAMERA_NORTH;
        }
        else // Y < 0
        {
            Orientation = CAMERA_WEST;
        }
	}
	else // X < 0
	{
		if (Y > 0)
		{
            Orientation = CAMERA_EAST;
		}
		else // Y < 0
		{
			Orientation = CAMERA_SOUTH;
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
		TempUpVector.z = 0;

		Vector3 CrossProduct = UpVector.crossProduct(LookVector);
		CrossProduct.z = 0;

		EyePosition += TempUpVector * Y * (1 / IsoScalar);
		LookPosition += TempUpVector * Y * (1 / IsoScalar);

		EyePosition += CrossProduct * X * (1 / IsoScalar);
		LookPosition += CrossProduct * X * (1 / IsoScalar);

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

	generateViewFrustum();
}

void Camera::MoveViewVertical(float Z)
{
	EyePosition.z += Z;
	LookPosition.z += Z;

	generateViewFrustum();
}

void Camera::ChangeViewLevels(Sint32 Change)
{
    if (Change != 0)
    {
        ViewLevels += Change;

        if (ViewLevels < 1)
        {
            ViewLevels = 1;
        }
        generateViewFrustum();
    }
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

void Camera::CenterView()
{
    if(MAP == NULL)
    {
        return;
    }

    LookPosition.x = MAP->getMapSizeX() / 2;
	LookPosition.y = MAP->getMapSizeY() / 2;
	LookPosition.z = MAP->getMapSizeZ() / 2;

	EyePosition.x = LookPosition.x + 1.0;
	EyePosition.y = LookPosition.y + 1.0;
	EyePosition.z = LookPosition.z + 1.0;

	UpVector.x = EyePosition.x - LookPosition.x;
	UpVector.y = EyePosition.y - LookPosition.y;
	UpVector.z = 0.0;

    IsoScalar = CONFIG->ZoomMax();

	generateViewFrustum();
}

bool Camera::InSlice(float Zlevel)
{
    if (Zlevel <= LookPosition.z)
	{
		float Depth = LookPosition.z - Zlevel;
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
	if (Zlevel <= LookPosition.z)
	{
		float Depth = LookPosition.z - Zlevel;
		if (Depth < ViewLevels)
		{
			float Shading = 1.0;
			if (Depth > 0) // Below look level
			{
				Shading -= (float) Depth / (float) ViewLevels;
				return Shading;
			}
			return Shading;
		}
		return 0.0;
	}
    return 0.0;
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

bool Camera::isAllFacesDrawing()
{
    return AllFacesDrawing;
}

void Camera::setAllFacesDrawing(bool NewValue)
{
    AllFacesDrawing = NewValue;
}
