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
#include <Game.h>
#include <Map.h>
#include <Cell.h>
#include <Gui.h>


DECLARE_SINGLETON(ScreenManager)

ScreenManager::ScreenManager()
{
	SCREEN_WIDTH = CONFIG->getXResolution();
	SCREEN_HEIGHT = CONFIG->getYResolution();
	SCREEN_BPP = 32;
}

ScreenManager::~ScreenManager()
{

}

bool ScreenManager::Draw()
{
	return true;
}

/*
bool ScreenManager::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) == -1) // Could we start SDL_VIDEO?
	{
		std::cerr << "Couldn't init SDL";
		return false;
	}

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, SCREEN_BPP);
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 0);
	SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 0);
	SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 0);
	SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 0);
	SDL_GL_SetAttribute( SDL_GL_ACCUM_ALPHA_SIZE, 0);

	atexit(SDL_Quit); // Now that we're enabled, make sure we cleanup

	/*
	int videoFlags;
	const SDL_VideoInfo *videoInfo;

	/*
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
		std::cerr << "Video initialization failed: %s\n";

    videoInfo = SDL_GetVideoInfo( );
    if ( !videoInfo )
	    std::cerr << "Video query failed: %s\n";

	/*
    videoFlags  = SDL_OPENGL;          // Enable OpenGL in SDL
    videoFlags |= SDL_GL_DOUBLEBUFFER; // Enable double buffering
    videoFlags |= SDL_HWPALETTE;       // Store the palette in hardware
    videoFlags |= SDL_RESIZABLE;       // Enable window resizing

    // This checks to see if surfaces can be stored in memory
    if ( videoInfo->hw_available )
		videoFlags |= SDL_HWSURFACE;
    else
		videoFlags |= SDL_SWSURFACE;


    if ( videoInfo->blit_hw )
		videoFlags |= SDL_HWACCEL;
	/

	ScreenSurface = SDL_SetVideoMode(CONFIG->getXResolution(), CONFIG->getYResolution(), SCREEN_BPP, SDL_OPENGL | SDL_RESIZABLE); //videoFlags);
	if (!ScreenSurface)
	{
		std::cerr << "Couldn't create screen";
		return false;
	}

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	//initGL();

	MainCamera = new Camera();
	MainCamera->Init(true);

	if (glGetError() != GL_NO_ERROR)
	{
		return false;
	}

	SDL_WM_SetCaption("Khazad Keep", "Icon goes here?");
	return true;
}
*/



bool ScreenManager::Init()
{
    // We simply initialise OpenGL and SDL as we would do with any OpenGL
    // and SDL application.
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    ScreenSurface = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_RESIZABLE);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glClearColor(0.0, 0.0, 0.0, 0.0);

    // We want unicode for the SDLInput object to function properly.
    SDL_EnableUNICODE(1);
    // We also want to enable key repeat.
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);


	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	//glBlendFunc(GL_ONE, GL_ONE);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);


    imageLoader = new gcn::OpenGLSDLImageLoader();
    gcn::Image::setImageLoader(imageLoader);


    MainCamera = new Camera();
	MainCamera->Init(true);
}


