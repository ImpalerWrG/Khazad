#include <stdafx.h>

#include <ScreenManager.h>
#include <Singleton.h>
#include <ImagePage.h>
#include <ClipImage.h>
#include <Camera.h>

#include <ConfigManager.h>
#include <FontManager.h>
#include <ColorManager.h>
#include <TextureManager.h>
#include <ImageManager.h>
#include <DataManager.h>
#include <Extract.h>
#include <Game.h>
#include <Map.h>
#include <Cell.h>
#include <Gui.h>
#include <Paths.h>

#include <IL/ilut.h>


DECLARE_SINGLETON(ScreenManager)

ScreenManager::ScreenManager()
{
	ScreenWidth = CONFIG->getXResolution();
	ScreenHight = CONFIG->getYResolution();
	ScreenBPP = 32;
	FullScreen = false;

	FrameDraw = true;
	ShadedDraw = true;
	HiddenDraw = false;
    SubTerranianDraw = true;
    SkyViewDraw = true;
    SunLitDraw = true;

	FlatDraw = false;
	DebuggingDraw = true;

    ScreenShotCounter = 0;

	LogoSurface = NULL;
}

ScreenManager::~ScreenManager()
{

}

bool ScreenManager::Init()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0);  //TODO make this a Config option

	SDL_WM_SetCaption("Khazad", "Khazad");

    //SDL_Surface* Icon = SDL_LoadBMP("Assets\\Textures\\Khazad_Icon.bmp");
    SDL_Surface* Icon = NULL;
    Icon = SDL_LoadBMP(Path("Assets\\Textures\\KIcon.bmp"));

    if(Icon)
    {
        Uint32 colorkey = SDL_MapRGB(Icon->format, 255, 0, 255);
        SDL_SetColorKey(Icon, SDL_SRCCOLORKEY, colorkey);
        SDL_WM_SetIcon(Icon, NULL);
    }

    LogoSurface = IMAGE->loadSurface(Path("Assets\\Textures\\KhazadLogo.png"));

    if(CONFIG->FullScreen())
    {
        ScreenSurface = SDL_SetVideoMode(ScreenWidth, ScreenHight, ScreenBPP, SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_FULLSCREEN );
    }
    else
    {
        ScreenSurface = SDL_SetVideoMode(ScreenWidth, ScreenHight, ScreenBPP, SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_RESIZABLE );
    }

    glViewport(0, 0, ScreenWidth, ScreenHight);
	glClearColor(0.0, 0.0, 0.0, 0.0);

    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    imageLoader = new gcn::OpenGLSDLImageLoader();
    gcn::Image::setImageLoader(imageLoader);

    MainCamera = new Camera();
	MainCamera->Init(true);

    //SDL_ShowCursor(false); //Use alternative cursor?

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool ScreenManager::ReSizeScreen(Uint16 Width, Uint16 Hight)
{
    ScreenWidth = Width;
    ScreenHight = Hight;

    setDrawing3D(); // Dispose of any Flat Projection

    MainCamera->setIsometricProj(ScreenWidth, ScreenHight, 1000000.0);
    glViewport(0, 0, ScreenWidth, ScreenHight);

    if(FlatDraw) // Reset Flat drawing if thats what we were in
    {
        setDrawingFlat();
    }

    // Will need full texture rebuild to work
    //ScreenSurface = SDL_SetVideoMode(ScreenWidth, ScreenHight, ScreenBPP, SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_RESIZABLE );


	return true;
}

void ScreenManager::RenderText(char *text, Sint8 FontIndex, SDL_Color Color, SDL_Rect *location)
{
    SDL_Surface* FontSurface = FONT->makeFontSurface(text, Color, FontIndex);
    RenderSurface(FontSurface, location);
    SDL_FreeSurface(FontSurface);
}

void ScreenManager::RenderTextCentered(char *text, Sint8 FontIndex, SDL_Color Color, Sint16 Verticaladjust)
{
    SDL_Surface* FontSurface = FONT->makeFontSurface(text, Color, FontIndex);

    SDL_Rect location;
    location.x = (ScreenWidth / 2) - (FontSurface->clip_rect.w / 2);
    location.y = (ScreenHight / 2) - (FontSurface->clip_rect.h / 2) + Verticaladjust;
    location.w = FontSurface->clip_rect.w;
    location.h = FontSurface->clip_rect.h;

    RenderSurface(FontSurface, &location);
    SDL_FreeSurface(FontSurface);
}

