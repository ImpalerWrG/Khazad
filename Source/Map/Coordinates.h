#ifndef COORDINATES__HEADER
#define COORDINATES__HEADER

#define CELLEDGESIZE 16
#define CELLBITSHIFT 4
#define CELLBITFLAG 15
#define CELLSPERBLOCK 256
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

	DIRECTION_NONE,

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

enum TileShape
{
    TILESHAPE_EMPTY = 0,

    // 254 implicit ramp shape combinations

    TILESHAPE_WALL = 255,
    TILESHAPE_FLOOR = 256,

    // Add other tile shapes here but increment NUM
    NUM_TILESHAPES = 257,
    TILESHAPE_START = 0
};

inline TileShape &operator++ (TileShape &OldTileShape)      { return OldTileShape = TileShape(OldTileShape + 1); }
inline TileShape &operator-- (TileShape &OldTileShape)      { return OldTileShape = TileShape(OldTileShape - 1); }

inline TileShape getTileShapeFromCornerHeight(uint8_t SWCorner, uint8_t SECorner, uint8_t NWCorner, uint8_t NECorner)
{
    uint16_t Shape = 0;

    Shape = Shape | (SWCorner << 0);

    Shape = Shape | (SECorner << 2);

    Shape = Shape | (NWCorner << 4);

    Shape = Shape | (NECorner << 6);

    return (TileShape) Shape;
}

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

struct CubeCoordinates;
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
        }
    };

    inline MapCoordinates(MapCoordinates SourceCoords, Direction DirectionType)
    {
        Set(SourceCoords.X, SourceCoords.Y, SourceCoords.Z);
        TranslateMapCoordinates(DirectionType);
    };

    inline MapCoordinates(int32_t NewX, int32_t NewY, int32_t NewZ)
    {
        Set(NewX, NewY, NewZ);
    };

    inline void Set(int32_t NewX, int32_t NewY, int32_t NewZ)
    {
        X = NewX;
        Y = NewY;
        Z = NewZ;
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

    int32_t operator[](unsigned i) const
    {
      switch (i)
      {
        case 0:
          return X;
        case 1:
          return Y;
        case 2:
          return Z;
      }
      return X;
    }

    void set(unsigned i, int32_t val)
    {
      switch (i)
      {
        case 0:
          X = val;
          break;
        case 1:
          Y = val;
          break;
        case 2:
          Z = val;
          break;
      }
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
    int32_t Z;
};

struct CellCoordinates
{
    CellCoordinates()
    {
        X = 0;
        Y = 0;
        Z = 0;
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
            X = SourceCoordinates.X / CELLEDGESIZE;
        }
        else
        {
            X = SourceCoordinates.X / CELLEDGESIZE - 1; //truncate to negative infinity
        }
        if (SourceCoordinates.Y >= 0)
        {
            Y = SourceCoordinates.Y / CELLEDGESIZE;
        }
        else
        {
            Y = SourceCoordinates.Y / CELLEDGESIZE - 1; //truncate to negative infinity
        }
        Z = SourceCoordinates.Z;
    };

    uint64_t Key()
    {
        uint64_t Key = X;
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

struct CubeCoordinates
{
    CubeCoordinates()
    {
        X = 0;
        Y = 0;
        Index = 0;
    };

    CubeCoordinates(uint8_t NewX, uint8_t NewY)
    {
        X = NewX;   Y = NewY;

        Index = (NewX << CELLBITSHIFT) + NewY;
    };

    CubeCoordinates& operator= (const CubeCoordinates& ArgumentCoordinates)
    {
        X = ArgumentCoordinates.X;
        Y = ArgumentCoordinates.Y;

        Index = ArgumentCoordinates.Index;

        return *this;
    };

    CubeCoordinates(MapCoordinates SourceCoordinates)
    {
        X = SourceCoordinates.X & CELLBITFLAG;
        Y = SourceCoordinates.Y & CELLBITFLAG;

        Index = (X << CELLBITSHIFT) + Y;
    };


    uint8_t X;
    uint8_t Y;

    uint8_t Index;
};

struct FaceCoordinates: public MapCoordinates
{
    FaceCoordinates()
    {
        X = Y = Z = 0;
        FaceDirection = DIRECTION_DOWN;
        Possitive = true;
    };

    FaceCoordinates(MapCoordinates SourceCoords, Direction DirectionComponent, bool isPossitive)
    {
        Set(SourceCoords.X, SourceCoords.Y, SourceCoords.Z);
        FaceDirection = DirectionComponent;
        Possitive = isPossitive;
    };

    int32_t X;
    int32_t Y;
    int32_t Z;

    Direction FaceDirection;
    bool Possitive;
};

#endif // COORDINATES__HEADER
