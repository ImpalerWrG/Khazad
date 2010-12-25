#ifndef CELL__HEADER
#define CELL__HEADER

#include <stdafx.h>

#include <Map.h>
#include <Renderer.h>

#include <OgreVector3.h>

#include <bitset>

class Building;
class Tree;
class Actor;



class Cell
{

public:

    Cell();
    ~Cell();

    bool InitializeCell(Map* ParentMap);
    bool isInitialized()    { return Initialized; }

    void setCellPosition(CellCoordinates Coordinates);


    //void Render(CameraOrientation CurrentOrientation);

    // update VBOs
    //void UpdateRenderLists(WallDisplayMode Mode);
    void LoadCellData(Geology* MapGeology);

    // sync with DF if applicable
    bool Update();

    void DrawCellCage();

    void BuildFaceData();
    void BuildStaticGeometry();

    void DestroyAllAttachedEntities(Ogre::SceneNode* TargetNode);

    Ogre::Vector3 getCubePosition(CubeCoordinates Coordinates);
    uint16_t TranslateCubeToIndex(CubeCoordinates Coordinates);
    MapCoordinates TranslateCubeToMap(CubeCoordinates Coordinates);

    void setCubeShape(CubeCoordinates Coordinates, TileShape NewShape);
    inline TileShape getCubeShape(CubeCoordinates Coordinates) const             { return CubeShapeTypes[Coordinates.X][Coordinates.Y]; }

    void setCubeMaterial(CubeCoordinates Coordinates, int16_t MaterialID)      { CubeMaterialTypes[Coordinates.X][Coordinates.Y] = MaterialID; }
    inline int16_t getCubeMaterial(CubeCoordinates Coordinates) const          { return CubeMaterialTypes[Coordinates.X][Coordinates.Y]; }

    void setCubeSurface(CubeCoordinates Coordinates, int16_t SurfaceID)        { CubeSurfaceTypes[Coordinates.X][Coordinates.Y] = SurfaceID; }
    inline int16_t getCubeSurface(CubeCoordinates Coordinates) const           { return CubeSurfaceTypes[Coordinates.X][Coordinates.Y]; }

    bool isCubeSloped(CubeCoordinates Coordinates);
    void addSlope(CubeCoordinates Coordinates, TileShape NewShape);

    inline bool isCubeHidden(CubeCoordinates Coordinates)                         { return Hidden.test((Coordinates.X * CELLEDGESIZE) + Coordinates.Y); }
    inline void setCubeHidden(CubeCoordinates Coordinates, bool NewValue)         { Hidden.set(((Coordinates.X * CELLEDGESIZE) + Coordinates.Y), NewValue); }

    inline bool isCubeSubTerranean(CubeCoordinates Coordinates)                   { return SubTerranean.test((Coordinates.X * CELLEDGESIZE) + Coordinates.Y); }
    inline void setCubeSubTerranean(CubeCoordinates Coordinates, bool NewValue)   { SubTerranean.set(((Coordinates.X * CELLEDGESIZE) + Coordinates.Y), NewValue); }

    inline bool isCubeSkyView(CubeCoordinates Coordinates)                        { return SkyView.test((Coordinates.X * CELLEDGESIZE) + Coordinates.Y); }
    inline void setCubeSkyView(CubeCoordinates Coordinates, bool NewValue)        { SkyView.set(((Coordinates.X * CELLEDGESIZE) + Coordinates.Y), NewValue); }

    inline bool isCubeSunLit(CubeCoordinates Coordinates)                         { return SunLit.test((Coordinates.X * CELLEDGESIZE) + Coordinates.Y); }
    inline void setCubeSunLit(CubeCoordinates Coordinates, bool NewValue)         { SunLit.set(((Coordinates.X * CELLEDGESIZE) + Coordinates.Y), NewValue); }

    inline bool isCubeSolid(CubeCoordinates Coordinates)                          { return Solid.test((Coordinates.X * CELLEDGESIZE) + Coordinates.Y); }
    inline void setCubeSolid(CubeCoordinates Coordinates, bool NewValue)          { Solid.set(((Coordinates.X * CELLEDGESIZE) + Coordinates.Y), NewValue); }

    inline bool isCubeDrawn(CubeCoordinates Coordinates)                          { return Drawn.test((Coordinates.X * CELLEDGESIZE) + Coordinates.Y); }
    inline void setCubeDrawn(CubeCoordinates Coordinates, bool NewValue)          { Drawn.set(((Coordinates.X * CELLEDGESIZE) + Coordinates.Y), NewValue); }