void ScreenManager::RenderSurface(SDL_Surface* RenderSurface, SDL_Rect *location)
{
	SDL_Surface *intermediary;
	SDL_Rect rect;
	GLuint texture;

	rect.w = nextpoweroftwo(RenderSurface->w);
	rect.h = nextpoweroftwo(RenderSurface->h);

	intermediary = SDL_CreateRGBSurface(0, rect.w, rect.h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	SDL_BlitSurface(RenderSurface, 0, intermediary, 0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, rect.w, rect.h, 0, GL_BGRA, GL_UNSIGNED_BYTE, intermediary->pixels );

	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    RenderTexture(texture, &rect, location);

	/* return the deltas in the unused w,h part of the rect */
	location->w = RenderSurface->w;
	location->h = RenderSurface->h;

	SDL_FreeSurface(intermediary);
	glDeleteTextures(1, &texture);
}

void ScreenManager::RenderTexture(GLuint texture, SDL_Rect *Size, SDL_Rect *location)
{
 	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);   glVertex2f(location->x           , location->y);
		glTexCoord2f(1.0f, 1.0f);   glVertex2f(location->x + Size->w , location->y);
		glTexCoord2f(1.0f, 0.0f);   glVertex2f(location->x + Size->w , location->y + Size->h);
		glTexCoord2f(0.0f, 0.0f);   glVertex2f(location->x           , location->y + Size->h);
	glEnd();

	glFinish();
}

void ScreenManager::RenderLogo()
{
    SDL_Rect location;

    location.x = (ScreenWidth / 2) - (LogoSurface->clip_rect.w / 2);
    location.y = (ScreenHight * 2 / 3) - (LogoSurface->clip_rect.h / 2);

    location.w = LogoSurface->clip_rect.w;
    location.h = LogoSurface->clip_rect.h;

    RenderSurface(LogoSurface, &location);
}

void ScreenManager::CaptureScreenShot()
{
    if(ilutGLScreen())
    {
        char buffer[256];
        sprintf(buffer, "ScreenShots\\ScreenShot%i.png", ScreenShotCounter);
        ilSaveImage(Path(buffer));
        ScreenShotCounter++;
        // Find a way to count up if screens already exist with initial count (increment untill error code is not returned?)
    }
}

int ScreenManager::round(double x)
{
	return (int)(x + 0.5);
}

int ScreenManager::nextpoweroftwo(int x)
{
	double logbase2 = log(x) / log(2);
	return round(pow(2, ceil(logbase2)));
}

void ScreenManager::applyClipAt(SDL_Rect Offset, ClipImage* Clip)
{
	if (Clip != NULL)
	{
		if (Clip->wholeImage)
		{
			SDL_BlitSurface( Clip->ParentPage->RawSurface, NULL, ScreenSurface, &Offset );
		}
		else
		{
			SDL_BlitSurface( Clip->ParentPage->RawSurface, &Clip->OffsetRect, ScreenSurface, &Offset );
		}
	}
}

void ScreenManager::applyClipCentered(SDL_Rect Offset, ClipImage* Clip)
{
    setDrawingFlat();

	if (Clip != NULL)
	{
		SDL_Rect finalOffset;
		finalOffset.x = Offset.x - Clip->ParentPage->clipWidth / 2;
		finalOffset.y = Offset.y - Clip->ParentPage->clipHight / 2;
		SDL_BlitSurface( Clip->ParentPage->RawSurface, &Clip->OffsetRect, ScreenSurface, &Offset );
	}
}

bool ScreenManager::WipeScreen()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SDL_GL_SwapBuffers();

	return true;
}

bool ScreenManager::ClearDevice()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	return true;
}

bool ScreenManager::Flip()
{
	glFlush();
	SDL_GL_SwapBuffers();

	return true;
}

Uint16 ScreenManager::getWidth()
{
	return ScreenWidth;
}

