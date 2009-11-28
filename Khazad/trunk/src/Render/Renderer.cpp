#include "stdafx.h"

#include "Renderer.h"
#include <Singleton.h>
#include <Camera.h>

#include <ConfigManager.h>
#include <FontManager.h>
#include <TextureManager.h>
#include <ImageManager.h>
#include <DataManager.h>
#include <ModelManager.h>

#include <DFTypes.h>
#include <DFHackAPI.h>

#include <Game.h>
#include <Map.h>
#include <Cell.h>
#include <Gui.h>
#include <Path.h>

#include <IL/ilut.h>

#include <iostream>
#include <sstream>


DECLARE_SINGLETON(Renderer)

Renderer::Renderer()
{
    FullScreen = false;

    FrameDraw = true;
    ShadedDraw = true;
    HiddenDraw = false;

    SubterraneanDraw = true;
    SkyViewDraw = true;
    SunLitDraw = true;

    FlatDraw = false;
    DebuggingDraw = true;

    ScreenShotCounter = 0;
    TotalTriangles = 0;
}

Renderer::~Renderer()
{
    if (imageLoader != NULL)
    {
        delete imageLoader;
    }
}

bool Renderer::Init()
{
    SDL_Init(SDL_INIT_VIDEO);
    /* Request an opengl 3.2 context.
     * SDL doesn't have the ability to choose which profile at this time of writing,
     * but it should default to the core profile */

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0);  //TODO make this a Config option

    SDL_WM_SetCaption("Khazad", "Khazad");

    SDL_Surface* Icon = NULL;
    Icon = IMAGE->loadSurface(Path("Assets\\Textures\\KIcon.png"));

    if(Icon)
    {
        SDL_WM_SetIcon(Icon, NULL);
    }

    WindowWidth = CONFIG->getXResolution();
    WindowHeight = CONFIG->getYResolution();
    const SDL_VideoInfo* info = SDL_GetVideoInfo();
    ScreenWidth = info->current_w;
    ScreenHeight = info->current_h;

	CurrentBPP = ScreenBPP = WindowBPP = 32;

    if(CONFIG->FullScreen())
    {
        FullScreen = true;
        CurrentWidth = ScreenWidth;
        CurrentHeight = ScreenHeight;
        CurrentBPP = ScreenBPP;
        ScreenSurface = SDL_SetVideoMode(CurrentWidth, CurrentHeight, CurrentBPP, SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_FULLSCREEN );
    }
    else
    {
        FullScreen = false;
        CurrentWidth = WindowWidth;
        CurrentHeight = WindowHeight;
        CurrentBPP = WindowBPP;

        ///FIXME: no resizing on windows
        #ifndef LINUX_BUILD
            ScreenSurface = SDL_SetVideoMode(CurrentWidth, CurrentHeight, CurrentBPP, SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL );
        #else
            ScreenSurface = SDL_SetVideoMode(CurrentWidth, CurrentHeight, CurrentBPP, SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_RESIZABLE );
        #endif
    }
    // let's try searching for VBO extension
    if(IsExtensionSupported("GL_ARB_vertex_buffer_object"))
    {
        haveVBO = true;
        // we have VBO ext, set up VBO functions
        glGenBuffers = (GL_GenBuffers_Func) SDL_GL_GetProcAddress("glGenBuffersARB");
        glBindBuffer =  (GL_BindBuffer_Func) SDL_GL_GetProcAddress("glBindBufferARB");
        glBufferData = (GL_BufferData_Func) SDL_GL_GetProcAddress("glBufferDataARB");
        glDeleteBuffers = (GL_DeleteBuffer_Func) SDL_GL_GetProcAddress("glDeleteBuffersARB");
        glMapBuffer = (GL_MapBuffer_Func) SDL_GL_GetProcAddress("glMapBufferARB");
        glUnmapBuffer = (GL_UnmapBuffer_Func) SDL_GL_GetProcAddress("glUnmapBufferARB");
    }
    else
    {
        haveVBO = false;
        //TODO: really exit and do cleanup when VBOs aren't supported
        cerr << "GL_vertex_buffer_object OpenGL extension not supported, using fallback rendering method.\n";
    }
    haveSHADOW = true;
    if(IsExtensionSupported("GL_ARB_depth_texture") && IsExtensionSupported("GL_ARB_shadow"))
    {
        cout << "Nice, we can do shadows." << endl;
        haveSHADOW = true;
    }
    // we use vertices, normals and texture coords in VAs/VBOs
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glViewport(0, 0, CurrentWidth, CurrentHeight);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

    // FIXME: smoothed lines look terrible on modern hardware, the GUI breaks
    /*glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);*/


    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_COLOR_MATERIAL);
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    // magic
    //glEnable(GL_MULTISAMPLE_ARB);
    //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

    //glEnable(GL_CULL_FACE); // force proper vertex ordering or suffer holes in geometry ;)
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthFunc(GL_LEQUAL/* GL_LESS*/); // show me those walls under floors, glitch style. yes.


    imageLoader = new gcn::OpenGLSDLImageLoader();
    gcn::Image::setImageLoader(imageLoader);

    MainCamera = new Camera();
    MainCamera->Init(true);

    //SDL_ShowCursor(false); //Use alternative cursor?

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return true;
}