    void setLiquid(CubeCoordinates Coordinates, bool liquidtype, uint8_t NewValue);
    inline uint8_t getLiquidLevel(CubeCoordinates Coordinates)                           { return LiquidLevel[Coordinates.X][Coordinates.Y]; }
    inline bool getLiquidType(CubeCoordinates Coordinates)                             { return LiquidType.test((Coordinates.X * CELLEDGESIZE) + Coordinates.Y); }

    inline uint32_t GenerateFaceKey(CubeCoordinates Coordinates, Direction DirectionType);

    Face* getFace(CubeCoordinates Coordinates, Direction DirectionType);
    bool hasFace(CubeCoordinates Coordinates, Direction DirectionType);

    int16_t getFaceMaterialType(CubeCoordinates Coordinates, Direction DirectionType);
    bool setFaceMaterialType(CubeCoordinates Coordinates, Direction DirectionType, int16_t MaterialTypeID);

    int16_t getFaceSurfaceType(CubeCoordinates Coordinates, Direction DirectionType);
    bool setFaceSurfaceType(CubeCoordinates Coordinates, Direction DirectionType, int16_t SurfaceTypeID);
    bool setBothFaceSurfaces(CubeCoordinates Coordinates, Direction DirectionType, int16_t SurfaceTypeID);

    bool removeFace(CubeCoordinates Coordinates, Direction DirectionType);
    Face* addFace(CubeCoordinates Coordinates, Direction DirectionType);


    void DigSlope(CubeCoordinates Coordinates);
    void Dig(CubeCoordinates Coordinates);


    inline bool isActive()                              { return Active; }
    inline void setActive(bool NewValue)                { Active = NewValue; }

    inline bool isLiquidActive()                        { return ActiveLiquid; }
    inline void setLiquidActive(bool NewValue)          { ActiveLiquid = NewValue; }

    inline bool getNeedsRedraw()                        { return NeedsRedraw; }
    inline void setNeedsRedraw(bool NewValue)           { NeedsRedraw = NewValue; }

    void addBuilding(Building* NewBuilding)             { buildings.push_back(NewBuilding); }
    void addTree(Tree* NewTree)                         { trees.push_back(NewTree); }

    CellCoordinates getCellCoordinates()                { return thisCellCoordinates; }
    Ogre::Vector3 getPosition()                         { return CellSceneNode->getPosition(); }

    //std::map<int16_t, vector <vertex>* > Geometry;

    int addActor(Actor* NewActor);
    void removeActor(int Index);

protected:

    bool NeedsRedraw; //TODO maintain a vector of cells that need redraw instead
    bool Active;
    bool ActiveLiquid;
    bool Initialized;

    // VBOs by texture and target
    //std::map<int16_t, ROstore > ROs;

    uint16_t TriangleCount;

    // Data specific to each Cube
    TileShape CubeShapeTypes[CELLEDGESIZE][CELLEDGESIZE];
    int16_t CubeMaterialTypes[CELLEDGESIZE][CELLEDGESIZE];
    int16_t CubeSurfaceTypes[CELLEDGESIZE][CELLEDGESIZE];

    // Bit values for each Cube
    bitset<(CELLEDGESIZE * CELLEDGESIZE)> Hidden;
    bitset<(CELLEDGESIZE * CELLEDGESIZE)> SubTerranean;
    bitset<(CELLEDGESIZE * CELLEDGESIZE)> SkyView;
    bitset<(CELLEDGESIZE * CELLEDGESIZE)> SunLit;
    bitset<(CELLEDGESIZE * CELLEDGESIZE)> Solid;

    bitset<(CELLEDGESIZE * CELLEDGESIZE)> Drawn;

    // Liquid Data for each Cube
    bitset<(CELLEDGESIZE * CELLEDGESIZE)> LiquidType; // Allow more liquid types?
    uint8_t LiquidLevel[CELLEDGESIZE][CELLEDGESIZE];

    // Keeps all Faces between Cubes, shares three sides with other Cubes to avoid doubling
    std::map<uint32_t, Face*> Faces;

    std::vector <Building*> buildings;
    std::vector <Tree*> trees;

    std::vector <Actor*> LocalActors;

    // Exact spacial Coordinates of the center of the cell, used for rendering
    Ogre::SceneNode* CellSceneNode;


    // The global position of this cell relative to other cells
    CellCoordinates thisCellCoordinates;

    Map* ParentMap;
};

#endif // CELL__HEADER