bool ScreenManager::ReSizeScreen(Uint16 Width, Uint16 Hight)
{
	ScreenSurface = SDL_SetVideoMode(Width, Hight, SCREEN_BPP, SDL_OPENGL | SDL_RESIZABLE);

	if(SDL_Flip(ScreenSurface) == -1)
	{
		return true;
	}
	return false;
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

void ScreenManager::FillRect(SDL_Rect* Rectangle)
{
	SDL_FillRect( ScreenSurface, Rectangle, SDL_MapRGB( ScreenSurface->format, 0xFF, 0xFF, 0xFF ) );
}

void ScreenManager::applyClipCentered(SDL_Rect Offset, ClipImage* Clip)
{
	if (Clip != NULL)
	{
		SDL_Rect finalOffset;
		finalOffset.x = Offset.x - Clip->ParentPage->clipWidth / 2;
		finalOffset.y = Offset.y - Clip->ParentPage->clipHight / 2;
		SDL_BlitSurface( Clip->ParentPage->RawSurface, &Clip->OffsetRect, ScreenSurface, &Offset );
	}
}

void ScreenManager::BlitText( const char* Text, SDL_Color Color, SDL_Rect Offset, Uint8 FontIndex )
{
	SDL_Surface* NewFontSurface = FONT->makeFontSurface(Text, Color, FontIndex);

	if (NewFontSurface != NULL)
	{
		SDL_BlitSurface( NewFontSurface, NULL, ScreenSurface, &Offset );
		SDL_FreeSurface( NewFontSurface );
	}
}

void ScreenManager::DrawPrim( DiffusePrim* pVertices, Uint32 count )
{
	glPushMatrix();
	glPushAttrib( GL_CURRENT_BIT );
	glLoadIdentity();

	//start pushing triangles through the pipeline
	glBegin( GL_TRIANGLES );

	for(Uint32 i = 0; i < count; i++)
	{
		glColor4f( pVertices[i].Red, pVertices[i].Green, pVertices[i].Blue, pVertices[i].Alpha );
		glVertex3f( pVertices[i].XComponent, pVertices[i].YComponent, pVertices[i].ZComponent );
	}

	//finished
	glEnd();
	//glLoadIdentity();
	glPopAttrib();
	glPopMatrix();
}

void ScreenManager::DrawPrim( DiffuseTexPrim* pVertices, Uint32 count )
{
	glPushMatrix();
	glPushAttrib( GL_CURRENT_BIT );
	glLoadIdentity();

	//start pushing triangles through the pipeline
	glBegin( GL_TRIANGLES );

	for(Uint32 i = 0; i < count; i++)
	{
		glColor4f( pVertices[i].Red, pVertices[i].Green, pVertices[i].Blue, pVertices[i].Alpha );
		glTexCoord2f( pVertices[i].tu, pVertices[i].tv );
		glVertex3f( pVertices[i].XComponent, pVertices[i].YComponent, pVertices[i].ZComponent );
	}

	//finished
	glEnd();
	glPopAttrib();
	glPopMatrix();
}

void ScreenManager::DrawPrim( NormalDiffuseTexPrim* pVertices, Uint32 count )
{
	glPushMatrix();
	glPushAttrib( GL_CURRENT_BIT );
	glLoadIdentity();

	//start pushing triangles through the pipeline
	glBegin( GL_TRIANGLES );

	for(Uint32 i = 0; i < count; i++)
	{
		glNormal3f( pVertices[i].XNormal, pVertices[i].YNormal, pVertices[i].ZNormal );
		glColor4f( pVertices[i].Red, pVertices[i].Green, pVertices[i].Blue, pVertices[i].Alpha );
		glTexCoord2f( pVertices[i].tu, pVertices[i].tv );
		glVertex3f( pVertices[i].XComponent, pVertices[i].YComponent, pVertices[i].ZComponent );
	}

	//finished
	glEnd();
	glPopAttrib();
	glPopMatrix();
}

void ScreenManager::DrawQuads( NormalDiffuseTexPrim* pVertices, Uint32 count )
{
	glPushMatrix();
	glPushAttrib( GL_CURRENT_BIT );
	glLoadIdentity();

	//start pushing triangles through the pipeline
	glBegin( GL_QUADS );

	for(Uint32 i = 0; i < count; i++)
	{
		glNormal3f( pVertices[i].XNormal, pVertices[i].YNormal, pVertices[i].ZNormal );
		glColor4f( pVertices[i].Red, pVertices[i].Green, pVertices[i].Blue, pVertices[i].Alpha );
		glTexCoord2f( pVertices[i].tu, pVertices[i].tv );
		glVertex3f( pVertices[i].XComponent, pVertices[i].YComponent, pVertices[i].ZComponent );
	}

	//finished
	glEnd();
	glPopAttrib();
	glPopMatrix();
}

bool ScreenManager::ColorBackground()
{
	SDL_FillRect( ScreenSurface, &ScreenSurface->clip_rect, SDL_MapRGB( ScreenSurface->format, 0x0, 0x0, 0xFF ) );
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
	return SCREEN_WIDTH;
}

Uint16 ScreenManager::getHight()
{
	return 	SCREEN_HEIGHT;
}

void ScreenManager::DirtyAllLists()
{
	for(Uint32 SizeZ = 0; SizeZ < MAP->CellSizeZ; SizeZ++)
	{
		for (Uint32 SizeX = 0; SizeX < MAP->CellSizeX; SizeX++)
		{
			for (Uint32 SizeY = 0; SizeY < MAP->CellSizeY; SizeY++)
			{
				MAP->getCell(SizeX, SizeY, SizeZ)->DirtyDrawlist = true;
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
	RedPickingValue = 0;
	GreenPickingValue = 0;
	BluePickingValue = 0;

	TotalTriangles = 0;
    TriangleCounter = 0;

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	MainCamera->UpdateView();
	Uint16 CellEdgeLenth = CONFIG->getCellEdgeLength();

    glBindTexture(GL_TEXTURE_2D, TEXTURE->SingularTextureLibrary[0]);

	for(Uint16 Zlevel = 0; Zlevel < MAP->CellSizeZ; Zlevel++) // Bottom up drawing
	{
		if (Zlevel <= MainCamera->LookZ())
		{
			for (Uint32 SizeX = 0; SizeX < MAP->CellSizeX; SizeX++)
			{
				for (Uint32 SizeY = 0; SizeY < MAP->CellSizeY; SizeY++)
				{
					Cell* LoopCell = MAP->getCell(SizeX, SizeY, Zlevel);

					if  (MainCamera->sphereInFrustum(LoopCell->Position, CellEdgeLenth))
					{
						if (LoopCell->DirtyDrawlist == true)
						{
							// Rebuild the new Drawlist
							glDeleteLists(LoopCell->DrawListID, 1);
							glNewList(LoopCell->DrawListID, GL_COMPILE_AND_EXECUTE);

                                TriangleCounter = 0;  // Reset Counter and Track Triangle count
                                glBegin(GL_TRIANGLES);
                                    LoopCell->Draw();
                                glEnd();
                                LoopCell->setTriangleCount(TriangleCounter);
                                TotalTriangles += TriangleCounter;

							glEndList();
							LoopCell->DirtyDrawlist = false;
						}
						else
						{
							glCallList(LoopCell->DrawListID);
							TotalTriangles += LoopCell->getTriangleCount();  // Use stored Triangle Count
						}
					}
				}
			}
		}
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

    glTranslatef(10.0, 10.0, 10.0);

	ShowAxis();

	//return Flip();
	return true;
}

void ScreenManager::IncrementTriangles(Uint32 Triangles)
{
    TriangleCounter += Triangles;
}

float ScreenManager::getShading(float Zlevel)
{
	if (Zlevel <= MainCamera->LookZ())
	{
		float Depth = MainCamera->LookZ() - Zlevel;
		if (Depth < MainCamera->getViewLevels())
		{
			float Shading = 1.0;
			if (Depth > 0) // Below look level
			{
				Shading -= Depth / MainCamera->getViewLevels();
				return Shading;
			}
			return Shading;
		}
		return 0.0;
	}
	else
	{
		return 0.0;
	}
}

void ScreenManager::ToggleFullScreen()
{
    SDL_WM_ToggleFullScreen(ScreenSurface);
}

void ScreenManager::Enable2D()
{
	int vPort[4];

	glGetIntegerv(GL_VIEWPORT, vPort);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
}

void ScreenManager::Disable2D()
{
    glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void ScreenManager::PrintDebugging()
{
	MainCamera->PrintDebugging();
}

void ScreenManager::ShowAxis(void)
{
    Vector3 Point;

    DrawPoint(Point, 100);
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
