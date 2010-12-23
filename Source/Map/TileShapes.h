#ifndef TILESHAPE__HEADER
#define TILESHAPE__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <Ogre.h>

#define HEIGHT_FRACTIONS 3

enum TileShape
{
    TILESHAPE_EMPTY = 0,
    TILESHAPE_FLOOR,

    // implicit ramp shape combinations

    TILESHAPE_WALL = ((HEIGHT_FRACTIONS + 2) * (HEIGHT_FRACTIONS + 2) * (HEIGHT_FRACTIONS + 2) * (HEIGHT_FRACTIONS + 2) * 2) - 1,

    // Add other tile shapes here but increment NUM
    NUM_TILESHAPES,
    TILESHAPE_START = 1
};

inline TileShape &operator++ (TileShape &OldTileShape)      { return OldTileShape = TileShape(OldTileShape + 1); }
inline TileShape &operator-- (TileShape &OldTileShape)      { return OldTileShape = TileShape(OldTileShape - 1); }

enum FaceShape  // Describes vertical faces between cubes, 2 bits of Directedion, 2 bits for each face height
{
    FACESHAPE_EMPTY = 0,

    // implicit face shape combinations

    FACESHAPE_SOUTH = 15,

    FACESHAPE_NORTH = 32,

    FACESHAPE_WEST = 47,

    FACESHAPE_EAST = 63,

    // Add other tile shapes here but increment NUM
    NUM_FACESHAPES = 64,
    FACESHAPE_START = 0
};

inline FaceShape &operator++ (FaceShape &OldFaceShape)      { return OldFaceShape = FaceShape(OldFaceShape + 1); }
inline FaceShape &operator-- (FaceShape &OldFaceShape)      { return OldFaceShape = FaceShape(OldFaceShape - 1); }


void CreateAllEntities();

void CreateFlatTiles();

void CreateFaceTiles();

void CreateSlopedTiles();

TileShape getTileShapeFromCornerHeight(uint8_t SWCorner, uint8_t SECorner, uint8_t NWCorner, uint8_t NECorner, bool SplittingLine);

FaceShape getFaceShapeFromCornerHeight(Direction DirectionType, uint8_t NegativeCorner, uint8_t PossitiveCorner);

#endif // TILESHAPE__HEADER
