#ifndef CELL__HEADER
#define CELL__HEADER
#include <stdafx.h>

#include <Actor.h>
//#include <GL/gl.h>
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

class Cell/*: public Actor*/
{
public:

    Cell();
    Cell(Sint32 X, Sint32 Y, Sint32 Z);
    ~Cell();
    bool Init();

    Cube* getCube(Uint8 x, Uint8 y);
    void setCube(Cube* NewCube, Uint8 x, Uint8 y);

    bool hasFace(Uint8 x, Uint8 y, Facet FaceType);

    void Render(bool drawtop);
    void ClearROs();

    // update VBOs
    void UpdateLists();

    // sync with DF if applicable
    bool Update();

    void DrawCellCage();

    inline bool isActive()                     { return Active; }
    void setActive(bool NewValue)       { Active = NewValue; }

    inline bool isLiquidActive()                     { return ActiveLiquid; }
    void setLiquidActive(bool NewValue)       { ActiveLiquid = NewValue; }

    inline bool isTopActive()                     { return ActiveTop; }
    void setTopActive(bool NewValue)       { ActiveTop = NewValue; }

    bool getNeedsRedraw()                  { return NeedsRedraw; }
    void setNeedsRedraw(bool NewValue)    { NeedsRedraw = NewValue; }

    void addBuilding(Building *);
    void addTree(Tree *);
    Vector3 getPosition(){return Position;};
protected:
    //TODO maintain a vector of cells that need redraw instead
    bool NeedsRedraw;

    // VBOs by texture and target
    map<int16_t, ROstore > ROs;

    Sint16 XOffset;
    Sint16 YOffset;
    Sint16 ZOffset;

    Uint16 TriangleCount;

    Cube* Cubes[CELLEDGESIZE][CELLEDGESIZE];

    vector <Building *> buildings;
    vector <Tree *> trees;
    bool Active;
    bool ActiveLiquid;
    bool ActiveTop;
    bool Initialized;
    Vector3 Position;
};

#endif // CELL__HEADER