bool Renderer::IsExtensionSupported( char* extesion )
{
    const unsigned char *extensions = NULL;
    const unsigned char *start;
    unsigned char *position, *terminator;

    // Extension names should not have spaces
    position = (unsigned char *) strchr( extesion, ' ' );
    if( position || *extesion == '\0' )
        return false;

    // Get Extensions String
    extensions = glGetString( GL_EXTENSIONS );

    // Search The Extensions String For An Exact Copy
    start = extensions;
    for(;;)
    {
        position = (unsigned char *) strstr( (const char *) start, extesion );
        if( !position )
            break;
        terminator = position + strlen( extesion );
        if( position == start || *( position - 1 ) == ' ' )
            if( *terminator == ' ' || *terminator == '\0' )
                return true;
            start = terminator;
    }
    return false;
}

bool Renderer::ReSizeScreen(Uint16 Width, Uint16 Height, bool fullscreen)
{
    #ifndef LINUX_BUILD
        return false;
    #endif
    CurrentWidth = Width;
    CurrentHeight = Height;
    FullScreen = fullscreen;
    if(!fullscreen)
    {
        WindowWidth = Width;
        WindowHeight = Height;
    }

    setDrawing3D(); // Dispose of any Flat Projection

    MainCamera->setIsometricProj(CurrentWidth, CurrentHeight, 1000000.0);
    glViewport(0, 0, CurrentWidth, CurrentHeight);

    if(FlatDraw) // Reset Flat drawing if thats what we were in
    {
        setDrawingFlat();
    }

    if(fullscreen)
    {
        ScreenSurface = SDL_SetVideoMode(CurrentWidth, CurrentHeight, CurrentBPP, SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_FULLSCREEN );
    }
    else
    {
        ScreenSurface = SDL_SetVideoMode(CurrentWidth, CurrentHeight, CurrentBPP, SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_RESIZABLE );
    }
    glViewport(0, 0, CurrentWidth, CurrentHeight);
    MainCamera->ReInit(true);
    UI->updateSizing();
    return true;
}

void Renderer::ToggleFullScreen()
{
    #ifndef LINUX_BUILD
        return;
    #endif
    FullScreen = !FullScreen;
    if(FullScreen)
    {
        ReSizeScreen(ScreenWidth,ScreenHeight,1);
    }
    else
    {
        ReSizeScreen(WindowWidth,WindowHeight,0);
    }
}

