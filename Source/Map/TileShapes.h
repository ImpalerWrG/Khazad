#ifndef TILESHAPE__HEADER
#define TILESHAPE__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <Ogre.h>

#define BELOW_CUBE_HEIGHT 0
#define CUBE_BOTTOM_HEIGHT 1
#define HEIGHT_FRACTIONS 3
#define CUBE_TOP_HEIGHT 4
#define ABOVE_CUBE_HEIGHT 5

struct CubeShape
{
    CubeShape()
    {
        CubeShape(false);
    }

    CubeShape(uint8_t SWCornerHeight, uint8_t SECornerHeight, uint8_t NWCornerHeight, uint8_t NECornerHeight, uint8_t NewFlags)
    {
        Flags = NewFlags;

        SouthWestCorner = SWCornerHeight;
        SouthEastCorner = SECornerHeight;

        NorthWestCorner = NWCornerHeight;
        NorthEastCorner = NECornerHeight;
    }

    CubeShape(bool Solid)
    {
        if (Solid)
        {
            SouthWestCorner = SouthEastCorner = NorthEastCorner = NorthWestCorner = ABOVE_CUBE_HEIGHT;
            Flags = 0;
        }
        else
        {
            SouthWestCorner = SouthEastCorner = NorthEastCorner = NorthWestCorner = BELOW_CUBE_HEIGHT;
            Flags = 0;
        }
    }

    bool isSolid()
    {
        return (SouthWestCorner >= HEIGHT_FRACTIONS + 2) && (SouthEastCorner >= HEIGHT_FRACTIONS + 2) && (NorthWestCorner >= HEIGHT_FRACTIONS + 2) && (NorthEastCorner >= HEIGHT_FRACTIONS + 2);
    }

    bool isEmpty()
    {
        return (SouthWestCorner <= 1) && (SouthEastCorner <= 1) && (NorthWestCorner <= 1) && (NorthEastCorner <= 1);
    }

    inline uint8_t SouthWestHeight() const
    {
        return SouthWestCorner;
    }

    inline uint8_t SouthEastHeight() const
    {
        return SouthEastCorner;
    }

    inline uint8_t NorthWestHeight() const
    {
        return NorthWestCorner;
    }

    inline uint8_t NorthEastHeight() const
    {
        return NorthEastCorner;
    }

    inline bool split() const
    {
        return Flags & 1;
    }

    CubeShape& operator++()
    {
        if (SouthWestHeight() < HEIGHT_FRACTIONS + 2)
        {
            SouthWestCorner++;
        }
        else if (SouthEastHeight() < HEIGHT_FRACTIONS + 2)
        {
            SouthWestCorner = 0;
            SouthEastCorner++;
        }
        else if (NorthWestHeight() < HEIGHT_FRACTIONS + 2)
        {
            SouthWestCorner = 0;
            SouthEastCorner = 0;
            NorthWestCorner++;
        }
        else if (NorthEastHeight() < HEIGHT_FRACTIONS + 2)
        {
            SouthWestCorner = 0;
            SouthEastCorner = 0;
            NorthWestCorner = 0;
            NorthEastCorner++;
        }
        else if (Flags)
        {
            SouthWestCorner = 0;
            SouthEastCorner = 0;
            NorthWestCorner = 0;
            NorthEastCorner = 0;
            Flags = 1;
        }
        else
        {
            SouthWestCorner = 0;
            SouthEastCorner = 0;
            NorthWestCorner = 0;
            NorthEastCorner = 0;
            Flags = 0;
        }

        return *this;
    }

    bool operator== (const CubeShape& ArgumentShape)
    {
        return SouthWestCorner == ArgumentShape.SouthWestCorner && SouthEastCorner == ArgumentShape.SouthEastCorner && NorthWestCorner == ArgumentShape.NorthWestCorner && NorthEastCorner == ArgumentShape.NorthEastCorner;
    }

    bool operator!= (const CubeShape& ArgumentShape)
    {
        return SouthWestCorner != ArgumentShape.SouthWestCorner || SouthEastCorner != ArgumentShape.SouthEastCorner || NorthWestCorner != ArgumentShape.NorthWestCorner || NorthEastCorner != ArgumentShape.NorthEastCorner;
    }

    uint32_t Key()
    {
        uint32_t Key = 0;

        Key = ((SouthWestCorner & 15) << 16) + ((SouthEastCorner & 15) << 12) + ((NorthWestCorner & 15) << 8) + ((NorthEastCorner & 15) << 4) + Flags;
    }

    uint8_t Flags;       // Which direction the Triangles split along and other possible Flags

    uint8_t SouthWestCorner;  // SouthWest and SouthEast packed bits
    uint8_t SouthEastCorner;  // SouthWest and SouthEast packed bits
    uint8_t NorthWestCorner;  // NorthWest and SouthEast packed bits
    uint8_t NorthEastCorner;  // NorthWest and SouthEast packed bits
};

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


void CreateAllEntities();

void CreateFlatTiles();

void CreateFaceTiles();

void CreateSlopedTiles();

CubeShape getTileShapeFromCornerHeight(uint8_t SWCorner, uint8_t SECorner, uint8_t NWCorner, uint8_t NECorner, bool SplittingLine);

FaceShape getFaceShapeFromCornerHeight(Direction DirectionType, uint8_t NegativeCorner, uint8_t PossitiveCorner);

#endif // TILESHAPE__HEADER
