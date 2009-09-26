#ifndef CELL__HEADER
#define CELL__HEADER
#include <stdafx.h>

#include <Actor.h>
//#include <GL/gl.h>
#define CELLEDGESIZE 16

class Cube;
class Building;
class Tree;

struct VBOStruct
{
    bool hasNormal;
    GLuint normal;
    uint32_t countNormal;
    bool hasTop;
    GLuint top;
    uint32_t countTop;
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

    Uint16 getTriangleCount() { return TriangleCount; }

    void setTriangleCount(Uint16 Triangles) { TriangleCount = Triangles; }
    void addTriangleCount(Uint16 Triangles) { TriangleCount += Triangles; }

    // FIXME: temporary, inefficient
    // render VBOs
    void CallVBO(GLuint vbo, uint32_t count);
    void Render(bool drawtop);
    void ClearVBOs();

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

    //GLuint getDrawListID()                  { return DrawListID; }
    bool getNeedsRedraw()                  { return NeedsRedraw; }
    void setNeedsRedraw(bool NewValue)    { NeedsRedraw = NewValue; }

    void addBuilding(Building *);
    void addTree(Tree *);
    Vector3 getPosition(){return Position;};
protected:
    //TODO maintain a vector of cells that need redraw instead
    bool NeedsRedraw;

    // VBOs by texture and target
    map<int16_t, VBOStruct > VBOs;

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
private:
    // move to renderer
    GLuint BuildVBO (vertex * data, uint32_t size);
};

#endif // CELL__HEADER