RenderObject *Renderer::CreateRenderObject (vector <vertex> * source)
{
    vector <vertex> & work = * source;
    // create descriptor
    RenderObject *ret = new RenderObject;
    ret->count = work.size();
    ret->gfxHandle = 0;
    if(haveVBO)
    {
        // Generate And Bind The Vertex Buffer
        RENDERER->glGenBuffers( 1, &ret->gfxHandle ); // Get A Valid Name
        RENDERER->glBindBuffer( GL_ARRAY_BUFFER, ret->gfxHandle ); // Bind The Buffer
        // Load The Data
        RENDERER->glBufferData( GL_ARRAY_BUFFER_ARB, ret->count * sizeof(vertex), &work[0], GL_STATIC_DRAW_ARB );
        // unbind
        RENDERER->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else
    {
        // init display list
        ret->gfxHandle = glGenLists(1);
        char * handle = (char *) &work[0];
        // draw into display list
        glNewList(ret->gfxHandle,GL_COMPILE);
            glTexCoordPointer(2, GL_FLOAT, 32, (const GLvoid*) (handle + 12)); // texture coords
            glNormalPointer(GL_FLOAT, 32, (const GLvoid*) (handle + 20)); // normal vectors
            glVertexPointer(3, GL_FLOAT, 32 , (const GLvoid*) handle);
            glDrawArrays(GL_TRIANGLES,0,ret->count);
        glEndList();
    }
    return ret;
}

void Renderer::CallRenderObject(RenderObject* obj)
{
    IncrementTriangles(obj->count / 3);
    if(haveVBO)
    {
        glBindBuffer(GL_ARRAY_BUFFER, obj->gfxHandle);
        glTexCoordPointer(2, GL_FLOAT, 32, (const GLvoid*) 12); // texture coords
        glNormalPointer(GL_FLOAT, 32, (const GLvoid*) 20); // normal vectors
        glVertexPointer(3, GL_FLOAT, 32 , 0);
        glDrawArrays(GL_TRIANGLES,0,obj->count);
        RENDERER->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else
    {
        glCallList(obj->gfxHandle);
    }
}

void Renderer::DeleteRenderObject(RenderObject* obj)
{
    if(haveVBO)
    {
        glDeleteBuffers(1, &obj->gfxHandle);
    }
    else
    {
        glDeleteLists(obj->gfxHandle,1);
    }
}

void Renderer::RenderText(const char *text, Sint8 FontIndex, SDL_Color Color, SDL_Rect *location)
{
    SDL_Surface* FontSurface = FONT->makeFontSurface(text, Color, FontIndex);
    RenderSurface(FontSurface, location);
    SDL_FreeSurface(FontSurface);
}

void Renderer::RenderTextCentered(const char *text, Sint8 FontIndex, SDL_Color Color, Sint16 Verticaladjust)
{
    SDL_Surface* FontSurface = FONT->makeFontSurface(text, Color, FontIndex);

    SDL_Rect location;
    location.x = (CurrentWidth / 2) - (FontSurface->clip_rect.w / 2);
    location.y = (CurrentHeight / 2) - (FontSurface->clip_rect.h / 2) + Verticaladjust;
    location.w = FontSurface->clip_rect.w;
    location.h = FontSurface->clip_rect.h;

    RenderSurface(FontSurface, &location);
    SDL_FreeSurface(FontSurface);
}

void Renderer::RenderSurface(SDL_Surface* RenderSurface, SDL_Rect *location)
{
	SDL_Surface *intermediary;
	SDL_Rect rect;
	GLuint texture;

	rect.w = nextpoweroftwo(RenderSurface->w);
	rect.h = nextpoweroftwo(RenderSurface->h);

	intermediary = SDL_CreateRGBSurface(0, rect.w, rect.h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    if(intermediary == NULL)
    {
        cerr << "RenderSurface:SDL_CreateRGBSurface failed: " << rect.w << " " << rect.h << endl;
        return;
    }


	SDL_BlitSurface(RenderSurface, 0, intermediary, 0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, rect.w, rect.h, 0, GL_BGRA, GL_UNSIGNED_BYTE, intermediary->pixels );

	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    RenderTexture(texture, &rect, location, false);

	/* return the deltas in the unused w,h part of the rect */
	location->w = RenderSurface->w;
	location->h = RenderSurface->h;

	SDL_FreeSurface(intermediary);
	glDeleteTextures(1, &texture);
}

void Renderer::RenderTexture(GLuint texture, SDL_Rect *Size, SDL_Rect *location, bool invert)
{
 	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);

        if(invert)
        {
            glTexCoord2f(0.0f, 0.0f);   glVertex2f(location->x           , location->y);
            glTexCoord2f(1.0f, 0.0f);   glVertex2f(location->x + Size->w , location->y);
            glTexCoord2f(1.0f, 1.0f);   glVertex2f(location->x + Size->w , location->y + Size->h);
            glTexCoord2f(0.0f, 1.0f);   glVertex2f(location->x           , location->y + Size->h);
        }
        else
        {
            glTexCoord2f(0.0f, 1.0f);   glVertex2f(location->x           , location->y);
            glTexCoord2f(1.0f, 1.0f);   glVertex2f(location->x + Size->w , location->y);
            glTexCoord2f(1.0f, 0.0f);   glVertex2f(location->x + Size->w , location->y + Size->h);
            glTexCoord2f(0.0f, 0.0f);   glVertex2f(location->x           , location->y + Size->h);
        }
	glEnd();

	glFinish();
}

void Renderer::RenderLogo()
{
    SDL_Rect size;
    SDL_Rect location;

    size.h = 200;
    size.w = 200;

    location.x = (CurrentWidth / 2) - (size.w / 2);
    location.y = (CurrentHeight * 2 / 3) - (size.h / 2);

    location.w = size.w;
    location.h = size.h;

    RenderTexture(TEXTURE->getLogoTexture(), &size, &location, true);
}

void Renderer::CaptureScreenShot()
{
    if(ilutGLScreen())
    {
        char buffer[256];
        sprintf(buffer, "ScreenShots\\ScreenShot%i.png", ScreenShotCounter);
        // dumb, but it works.
        /// TODO: add this functionality to Path
        while(!ilSaveImage(Path(buffer)))
        {
            ScreenShotCounter++;
            sprintf(buffer, "ScreenShots\\ScreenShot%i.png", ScreenShotCounter);
        }
        ScreenShotCounter++;
    }
}

int Renderer::round(double x)
{
	return (int)(x + 0.5);
}

int Renderer::nextpoweroftwo(int x)
{
	double logbase2 = log(x) / log(2);
	return round(pow(2, ceil(logbase2)));
}

bool Renderer::WipeScreen()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SDL_GL_SwapBuffers();

	return true;
}

bool Renderer::ClearDevice()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	return true;
}

