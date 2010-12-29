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

    Ogre::Vector3 getCubePosition(CubeCoordinates Coordinates) const;

    void setCubeShape(CubeCoordinates Coordinates, TileShape NewShape);
    inline TileShape getCubeShape(CubeCoordinates Coordinates) const             { return CubeShapeTypes[Coordinates]; }

    void setCubeMaterial(CubeCoordinates Coordinates, int16_t MaterialID)      { CubeMaterialTypes[Coordinates] = MaterialID; }
    inline int16_t getCubeMaterial(CubeCoordinates Coordinates) const          { return CubeMaterialTypes[Coordinates]; }

    bool isCubeSloped(CubeCoordinates Coordinates) const;

    inline bool isCubeHidden(CubeCoordinates Coordinates)                         { return Hidden.test(Coordinates); }
    inline void setCubeHidden(CubeCoordinates Coordinates, bool NewValue)         { Hidden.set(Coordinates, NewValue); }

    inline bool isCubeSubTerranean(CubeCoordinates Coordinates)                   { return SubTerranean.test(Coordinates); }
    inline void setCubeSubTerranean(CubeCoordinates Coordinates, bool NewValue)   { SubTerranean.set(Coordinates, NewValue); }

    inline bool isCubeSkyView(CubeCoordinates Coordinates)                        { return SkyView.test(Coordinates); }
    inline void setCubeSkyView(CubeCoordinates Coordinates, bool NewValue)        { SkyView.set(Coordinates, NewValue); }

    inline bool isCubeSunLit(CubeCoordinates Coordinates)                         { return SunLit.test(Coordinates); }
    inline void setCubeSunLit(CubeCoordinates Coordinates, bool NewValue)         { SunLit.set(Coordinates, NewValue); }

    inline bool isCubeDrawn(CubeCoordinates Coordinates)                          { return Drawn.test(Coordinates); }
    inline void setCubeDrawn(CubeCoordinates Coordinates, bool NewValue)          { Drawn.set(Coordinates, NewValue); }





    Face* getFace(FaceCoordinates TargetCoordinates) const      { return Faces.find(TargetCoordinates.FaceKey())->second; }
    bool hasFace(FaceCoordinates TargetCoordinates) const       { return Faces.find(TargetCoordinates.FaceKey()) != Faces.end(); }

    int16_t getFaceMaterialType(FaceCoordinates TargetCoordinates) const;
    bool setFaceMaterialType(FaceCoordinates TargetCoordinates, int16_t MaterialTypeID);

    int16_t getFaceSurfaceType(FaceCoordinates TargetCoordinates) const;
    bool setFaceSurfaceType(FaceCoordinates TargetCoordinates, int16_t SurfaceTypeID);

    TileShape getFaceShape(FaceCoordinates TargetCoordinates) const;
    bool setFaceShape(FaceCoordinates TargetCoordinates, TileShape NewShape);

    bool removeFace(FaceCoordinates TargetCoordinates);
    Face* addFace(FaceCoordinates TargetCoordinates);


    void Dig(CubeCoordinates Coordinates);


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

    // Keeps all Faces between and inside Cubes
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
