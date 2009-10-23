#ifndef CELL__HEADER
#define CELL__HEADER

#include <stdafx.h>
#include <Renderer.h>
#include <Vector3.h>
#include <bitset>

#define CELLEDGESIZE 16
#define HALFCUBE 0.5

class Building;
class Tree;

struct ROstore
{
    inline ROstore( RenderObject* n, RenderObject* t ): normal(n){};
    inline ROstore(){};

    RenderObject* normal;
};

struct Face
{
    Sint16 PositiveAxisSurfaceTypeID;
    Sint16 NegativeAxisSurfaceTypeID;

    Sint16 MaterialTypeID;
};

class Cell
{

public:

    Cell();
    ~Cell();
    bool Init();
    void setPosition(Sint32 X, Sint32 Y, Sint32 Z);


    void Render(CameraOrientation CurrentOrientation);
    void ClearROs();

    // update VBOs
    void UpdateRenderLists();

    // sync with DF if applicable
    bool Update();

    void DrawCellCage();

    bool Draw(Uint8 CubeX, Uint8 CubeY);
    bool DrawFaces(Uint8 CubeX, Uint8 CubeY);
    bool DrawSlope(Uint8 CubeX, Uint8 CubeY);
    void BuildFaceData();

    Cell* getAdjacentCell(Facet FacetType);

    Vector3 getCubePosition(Uint8 CubeX, Uint8 CubeY);

    void setCubeShape(Uint8 CubeX, Uint8 CubeY, Sint16 TileShape);
    inline Sint16 getCubeShape(Uint8 CubeX, Uint8 CubeY)                   { return CubeShapeTypes[CubeX][CubeY]; }

    void setCubeMaterial(Uint8 CubeX, Uint8 CubeY, Sint16 MaterialID)      { CubeMaterialTypes[CubeX][CubeY] = MaterialID; }
    inline Sint16 getCubeMaterial(Uint8 CubeX, Uint8 CubeY)                { return CubeMaterialTypes[CubeX][CubeY]; }

    void setCubeSurface(Uint8 CubeX, Uint8 CubeY, Sint16 SurfaceID)        { CubeSurfaceTypes[CubeX][CubeY] = SurfaceID; }
    inline Sint16 getCubeSurface(Uint8 CubeX, Uint8 CubeY)                 { return CubeSurfaceTypes[CubeX][CubeY]; }

    bool isCubeSloped(Uint8 CubeX, Uint8 CubeY);


    inline bool isCubeHidden(Uint8 CubeX, Uint8 CubeY)                         { return Hidden.test((CubeX * CELLEDGESIZE) + CubeY); }
    inline void setCubeHidden(Uint8 CubeX, Uint8 CubeY, bool NewValue)         { Hidden.set(((CubeX * CELLEDGESIZE) + CubeY), NewValue); }

    inline bool isCubeSubTerranean(Uint8 CubeX, Uint8 CubeY)                   { return SubTerranean.test((CubeX * CELLEDGESIZE) + CubeY); }
    inline void setCubeSubTerranean(Uint8 CubeX, Uint8 CubeY, bool NewValue)   { SubTerranean.set(((CubeX * CELLEDGESIZE) + CubeY), NewValue); }

    inline bool isCubeSkyView(Uint8 CubeX, Uint8 CubeY)                        { return SkyView.test((CubeX * CELLEDGESIZE) + CubeY); }
    inline void setCubeSkyView(Uint8 CubeX, Uint8 CubeY, bool NewValue)        { SkyView.set(((CubeX * CELLEDGESIZE) + CubeY), NewValue); }

    inline bool isCubeSunLit(Uint8 CubeX, Uint8 CubeY)                         { return SunLit.test((CubeX * CELLEDGESIZE) + CubeY); }
    inline void setCubeSunLit(Uint8 CubeX, Uint8 CubeY, bool NewValue)         { SunLit.set(((CubeX * CELLEDGESIZE) + CubeY), NewValue); }