Uint16 ScreenManager::getHight()
{
	return 	ScreenHight;
}

void ScreenManager::DirtyAllLists()
{
    if(MAP == NULL || !MAP->isInitialized())
    {
        return;
    }

	for(Uint32 SizeZ = 0; SizeZ < MAP->getCellSizeZ(); SizeZ++)
	{
		for (Uint32 SizeX = 0; SizeX < MAP->getCellSizeX(); SizeX++)
		{
			for (Uint32 SizeY = 0; SizeY < MAP->getCellSizeY(); SizeY++)
			{
			    Cell* TargetCell = MAP->getCell(SizeX, SizeY, SizeZ);
			    if (TargetCell != NULL)
			    {
                    TargetCell->setDirtyDrawList(true);
			    }
			}
		}
	}
}

SDL_Color ScreenManager::getPickingColor()
{
	// produce color


	RedPickingValue++;
	if (RedPickingValue == 255)
	{
		GreenPickingValue++;
		RedPickingValue = 0;

		if (GreenPickingValue == 255)
		{
			BluePickingValue++;
			GreenPickingValue = 0;
		}
	}

	return BLACK;
}

bool ScreenManager::Render()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(MAP == NULL || !MAP->isMapLoaded())
    {
        setDrawingFlat();
        RenderLogo();
        RenderTextCentered("KHAZAD", 0, WHITE, +40);

        return false;
    }

	TotalTriangles = 0;
    TriangleCounter = 0;

    if(FlatDraw)
    {
        setDrawing3D();
    }

	MainCamera->UpdateView();

    if(FrameDraw)
    {
        Vector3 Point;
        Point.x -= 0.48;
        Point.y -= 0.48;
        Point.z = MainCamera->LookZ() - (MainCamera->LookZ() * MainCamera->getLevelSeperation());
        Point.z -= 0.48;
        DrawCage(Point, MAP->getMapSizeX() - 0.04, MAP->getMapSizeY() - 0.04, MAP->getMapSizeZ() * MainCamera->getLevelSeperation() - 0.04);

        Vector3 RawCursor = MainCamera->getCursor();
        Vector3 Cursor;

        Cursor.x = (int) (RawCursor.x + 0.5);
        Cursor.y = (int) (RawCursor.y + 0.5);
        Cursor.z = (int) (RawCursor.z + 1);

        Point.x = Cursor.x - 0.48;  //TODO move most of this logic into DrawCage
        Point.y = Cursor.y - 0.48;
        Point.z = Cursor.z - 0.48;

        DrawCage(Point, .96, .96, .98);             // Keeps Cube lines from disapearing into tiles
    }

	RedPickingValue = 0;
	GreenPickingValue = 0;
	BluePickingValue = 0;

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(1.0 / (float) TEXTURE->getAggragateTextureSize(), 1.0 / (float) TEXTURE->getAggragateTextureSize(), 1.0);
    glBindTexture(GL_TEXTURE_2D, TEXTURE->getAggragateTexture());

	glMatrixMode(GL_MODELVIEW);
    CameraOrientation CurrentOrientation = MainCamera->getOrientation();


	for(Uint16 Zlevel = 0; Zlevel < MAP->getCellSizeZ(); Zlevel++)
	{
        glPushMatrix();
        float ZTranslate = MainCamera->LookZ() - ((MainCamera->LookZ() - Zlevel) * MainCamera->getLevelSeperation());
        glTranslatef(0.0, 0.0, ZTranslate);

		if(MainCamera->InSlice(Zlevel))
		{
            float Shading = 1.0;
            if(ShadedDraw)
            {
                Shading = MainCamera->getShading(Zlevel);
            }

            for(Uint32 SizeX = 0; SizeX < MAP->getCellSizeX(); SizeX++)
            {
                for(Uint32 SizeY = 0; SizeY < MAP->getCellSizeY(); SizeY++)
                {
                    Cell* LoopCell = MAP->getCell(SizeX, SizeY, Zlevel);

                    if(LoopCell != NULL && LoopCell->isActive())
                    {
                        Vector3 RenderingPosition = LoopCell->getPosition();
                        RenderingPosition.z = ZTranslate;

                        if(MainCamera->sphereInFrustum(RenderingPosition, CELLEDGESIZE))
                        {
                            if(LoopCell->isDirtyDrawList())
                            {
                                // Rebuild the new Drawlist
                                GLuint DrawListID = LoopCell->getDrawListID();
                                glDeleteLists(DrawListID, 5);

                                for(CameraOrientation Orientation = CAMERA_DOWN; Orientation < NUM_ORIENTATIONS; ++Orientation)
                                {
                                    RefreshDrawlist(LoopCell, DrawListID + (GLuint) Orientation, Orientation);
                                }
                                LoopCell->setDirtyDrawList(false);
                            }

                            glColor3f(Shading, Shading, Shading);
                            glCallList(LoopCell->getDrawListID() + (GLuint) CurrentOrientation);

                            TotalTriangles += LoopCell->getTriangleCount(CurrentOrientation);  // Use stored Triangle Count
                        }
                    }
                }
            }
		}

		glPopMatrix();
	}

