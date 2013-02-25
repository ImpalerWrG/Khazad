#ifndef CELL__HEADER
#define CELL__HEADER

#include <stdafx.h>

#include <Map.h>
#include <Renderer.h>

#include <OgreVector3.h>

#include <bitset>
#include <boost/array.hpp>

class Building;
class Tree;
class Actor;



class Cell
{

public:

    Cell();
    ~Cell();

    bool InitializeCell(Map* ParentMap);

    void setCellPosition(CellCoordinates Coordinates);

    void LoadCellData(Geology* MapGeology);

    void DrawCellCage();


    //----------------CUBE ACESS--------------//

    void setCubeShape(CubeCoordinates Coordinates, CubeShape NewShape);
    CubeShape getCubeShape(CubeCoordinates Coordinates) const                  { return CubeShapeTypes[Coordinates]; }

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

    inline bool isCubeZone(CubeCoordinates Coordinates)                          { return Zone.test(Coordinates); }
    inline void setCubeZone(CubeCoordinates Coordinates, bool NewValue)          { Zone.set(Coordinates, NewValue); }


    //----------------FACE ACESS---------------//

    Face* getFace(FaceCoordinates TargetCoordinates) const      { return Faces.find(TargetCoordinates.FaceKey())->second; }
    bool hasFace(FaceCoordinates TargetCoordinates) const       { return Faces.find(TargetCoordinates.FaceKey()) != Faces.end(); }

    int16_t getFaceMaterialType(FaceCoordinates TargetCoordinates) const;
    bool setFaceMaterialType(FaceCoordinates TargetCoordinates, int16_t MaterialTypeID);

    int16_t getFaceSurfaceType(FaceCoordinates TargetCoordinates) const;
    bool setFaceSurfaceType(FaceCoordinates TargetCoordinates, int16_t SurfaceTypeID);

    FaceShape getFaceShape(FaceCoordinates TargetCoordinates) const;
    bool setFaceShape(FaceCoordinates TargetCoordinates, FaceShape NewShape);

    bool removeFace(FaceCoordinates TargetCoordinates);
    Face* addFace(FaceCoordinates TargetCoordinates);


    //-----------GEOMETRY COMPRESSION----------//

    void BuildFaceData();
    void BuildStaticGeometry();

    void DestroyAllAttachedEntities(Ogre::SceneNode* TargetNode);

    inline bool getNeedsReBuild()                       { return NeedsReBuild; }
    inline void setNeedsReBuild(bool NewValue)          { NeedsReBuild = NewValue;  ParentMap->setNeedsReBuild(true); }

    CellCoordinates getCellCoordinates()                { return thisCellCoordinates; }
    Ogre::Vector3 getPosition()                         { return CellSceneNode->getPosition(); }

    Ogre::StaticGeometry* getCellGeometry()             { return CellGeometry; }
    inline void setVisible(bool NewVisibility)          { CellSceneNode->setVisible(NewVisibility); Visible = NewVisibility; CellGeometry->setVisible(NewVisibility); }


    //-------------LOCAL OBJECTS--------------//

    void addActor(Actor* NewActor);
    void removeActor(Actor* NewActor);

    void addBuilding(Building* NewBuilding)             { buildings.push_back(NewBuilding); }
    void addTree(Tree* NewTree)                         { trees.push_back(NewTree); }

protected:

    bool NeedsReBuild;
    bool Visible;

    // Larger DataValues specific to each Cube
    boost::array< CubeShape, CUBESPERCELL > CubeShapeTypes;
    boost::array< int16_t, CUBESPERCELL > CubeMaterialTypes;

    // Bit values for each Cube
    std::bitset< CUBESPERCELL > Hidden;
    std::bitset< CUBESPERCELL > SubTerranean;
    std::bitset< CUBESPERCELL > SkyView;
    std::bitset< CUBESPERCELL > SunLit;
    std::bitset< CUBESPERCELL > Zone;

    // Keeps all Faces between and inside Cubes
    std::map<uint16_t, Face*> Faces;

    // Game Objects located in the Cell for easy reference
    std::vector <Building*> buildings;
    std::vector <Tree*> trees;
    std::vector <Actor*> LocalActors;

    // Exact spacial Coordinates of the center of the cell, used for rendering
    Ogre::SceneNode* CellSceneNode;
    Ogre::StaticGeometry* CellGeometry;

    // The global position of this cell relative to other cells
    CellCoordinates thisCellCoordinates;

    Map* ParentMap;

};

#endif // CELL__HEADER
