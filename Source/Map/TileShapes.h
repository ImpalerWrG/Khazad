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
        CubeShape(CUBE_TOP_HEIGHT);
    }

    CubeShape(uint8_t NewHeight)
    {
        SouthWestCorner = SouthEastCorner = NorthEastCorner = NorthWestCorner = NewHeight;
    }

    CubeShape(uint8_t SWCornerHeight, uint8_t SECornerHeight, uint8_t NWCornerHeight, uint8_t NECornerHeight, uint8_t NewFlags)
    {
        Flags = NewFlags;

        SouthWestCorner = SWCornerHeight;
        SouthEastCorner = SECornerHeight;

        NorthWestCorner = NWCornerHeight;
        NorthEastCorner = NECornerHeight;
    }

    bool isSolid()
    {
        return (SouthWestCorner >= CUBE_TOP_HEIGHT) && (SouthEastCorner >= CUBE_TOP_HEIGHT) && (NorthWestCorner >= CUBE_TOP_HEIGHT) && (NorthEastCorner >= CUBE_TOP_HEIGHT);
    }

    bool isEmpty()
    {
        return (SouthWestCorner <= CUBE_BOTTOM_HEIGHT) && (SouthEastCorner <= CUBE_BOTTOM_HEIGHT) && (NorthWestCorner <= CUBE_BOTTOM_HEIGHT) && (NorthEastCorner <= CUBE_BOTTOM_HEIGHT);
    }

    bool hasFloor()
    {
       if (split())
       {
            if (SouthEastCorner == CUBE_BOTTOM_HEIGHT && NorthEastCorner == CUBE_BOTTOM_HEIGHT && NorthWestCorner == CUBE_BOTTOM_HEIGHT)
            {
                return true;
            }
            if (NorthWestCorner == CUBE_BOTTOM_HEIGHT && SouthWestCorner == CUBE_BOTTOM_HEIGHT && SouthEastCorner == CUBE_BOTTOM_HEIGHT)
            {
                return true;
            }
            return false;
       }
       else
       {
            if (NorthEastCorner == CUBE_BOTTOM_HEIGHT && NorthWestCorner == CUBE_BOTTOM_HEIGHT && SouthWestCorner == CUBE_BOTTOM_HEIGHT)
            {
                return true;
            }
            if (SouthWestCorner == CUBE_BOTTOM_HEIGHT && SouthEastCorner == CUBE_BOTTOM_HEIGHT && NorthEastCorner == CUBE_BOTTOM_HEIGHT)
            {
                return true;
            }
            return false;
        }
    }

    bool hasCeiling()
    {
       if (split())
       {
            if (SouthEastCorner == CUBE_TOP_HEIGHT && NorthEastCorner == CUBE_TOP_HEIGHT && NorthWestCorner == CUBE_TOP_HEIGHT)
            {
                return true;
            }
            if (NorthWestCorner == CUBE_TOP_HEIGHT && SouthWestCorner == CUBE_TOP_HEIGHT && SouthEastCorner == CUBE_TOP_HEIGHT)
            {
                return true;
            }
            return false;
       }
       else
       {
            if (NorthEastCorner == CUBE_TOP_HEIGHT && NorthWestCorner == CUBE_TOP_HEIGHT && SouthWestCorner == CUBE_TOP_HEIGHT)
            {
                return true;
            }
            if (SouthWestCorner == CUBE_TOP_HEIGHT && SouthEastCorner == CUBE_TOP_HEIGHT && NorthEastCorner == CUBE_TOP_HEIGHT)
            {
                return true;
            }
            return false;
        }
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

    bool operator== (const CubeShape& ArgumentShape)
    {
        return SouthWestCorner == ArgumentShape.SouthWestCorner && SouthEastCorner == ArgumentShape.SouthEastCorner && NorthWestCorner == ArgumentShape.NorthWestCorner && NorthEastCorner == ArgumentShape.NorthEastCorner;
    }

    bool operator!= (const CubeShape& ArgumentShape)
    {
        return SouthWestCorner != ArgumentShape.SouthWestCorner || SouthEastCorner != ArgumentShape.SouthEastCorner || NorthWestCorner != ArgumentShape.NorthWestCorner || NorthEastCorner != ArgumentShape.NorthEastCorner;
    }

    uint8_t Flags;       // Which direction the Triangles split along and other possible Flags

    uint8_t SouthWestCorner;  // SouthWest and SouthEast packed bits
    uint8_t SouthEastCorner;  // SouthWest and SouthEast packed bits
    uint8_t NorthWestCorner;  // NorthWest and SouthEast packed bits
    uint8_t NorthEastCorner;  // NorthWest and SouthEast packed bits
};

