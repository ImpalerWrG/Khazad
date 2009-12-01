#ifndef COORDINATES__HEADER
#define COORDINATES__HEADER

#define CELLEDGESIZE 16
#define HALFCUBE 0.5

#include <stdint.h>

enum Dimension
{
    DIMENSION_X,
    DIMENSION_Y,
    DIMENSION_Z,

    NUM_DIMENSIONS,
    DIMENSIONS_START = 0
};

inline Dimension &operator++ (Dimension &OldDimension)      { return OldDimension = Dimension(OldDimension + 1); }
inline Dimension &operator-- (Dimension &OldDimension)      { return OldDimension = Dimension(OldDimension - 1); }

enum Direction
{
    DIRECTION_DOWN,
	DIRECTION_UP,

	DIRECTION_NORTH,
	DIRECTION_SOUTH,
	DIRECTION_WEST,
	DIRECTION_EAST,

    DIRECTION_NORTHWEST,
	DIRECTION_SOUTHEAST,
	DIRECTION_NORTHEAST,
	DIRECTION_SOUTHWEST,

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

inline Direction OppositeDirection(Direction DirectionType)
{
    return  (Direction) ((int) DirectionType ^ 1);  // Flips the last bit
}

inline bool isDirectionPositive(Direction DirectionType)
{
    return DirectionType & 1;  // Referes to Z axis for Directions that span the Z axis
}

struct CubeCoordinates;
struct CellCoordinates;

struct MapCoordinates
{
    MapCoordinates()
    {
        X = Y = Z = 0;
    };

    /*MapCoordinates(Vector3 Point)
    {
        X = Point.x;
        Y = Point.y;
        Z = Point.z;
    };*/

    //MapCoordinates(CellCoordinates CellCoords, CubeCoordinates CubeCoords);

    inline void TranslateMapCoordinates(Direction DirectionType)
    {
        switch (DirectionType)
        {
            case DIRECTION_UP:
                Z += 1;
                break;
            case DIRECTION_DOWN:
                Z -= 1;
                break;

            case DIRECTION_NORTH:
                Y -= 1;
                break;
            case DIRECTION_SOUTH:
                Y += 1;
                break;
            case DIRECTION_EAST:
                X += 1;
                break;
            case DIRECTION_WEST:
                X -= 1;
                break;

            case DIRECTION_NORTHWEST:
                Y -= 1;
                X -= 1;
                break;
            case DIRECTION_SOUTHEAST:
                Y += 1;
                X += 1;
                break;
            case DIRECTION_NORTHEAST:
                Y -= 1;
                X += 1;
                break;
            case DIRECTION_SOUTHWEST:
                Y += 1;
                X -= 1;
                break;

            case DIRECTION_UP_NORTH:
                Z += 1;
                Y -= 1;
                break;
            case DIRECTION_DOWN_SOUTH:
                Z -= 1;
                Y += 1;
                break;
            case DIRECTION_UP_EAST:
                Z += 1;
                X += 1;
                break;
            case DIRECTION_DOWN_WEST:
                Z -= 1;
                X -= 1;
                break;
            case DIRECTION_DOWN_NORTH:
                Z -= 1;
                Y -= 1;
                break;
            case DIRECTION_UP_SOUTH:
                Z += 1;
                Y += 1;
                break;
            case DIRECTION_DOWN_EAST:
                Z -= 1;
                X += 1;
                break;
            case DIRECTION_UP_WEST:
                Z += 1;
                X -= 1;
                break;

            case DIRECTION_UP_NORTHWEST:
                X -= 1;
                Y -= 1;
                Z += 1;
                break;
            case DIRECTION_DOWN_SOUTHEAST:
                X += 1;
                Y += 1;
                Z -= 1;
                break;
            case DIRECTION_UP_NORTHEAST:
                X += 1;
                Y -= 1;
                Z += 1;
                break;
            case DIRECTION_DOWN_SOUTHWEST:
                X -= 1;
                Y += 1;
                Z -= 1;
                break;
            case DIRECTION_UP_SOUTHEAST:
                X += 1;
                Y += 1;
                Z += 1;
                break;
            case DIRECTION_DOWN_NORTHWEST:
                X -= 1;
                Y -= 1;
                Z -= 1;
                break;
            case DIRECTION_UP_SOUTHWEST:
                X -= 1;
                Y += 1;
                Z += 1;
                break;
            case DIRECTION_DOWN_NORTHEAST:
                X += 1;
                Y -= 1;
                Z -= 1;
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

    CellCoordinates(MapCoordinates SourceCoordinates)
    {
        X = SourceCoordinates.X / CELLEDGESIZE;
        Y = SourceCoordinates.Y / CELLEDGESIZE;
        Z = SourceCoordinates.Z;
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
    };

    CubeCoordinates(uint8_t NewX, uint8_t NewY)
    {
        X = NewX;
        Y = NewY;
    };

    CubeCoordinates& operator= (const CubeCoordinates& ArgumentCoordinates)
    {
        X = ArgumentCoordinates.X;
        Y = ArgumentCoordinates.Y;

        return *this;
    };

    CubeCoordinates(MapCoordinates SourceCoordinates)
    {
        X = SourceCoordinates.X % CELLEDGESIZE;
        Y = SourceCoordinates.Y % CELLEDGESIZE;
    };

    uint8_t X;
    uint8_t Y;
};

/*
MapCoordinates::MapCoordinates(CellCoordinates CellCoords, CubeCoordinates CubeCoords)
{
    X = (CellCoords.X * CELLEDGESIZE) + CubeCoords.X;
    Y = (CellCoords.Y * CELLEDGESIZE) + CubeCoords.Y;
    Z = CellCoords.Z;
}
*/

#endif // COORDINATES__HEADER