bool Renderer::Flip()
{
	SDL_GL_SwapBuffers();

	return true;
}

void Renderer::DirtyAllLists()
{
    if(MAP == NULL || !MAP->isInitialized())
    {
        return;
    }

    CellCoordinates TargetCellCoodinates;

    for (TargetCellCoodinates.Z = 0; TargetCellCoodinates.Z < MAP->getCellSizeZ(); TargetCellCoodinates.Z += 1)
    {
        for (TargetCellCoodinates.X = 0; TargetCellCoodinates.X < MAP->getCellSizeX(); TargetCellCoodinates.X += 1)
        {
            for (TargetCellCoodinates.Y = 0; TargetCellCoodinates.Y < MAP->getCellSizeY(); TargetCellCoodinates.Y += 1)
            {
			    Cell* TargetCell = MAP->getCell(TargetCellCoodinates);
			    if (TargetCell != NULL)
			    {
                    TargetCell->setNeedsRedraw(true);
			    }
			}
		}
	}
}

// FIXME: move to cell
void Renderer::RenderCell(CellCoordinates Coordinates, float ZTranslate, float Shading)
{
    Cell* LoopCell = MAP->getCell(Coordinates);

    if(LoopCell != NULL && LoopCell->isActive())
    {
        Vector3 RenderingPosition = LoopCell->getPosition();
        RenderingPosition.z = ZTranslate;

        if(MainCamera->sphereInFrustum(RenderingPosition, CELLEDGESIZE))
        {
            glPushMatrix();

                glTranslatef(Coordinates.X * CELLEDGESIZE, Coordinates.Y * CELLEDGESIZE, ZTranslate);
                if(LoopCell->getNeedsRedraw())
                {
                    // Rebuild the VBOs
                    LoopCell->UpdateRenderLists(MainCamera->getWallDisplayMode());
                    LoopCell->setNeedsRedraw(false);
                }

                //glColor4f(0.5, 0.5, 0.5, 1.0);
                //glColor4f(0.5, 0.5, 0.5, 0.3);

                LoopCell->Render(MainCamera->getOrientation());
                //TotalTriangles += LoopCell->getTriangleCount();  // Use stored Triangle Count

            glPopMatrix();
        }
    }
}

