#include <stdafx.h>

#include <Cell.h>
#include <Slope.h>
#include <Face.h>
#include <Cube.h>
#include <ConfigManager.h>
#include <Random.h>
#include <Game.h>
#include <Map.h>

/*
Cell::Cell()
{
	Active = false;
	setType(CELL_ACTOR);
    Initalized = false;
}*/

Cell::~Cell()
{
    MAP->ChangeCellCount(-1);

    if(Initalized)
    {
        MAP->ChangeInitedCellCount(-1);
    }
}

bool Cell::Init()
{
    Initalized = true;
	DirtyDrawlist = true;

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

    MAP->ChangeInitedCellCount(1);

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

    MAP->ChangeCellCount(1);
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

bool Cell::Draw(CameraOrientation Orientation, bool DrawHidden, bool DrawSubTerranean, bool DrawSkyView, bool DrawSunLit)
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
                    if(LoopCube->isHidden() && !DrawHidden)
                    {
                        continue;
                    }
                    if(LoopCube->isSubTerranean() && !DrawSubTerranean)
                    {
                        continue;
                    }
                    if(LoopCube->isSkyView() && !DrawSkyView)
                    {
                        continue;
                    }
                    if(LoopCube->isSunLit() && !DrawSunLit)
                    {
                        continue;
                    }

                    if(LoopCube->getSlope())
                    {
                        LoopCube->getSlope()->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                    }
                    if(LoopCube->getLiquid())
                    {
                        if(Facets[x][y][FACET_TOP])
                        {
                            Facets[x][y][FACET_TOP]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                        }
                    }

                    switch(Orientation)
                    {
                        case CAMERA_DOWN:
                        {
                            if(Facets[x][y][FACET_TOP])
                            {
                                Facets[x][y][FACET_TOP]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                            }
                            else
                            {
                                if(Facets[x][y][FACET_BOTTOM])
                                {
                                    Facets[x][y][FACET_BOTTOM]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                                }
                            }
                            break;
                        }
                        case CAMERA_NORTH:
                        {
                            if(LoopCube->isSolid())
                            {
                                if(Facets[x][y][FACET_TOP])
                                    Facets[x][y][FACET_TOP]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_SOUTH_EAST])
                                    Facets[x][y][FACET_SOUTH_EAST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_SOUTH_WEST])
                                    Facets[x][y][FACET_SOUTH_WEST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                            }
                            else
                            {
                                if(Facets[x][y][FACET_BOTTOM])
                                    Facets[x][y][FACET_BOTTOM]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_NORTH_EAST])
                                    Facets[x][y][FACET_NORTH_EAST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_NORTH_WEST])
                                    Facets[x][y][FACET_NORTH_WEST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                            }
                            break;
                        }
                        case CAMERA_EAST:
                        {
                            if(LoopCube->isSolid())
                            {
                                if(Facets[x][y][FACET_TOP])
                                    Facets[x][y][FACET_TOP]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_NORTH_WEST])
                                    Facets[x][y][FACET_NORTH_WEST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_SOUTH_WEST])
                                    Facets[x][y][FACET_SOUTH_WEST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                            }
                            else
                            {
                                if(Facets[x][y][FACET_BOTTOM])
                                    Facets[x][y][FACET_BOTTOM]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_NORTH_EAST])
                                    Facets[x][y][FACET_NORTH_EAST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_SOUTH_EAST])
                                    Facets[x][y][FACET_SOUTH_EAST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                            }
                            break;
                        }
                        case CAMERA_WEST:
                        {
                            if(LoopCube->isSolid())
                            {
                                if(Facets[x][y][FACET_TOP])
                                    Facets[x][y][FACET_TOP]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_NORTH_EAST])
                                    Facets[x][y][FACET_NORTH_EAST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_SOUTH_EAST])
                                    Facets[x][y][FACET_SOUTH_EAST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                            }
                            else
                            {
                                if(Facets[x][y][FACET_BOTTOM])
                                    Facets[x][y][FACET_BOTTOM]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_NORTH_WEST])
                                    Facets[x][y][FACET_NORTH_WEST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_SOUTH_WEST])
                                    Facets[x][y][FACET_SOUTH_WEST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                            }
                            break;
                        }
                        case CAMERA_SOUTH:
                        {
                            if(LoopCube->isSolid())
                            {
                                if(Facets[x][y][FACET_TOP])
                                    Facets[x][y][FACET_TOP]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_NORTH_EAST])
                                    Facets[x][y][FACET_NORTH_EAST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_NORTH_WEST])
                                    Facets[x][y][FACET_NORTH_WEST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                            }
                            else
                            {
                                if(Facets[x][y][FACET_BOTTOM])
                                    Facets[x][y][FACET_BOTTOM]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_SOUTH_EAST])
                                    Facets[x][y][FACET_SOUTH_EAST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);

                                if(Facets[x][y][FACET_SOUTH_WEST])
                                    Facets[x][y][FACET_SOUTH_WEST]->Draw(Position.x + x - HalfCell + HALFCUBE, Position.y + y - HalfCell + HALFCUBE);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

	return true;
}
