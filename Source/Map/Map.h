/* Copyright 2010 Kenneth Ferland

This file is part of Khazad.

Khazad is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Khazad is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Khazad.  If not, see <http://www.gnu.org/licenses/> */


#ifndef MAP__HEADER
#define MAP__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <Renderer.h>
#include <TileShapes.h>
#include <Cell.h>

#include <boost/unordered_map.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

class Cell;
class Geology;
class Face;
class Zone;
class VolumeSelection;

class Map
{

public:

    Map();

	~Map();
	bool Init();

    bool isInitialized()        { return Initialized; }
    bool isMapLoaded()          { return MapLoaded; }


    // --------------------CELL MANAGMENT--------------------

	Cell* getCell(CellCoordinates) const;
	Cell* getCubeOwner(MapCoordinates) const;

	boost::unordered_map<uint64_t, Cell*>* getCellMap()		{ return &Cells; };

    bool insertCell(Cell* NewCell);

    bool isCubeInited(MapCoordinates) const;

    void setCubeShape(MapCoordinates Coordinates, CubeShape NewShape);
    CubeShape getCubeShape(MapCoordinates Coordinates) const;

    void setCubeMaterial(MapCoordinates Coordinates, int16_t MaterialID);
    inline int16_t getCubeMaterial(MapCoordinates Coordinates) const;

    // -----------------FACE CALLS DIRECTED TO CELLS------------

    std::pair< MapCoordinates, Direction > FaceCoordinateConvertion(MapCoordinates TargetMapCoordinates, Direction DirectionType) const;


    Face* getFace(MapCoordinates Coordinates, Direction DirectionType) const;
    bool hasFace(MapCoordinates Coordinates, Direction DirectionType) const;

    bool removeFace(MapCoordinates Coordinates, Direction DirectionType);
    Face* addFace(MapCoordinates Coordinates, Direction DirectionType);

    void setFaceMaterial(MapCoordinates Coordinates, Direction DirectionType, int16_t MaterialID);
    inline int16_t getFaceMaterial(MapCoordinates Coordinates, Direction DirectionType) const;

    void setFaceSurfaceType(MapCoordinates Coordinates, Direction DirectionType, int16_t SurfaceID);
    inline int16_t getFaceSurfaceType(MapCoordinates Coordinates, Direction DirectionType) const;


    bool isCubeHidden(MapCoordinates Coordinates) const;
    void setCubeHidden(MapCoordinates Coordinates, bool NewValue);

    bool isCubeSubTerranean(MapCoordinates Coordinates) const;
    void setCubeSubTerranean(MapCoordinates Coordinates, bool NewValue);

    bool isCubeSkyView(MapCoordinates Coordinates) const;
    void setCubeSkyView(MapCoordinates Coordinates, bool NewValue);

    bool isCubeSunLit(MapCoordinates Coordinates) const;
    void setCubeSunLit(MapCoordinates Coordinates, bool NewValue);

	void RegisterWithRendering();
    void ReleaseMap();

    void UpdateCubeShape(MapCoordinates Coordinates, CubeShape NewShape);
    void UpdateFace(MapCoordinates TargetCoordinates, Direction DirectionType);

    MapCoordinates getRayIntersection(Ogre::Ray MouseRay, uint16_t Top, uint16_t Bottom);

    int32_t getHighest()                            { return HighestCell; }
    int32_t getLowest()                             { return LowestCell; }

    Zone* getZone() const;
    void addZone(Zone* NewZone);
    Zone* createZone(std::vector<VolumeSelection*> Volumes);
    Zone* getZoneAt(MapCoordinates TestCoordinates);

    void Save(boost::filesystem::basic_ofstream<char>& Stream) const;
    void Load(boost::filesystem::basic_ifstream<char>& Stream);

protected:

    bool Initialized;
    bool MapLoaded;

    boost::unordered_map<uint64_t, Cell*> Cells;

    int32_t HighestCell;
    int32_t LowestCell;

    std::vector<Zone*> Zones;

    MapCoordinates LastRayTestResult;  // Used to smoothout Map Picking
};

#endif // MAP__HEADER