/*
	for (Uint32 i = 0; i < GAME->ActorList.size(); i++)
	{
	    if (GAME->ActorList[i]->getType() == PAWN_ACTOR)
	    {
            GAME->ActorList[i]->Draw();
	    }
	}
*/

    glMatrixMode(GL_TEXTURE);  //return to normal Texturing for UI
    glPopMatrix();
    glLoadIdentity();

	return true;
}

void ScreenManager::RefreshDrawlist(Cell* TargetCell, GLuint DrawListID, CameraOrientation Orientation)
{
    glNewList(DrawListID, GL_COMPILE);

    TriangleCounter = 0;  // Reset Counter and Track Triangle count

    glBegin(GL_TRIANGLES);

        TargetCell->Draw(Orientation, HiddenDraw, SubTerranianDraw, SkyViewDraw, SunLitDraw);

        glColor3f(1.0, 1.0, 1.0);

    glEnd();

    glEndList();

    TargetCell->setTriangleCount(Orientation, TriangleCounter);
}

void ScreenManager::IncrementTriangles(Uint32 Triangles)
{
    TriangleCounter += Triangles;
}

void ScreenManager::ToggleFullScreen()
{
    FullScreen = !FullScreen;
    if(FullScreen)
    {
        ScreenSurface = SDL_SetVideoMode(ScreenWidth, ScreenHight, ScreenBPP, SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_FULLSCREEN);
        glViewport(0, 0, ScreenWidth, ScreenHight);
    }
    else
    {
        ScreenSurface = SDL_SetVideoMode(ScreenWidth, ScreenHight, ScreenBPP, SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_RESIZABLE);
        glViewport(0, 0, ScreenWidth, ScreenHight);
    }
    TEXTURE->FreeInstance();
	TEXTURE->CreateInstance();
	TEXTURE->Init();
}

void ScreenManager::setDrawingFlat()
{
    if(!FlatDraw)
    {
        int vPort[4];

        glGetIntegerv(GL_VIEWPORT, vPort);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        glOrtho(0, vPort[2], 0, vPort[3], -10000, 10000);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);

        FlatDraw = true;
    }
}

void ScreenManager::setDrawing3D()
{
    if(FlatDraw)  // Discard the FlatProjection Matrices
    {
        glEnable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        FlatDraw = false;
    }
}

