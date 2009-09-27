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

    for(Uint8 i = 0; i < CELLEDGESIZE; i++)
    {
        for(Uint8 j = 0; j < CELLEDGESIZE; j++)
        {
            Cubes[i][j] = NULL;
        }
    }

    MAP->ChangeInitedCellCount(1);

    return true;
}

Cell::Cell(Sint32 X, Sint32 Y, Sint32 Z)
{
    Active = false;
    Initialized = false;
    Position.x = (float) X + (CELLEDGESIZE / 2) - HALFCUBE;
    Position.y = (float) Y + (CELLEDGESIZE / 2) - HALFCUBE;
    Position.z = (float) Z;
    MAP->ChangeCellCount(1);
}

Cube* Cell::getCube(Uint8 x, Uint8 y)
{
    if(Initialized)
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
    if(!Initialized)
    {
        Init();
    }
    Cubes[x][y] = NewCube;
    Cubes[x][y]->SetOwner(this, x, y);
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

void Cell::addBuilding(Building * b)
{
    buildings.push_back(b);
}

void Cell::addTree(Tree * t)
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
}

void Cell::UpdateLists()
{
    //maps between texture and vertex vectors
    map<int16_t, vector < vertex > * > normal; // normal geometry
    map<int16_t, vector < vertex > * > tops; // tops blocked by geometry above
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
                    LoopCube->Draw(x, y, normal, tops);
                }
            }
        }
        //TODO: only regenerate changed ones
        // destroy old
        ClearROs();
        // for each material in normal geometry
        for( map<int16_t, vector < vertex > * >::iterator it = normal.begin(); it != normal.end(); ++it)
        {
            int16_t material = it->first;
            vector < vertex > * vertices = it->second;

            // generate VBOs out of vertex arrays
            RenderObject * RO = RENDERER->CreateRenderObject(vertices);

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
        // for each material in top geometry
        for( map<int16_t, vector < vertex > * >::iterator it = tops.begin(); it != tops.end(); ++it)
        {
            int16_t material = it->first;
            vector < vertex > * vertices = it->second;
            // build vbo
            RenderObject * RO = RENDERER->CreateRenderObject(vertices);

            // assign vbo to descriptor
            ROstore tempRO;
            if(ROs.count(material))
            {
                tempRO = ROs[material];
                tempRO.top = RO;
            }
            else
            {
                tempRO= ROstore(NULL,RO);
            }
            // store descriptor
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
            RENDERER->DeleteRenderObject( tmp.normal );
        if(tmp.top)
            RENDERER->DeleteRenderObject( tmp.top );
    }
    ROs.clear();
}
void Cell::DrawCellCage()
{
    Vector3 AdjustedPoint = Position;

    AdjustedPoint.x -= (CELLEDGESIZE / 2) - 0.5;
    AdjustedPoint.y -= (CELLEDGESIZE / 2) - 0.5;

    RENDERER->DrawCage(AdjustedPoint, CELLEDGESIZE, CELLEDGESIZE, 1.0, true,1,1,1);
}