bool Renderer::Render()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(MAP == NULL || !MAP->isMapLoaded())
    {
        setDrawingFlat();
        RenderLogo();

        static const SDL_Color WHITE = {255, 255, 255};

        RenderTextCentered("KHAZAD", 0, WHITE, +10);

        return false;
    }

	TotalTriangles = 0;

    if(FlatDraw)
    {
        setDrawing3D();
    }

    MainCamera->UpdateView();

    if(FrameDraw)
    {
        MapCoordinates Coodinates;
        DrawCage(Coodinates, MAP->getMapSizeX(), MAP->getMapSizeY(), MAP->getMapSizeZ() * MainCamera->getLevelSeperation(), false, 0, 1, 0);

		MapCoordinates AdjustedCursor = Cursor;
        AdjustedCursor.Z = MainCamera->ZlevelSeperationAdjustment(Cursor.Z);

        if (MAP->isCubeSolid(Cursor))
        {
            DrawCage(AdjustedCursor, 1, 1, 1, true, 1, 1, 1);
        }
        else
        {
            DrawCage(AdjustedCursor, 1, 1, 1, false, 1, 1, 1);
        }
    }

    vector<MapCoordinates> PathCoordinates;

    PathCoordinates.push_back(MapCoordinates(0, 1, 0));
    PathCoordinates.push_back(MapCoordinates(1, 1, 0));
    PathCoordinates.push_back(MapCoordinates(1, 1, 1));
    PathCoordinates.push_back(MapCoordinates(2, 1, 1));
    PathCoordinates.push_back(MapCoordinates(2, 2, 1));

    MapPath TestPath = new MapPath(PathCoordinates);
    DrawMapPath(TestPath);


    TEXTURE->ResetTextureBinding();
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);


    CameraOrientation CurrentOrientation = MainCamera->getOrientation();


    Sint16 Start = MainCamera->getSliceBottom();
    Sint16 Stop = MainCamera->getSliceTop();

    /// turn on z-buffer accumulation
    glDepthMask(GL_TRUE);

    /// turn of blending
    //glDisable(GL_BLEND);

    /// enable stuff

    /// render non-transparent stuff in any order

    CellCoordinates TargetCellCoodinates;

    for (TargetCellCoodinates.Z = Start; TargetCellCoodinates.Z < Stop; TargetCellCoodinates.Z += 1)
    {
        float Shading = 1.0;
        if(ShadedDraw)
        {
            Shading = MainCamera->getShading(TargetCellCoodinates.Z);
        }
        float ZTranslate = MainCamera->ZlevelSeperationAdjustment(TargetCellCoodinates.Z);

        GLfloat specular[] = {Shading, Shading, Shading , 1.0f};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, specular);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        GLfloat ambient[] = { Shading, Shading, Shading };
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        GLfloat position[] = { 1.0f, 1.5f, 2.0f, 0.0f };
        //GLfloat dir[] = {0.0, -3.0, 3.0};
        //glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
        glLightfv(GL_LIGHT0, GL_POSITION, position);


        GLfloat specular2[] = {0.15f, 0.15f, 0.25f , 1.0f};
        glLightfv(GL_LIGHT1, GL_DIFFUSE, specular2);
        GLfloat ambient2[] = { 0.15f, 0.15f, 0.25f };
        glLightfv(GL_LIGHT1, GL_AMBIENT, ambient2);
        GLfloat position2[] = { -1.0f, -1.5f, 2.0f, 0.0f };
        //GLfloat dir[] = {0.0, -3.0, 3.0};
        //glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
        glLightfv(GL_LIGHT1, GL_POSITION, position2);

        for (TargetCellCoodinates.X = 0; TargetCellCoodinates.X < MAP->getCellSizeX(); TargetCellCoodinates.X += 1)
        {
            for (TargetCellCoodinates.Y = 0; TargetCellCoodinates.Y < MAP->getCellSizeY(); TargetCellCoodinates.Y += 1)
            {
                RenderCell(TargetCellCoodinates, ZTranslate, Shading);
            }
        }
    }
    /// bind normal stuff
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /// turn off z-buffer accumulation
    glDepthMask(GL_FALSE);

    /// turn on blending
    glEnable(GL_BLEND);

    /// sort transparent stuff by distance from camera, do nothing if there's no change

    /// render semi-transparent stuff back to front

    /// turn depth mask on again
    glDepthMask(GL_TRUE);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    /// deprecated
/*
	for (Uint32 i = 0; i < GAME->ActorList.size(); i++)
	{
	    if (GAME->ActorList[i]->getType() == PAWN_ACTOR)
	    {
            GAME->ActorList[i]->Draw();
	    }
	}
*/
	return true;
}

void Renderer::IncrementTriangles(Uint32 Triangles)
{
    TotalTriangles += Triangles;
}

void Renderer::setDrawingFlat()
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

void Renderer::setDrawing3D()
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

bool Renderer::isCubeDrawn(MapCoordinates Coordinates)
{
    Cell* TargetCell = MAP->getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        if(MAP->isCubeHidden(Coordinates) && !isHiddenDraw())
        {
            return false;
        }
        if(MAP->isCubeSubTerranean(Coordinates) && !isSubterraneanDraw())
        {
            return false;
        }
        if(MAP->isCubeSkyView(Coordinates) && !isSkyViewDraw())
        {
            return false;
        }
        if(MAP->isCubeSunLit(Coordinates) && !isSunLitDraw())
        {
            return false;
        }
        return true;
    }
    return false;
}