void ScreenManager::PrintDebugging()
{
    if (EXTRACT->isMapLoaded())
    {
        char buffer[256];
        setDrawingFlat();

        Vector3 Point = MainCamera->getCursor();
        Point.x = (int) MainCamera->LookX();
        Point.y = (int) MainCamera->LookY();
        Point.z = (int) MainCamera->LookZ();

        int TileType = 0;
        int Designation = 0;
        int Ocupancy = 0;

        DfMap * df_map = EXTRACT->getMap();
        if(!(HiddenDraw ^ df_map->isHidden( Point.x, Point.y, Point.z)))
        {
            TileType = df_map->getTileType(Point.x, Point.y, Point.z);
            Designation = df_map->getDesignations(Point.x, Point.y, Point.z);
            Ocupancy = df_map->getOccupancies(Point.x, Point.y, Point.z);
        }

        SDL_Rect position;
        position.x = 10;
        position.y = 200;

        sprintf (buffer, "Cordinates: x%i y%i z%i", (int)Point.x, (int)Point.y, (int)Point.z);
        SCREEN->RenderText(buffer, 0, WHITE, &position);
        position.y -= 40;

        sprintf (buffer, "Tile: %i", TileType);
        SCREEN->RenderText(buffer, 0, WHITE, &position);
        position.y -= 40;

        char binarybuffer[33];

        binarysprintf(binarybuffer, Designation);
        sprintf (buffer, "Designation: %s", binarybuffer);
        SCREEN->RenderText(buffer, 0, WHITE, &position);

        position.y -= 40;

        binarysprintf(binarybuffer, Ocupancy);
        sprintf (buffer, "Ocupancy: %s", binarybuffer);
        SCREEN->RenderText(buffer, 0, WHITE, &position);

        position.y -= 40;

        sprintf (buffer, "Cells: %i  Cubes: %i  Faces: %i  Slopes: %i", MAP->getCellCount(), MAP->getCubeCount(), MAP->getFaceCount(), MAP->getSlopeCount());
        SCREEN->RenderText(buffer, 0, WHITE, &position);

        position.y -= 40;

        sprintf (buffer, "InitCells: %i  InitCubes: %i  InitFaces: %i  InitSlopes: %i", MAP->getInitedCellCount(), MAP->getInitedCubeCount(), MAP->getInitedFaceCount(), MAP->getInitedSlopeCount());
        SCREEN->RenderText(buffer, 0, WHITE, &position);
    }
}

void ScreenManager::binarysprintf(char* buffer, int Input)
{
    for (int i = 31; i > -1; i--)
    {
        if (Input & (1 << i))
        {
            buffer[31 - i] = '1';
        }
        else
        {
            buffer[31 - i] = '0';
        }
    }
    buffer[32] = NULL;
}

void ScreenManager::ShowAxis(void)
{
    Vector3 Point;

    DrawPoint(Point, 100);
}

void ScreenManager::setHiddenDraw(bool NewValue)
{
    HiddenDraw = NewValue;
    DirtyAllLists();
}

void ScreenManager::setSubTerranianDraw(bool NewValue)
{
    SubTerranianDraw = NewValue;
    DirtyAllLists();
}

void ScreenManager::setSkyViewDraw(bool NewValue)
{
    SkyViewDraw = NewValue;
    DirtyAllLists();
}

void ScreenManager::setSunLitDraw(bool NewValue)
{
    SunLitDraw = NewValue;
    DirtyAllLists();
}

void ScreenManager::DrawPoint(Vector3 Point, float Length)
{
	// Draw the positive side of the lines x,y,z
	glBegin(GL_LINES);
		glColor3f (0.0, 1.0, 0.0); // Green for x axis
		glVertex3f(Point.x, Point.y, Point.z);
		glVertex3f(Point.x + Length, Point.y, Point.z);

		glColor3f(1.0, 0.0, 0.0); // Red for y axis
		glVertex3f(Point.x, Point.y, Point.z);
		glVertex3f(Point.x, Point.y + Length, Point.z);

		glColor3f(0.0, 0.0, 1.0); // Blue for z axis
		glVertex3f(Point.x, Point.y, Point.z);
		glVertex3f(Point.x, Point.y, Point.z + Length);
	glEnd();

	// Dotted lines for the negative sides of x,y,z
	glEnable(GL_LINE_STIPPLE); // Enable line stipple to use a dotted pattern for the lines
	glLineStipple(1, 0x0101); // Dotted stipple pattern for the lines

	glBegin(GL_LINES);
		glColor3f (0.0, 1.0, 0.0); // Green for x axis
		glVertex3f(Point.x, Point.y, Point.z);
		glVertex3f(Point.x - Length, Point.y, Point.z);

		glColor3f(1.0, 0.0, 0.0); // Red for y axis
		glVertex3f(Point.x, Point.y, Point.z);
		glVertex3f(Point.x, Point.y - Length, Point.z);

		glColor3f(0.0, 0.0, 1.0); // Blue for z axis
		glVertex3f(Point.x, Point.y, Point.z);
		glVertex3f(Point.x, Point.y, Point.z - Length);
	glEnd();

	glDisable(GL_LINE_STIPPLE); // Disable the line stipple
}

