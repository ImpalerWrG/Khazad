#ifndef COORDINATES__HEADER
#define COORDINATES__HEADER

#define CELLEDGESIZE 16
#define CELLBITSHIFT 4
#define CELLBITFLAG 15
#define CUBESPERCELL 256
#define HALFCUBE 0.5

#ifdef _MSC_VER
#include <stdint_win.h>
#else
#include <stdint.h>
#endif

#include <OgreVector3.h>

enum Axis
{
    AXIS_Z,   // Vertical
    AXIS_Y,   // North/South
    AXIS_X,   // East/West

    NUM_AXIS,
    AXIS_START = 0
};

inline Axis &operator++ (Axis &OldAxis)      { return OldAxis = Axis(OldAxis + 1); }
inline Axis &operator-- (Axis &OldAxis)      { return OldAxis = Axis(OldAxis - 1); }

enum Direction
{
    DIRECTION_DOWN,
	DIRECTION_UP,

	DIRECTION_SOUTH,
	DIRECTION_NORTH,
	DIRECTION_WEST,
	DIRECTION_EAST,

	DIRECTION_SOUTHEAST,
    DIRECTION_NORTHWEST,
	DIRECTION_SOUTHWEST,
	DIRECTION_NORTHEAST,

	DIRECTION_DOWN_SOUTH,
	DIRECTION_UP_NORTH,
	DIRECTION_DOWN_WEST,
	DIRECTION_UP_EAST,
	DIRECTION_DOWN_NORTH,
	DIRECTION_UP_SOUTH,
	DIRECTION_DOWN_EAST,
	DIRECTION_UP_WEST,

	DIRECTION_DOWN_SOUTHEAST,
    DIRECTION_UP_NORTHWEST,
	DIRECTION_DOWN_SOUTHWEST,
	DIRECTION_UP_NORTHEAST,
    DIRECTION_DOWN_NORTHWEST,
	DIRECTION_UP_SOUTHEAST,
	DIRECTION_DOWN_NORTHEAST,
	DIRECTION_UP_SOUTHWEST,

	DIRECTION_NONE,          // Lack of Direction, Center of Cube
	DIRECTION_UNKNOWN,       // Truly Undefined Direction

    CARDINAL_DIRECTIONS_START = 2,  // North, South, East and West
	NUM_CARDINAL_DIRECTIONS = 6,

	COMPASS_DIRECTIONS_START = 2,   // The 8 points of a compass
	NUM_COMPASS_DIRECTIONS = 10,

	AXIAL_DIRECTIONS_START = 0,     // Cardinal Directions pluss Up and Down
	NUM_AXIAL_DIRECTIONS = 6,

	ANGULAR_DIRECTIONS_START = 0,  // All Possible Directions
	NUM_ANGULAR_DIRECTIONS = 26
};

inline Direction &operator++ (Direction &OldDirection)      { return OldDirection = Direction(OldDirection + 1); }
inline Direction &operator-- (Direction &OldDirection)      { return OldDirection = Direction(OldDirection - 1); }

inline Axis AxisFromDirection(Direction DirectionType)
{
    if (DirectionType < NUM_AXIAL_DIRECTIONS)
    {
        return (Axis) ((int) DirectionType >> 1);
    }
    return NUM_AXIS;
}

inline Direction DirectionFromAxis(Axis AxisType, bool Possitive)
{
    if (Possitive)
    {
        return (Direction) ((int) AxisType << 1);
    }
    else
    {
        return (Direction) (((int) AxisType << 1) + 1);
    }
}

inline Direction OppositeDirection(Direction DirectionType)
{
    return  (Direction) ((int) DirectionType ^ 1);  // Flips the last bit
}

inline bool isDirectionPositive(Direction DirectionType)
{
    return DirectionType & 1;  // Referes to Z axis for Directions that span the Z axis
}

inline int DirectionValueOnAxis(Direction DirectionType, Axis TestAxis)
{
    if (DirectionType <= NUM_AXIAL_DIRECTIONS)
    {
        if (TestAxis == (DirectionType << 1))
        {
            if (DirectionType & 1)
            {
                return 1;  // On Axis and positive
            }
            else
            {
                return -1; // On Axis and negative
            }
        }
        else
        {
            return 0;  // Not on Axis
        }
    }
    return 0;  // TODO finish for remaining directions
};

inline Ogre::Vector3 DirectionToVector(Direction DirectionType)
{
    if (DirectionType <= NUM_AXIAL_DIRECTIONS)
    {
        switch (DirectionType)
        {
            case DIRECTION_UP:
                return Ogre::Vector3::UNIT_Z;

            case DIRECTION_DOWN:
                return Ogre::Vector3::NEGATIVE_UNIT_Z;

            case DIRECTION_NORTH:
                return Ogre::Vector3::UNIT_Y;

            case DIRECTION_SOUTH:
                return Ogre::Vector3::NEGATIVE_UNIT_Y;

            case DIRECTION_EAST:
                return Ogre::Vector3::UNIT_X;

            case DIRECTION_WEST:
                return Ogre::Vector3::NEGATIVE_UNIT_X;

            default:
                return Ogre::Vector3::ZERO;
        }
    }
    return Ogre::Vector3::ZERO;  // TODO finish for remaining directions
};

