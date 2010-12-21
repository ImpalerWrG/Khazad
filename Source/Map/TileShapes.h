#ifndef TILESHAPE__HEADER
#define TILESHAPE__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <Ogre.h>

#define HEIGHT_FRACTIONS 1

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

inline TileShape getTileShapeFromCornerHeight(uint8_t SWCorner, uint8_t SECorner, uint8_t NWCorner, uint8_t NECorner, bool SplittingLine)
{
    uint16_t Shape = 0;

    if (SplittingLine)
    {
        if (NWCorner == 0 || SECorner == 0)
        {
            return TILESHAPE_EMPTY;
        }
    }
    else
    {
        if (SWCorner == 0 || NECorner == 0)
        {
            return TILESHAPE_EMPTY;
        }
    }
    //if (((NECorner <= SECorner) && (NECorner <= NWCorner))  &&  ((SWCorner >= SECorner) && (SWCorner >= NWCorner)))  // Break quad along a NW-SE line

    uint16_t Multiplier = 1;

    if (SplittingLine)
    {
        Shape += 1;  // Bit 0 holds the triangle split value, 1 is NW-SE splitting, 0 indicates the alternative SW-NE split
    }
    Multiplier *= 2;

    Shape += SWCorner * Multiplier;
    Multiplier *= (HEIGHT_FRACTIONS + 2);

    Shape += SECorner * Multiplier;
    Multiplier *= (HEIGHT_FRACTIONS + 2);

    Shape += NWCorner * Multiplier;
    Multiplier *= (HEIGHT_FRACTIONS + 2);

    Shape += NECorner * Multiplier;

    return (TileShape) Shape;
}

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

inline FaceShape getFaceShapeFromCornerHeight(Direction DirectionType, uint8_t NegativeCorner, uint8_t PossitiveCorner)
{
    uint16_t Shape = 0;

    switch (DirectionType)
    {
        case DIRECTION_DOWN_SOUTH:
        case DIRECTION_DOWN_EAST:

            Shape = Shape | (NegativeCorner << 0);
            Shape = Shape | (PossitiveCorner << 2);
            Shape = Shape | ((DirectionType - 2) << 4);

            break;


        case DIRECTION_DOWN_NORTH:  // Reverse Possitive Negative Heights to ensure proper alignment durring drawing
        case DIRECTION_DOWN_WEST:

            Shape = Shape | (PossitiveCorner << 0);
            Shape = Shape | (NegativeCorner << 2);
            Shape = Shape | ((DirectionType - 2) << 4);

            break;
    }

    return (FaceShape) Shape;
};


void CreateAllEntities();

void CreateFlatTiles();

void CreateFaceTiles();

void CreateSlopedTiles();

#endif // TILESHAPE__HEADER
