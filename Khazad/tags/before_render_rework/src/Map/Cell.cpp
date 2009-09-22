#include <stdafx.h>

#include <Cell.h>
//#include <Slope.h>
//#include <Face.h>
#include <Cube.h>
#include <Map.h>
/// FIXME: dfhack paths
#include <DFTypes.h>
#include <Building.h>
#include <Tree.h>


Cell::Cell()
{
	Active = false;
	ActiveLiquid = false;
    ActiveTop = false;
	setType(CELL_ACTOR);
    Initalized = false;
}

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
/*                for(Facet Face = FACETS_START; Face < NUM_FACETS; ++Face)
                {
                    if(Facets[i][j][Face] != NULL) delete Facets[i][j][Face];
                }*/
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
/*            for(Facet Face = FACETS_START; Face < NUM_FACETS; ++Face)
            {
                Facets[i][j][Face] = NULL;
            }*/
		}
	}

    MAP->ChangeInitedCellCount(1);

	return true;
}

Cell::Cell(Sint32 X, Sint32 Y, Sint32 Z)
{
	Active = false;
    Initalized = false;

    DrawListID = glGenLists(7);

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

bool Cell::hasFace(Uint8 x, Uint8 y, Facet FaceType)
{
    assert(x < CELLEDGESIZE && y < CELLEDGESIZE );
    if(Initalized)
    {
        return Cubes[x][y]->hasFace(FaceType);
    }
    return false;
}
/*
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
*/
bool Cell::Update()
{
	return true;
}
/*
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
*/
void Cell::addBuilding(Building * b)
{
    buildings.push_back(b);
}

void Cell::addTree(Tree * t)
{
    trees.push_back(t);
}

bool Cell::DrawLiquids()
{
    Cube* LoopCube = NULL;
    if(Initalized)
    {
        for (Uint16 x = 0; x < CELLEDGESIZE; x++)
        {
            for (Uint16 y = 0; y < CELLEDGESIZE; y++)
            {
                LoopCube = getCube(x, y);
                if (LoopCube != NULL && SCREEN->isCubeDrawn(LoopCube))
                {
                    LoopCube->DrawLiquid(x,y);
                }
            }
        }
    }
}

bool Cell::DrawSolids(CameraOrientation Orientation)
{
    Cube* LoopCube = NULL;

    if(Initalized)
    {
        for (Uint16 x = 0; x < CELLEDGESIZE; x++)
        {
            for (Uint16 y = 0; y < CELLEDGESIZE; y++)
            {
                LoopCube = getCube(x, y);
                if (LoopCube != NULL && SCREEN->isCubeDrawn(LoopCube))
                {
                    if(LoopCube->isSlope())
                    {
                        LoopCube->DrawSlope(x, y);
                    }
                    switch(Orientation)
                    {
                        case CAMERA_DOWN:
                        {
                            LoopCube->DrawFace(x,y,FACET_TOP,true);
                            LoopCube->DrawFace(x,y,FACET_BOTTOM,true);
                            break;
                        }
                        case CAMERA_NORTH_WEST:
                        {
                            if(LoopCube->isSolid())
                            {
                                LoopCube->DrawFace(x,y,FACET_TOP,true);
                                LoopCube->DrawFace(x,y,FACET_EAST,true);
                                LoopCube->DrawFace(x,y,FACET_SOUTH,true);
                            }
                            else
                            {
                                LoopCube->DrawFace(x,y,FACET_BOTTOM,true);
                                LoopCube->DrawFace(x,y,FACET_WEST,true);
                                LoopCube->DrawFace(x,y,FACET_NORTH,true);
                            }
                            break;
                        }
                        case CAMERA_SOUTH_WEST:
                        {
                            if(LoopCube->isSolid())
                            {
                                LoopCube->DrawFace(x,y,FACET_TOP,true);
                                LoopCube->DrawFace(x,y,FACET_EAST,true);
                                LoopCube->DrawFace(x,y,FACET_NORTH,true);
                            }
                            else
                            {
                                LoopCube->DrawFace(x,y,FACET_BOTTOM,true);
                                LoopCube->DrawFace(x,y,FACET_WEST,true);
                                LoopCube->DrawFace(x,y,FACET_SOUTH,true);
                            }
                            break;
                        }
                        case CAMERA_NORTH_EAST:
                        {
                            if(LoopCube->isSolid())
                            {
                                LoopCube->DrawFace(x,y,FACET_TOP,true);
                                LoopCube->DrawFace(x,y,FACET_WEST,true);
                                LoopCube->DrawFace(x,y,FACET_SOUTH,true);
                            }
                            else
                            {
                                LoopCube->DrawFace(x,y,FACET_BOTTOM,true);
                                LoopCube->DrawFace(x,y,FACET_EAST,true);
                                LoopCube->DrawFace(x,y,FACET_NORTH,true);
                            }
                            break;
                        }
                        case CAMERA_SOUTH_EAST:
                        {
                            if(LoopCube->isSolid())
                            {
                                LoopCube->DrawFace(x,y,FACET_TOP,true);
                                LoopCube->DrawFace(x,y,FACET_NORTH,true);
                                LoopCube->DrawFace(x,y,FACET_WEST,true);
                            }
                            else
                            {
                                LoopCube->DrawFace(x,y,FACET_BOTTOM,true);
                                LoopCube->DrawFace(x,y,FACET_EAST,true);
                                LoopCube->DrawFace(x,y,FACET_SOUTH,true);
                            }
                            break;
                        }
                    }
                }
            }
        }
        for(int i = 0; i < buildings.size();i++)
        {
            buildings[i]->Draw(Orientation);
        }
        for(int i = 0; i < trees.size();i++)
        {
            trees[i]->Draw(Orientation);
        }
    }

	return true;
}

bool Cell::DrawTops()
{
    Cube* LoopCube = NULL;

    if(Initalized)
    {
        for (Uint16 x = 0; x < CELLEDGESIZE; x++)
        {
            for (Uint16 y = 0; y < CELLEDGESIZE; y++)
            {
                LoopCube = getCube(x, y);
                if (LoopCube != NULL && SCREEN->isCubeDrawn(LoopCube))
                {
                    LoopCube->DrawFace(x,y,FACET_TOP,false);
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

    SCREEN->DrawCage(AdjustedPoint, CELLEDGESIZE, CELLEDGESIZE, 1.0, true,1,1,1);
}