typedef uint8_t CubeCoordinates;  // Holds one of the 256 locations inside a Cell

struct CellCoordinates;

struct MapCoordinates
{
    MapCoordinates()
    {
        X = Y = Z = 0;
    };

    MapCoordinates(Ogre::Vector3 Point)
    {
        X = Point.x + HALFCUBE;
        Y = Point.y + HALFCUBE;
        Z = Point.z + HALFCUBE;
    };

    MapCoordinates(CellCoordinates CellCoords, CubeCoordinates CubeCoords);

    inline void TranslateMapCoordinates(Direction DirectionType, int Length = 1)
    {
        switch (DirectionType)
        {
            case DIRECTION_UP:
                Z += Length;
                break;
            case DIRECTION_DOWN:
                Z -= Length;
                break;

            case DIRECTION_NORTH:
                Y += Length;
                break;
            case DIRECTION_SOUTH:
                Y -= Length;
                break;
            case DIRECTION_EAST:
                X += Length;
                break;
            case DIRECTION_WEST:
                X -= Length;
                break;

            case DIRECTION_NORTHWEST:
                Y += Length;
                X -= Length;
                break;
            case DIRECTION_SOUTHEAST:
                Y -= Length;
                X += Length;
                break;
            case DIRECTION_NORTHEAST:
                Y += Length;
                X += Length;
                break;
            case DIRECTION_SOUTHWEST:
                Y -= Length;
                X -= Length;
                break;

            case DIRECTION_UP_NORTH:
                Z += Length;
                Y += Length;
                break;
            case DIRECTION_DOWN_SOUTH:
                Z -= Length;
                Y -= Length;
                break;
            case DIRECTION_UP_EAST:
                Z += Length;
                X += Length;
                break;
            case DIRECTION_DOWN_WEST:
                Z -= Length;
                X -= Length;
                break;
            case DIRECTION_DOWN_NORTH:
                Z -= Length;
                Y += Length;
                break;
            case DIRECTION_UP_SOUTH:
                Z += Length;
                Y -= Length;
                break;
            case DIRECTION_DOWN_EAST:
                Z -= Length;
                X += Length;
                break;
            case DIRECTION_UP_WEST:
                Z += Length;
                X -= Length;
                break;

            case DIRECTION_UP_NORTHWEST:
                X -= Length;
                Y += Length;
                Z += Length;
                break;
            case DIRECTION_DOWN_SOUTHEAST:
                X += Length;
                Y -= Length;
                Z -= Length;
                break;
            case DIRECTION_UP_NORTHEAST:
                X += Length;
                Y += Length;
                Z += Length;
                break;
            case DIRECTION_DOWN_SOUTHWEST:
                X -= Length;
                Y -= Length;
                Z -= Length;
                break;
            case DIRECTION_UP_SOUTHEAST:
                X += Length;
                Y -= Length;
                Z += Length;
                break;
            case DIRECTION_DOWN_NORTHWEST:
                X -= Length;
                Y += Length;
                Z -= Length;
                break;
            case DIRECTION_UP_SOUTHWEST:
                X -= Length;
                Y -= Length;
                Z += Length;
                break;
            case DIRECTION_DOWN_NORTHEAST:
                X += Length;
                Y += Length;
                Z -= Length;
                break;
            case DIRECTION_NONE:
                break;

            default:
                break;
        }
    };

    inline MapCoordinates(MapCoordinates SourceCoords, Direction DirectionType)
    {
        Set(SourceCoords.X, SourceCoords.Y, SourceCoords.Z);
        TranslateMapCoordinates(DirectionType);
    };

    inline MapCoordinates(int32_t NewX, int32_t NewY, int16_t NewZ)
    {
        Set(NewX, NewY, NewZ);
    };

    inline void Set(int32_t NewX, int32_t NewY, int16_t NewZ)
    {
        X = NewX;   Y = NewY;   Z = NewZ;
    };

    inline void Set(Axis AxialComponent, int32_t NewValue)
    {
        switch (AxialComponent)
        {
            case AXIS_Z:
                Z = NewValue;
                break;
            case AXIS_Y:
                Y = NewValue;
                break;
            case AXIS_X:
                X = NewValue;
                break;

            default:
                break;
        }
    }

    inline CubeCoordinates Cube() const
    {
        return ((X & CELLBITFLAG) << CELLBITSHIFT) + (Y & CELLBITFLAG);
    };

    inline MapCoordinates& operator= (const MapCoordinates& ArgumentCoordinates)
    {
        X = ArgumentCoordinates.X;
        Y = ArgumentCoordinates.Y;
        Z = ArgumentCoordinates.Z;

        return *this;
    };

