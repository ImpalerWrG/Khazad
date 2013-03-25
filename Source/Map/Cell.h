#ifndef CELL__HEADER
#define CELL__HEADER

#include <stdafx.h>

#include <Face.h>

#include <bitset>
#include <boost/array.hpp>
#include <boost/unordered_map.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

class Building;
class Tree;
class Actor;
class Face;
class TerrainRendering;


class Cell
{

public:

    Cell();
    ~Cell();


    //----------------CUBE ACESS--------------//

    void setCubeShape(CubeCoordinates Coordinates, CubeShape NewShape);
    CubeShape getCubeShape(CubeCoordinates Coordinates) const                  { return CubeShapeTypes[Coordinates]; }

    void setCubeMaterial(CubeCoordinates Coordinates, int16_t MaterialID)      { CubeMaterialTypes[Coordinates] = MaterialID; }
    inline int16_t getCubeMaterial(CubeCoordinates Coordinates) const          { return CubeMaterialTypes[Coordinates]; }

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

	std::map<uint16_t, Face*> getFaceMap()						{ return Faces; }

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
    void RegisterWithRendering();
    void setNeedsReRendering();

    CellCoordinates getCellCoordinates()                { return thisCellCoordinates; }
    void setCellPosition(CellCoordinates Coordinates);


    //-------------LOCAL OBJECTS--------------//

    void addActor(Actor* NewActor);
    void removeActor(Actor* NewActor);

    void addBuilding(Building* NewBuilding)             { buildings.push_back(NewBuilding); }
    void addTree(Tree* NewTree)                         { trees.push_back(NewTree); }


    void Save(boost::filesystem::basic_ofstream<char>& Stream) const;
    void Load(boost::filesystem::basic_ifstream<char>& Stream);

protected:

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

    // The global position of this cell relative to other cells
    CellCoordinates thisCellCoordinates;

	// Class that handles all rendering implementation
	TerrainRendering* Render;
};

#endif // CELL__HEADER
