#include <stdafx.h>

#include <Cell.h>
#include <Slope.h>
#include <Face.h>
#include <Cube.h>
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

    for(Uint8 i = 0; i < CELLEDGESIZE; i++)
	{
		for(Uint8 j = 0; j < CELLEDGESIZE; j++)
		{
		    if(Cubes[i][j] != NULL)
		    {
                delete Cubes[i][j];
                for(Facet Face = FACETS_START; Face < NUM_FACETS; ++Face)
                {
                    if(Facets[i][j][Face] != NULL) delete Facets[i][j][Face];
                }
		    }
		}
	}
}

bool Cell::Init()
{
    Initalized = true;
	DirtyDrawlist = true;

	for(Uint8 i = 0; i < CELLEDGESIZE; i++)
	{
		for(Uint8 j = 0; j < CELLEDGESIZE; j++)
		{
		    Cubes[i][j] = NULL;
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

void Cell::CreateIndices()
{
    // INCOMPLETE

    VertIndices[0] = new Uint16[CELLEDGESIZE];
    TextIndices[0] = new Uint16[CELLEDGESIZE];

    Cube* LoopCube = NULL;

    for (Uint16 x = 0; x < CELLEDGESIZE; x++)
    {
        for (Uint16 y = 0; y < CELLEDGESIZE; y++)
        {
            LoopCube = getCube(x, y);
            if (SCREEN->isCubeDrawn(LoopCube))
            {
                if(LoopCube->getSlope())
                {
                    LoopCube->getSlope()->Draw(x, y);
                }




            }
        }
    }
}

bool Cell::Draw(CameraOrientation Orientation)
{
    Cube* LoopCube = NULL;

    //float HalfCell = CELLEDGESIZE / 2;

    if(Initalized)
    {
        for (Uint16 x = 0; x < CELLEDGESIZE; x++)
        {
            for (Uint16 y = 0; y < CELLEDGESIZE; y++)
            {
                LoopCube = getCube(x, y);
                if (SCREEN->isCubeDrawn(LoopCube))
                {
                    if(LoopCube->getSlope())
                    {
                        LoopCube->getSlope()->Draw(x, y);
                        continue;
                    }
                    if(LoopCube->getLiquid())
                    {
                        if(Facets[x][y][FACET_TOP])
                        {
                            Facets[x][y][FACET_TOP]->Draw(x, y);
                        }
                    }

                    switch(Orientation)
                    {
                        case CAMERA_DOWN:
                        {
                            if(Facets[x][y][FACET_TOP])
                            {
                                Facets[x][y][FACET_TOP]->Draw(x, y);
                            }
                            else
                            {
                                if(Facets[x][y][FACET_BOTTOM])
                                {
                                    Facets[x][y][FACET_BOTTOM]->Draw(x, y);
                                }
                            }
                            break;
                        }
                        case CAMERA_NORTH:
                        {
                            if(LoopCube->isSolid())
                            {
                                if(Facets[x][y][FACET_TOP])
                                    Facets[x][y][FACET_TOP]->Draw(x, y);

                                if(Facets[x][y][FACET_SOUTH_EAST])
                                    Facets[x][y][FACET_SOUTH_EAST]->Draw(x, y);

                                if(Facets[x][y][FACET_SOUTH_WEST])
                                    Facets[x][y][FACET_SOUTH_WEST]->Draw(x, y);
                            }
                            else
                            {
                                if(Facets[x][y][FACET_BOTTOM])
                                    Facets[x][y][FACET_BOTTOM]->Draw(x, y);

                                if(Facets[x][y][FACET_NORTH_EAST])
                                    Facets[x][y][FACET_NORTH_EAST]->Draw(x, y);

                                if(Facets[x][y][FACET_NORTH_WEST])
                                    Facets[x][y][FACET_NORTH_WEST]->Draw(x, y);
                            }
                            break;
                        }
                        case CAMERA_EAST:
                        {
                            if(LoopCube->isSolid())
                            {
                                if(Facets[x][y][FACET_TOP])
                                    Facets[x][y][FACET_TOP]->Draw(x, y);

                                if(Facets[x][y][FACET_NORTH_WEST])
                                    Facets[x][y][FACET_NORTH_WEST]->Draw(x, y);

                                if(Facets[x][y][FACET_SOUTH_WEST])
                                    Facets[x][y][FACET_SOUTH_WEST]->Draw(x, y);
                            }
                            else
                            {
                                if(Facets[x][y][FACET_BOTTOM])
                                    Facets[x][y][FACET_BOTTOM]->Draw(x, y);

                                if(Facets[x][y][FACET_NORTH_EAST])
                                    Facets[x][y][FACET_NORTH_EAST]->Draw(x, y);

                                if(Facets[x][y][FACET_SOUTH_EAST])
                                    Facets[x][y][FACET_SOUTH_EAST]->Draw(x, y);
                            }
                            break;
                        }
                        case CAMERA_WEST:
                        {
                            if(LoopCube->isSolid())
                            {
                                if(Facets[x][y][FACET_TOP])
                                    Facets[x][y][FACET_TOP]->Draw(x, y);

                                if(Facets[x][y][FACET_NORTH_EAST])
                                    Facets[x][y][FACET_NORTH_EAST]->Draw(x, y);

                                if(Facets[x][y][FACET_SOUTH_EAST])
                                    Facets[x][y][FACET_SOUTH_EAST]->Draw(x, y);
                            }
                            else
                            {
                                if(Facets[x][y][FACET_BOTTOM])
                                    Facets[x][y][FACET_BOTTOM]->Draw(x, y);

                                if(Facets[x][y][FACET_NORTH_WEST])
                                    Facets[x][y][FACET_NORTH_WEST]->Draw(x, y);

                                if(Facets[x][y][FACET_SOUTH_WEST])
                                    Facets[x][y][FACET_SOUTH_WEST]->Draw(x, y);
                            }
                            break;
                        }
                        case CAMERA_SOUTH:
                        {
                            if(LoopCube->isSolid())
                            {
                                if(Facets[x][y][FACET_TOP])
                                    Facets[x][y][FACET_TOP]->Draw(x, y);

                                if(Facets[x][y][FACET_NORTH_EAST])
                                    Facets[x][y][FACET_NORTH_EAST]->Draw(x, y);

                                if(Facets[x][y][FACET_NORTH_WEST])
                                    Facets[x][y][FACET_NORTH_WEST]->Draw(x, y);
                            }
                            else
                            {
                                if(Facets[x][y][FACET_BOTTOM])
                                    Facets[x][y][FACET_BOTTOM]->Draw(x, y);

                                if(Facets[x][y][FACET_SOUTH_EAST])
                                    Facets[x][y][FACET_SOUTH_EAST]->Draw(x, y);

                                if(Facets[x][y][FACET_SOUTH_WEST])
                                    Facets[x][y][FACET_SOUTH_WEST]->Draw(x, y);
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

void Cell::DrawCellCage()
{
    Vector3 AdjustedPoint = getPosition();

    AdjustedPoint.x -= (CELLEDGESIZE / 2) - 0.5;
    AdjustedPoint.y -= (CELLEDGESIZE / 2) - 0.5;

    SCREEN->DrawCage(AdjustedPoint, CELLEDGESIZE, CELLEDGESIZE, 1.0, true);
}
