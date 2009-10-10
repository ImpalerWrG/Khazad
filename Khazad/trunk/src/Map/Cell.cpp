#include <stdafx.h>

#include <Cell.h>
#include <Cube.h>
#include <Map.h>
#include <DFTypes.h>
#include <Building.h>
#include <Tree.h>
#include <TextureManager.h>
#include <Renderer.h>
#include <string.h>

Cell::Cell()
{
    Active = false;
    ActiveLiquid = false;
    ActiveTop = false;
    Initialized = false;

    for(Uint8 i = 0; i < CELLEDGESIZE; i++)
    {
        for(Uint8 j = 0; j < CELLEDGESIZE; j++)
        {
            Cubes[i][j] = NULL;
            CubeShapeTypes[i][j] = -1;
            CubeMaterialTypes[i][j] = -1;
        }
    }
}

Cell::~Cell()
{
    MAP->ChangeCellCount(-1);

    if(Initialized)
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
            }
        }
    }
    ClearROs();
}

bool Cell::Init()
{
    Initialized = true;
    NeedsRedraw = true;

    MAP->ChangeInitedCellCount(1);

    for(Uint32 CubeX = 0; CubeX < CELLEDGESIZE; CubeX++)
    {
        for(Uint32 CubeY = 0; CubeY < CELLEDGESIZE; CubeY++)
        {
            Cube* TargetCube = Cubes[CubeX][CubeY];
            if(TargetCube != NULL)
            {
                TargetCube->Init();
            }
        }
    }

    return true;
}

Cell::Cell(Sint32 X, Sint32 Y, Sint32 Z)
{
    Active = false;
    Initialized = false;

    for(Uint8 i = 0; i < CELLEDGESIZE; i++)
    {
        for(Uint8 j = 0; j < CELLEDGESIZE; j++)
        {
            Cubes[i][j] = NULL;
            CubeShapeTypes[i][j] = -1;
            CubeMaterialTypes[i][j] = -1;
        }
    }

    Position.x = (float) X + (CELLEDGESIZE / 2) - HALFCUBE;
    Position.y = (float) Y + (CELLEDGESIZE / 2) - HALFCUBE;
    Position.z = (float) Z;

    MAP->ChangeCellCount(1);
}

Cube* Cell::getCube(Uint8 X, Uint8 Y)
{
	if(X < CELLEDGESIZE && Y < CELLEDGESIZE)
	{
	    return Cubes[X][Y];
	}

    return NULL;
}

Cube* Cell::GenerateCube(Uint8 CubeX, Uint8 CubeY)
{
	if (Cubes[CubeX][CubeY] == NULL)
	{
	    Cube* NewCube = new Cube();
	    Cubes[CubeX][CubeY] = NewCube;
        NewCube->setOwner(this, CubeX, CubeY);

        return NewCube;
	}

    return NULL;
}

void Cell::setCube(Cube* NewCube, Uint8 x, Uint8 y)
{
    Cubes[x][y] = NewCube;
    Cubes[x][y]->setOwner(this, x, y);
}

bool Cell::hasFace(Uint8 x, Uint8 y, Facet FaceType)
{
    assert(x < CELLEDGESIZE && y < CELLEDGESIZE );
    if(Initialized)
    {
        return Cubes[x][y]->hasFace(FaceType);
    }
    return false;
}

bool Cell::Update()
{
    return true;
}

void Cell::addBuilding(Building* b)
{
    buildings.push_back(b);
}

void Cell::addTree(Tree* t)
{
    trees.push_back(t);
}

void Cell::Render(bool drawtop)
{
    for( map<int16_t, ROstore >::iterator it = ROs.begin(); it != ROs.end(); ++it)
    {
        TEXTURE->BindTexture(it->first);
        if(it->second.normal)
        {
            RENDERER->CallRenderObject(it->second.normal);
        }
        if(drawtop && it->second.top)
        {
            RENDERER->CallRenderObject(it->second.top);
        }
    }
    for(Uint32 i = 0; i< trees.size(); i++)
    {
        trees[i]->Draw();
    }
}

void Cell::UpdateLists()
{
    //maps between texture and vertex vectors
    Geometry.clear();

    Cube* LoopCube = NULL;
    if(Initialized)
    {
        for (Uint16 x = 0; x < CELLEDGESIZE; x++)
        {
            for (Uint16 y = 0; y < CELLEDGESIZE; y++)
            {
                LoopCube = getCube(x, y);
                if (LoopCube != NULL && RENDERER->isCubeDrawn(LoopCube))
                {
                    LoopCube->Draw(x, y);
                }
            }
        }

        //TODO: only regenerate changed ones
        // destroy old
        ClearROs();
        // for each material in normal geometry
        for( map<int16_t, vector < vertex > * >::iterator it = Geometry.begin(); it != Geometry.end(); ++it)
        {
            int16_t material = it->first;
            vector < vertex > * vertices = it->second;

            // generate VBOs out of vertex arrays
            RenderObject * RO = RENDERER->CreateRenderObject(vertices);
            delete vertices;
            // create descriptor
            ROstore tempRO;
            if(ROs.count(material))
            {
                tempRO = ROs[material];
                tempRO.normal = RO;
            }
            else
            {
                tempRO= ROstore(RO,NULL);
            }
            ROs[material] = tempRO;
        }
    }
}

void Cell::ClearROs()
{
    for( map<int16_t, ROstore >::iterator it = ROs.begin(); it != ROs.end(); ++it)
    {
        ROstore tmp = it->second;
        if(tmp.normal)
        {
            RENDERER->DeleteRenderObject( tmp.normal );
        }
        if(tmp.top)
        {
            RENDERER->DeleteRenderObject( tmp.top );
        }
    }
    ROs.clear();
}

void Cell::DrawCellCage()
{
    Vector3 AdjustedPoint = Position;

    AdjustedPoint.x -= (CELLEDGESIZE / 2) - 0.5;
    AdjustedPoint.y -= (CELLEDGESIZE / 2) - 0.5;

    RENDERER->DrawCage(AdjustedPoint, CELLEDGESIZE, CELLEDGESIZE, 1.0, true, 1, 1, 1);
}