void ScreenManager::DrawPlane(Plane ArgumentPlane, float Length)
{
    // Draws a square line grid in the plane
    Vector3 CenterPoint = ArgumentPlane.Point;
    Vector3 NormalPoint = ArgumentPlane.Normal;
    Vector3 NoramlVector = NormalPoint - CenterPoint;

    NoramlVector.normalize();
    float D = ArgumentPlane.D;



    glBegin(GL_LINES);

		glColor3f (0.0, 1.0, 0.0); // Green for x axis
		glVertex3f(CenterPoint.x, CenterPoint.y, CenterPoint.z);
		glVertex3f(CenterPoint.x + 1, CenterPoint.y, CenterPoint.z);

		glColor3f(1.0, 0.0, 0.0); // Red for y axis
		glVertex3f(CenterPoint.x, CenterPoint.y, CenterPoint.z);
		glVertex3f(CenterPoint.x, CenterPoint.y + 1, CenterPoint.z);

		glColor3f(0.0, 0.0, 1.0); // Blue for z axis
		glVertex3f(CenterPoint.x, CenterPoint.y, CenterPoint.z);
		glVertex3f(CenterPoint.x, CenterPoint.y, CenterPoint.z + 1);



		glColor3f(1.0, 1.0, 1.0); // White for D
		glVertex3f(0, 0, 0);
		glVertex3f(NoramlVector.x * D, NoramlVector.y * D, NoramlVector.z * D);

	glEnd();
}

void ScreenManager::DrawCage(Vector3 Point, float x, float y, float z)
{
    glBegin(GL_LINES);

        glColor3f (1.0, 1.0, 1.0);
		glVertex3f(Point.x, Point.y, Point.z);
		glVertex3f(Point.x + x, Point.y, Point.z);

        glColor3f (1.0, 1.0, 1.0);
		glVertex3f(Point.x, Point.y, Point.z);
		glVertex3f(Point.x, Point.y + y, Point.z);

        glColor3f (1.0, 1.0, 1.0);
		glVertex3f(Point.x + x, Point.y, Point.z);
		glVertex3f(Point.x + x, Point.y + y, Point.z);

        glColor3f (1.0, 1.0, 1.0);
		glVertex3f(Point.x, Point.y + y, Point.z);
		glVertex3f(Point.x + x, Point.y + y, Point.z);


        glColor3f (1.0, 1.0, 1.0);
		glVertex3f(Point.x, Point.y, Point.z);
		glVertex3f(Point.x, Point.y, Point.z + z);

        glColor3f (1.0, 1.0, 1.0);
		glVertex3f(Point.x + x, Point.y, Point.z);
		glVertex3f(Point.x + x, Point.y, Point.z + z);

        glColor3f (1.0, 1.0, 1.0);
		glVertex3f(Point.x, Point.y + y, Point.z);
		glVertex3f(Point.x, Point.y + y, Point.z + z);

        glColor3f (1.0, 1.0, 1.0);
		glVertex3f(Point.x + x, Point.y + y, Point.z);
		glVertex3f(Point.x + x, Point.y + y, Point.z + z);


        glColor3f (1.0, 1.0, 1.0);
		glVertex3f(Point.x, Point.y, Point.z + z);
		glVertex3f(Point.x + x, Point.y, Point.z + z);

        glColor3f (1.0, 1.0, 1.0);
		glVertex3f(Point.x, Point.y, Point.z + z);
		glVertex3f(Point.x, Point.y + y, Point.z + z);

        glColor3f (1.0, 1.0, 1.0);
		glVertex3f(Point.x + x, Point.y, Point.z + z);
		glVertex3f(Point.x + x, Point.y + y, Point.z + z);

        glColor3f (1.0, 1.0, 1.0);
		glVertex3f(Point.x, Point.y + y, Point.z + z);
		glVertex3f(Point.x + x, Point.y + y, Point.z + z);

	glEnd();
}