void Renderer::PrintDebugging()
{
    if (MAP->isMapLoaded())
    {
        char buffer[256];
        setDrawingFlat();

        int TileType = 0;
        int Designation = 0;
        int Ocupancy = 0;
        int BiomeOffset = 0;
        int Geolayer = 0;
        int veinMatgloss = 0;

        string matgloss;
        string matglosstype;
        t_matglossPair matglossdesc;
        t_building *building;
        t_tree_desc *tree;

        uint32_t region_x = 0;
        uint32_t region_y = 0;
        uint32_t region_z = 0;

        SDL_Rect position;
        position.x = 10;
        position.y = 160;

        sprintf (buffer, "Coordinates: x%i y%i z%i", Cursor.X, Cursor.Y, Cursor.Z);
        static const SDL_Color WHITE = {255, 255, 255};

        RenderText(buffer, 0, WHITE, &position);
        position.y -= 40;

        /*
        Cube* SelectedCube  = MAP->getCube(x,y,z);
        if(SelectedCube != NULL)
        {
            sprintf (buffer, "NorthFacet: %d  SouthFacet: %d  EastFacet: %d  WestFacet: % d", SelectedCube->getFacetMaterialType(FACET_NORTH), SelectedCube->getFacetMaterialType(FACET_SOUTH), SelectedCube->getFacetMaterialType(FACET_EAST), SelectedCube->getFacetMaterialType(FACET_WEST));
            RenderText(buffer, 0, WHITE, &position);
            position.y -= 40;

            sprintf (buffer, "Shape: %d:  Visible: %d   Hidden: %d", SelectedCube->getShape(), SelectedCube->isVisible(), SelectedCube->isHidden());
            RenderText(buffer, 0, WHITE, &position);
            position.y -= 40;
        }
        */

/*        Block* TargetBlock = df_map->getBlock(x / 16, y / 16, z);
        if(TargetBlock != NULL)
        {
            sprintf (buffer, "Block offset: 0x%X", TargetBlock->origin);
            SCREEN->RenderText(buffer, 0, WHITE, &position);
            position.y -= 40;
        }*/
/*
        sprintf (buffer, "Tile: %i", TileType);
        SCREEN->RenderText(buffer, 0, WHITE, &position);
        position.y -= 40;

        // display tile material
        stringstream acc (stringstream::in | stringstream::out);
        matglosstype = df_map->getMaterialTypeString(matglossdesc.type);
        matgloss = df_map->getMaterialString(matglossdesc.type,matglossdesc.index);
        acc << "material: " << matglossdesc.type << ":" << matglossdesc.index << " = " <<
               matglosstype << ":" << matgloss;
        SCREEN->RenderText(acc.str().c_str(), 0, WHITE, &position);
        position.y -= 40;*/
/*
        sprintf (buffer, "biome %i, layer %i, vein: %s #%i", Biome, Geolayer, matgloss.c_str(), veinMatgloss);

        SCREEN->RenderText(buffer, 0, WHITE, &position);
        position.y -= 40;
*/
/*
        if(building)
        {
            matglosstype = df_map->getMaterialTypeString(building->material.type);
            matgloss = df_map->getMaterialString(building->material.type,building->material.index);
            sprintf (buffer, "building: 0x%x:%s, material: %d:%d = %s:%s ", building->vtable, df_map->getBuildingTypeName(building->type).c_str(),building->material.type, building->material.index,matglosstype.c_str(), matgloss.c_str());
            SCREEN->RenderText(buffer, 0, WHITE, &position);
            position.y -= 40;
        }
        if (tree)
        {
            matglosstype = df_map->getMaterialTypeString(tree->material.type);
            matgloss = df_map->getMaterialString(tree->material.type,tree->material.index);
            sprintf (buffer, "plant: %d:%d = %s:%s ",tree->material.type, tree->material.index,matglosstype.c_str(), matgloss.c_str());
            SCREEN->RenderText(buffer, 0, WHITE, &position);
            position.y -= 40;
        }*/
/*
        char binarybuffer[33];
        binarysprintf(binarybuffer, Designation);
        sprintf (buffer, "Designation: %s", binarybuffer);
        SCREEN->RenderText(buffer, 0, WHITE, &position);

        position.y -= 40;

        binarysprintf(binarybuffer, Ocupancy);
        sprintf (buffer, "Ocupancy: %s", binarybuffer);
        SCREEN->RenderText(buffer, 0, WHITE, &position);

        position.y -= 40;
*/

/*
        sprintf (buffer, "Cells: %i  Cubes: %i  Faces: %i  Slopes: %i", MAP->getCellCount(), MAP->getCubeCount(), MAP->getFaceCount(), MAP->getSlopeCount());
        SCREEN->RenderText(buffer, 0, WHITE, &position);

        position.y -= 40;

        sprintf (buffer, "InitCells: %i  InitCubes: %i  InitFaces: %i  InitSlopes: %i", MAP->getInitedCellCount(), MAP->getInitedCubeCount(), MAP->getInitedFaceCount(), MAP->getInitedSlopeCount());
        SCREEN->RenderText(buffer, 0, WHITE, &position);
*/

/*
        GLint MaxVerts, MaxIndex;
        glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &MaxVerts);
        glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &MaxIndex);
        sprintf (buffer, "MaxElementsVertices: %i  MaxElementsIndices: %i", MaxVerts, MaxIndex);
        SCREEN->RenderText(buffer, 0, WHITE, &position);
*/
    }
}

