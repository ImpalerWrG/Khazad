#include <stdafx.h>

#include <Cell.h>
#include <Slope.h>
#include <Face.h>
#include <Cube.h>
#include <ConfigManager.h>
#include <Random.h>
#include <Game.h>

/*
Cell::Cell()
{
	Active = false;
	setType(CELL_ACTOR);
    Initalized = false;
}*/

Cell::~Cell()
{

}

bool Cell::Init()
{
    Initalized = true;

    Cubes = new Cube**[CELLEDGESIZE];
    Facets = new Face***[CELLEDGESIZE];

	float HalfCell = CELLEDGESIZE / 2;

	for(Uint8 i = 0; i < CELLEDGESIZE; i++)
	{
		Cubes[i] = new Cube*[CELLEDGESIZE];
		Facets[i] = new Face**[CELLEDGESIZE];

		for(Uint8 j = 0; j < CELLEDGESIZE; j++)
		{
		    Cubes[i][j] = NULL;

            Facets[i][j] = new Face*[NUM_FACETS];

            for(Facet Face = FACETS_START; Face < NUM_FACETS; ++Face)
            {
                Facets[i][j][Face] = NULL;
            }
		}
	}

	return true;
}

Cell::Cell(Sint32 X, Sint32 Y, Sint32 Z)
{
	Active = false;
    Initalized = false;

    DrawListID = glGenLists(5);

	setType(CELL_ACTOR);

	Position.x = (float) X + (CELLEDGESIZE / 2) - HALFCUBE;
	Position.y = (float) Y + (CELLEDGESIZE / 2) - HALFCUBE;
	Position.z = (float) Z;

    //Basment = false;
}

Cube* Cell::getCube(Uint8 x, Uint8 y)
{
    if(Initalized)
    {
        if(x < CELLEDGESIZE && y < CELLEDGESIZE)
        {
            return Cubes[x][y];
        }
    }
    return NULL;
}

void Cell::setCube(Cube* NewCube, Uint8 x, Uint8 y)
{
    if(!Initalized)
    {
        Init();
    }

    Cubes[x][y] = NewCube;
    Cubes[x][y]->SetOwner(this, x, y);
}

Face* Cell::getFace(Uint8 x, Uint8 y, Facet FaceType)
{
    if(Initalized)
    {
        if(x < CELLEDGESIZE && y < CELLEDGESIZE && FaceType < NUM_FACETS)
        {
            return Facets[x][y][FaceType];
        }
    }
    return NULL;
}

void Cell::setFace(Face* NewFace, Uint8 x, Uint8 y, Facet FaceType)
{
    if(x < CELLEDGESIZE && y < CELLEDGESIZE && FaceType < NUM_FACETS)
    {
        if(!Initalized)
        {
            Init();
        }

        Facets[x][y][FaceType] = NewFace;
    }
}

bool Cell::Update()
{
	return true;
}

bool Cell::Draw(CameraOrientation Orientation, bool DrawHidden, bool DrawSubTerranian, bool DrawSkyView, bool DrawSunLit)
{
    Cube* LoopCube = NULL;

    float HalfCell = CELLEDGESIZE / 2;

    if(Initalized)
    {
        for (Uint16 x = 0; x < CELLEDGESIZE; x++)
        {
            for (Uint16 y = 0; y < CELLEDGESIZE; y++)
            {
                LoopCube = getCube(x, y);
                if (LoopCube->isVisible())
                {
                    if(LoopCube->getSlope())
                    {
                        LoopCube->getSlope()->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                    }
                    //LoopCube->Draw(Orientation, x - HalfCell + HALFCUBE, y - HalfCell + HALFCUBE, DrawHidden, DrawSubTerranian, DrawSkyView, DrawSunLit);

                    for(Facet Face = FACETS_START; Face < NUM_FACETS; ++Face)
                    {
                        if(Facets[x][y][Face])
                        {
                            Facets[x][y][Face]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                        }
                    }
                }
            }
        }
    }

	return true;
}