struct FaceShape
{
    FaceShape()
    {
        CubeComponent = CubeShape(BELOW_CUBE_HEIGHT);
        FaceDirection = DIRECTION_UNKNOWN;
    }

    FaceShape(CubeShape ShapeType, Direction DirectionType)
    {
        CubeComponent = ShapeType;
        FaceDirection = DirectionType;
    }

    bool operator== (const FaceShape& ArgumentShape)
    {
        return CubeComponent == ArgumentShape.CubeComponent && FaceDirection == ArgumentShape.FaceDirection;
    }

    bool operator!= (const FaceShape& ArgumentShape)
    {
        return CubeComponent != ArgumentShape.CubeComponent || FaceDirection != ArgumentShape.FaceDirection;
    }

    void getName(char* buffer)
    {
        if (FaceDirection == DIRECTION_NONE)
        {
            uint32_t Hash = ((CubeComponent.SouthWestCorner & 15) << 16) + ((CubeComponent.SouthEastCorner & 15) << 12) + ((CubeComponent.NorthWestCorner & 15) << 8) + ((CubeComponent.NorthEastCorner & 15) << 4) + CubeComponent.Flags;

            sprintf(buffer, "Slope%i", Hash);
        }
        else
        {
            switch (FaceDirection)
            {
                uint32_t Hash;

                case DIRECTION_UP:
                    Hash = ((CubeComponent.SouthWestCorner & 15) << 16) + ((CubeComponent.SouthEastCorner & 15) << 12) + ((CubeComponent.NorthWestCorner & 15) << 8) + ((CubeComponent.NorthEastCorner & 15) << 4) + CubeComponent.Flags;
                    sprintf(buffer, "UpFace%i", Hash);
                    break;

                case DIRECTION_DOWN:
                    Hash = ((CubeComponent.SouthWestCorner & 15) << 16) + ((CubeComponent.SouthEastCorner & 15) << 12) + ((CubeComponent.NorthWestCorner & 15) << 8) + ((CubeComponent.NorthEastCorner & 15) << 4) + CubeComponent.Flags;
                    sprintf(buffer, "DownFace%i", Hash);
                    break;

                case DIRECTION_NORTH:
                    Hash = ((CubeComponent.NorthWestCorner & 15) << 8) + ((CubeComponent.NorthEastCorner & 15) << 4);
                    sprintf(buffer, "NorthFace%i", Hash);
                    break;

                case DIRECTION_SOUTH:
                    Hash = ((CubeComponent.SouthWestCorner & 15) << 8) + ((CubeComponent.SouthEastCorner & 15) << 4);
                    sprintf(buffer, "SouthFace%i", Hash);
                    break;

                case DIRECTION_EAST:
                    Hash = ((CubeComponent.NorthEastCorner & 15) << 8) + ((CubeComponent.SouthEastCorner & 15) << 4);
                    sprintf(buffer, "EastFace%i", Hash);
                    break;

                case DIRECTION_WEST:
                    Hash = ((CubeComponent.NorthWestCorner & 15) << 8) + ((CubeComponent.SouthWestCorner & 15) << 4);
                    sprintf(buffer, "WestFace%i", Hash);
                    break;

                default:
                    sprintf(buffer, "NoFace");
            }
        }
    }

    CubeShape CubeComponent;

    Direction FaceDirection;
};

void CreateAllEntities();

void CreateFlatTiles();

void CreateFaceTiles();

void CreateSlopedTiles();

void CreateOther();


#endif // TILESHAPE__HEADER