void Renderer::binarysprintf(char* buffer, int Input)
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

void Renderer::ShowAxis(void)
{
    Vector3 Point;

    DrawPoint(Point, 100);
}

void Renderer::setHiddenDraw(bool NewValue)
{
    HiddenDraw = NewValue;
    DirtyAllLists();
}

void Renderer::setSubterraneanDraw(bool NewValue)
{
    SubterraneanDraw = NewValue;
    DirtyAllLists();
}

void Renderer::setSkyViewDraw(bool NewValue)
{
    SkyViewDraw = NewValue;
    DirtyAllLists();
}

void Renderer::setSunLitDraw(bool NewValue)
{
    SunLitDraw = NewValue;
    DirtyAllLists();
}

void Renderer::DrawPoint(Vector3 Point, float Length)
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

void Renderer::DrawPlane(Plane ArgumentPlane, float Length)
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

void Renderer::DrawCage(MapCoordinates Coodinates, float x, float y, float z, bool Inflated, float red, float green, float blue)
{
    Vector3 Point;

    Point.x = Coodinates.X;
    Point.y = Coodinates.Y;
    Point.z = Coodinates.Z;

    float X, Y, Z;

    if(Inflated)
    {
        X = x + 0.08;
        Y = y + 0.08;
        Z = z + 0.08;

        Point.x -= 0.54;
        Point.y -= 0.54;
        Point.z -= 0.54;
    }
    else
    {
        X = x - 0.04;
        Y = y - 0.04;
        Z = z - 0.04;

        Point.x -= 0.48;
        Point.y -= 0.48;
        Point.z -= 0.48;
    }

    glColor3f (red, green, blue);

    glBegin(GL_LINES);

        glVertex3f(Point.x, Point.y, Point.z);
        glVertex3f(Point.x + X, Point.y, Point.z);

        glVertex3f(Point.x, Point.y, Point.z);
        glVertex3f(Point.x, Point.y + Y, Point.z);

        glVertex3f(Point.x + X, Point.y, Point.z);
        glVertex3f(Point.x + X, Point.y + Y, Point.z);

        glVertex3f(Point.x, Point.y + Y, Point.z);
        glVertex3f(Point.x + X, Point.y + Y, Point.z);

        glVertex3f(Point.x, Point.y, Point.z);
        glVertex3f(Point.x, Point.y, Point.z + Z);

        glVertex3f(Point.x + X, Point.y, Point.z);
        glVertex3f(Point.x + X, Point.y, Point.z + Z);

        glVertex3f(Point.x, Point.y + Y, Point.z);
        glVertex3f(Point.x, Point.y + Y, Point.z + Z);

        glVertex3f(Point.x + X, Point.y + Y, Point.z);
        glVertex3f(Point.x + X, Point.y + Y, Point.z + Z);


        glVertex3f(Point.x, Point.y, Point.z + Z);
        glVertex3f(Point.x + X, Point.y, Point.z + Z);

        glVertex3f(Point.x, Point.y, Point.z + Z);
        glVertex3f(Point.x, Point.y + Y, Point.z + Z);

        glVertex3f(Point.x + X, Point.y, Point.z + Z);
        glVertex3f(Point.x + X, Point.y + Y, Point.z + Z);

        glVertex3f(Point.x, Point.y + Y, Point.z + Z);
        glVertex3f(Point.x + X, Point.y + Y, Point.z + Z);

    glEnd();
}

