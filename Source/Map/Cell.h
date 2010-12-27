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

    bool Update();

    void DrawCellCage();

    void BuildFaceData();
    void BuildStaticGeometry();

    void DestroyAllAttachedEntities(Ogre::SceneNode* TargetNode);

    Ogre::Vector3 getCubePosition(CubeCoordinates Coordinates);
    MapCoordinates TranslateCubeToMap(CubeCoordinates Coordinates);

    void setCubeShape(CubeCoordinates Coordinates, TileShape NewShape);
    inline TileShape getCubeShape(CubeCoordinates Coordinates) const             { return CubeShapeTypes[Coordinates.Index]; }

    void setCubeMaterial(CubeCoordinates Coordinates, int16_t MaterialID)      { CubeMaterialTypes[Coordinates.Index] = MaterialID; }
    inline int16_t getCubeMaterial(CubeCoordinates Coordinates) const          { return CubeMaterialTypes[Coordinates.Index]; }

    bool isCubeSloped(CubeCoordinates Coordinates);
    void addSlope(CubeCoordinates Coordinates, TileShape NewShape);

    inline bool isCubeHidden(CubeCoordinates Coordinates)                         { return Hidden.test(Coordinates.Index); }
    inline void setCubeHidden(CubeCoordinates Coordinates, bool NewValue)         { Hidden.set(Coordinates.Index, NewValue); }

    inline bool isCubeSubTerranean(CubeCoordinates Coordinates)                   { return SubTerranean.test(Coordinates.Index); }
    inline void setCubeSubTerranean(CubeCoordinates Coordinates, bool NewValue)   { SubTerranean.set(Coordinates.Index, NewValue); }

    inline bool isCubeSkyView(CubeCoordinates Coordinates)                        { return SkyView.test(Coordinates.Index); }
    inline void setCubeSkyView(CubeCoordinates Coordinates, bool NewValue)        { SkyView.set(Coordinates.Index, NewValue); }

    inline bool isCubeSunLit(CubeCoordinates Coordinates)                         { return SunLit.test(Coordinates.Index); }
    inline void setCubeSunLit(CubeCoordinates Coordinates, bool NewValue)         { SunLit.set(Coordinates.Index, NewValue); }

    inline bool isCubeDrawn(CubeCoordinates Coordinates)                          { return Drawn.test(Coordinates.Index); }
    inline void setCubeDrawn(CubeCoordinates Coordinates, bool NewValue)          { Drawn.set(Coordinates.Index, NewValue); }


    void setLiquid(CubeCoordinates Coordinates, bool liquidtype, uint8_t NewValue);
    inline uint8_t getLiquidLevel(CubeCoordinates Coordinates)                             { return LiquidLevel[Coordinates.Index]; }
    inline bool getLiquidType(CubeCoordinates Coordinates)                                 { return LiquidType.test(Coordinates.Index); }



    Face* getFace(FaceCoordinates TargetCoordinates)      { return Faces.find(TargetCoordinates.FaceKey())->second; }
    bool hasFace(FaceCoordinates TargetCoordinates)       { return Faces.find(TargetCoordinates.FaceKey()) != Faces.end(); }

    int16_t getFaceMaterialType(FaceCoordinates TargetCoordinates);
    bool setFaceMaterialType(FaceCoordinates TargetCoordinates, int16_t MaterialTypeID);

    int16_t getFaceSurfaceType(FaceCoordinates TargetCoordinates);
    bool setFaceSurfaceType(FaceCoordinates TargetCoordinates, int16_t SurfaceTypeID);

    bool removeFace(FaceCoordinates TargetCoordinates);
    Face* addFace(FaceCoordinates TargetCoordinates);


    void Dig(CubeCoordinates Coordinates);


    inline bool isActive()                              { return Active; }
    inline void setActive(bool NewValue)                { Active = NewValue; }

    inline bool isLiquidActive()                        { return ActiveLiquid; }
    inline void setLiquidActive(bool NewValue)          { ActiveLiquid = NewValue; }

    inline bool getNeedsReBuild()                       { return NeedsReBuild; }
    inline void setNeedsReBuild(bool NewValue)          { NeedsReBuild = NewValue;  ParentMap->setNeedsReBuild(true); }

    void addBuilding(Building* NewBuilding)             { buildings.push_back(NewBuilding); }
    void addTree(Tree* NewTree)                         { trees.push_back(NewTree); }

    CellCoordinates getCellCoordinates()                { return thisCellCoordinates; }
    Ogre::Vector3 getPosition()                         { return CellSceneNode->getPosition(); }


    int addActor(Actor* NewActor);
    void removeActor(int Index);

protected:

    bool NeedsReBuild;

    bool Active;
    bool ActiveLiquid;
    bool Initialized;


    uint16_t TriangleCount;

    // Data specific to each Cube
    TileShape CubeShapeTypes[CUBESPERCELL];
    int16_t CubeMaterialTypes[CUBESPERCELL];

    // Bit values for each Cube
    bitset<(CUBESPERCELL)> Hidden;
    bitset<(CUBESPERCELL)> SubTerranean;
    bitset<(CUBESPERCELL)> SkyView;
    bitset<(CUBESPERCELL)> SunLit;
    bitset<(CUBESPERCELL)> Drawn;

    // Liquid Data for each Cube
    bitset<(CUBESPERCELL)> LiquidType; // Allow more liquid types?
    uint8_t LiquidLevel[CUBESPERCELL];

    // Keeps all Faces between Cubes, shares three sides with other Cubes to avoid doubling
    std::map<uint16_t, Face*> Faces;

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
