#ifndef TILESHAPE__HEADER
#define TILESHAPE__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <Ogre.h>

#define BELOW_CUBE_HEIGHT 0
#define CUBE_BOTTOM_HEIGHT 1
#define HEIGHT_FRACTIONS 3
#define CUBE_TOP_HEIGHT 4

// BitPacking  SWW SEE NWW NEE FLAG
//             FED CBA 987 654 3210

#define SWMASK 57344
#define SEMASK 7168
#define NWMASK 896
#define NEMASK 112

#define FLAGMASK 15

#define SWSHIFT 13
#define SESHIFT 10
#define NWSHIFT 7
#define NESHIFT 4


struct CubeShape
{
    CubeShape()
    {
        CubeShape(CUBE_TOP_HEIGHT);
    }

    CubeShape(uint8_t NewHeight)
    {
		CubeShape(NewHeight, NewHeight, NewHeight, NewHeight, false);
    }

    CubeShape(uint8_t SWCornerHeight, uint8_t SECornerHeight, uint8_t NWCornerHeight, uint8_t NECornerHeight, uint8_t NewFlags)
    {
		Data = ((SWCornerHeight << SWSHIFT) & SWMASK) | ((SECornerHeight << SESHIFT) & SEMASK) | ((NWCornerHeight << NWSHIFT) & NWMASK) | ((NECornerHeight << NESHIFT) & NEMASK) | (NewFlags & FLAGMASK);
    }

    bool isSolid()
    {
        return (SouthWestCorner() >= CUBE_TOP_HEIGHT) && (SouthEastCorner() >= CUBE_TOP_HEIGHT) && (NorthWestCorner() >= CUBE_TOP_HEIGHT) && (NorthEastCorner() >= CUBE_TOP_HEIGHT);
    }

    bool isEmpty()
    {
        return (SouthWestCorner() <= CUBE_BOTTOM_HEIGHT) && (SouthEastCorner() <= CUBE_BOTTOM_HEIGHT) && (NorthWestCorner() <= CUBE_BOTTOM_HEIGHT) && (NorthEastCorner() <= CUBE_BOTTOM_HEIGHT);
    }

    bool isSky()
    {
        int Counter = 0;

        if (SouthWestCorner() < CUBE_BOTTOM_HEIGHT)
            Counter++;
        if (SouthEastCorner() < CUBE_BOTTOM_HEIGHT)
            Counter++;
        if (NorthEastCorner() < CUBE_BOTTOM_HEIGHT)
            Counter++;
        if (NorthWestCorner() < CUBE_BOTTOM_HEIGHT)
            Counter++;

        if (Counter >= 2) {
            return true;
        }

        return false;
    }

    bool hasFace(Direction DirectionType)
    {
        if (DirectionType == DIRECTION_DOWN)
            return hasFloor();
        if (DirectionType == DIRECTION_UP)
            return hasCeiling();
        if (DirectionType == DIRECTION_EAST)
            return (SouthEastCorner() > CUBE_BOTTOM_HEIGHT || NorthEastCorner() > CUBE_BOTTOM_HEIGHT);
        if (DirectionType == DIRECTION_WEST)
            return (SouthWestCorner() > CUBE_BOTTOM_HEIGHT || NorthWestCorner() > CUBE_BOTTOM_HEIGHT);
        if (DirectionType == DIRECTION_NORTH)
            return (NorthEastCorner() > CUBE_BOTTOM_HEIGHT || NorthWestCorner() > CUBE_BOTTOM_HEIGHT);
        if (DirectionType == DIRECTION_SOUTH)
            return (SouthEastCorner() > CUBE_BOTTOM_HEIGHT || SouthWestCorner() > CUBE_BOTTOM_HEIGHT);
    }

