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
    ClearVBOs();
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

GLuint Cell::BuildVBO(vertex * data, uint32_t size)
{
    GLuint vbo = 0;
    // Generate And Bind The Vertex Buffer
    RENDERER->glGenBuffers( 1, &vbo ); // Get A Valid Name
    RENDERER->glBindBuffer( GL_ARRAY_BUFFER, vbo ); // Bind The Buffer
    // Load The Data
    RENDERER->glBufferData( GL_ARRAY_BUFFER_ARB, size, data, GL_STATIC_DRAW_ARB );
    // unbind
    RENDERER->glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vbo;
}

void Cell::CallVBO(GLuint vbo, uint32_t count)
{
    RENDERER->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glTexCoordPointer(2, GL_FLOAT, 32, (const GLvoid*) 12); // texture coords
    glNormalPointer(GL_FLOAT, 32, (const GLvoid*) 20); // normal vectors
    glVertexPointer(3, GL_FLOAT, 32 , 0);
    glDrawArrays(GL_TRIANGLES,0,count);
    RENDERER->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cell::Render(bool drawtop)
{
    for( map<int16_t, VBOStruct >::iterator it = VBOs.begin(); it != VBOs.end(); ++it)
    {
        TEXTURE->BindTexture(it->first);
        if(it->second.hasNormal)
        {
            CallVBO(it->second.normal, it->second.countNormal);
        }
        if(drawtop && it->second.hasTop)
        {
            CallVBO(it->second.top, it->second.countTop);
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
        ClearVBOs();
        // for each material in normal geometry
        for( map<int16_t, vector < vertex > * >::iterator it = normal.begin(); it != normal.end(); ++it)
        {
            int16_t material = it->first;
            vector < vertex > & vertices = *it->second;

            // generate VBOs out of vertex arrays
            GLuint VBO = BuildVBO(&vertices[0],vertices.size()*sizeof(vertex));

            // create descriptor
            VBOStruct tempVBO;
            if(VBOs.count(material))
            {
                tempVBO = VBOs[material];
                tempVBO.countNormal = vertices.size();
                tempVBO.normal = VBO;
                tempVBO.hasNormal = 1;
            }
            else
            {
                tempVBO= VBOStruct(true,VBO,vertices.size(),   false,0,0);
            }
            VBOs[material] = tempVBO;

            // delete the original arrays
            delete(&vertices);
        }
        // for each material in top geometry
        for( map<int16_t, vector < vertex > * >::iterator it = tops.begin(); it != tops.end(); ++it)
        {
            int16_t material = it->first;
            vector < vertex > & vertices = *it->second;
            // build vbo
            GLuint VBO = BuildVBO(&vertices[0],vertices.size()*sizeof(vertex));

            // assign vbo to descriptor
            VBOStruct tempVBO;
            if(VBOs.count(material))
            {
                tempVBO = VBOs[material];
                tempVBO.countTop = vertices.size();
                tempVBO.top = VBO;
                tempVBO.hasTop = 1;
            }
            else
            {
                tempVBO= VBOStruct(false,0,0, true,VBO,vertices.size());
            }

            // store descriptor
            VBOs[material] = tempVBO;
            // delete the original arrays
            delete(&vertices);
        }
    }
}
void Cell::ClearVBOs()
{
    for( map<int16_t, VBOStruct >::iterator it = VBOs.begin(); it != VBOs.end(); ++it)
    {
        VBOStruct tmp = it->second;
        if(tmp.hasNormal)
            RENDERER->glDeleteBuffers(1, &tmp.normal);
        if(it->second.hasTop)
            RENDERER->glDeleteBuffers(1, &tmp.top);
    }
    VBOs.clear();
}
void Cell::DrawCellCage()
{
    Vector3 AdjustedPoint = Position;

    AdjustedPoint.x -= (CELLEDGESIZE / 2) - 0.5;
    AdjustedPoint.y -= (CELLEDGESIZE / 2) - 0.5;

    RENDERER->DrawCage(AdjustedPoint, CELLEDGESIZE, CELLEDGESIZE, 1.0, true,1,1,1);
}