void Renderer::DrawDiamond(MapCoordinates Coodinates, float red, float green, float blue)
{
    glColor3f (red, green, blue);

    Vector3 Point;

    Point.x = Coodinates.X;
    Point.y = Coodinates.Y;
    Point.z = Coodinates.Z;

    glBegin(GL_TRIANGLES);
    {
        // Top NorthEast
        glVertex3f(Point.x, Point.y, Point.z + (HALFCUBE / 2)); // Top
        glVertex3f(Point.x, Point.y + (HALFCUBE / 2), Point.z); // North
        glVertex3f(Point.x + (HALFCUBE / 2), Point.y, Point.z); // East

        // Top SouthEast
        glVertex3f(Point.x, Point.y, Point.z + (HALFCUBE / 2)); // Top
        glVertex3f(Point.x, Point.y - (HALFCUBE / 2), Point.z); // South
        glVertex3f(Point.x + (HALFCUBE / 2), Point.y, Point.z); // East

        // Top SouthWest
        glVertex3f(Point.x, Point.y, Point.z + (HALFCUBE / 2)); // Top
        glVertex3f(Point.x, Point.y - (HALFCUBE / 2), Point.z); // South
        glVertex3f(Point.x - (HALFCUBE / 2), Point.y, Point.z); // West

        // Top NorthWest
        glVertex3f(Point.x, Point.y, Point.z + (HALFCUBE / 2)); // Top
        glVertex3f(Point.x, Point.y - (HALFCUBE / 2), Point.z); // North
        glVertex3f(Point.x - (HALFCUBE / 2), Point.y, Point.z); // West



        // Bottom NorthEast
        glVertex3f(Point.x, Point.y, Point.z - (HALFCUBE / 2)); // Bottom
        glVertex3f(Point.x, Point.y + (HALFCUBE / 2), Point.z); // North
        glVertex3f(Point.x + (HALFCUBE / 2), Point.y, Point.z); // East

        // Bottom SouthEast
        glVertex3f(Point.x, Point.y, Point.z - (HALFCUBE / 2)); // Bottom
        glVertex3f(Point.x, Point.y - (HALFCUBE / 2), Point.z); // South
        glVertex3f(Point.x + (HALFCUBE / 2), Point.y, Point.z); // East

        // Bottom SouthWest
        glVertex3f(Point.x, Point.y, Point.z - (HALFCUBE / 2)); // Bottom
        glVertex3f(Point.x, Point.y - (HALFCUBE / 2), Point.z); // South
        glVertex3f(Point.x - (HALFCUBE / 2), Point.y, Point.z); // West

        // Bottom NorthWest
        glVertex3f(Point.x, Point.y, Point.z - (HALFCUBE / 2)); // Bottom
        glVertex3f(Point.x, Point.y - (HALFCUBE / 2), Point.z); // North
        glVertex3f(Point.x - (HALFCUBE / 2), Point.y, Point.z); // West
    }
}

void Renderer::DrawMapPath(MapPath* TargetPath)
{
    for (int Step = 0; i < TargetPath->StepCount;)
    {
        DrawDiamond(TargetPath->NextCoordinate, 1.0, 1.0, 1.0);
    }
}

/*
void Renderer::DrawStreamers(Vector3 Point, float x, float y, float z, float Length)
{
    DrawStreamers(Point,x,y,z,Length,1,1,1);
}

void Renderer::DrawStreamers(Vector3 Point, float x, float y, float z, float Length, float red, float green, float blue)
{
    glEnable(GL_LINE_STIPPLE);  // Enable line stipple to use a dotted pattern for the lines
    glLineStipple(1, 0x0101);   // Dotted stipple pattern for the lines

    glBegin(GL_LINES);
        glColor3f (red, green, blue);
        glVertex3f(Point.x + x, Point.y, Point.z);
        glVertex3f(Point.x + x, Point.y, Point.z - Length);

        glVertex3f(Point.x, Point.y + y, Point.z);
        glVertex3f(Point.x, Point.y + y, Point.z - Length);

        glVertex3f(Point.x + x, Point.y + y, Point.z);
        glVertex3f(Point.x + x, Point.y + y, Point.z - Length);

        glVertex3f(Point.x, Point.y, Point.z);
        glVertex3f(Point.x, Point.y, Point.z - Length);
    glEnd();

    glDisable(GL_LINE_STIPPLE); // Disable the line stipple
}
*/

void Renderer::setCursor(MapCoordinates Coordinates)
{
    Cursor.X = Coordinates.X;
    Cursor.Y = Coordinates.Y;
    Cursor.Z = Coordinates.Z;
}

void Renderer::ConfineCursor()
{
    if(Cursor.X < 0)
    {
        Cursor.X = 0;
    }
    if(Cursor.X >= MAP->getMapSizeX())
    {
        Cursor.X = MAP->getMapSizeX() - 1;
    }
    if(Cursor.Y < 0)
    {
        Cursor.Y = 0;
    }
    if(Cursor.Y >= MAP->getMapSizeY())
    {
        Cursor.Y = MAP->getMapSizeY() - 1;
    }
    if(Cursor.Z < 0)
    {
        Cursor.Z = 0;
    }
    if(Cursor.Z >= MAP->getMapSizeZ())
    {
        Cursor.Z = MAP->getMapSizeZ() - 1;
    }
}