    inline bool isCubeSolid(Uint8 CubeX, Uint8 CubeY)                          { return Solid.test((CubeX * CELLEDGESIZE) + CubeY); }
    inline void setCubeSolid(Uint8 CubeX, Uint8 CubeY, bool NewValue)          { Solid.set(((CubeX * CELLEDGESIZE) + CubeY), NewValue); }

    void setLiquid(Uint8 CubeX, Uint8 CubeY, bool liquidtype, Uint8 NewValue);


    Face* getFace(Uint8 CubeX, Uint8 CubeY, Facet FacetType);
    bool hasFace(Uint8 CubeX, Uint8 CubeY, Facet FaceType);

    Sint16 getFaceMaterialType(Uint8 CubeX, Uint8 CubeY, Facet FacetType);
    bool setFaceMaterialType(Uint8 CubeX, Uint8 CubeY, Facet FacetType, Sint16 MaterialTypeID);

    Sint16 getFaceSurfaceType(Uint8 CubeX, Uint8 CubeY, Facet FacetType);
    bool setFaceSurfaceType(Uint8 CubeX, Uint8 CubeY, Facet FacetType, Sint16 SurfaceTypeID);

    bool removeFace(Uint8 CubeX, Uint8 CubeY, Facet FacetType);
    Face* addFace(Uint8 CubeX, Uint8 CubeY, Facet FacetType);


    inline bool isActive()                              { return Active; }
    inline void setActive(bool NewValue)                { Active = NewValue; }

    inline bool isLiquidActive()                        { return ActiveLiquid; }
    inline void setLiquidActive(bool NewValue)          { ActiveLiquid = NewValue; }

    inline bool getNeedsRedraw()                        { return NeedsRedraw; }
    inline void setNeedsRedraw(bool NewValue)           { NeedsRedraw = NewValue; }

    void addBuilding(Building* NewBuilding)             { buildings.push_back(NewBuilding); }
    void addTree(Tree* NewTree)                         { trees.push_back(NewTree); }


    Vector3 getPosition()   { return CellPosition; }

    map<int16_t, vector <vertex>* > Geometry;

protected:

    bool NeedsRedraw; //TODO maintain a vector of cells that need redraw instead
    bool Active;
    bool ActiveLiquid;
    bool Initialized;

    // VBOs by texture and target
    map<int16_t, ROstore > ROs;

    Uint16 TriangleCount;

    // Data specific to each Cube
    Sint16 CubeShapeTypes[CELLEDGESIZE][CELLEDGESIZE];
    Sint16 CubeMaterialTypes[CELLEDGESIZE][CELLEDGESIZE];
    Sint16 CubeSurfaceTypes[CELLEDGESIZE][CELLEDGESIZE];

    // Bit values for each Cube
    bitset<(CELLEDGESIZE * CELLEDGESIZE)> Hidden;
    bitset<(CELLEDGESIZE * CELLEDGESIZE)> SubTerranean;
    bitset<(CELLEDGESIZE * CELLEDGESIZE)> SkyView;
    bitset<(CELLEDGESIZE * CELLEDGESIZE)> SunLit;
    bitset<(CELLEDGESIZE * CELLEDGESIZE)> Solid;

    // Liquid Data for each Cube
    bitset<(CELLEDGESIZE * CELLEDGESIZE)> LiquidType; // Allow more liquid types?
    Uint8 LiquidLevel[CELLEDGESIZE][CELLEDGESIZE];

    // Keeps all Faces between Cubes, shares three sides with other Cells to avoid doubling
    map<Uint32, Face*> NorthSouthFaces;
    map<Uint32, Face*> EastWestFaces;
    map<Uint32, Face*> BottomFaces;

    vector <Building*> buildings;
    vector <Tree*> trees;

    // Exact spacial cordinates of the center of the cell, used for frustrum culling
    Vector3 CellPosition;

    // The global position of this cell relative to other cells
    Sint16 CellX;
    Sint16 CellY;
    Sint16 CellZ;
};

#endif // CELL__HEADER