    inline bool operator == (const MapCoordinates& ArgumentCoordinates) const
    {
        return (X == ArgumentCoordinates.X && Y == ArgumentCoordinates.Y && Z == ArgumentCoordinates.Z);
    };

    inline bool operator < (const MapCoordinates& ArgumentCoordinates) const
    {
        if (X < ArgumentCoordinates.X)
        {
            return true;
        }
        if (X > ArgumentCoordinates.X)
        {
            return false;
        }
        if (Y < ArgumentCoordinates.Y)
        {
            return true;
        }
        if (Y > ArgumentCoordinates.Y)
        {
            return false;
        }
        if (Z < ArgumentCoordinates.Z)
        {
            return true;
        }
        if (Z > ArgumentCoordinates.Z)
        {
            return false;
        }

        return false;  // All values equal thus not less than
    };

    int32_t operator[] (Axis AxialComponent) const
    {
        switch (AxialComponent)
        {
            case AXIS_Z:
                return Z;
            case AXIS_Y:
                return Y;
            case AXIS_X:
                return X;
			default:
				return 0;
        }
        return 0;
    }

    struct hash
    {
        unsigned operator()(const MapCoordinates& ArgumentCoordinates) const
        {
            unsigned HashValue = 1;

            HashValue += ArgumentCoordinates.X * (1 << (5 * 0));
            HashValue += ArgumentCoordinates.Y * (1 << (5 * 1));
            HashValue += ArgumentCoordinates.Z * (1 << (5 * 2));

            return HashValue;
        }
    };

    int32_t X;
    int32_t Y;
    int16_t Z;
};

struct CellCoordinates  // Holds the relative Relative position of Map Cells, 6 Bytes
{
    CellCoordinates()
    {
        X = Y = Z = 0;
    };

    CellCoordinates(int16_t NewX, int16_t NewY, int16_t NewZ)
    {
        X = NewX;
        Y = NewY;
        Z = NewZ;
    };

    CellCoordinates& operator= (const CellCoordinates& ArgumentCoordinates)
    {
        X = ArgumentCoordinates.X;
        Y = ArgumentCoordinates.Y;
        Z = ArgumentCoordinates.Z;

        return *this;
    };

    bool operator!= (const CellCoordinates& ArgumentCoordinates)
    {
        return (X != ArgumentCoordinates.X || Y != ArgumentCoordinates.Y || Z != ArgumentCoordinates.Z);
    };

    bool operator== (const CellCoordinates& ArgumentCoordinates)
    {
        return (X == ArgumentCoordinates.X && Y == ArgumentCoordinates.Y && Z == ArgumentCoordinates.Z);
    };

    CellCoordinates(MapCoordinates SourceCoordinates)
    {
        if (SourceCoordinates.X >= 0)
        {
            X = (SourceCoordinates.X >> CELLBITSHIFT);
        }
        else
        {
            X = (SourceCoordinates.X >> CELLBITSHIFT) - 1; //truncate to negative infinity
        }
        if (SourceCoordinates.Y >= 0)
        {
            Y = (SourceCoordinates.Y >> CELLBITSHIFT);
        }
        else
        {
            Y = (SourceCoordinates.Y >> CELLBITSHIFT) - 1; //truncate to negative infinity
        }
        Z = SourceCoordinates.Z;
    };

    uint64_t Key()
    {
        uint64_t Key = 0;

        Key += X;
        Key <<= 16;
        Key += Y;
        Key <<= 16;
        Key += Z;

        return Key;
    };

    int16_t X;
    int16_t Y;
    int16_t Z;
};

struct FaceCoordinates
{
    FaceCoordinates()
    {
        Coordinates = 0;
        FaceDirection = DIRECTION_UNKNOWN;
    };

    FaceCoordinates(CubeCoordinates SourceCoords, Direction DirectionComponent)
    {
        Set(SourceCoords, DirectionComponent);
    };

    FaceCoordinates& operator= (const FaceCoordinates& ArgumentCoordinates)
    {
        Coordinates = ArgumentCoordinates.Coordinates;
        FaceDirection = ArgumentCoordinates.FaceDirection;

        return *this;
    };

    inline void Set(CubeCoordinates TargetCoords, Direction DirectionComponent)
    {
        Coordinates = TargetCoords;
        FaceDirection = DirectionComponent;
    };

    inline void Set(int32_t NewX, int32_t NewY, Direction DirectionComponent)
    {
        Coordinates = (NewX >> CELLBITSHIFT) + NewY;
        FaceDirection = DirectionComponent;
    };

    uint16_t FaceKey()
    {
        uint16_t Key = Coordinates;
        Key <<= CELLBITSHIFT * 2;

        if (FaceDirection != DIRECTION_NONE)
        {
            Key += uint16_t (FaceDirection) + 1;
        }

        return Key;
    };

    CubeCoordinates Coordinates;
    Direction FaceDirection;
};

#endif // COORDINATES__HEADER