    bool hasFloor()
    {
       if (split())
       {
            if (SouthEastCorner() == CUBE_BOTTOM_HEIGHT && NorthEastCorner() == CUBE_BOTTOM_HEIGHT && NorthWestCorner() == CUBE_BOTTOM_HEIGHT)
            {
                return true;
            }
            if (NorthWestCorner() == CUBE_BOTTOM_HEIGHT && SouthWestCorner() == CUBE_BOTTOM_HEIGHT && SouthEastCorner() == CUBE_BOTTOM_HEIGHT)
            {
                return true;
            }
            return false;
       }
       else
       {
            if (NorthEastCorner() == CUBE_BOTTOM_HEIGHT && NorthWestCorner() == CUBE_BOTTOM_HEIGHT && SouthWestCorner() == CUBE_BOTTOM_HEIGHT)
            {
                return true;
            }
            if (SouthWestCorner() == CUBE_BOTTOM_HEIGHT && SouthEastCorner() == CUBE_BOTTOM_HEIGHT && NorthEastCorner() == CUBE_BOTTOM_HEIGHT)
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
            if (SouthEastCorner() == CUBE_TOP_HEIGHT && NorthEastCorner() == CUBE_TOP_HEIGHT && NorthWestCorner() == CUBE_TOP_HEIGHT)
            {
                return true;
            }
            if (NorthWestCorner() == CUBE_TOP_HEIGHT && SouthWestCorner() == CUBE_TOP_HEIGHT && SouthEastCorner() == CUBE_TOP_HEIGHT)
            {
                return true;
            }
            return false;
       }
       else
       {
            if (NorthEastCorner() == CUBE_TOP_HEIGHT && NorthWestCorner() == CUBE_TOP_HEIGHT && SouthWestCorner() == CUBE_TOP_HEIGHT)
            {
                return true;
            }
            if (SouthWestCorner() == CUBE_TOP_HEIGHT && SouthEastCorner() == CUBE_TOP_HEIGHT && NorthEastCorner() == CUBE_TOP_HEIGHT)
            {
                return true;
            }
            return false;
        }
    }

    inline uint8_t SouthWestCorner() const
    {
    	return (Data & SWMASK) >> SWSHIFT;
    }

    inline uint8_t SouthEastCorner() const
    {
    	return (Data & SEMASK) >> SESHIFT;
    }

    inline uint8_t NorthWestCorner() const
    {
    	return (Data & NWMASK) >> NWSHIFT;
    }

    inline uint8_t NorthEastCorner() const
    {
    	return (Data & NEMASK) >> NESHIFT;
    }

	void setSouthWestCorner(uint8_t Height)
	{
		Data |= SWMASK;
		Data &= ((Height << SWSHIFT) & SWMASK);
	}

	void setSouthEastCorner(uint8_t Height)
	{
		Data |= SEMASK;
		Data &= ((Height << SESHIFT) & SEMASK);
	}

	void setNorthWestCorner(uint8_t Height)
	{
		Data |= NWMASK;
		Data &= ((Height << NWSHIFT) & NWMASK);
	}

	void setNorthEastCorner(uint8_t Height)
	{
		Data |= NEMASK;
		Data &= ((Height << NESHIFT) & NEMASK);
	}

    inline bool split() const
    {
        return (Data & 1);
    }

    bool operator== (const CubeShape& ArgumentShape)
    {
		return Data == ArgumentShape.Data;
    }

    bool operator!= (const CubeShape& ArgumentShape)
    {
		return Data != ArgumentShape.Data;
    }

	uint16_t Data;		// Bit compressed heights of each corner and flags
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

	uint32_t Key()
	{
		return (CubeComponent.Data << 16) + FaceDirection.Data;
	}

    CubeShape CubeComponent;
    Direction FaceDirection;
};

Ogre::MeshPtr CreateFlatFace(FaceShape Shape);

Ogre::MeshPtr CreateSideFace(FaceShape Shape);

Ogre::MeshPtr CreateSlopeFace(FaceShape Shape);

#endif // TILESHAPE__HEADER
