#ifndef CELL__HEADER
#define CELL__HEADER

#include <stdafx.h>
#include <Renderer.h>
#include <Vector3.h>

#define CELLEDGESIZE 16

class Cube;
class Building;
class Tree;

struct ROstore
{
    inline ROstore(  RenderObject * n, RenderObject * t ): normal(n),top(t){};
    inline ROstore(){};

    RenderObject * normal;
    RenderObject * top;
};

class Cell
{

public:

    Cell();
    Cell(Sint32 X, Sint32 Y, Sint32 Z);
    ~Cell();
    bool Init();

    Cube* getCube(Uint8 x, Uint8 y);
    Cube* GenerateCube(Uint8 CubeX, Uint8 CubeY);
    void setCube(Cube* NewCube, Uint8 CubeX, Uint8 CubeY);

    bool hasFace(Uint8 CubeX, Uint8 CubeY, Facet FaceType);

    void Render(CameraOrientation CurrentOrientation);
    void ClearROs();

    // update VBOs
    void UpdateLists();

    // sync with DF if applicable
    bool Update();

    void DrawCellCage();

    void setCubeShape(Uint8 CubeX, Uint8 CubeY, Sint16 TileShape);
    Sint16 getCubeShape(Uint8 CubeX, Uint8 CubeY)             { return CubeShapeTypes[CubeX][CubeY]; }

    void setCubeMaterial(Uint8 CubeX, Uint8 CubeY, Sint16 MaterialID);
    Sint16 getCubeMaterial(Uint8 CubeX, Uint8 CubeY)          { return CubeMaterialTypes[CubeX][CubeY]; }

    inline bool isActive()                      { return Active; }
    void setActive(bool NewValue)               { Active = NewValue; }

    inline bool isLiquidActive()                { return ActiveLiquid; }
    void setLiquidActive(bool NewValue)         { ActiveLiquid = NewValue; }

    inline bool isTopActive()                   { return ActiveTop; }
    void setTopActive(bool NewValue)            { ActiveTop = NewValue; }

    bool getNeedsRedraw()                       { return NeedsRedraw; }
    void setNeedsRedraw(bool NewValue)          { NeedsRedraw = NewValue; }

    void addBuilding(Building *);
    void addTree(Tree *);

    Vector3 getPosition(){return Position;};

    map<int16_t, vector <vertex>* > Geometry;

protected:

    bool NeedsRedraw; //TODO maintain a vector of cells that need redraw instead

    // VBOs by texture and target
    map<int16_t, ROstore > ROs;

    Sint16 XOffset;
    Sint16 YOffset;
    Sint16 ZOffset;

    Uint16 TriangleCount;

    Cube* Cubes[CELLEDGESIZE][CELLEDGESIZE];

    Sint16 CubeShapeTypes[CELLEDGESIZE][CELLEDGESIZE];
    Sint16 CubeMaterialTypes[CELLEDGESIZE][CELLEDGESIZE];

    vector <Building*> buildings;
    vector <Tree*> trees;

    bool Active;
    bool ActiveLiquid;
    bool ActiveTop;
    bool Initialized;

    Vector3 Position;
};

#endif // CELL__HEADER
