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
#include <DataManager.h>
#include <Game.h>
#include <Map.h>
#include <Cell.h>
#include <Gui.h>


DECLARE_SINGLETON(ScreenManager)

ScreenManager::ScreenManager()
{
	ScreenWidth = CONFIG->getXResolution();
	ScreenHight = CONFIG->getYResolution();
	ScreenBPP = 32;
	FullScreen = false;
}

ScreenManager::~ScreenManager()
{

}

bool ScreenManager::Init()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_WM_SetCaption("Khazad", "Khazad");

    SDL_Surface* Icon = SDL_LoadBMP("Assets\\Textures\\Khazad_Icon.bmp");
    Uint32 colorkey = SDL_MapRGB(Icon->format, 255, 0, 255);
    SDL_SetColorKey(Icon, SDL_SRCCOLORKEY, colorkey);
    SDL_WM_SetIcon(Icon, NULL);

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
    ScreenWidth = Width;
    ScreenHight = Hight;

    glViewport(0, 0, ScreenWidth, ScreenHight);
    MainCamera->setIsometricProj(ScreenWidth, ScreenHight, 10000.0);

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
	return ScreenWidth;
}

Uint16 ScreenManager::getHight()
{
	return 	ScreenHight;
}

void ScreenManager::DirtyAllLists()
{
    if(MAP == NULL)
        return;

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

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(1.0 / (float) TEXTURE->getAggragateTextureSize(), 1.0 / (float) TEXTURE->getAggragateTextureSize(), 1);
    glBindTexture(GL_TEXTURE_2D, TEXTURE->getAggragateTexture());

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

    //glTranslatef(10.0, 10.0, 10.0);

	//ShowAxis();


    glMatrixMode(GL_TEXTURE);  //return to normal Texturing for UI
    glPopMatrix();
    glLoadIdentity();

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
