#ifndef COORDINATES__HEADER
#define COORDINATES__HEADER

#include <Direction.h>

#define CELLEDGESIZE 16
#define CELLBITSHIFT 4
#define CELLBITFLAG 15
#define CUBESPERCELL 256
#define HALFCUBE 0.5

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


typedef uint8_t CubeCoordinates;  // Holds one of the 256 locations inside a Cell

struct CellCoordinates;

struct MapCoordinates
{
friend class boost::serialization::access;

    MapCoordinates()
    {
        X = Y = Z = 0;
    };

    MapCoordinates(CellCoordinates CellCoords, CubeCoordinates CubeCoords);

    inline void TranslateMapCoordinates(Direction DirectionType, int Length = 1)
    {
    	X += DirectionType.ValueonAxis(AXIS_X);
    	Y += DirectionType.ValueonAxis(AXIS_Y);
    	Z += DirectionType.ValueonAxis(AXIS_Z);
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

    inline MapCoordinates& operator=(const MapCoordinates& ArgumentCoordinates)
    {
        X = ArgumentCoordinates.X;
        Y = ArgumentCoordinates.Y;
        Z = ArgumentCoordinates.Z;

        return *this;
    };

    inline bool operator==(const MapCoordinates& ArgumentCoordinates) const
    {
        return (X == ArgumentCoordinates.X && Y == ArgumentCoordinates.Y && Z == ArgumentCoordinates.Z);
    };

    inline bool operator<(const MapCoordinates& ArgumentCoordinates) const
    {
		if (Z < ArgumentCoordinates.Z)
        {
            return true;
        }
        if (Z > ArgumentCoordinates.Z)
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
        if (X < ArgumentCoordinates.X)
        {
            return true;
        }
        if (X > ArgumentCoordinates.X)
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

	template<class Archive>
	void serialize(Archive & Arc, const unsigned int version)
	{
		Arc& X;
		Arc& Y;
		Arc& Z;
	};
};

struct CellCoordinates  // Holds the relative Relative position of Map Cells, 6 Bytes
{
friend class boost::serialization::access;

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

	template<class Archive>
	void serialize(Archive & Arc, const unsigned int version)
	{
		Arc& X;
		Arc& Y;
		Arc& Z;
	};
};

struct FaceCoordinates
{
friend class boost::serialization::access;

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
        return (FaceDirection.Data << 8) + Coordinates;
    };

    CubeCoordinates Coordinates;
    Direction FaceDirection;

	template<class Archive>
	void serialize(Archive & Arc, const unsigned int version)
	{
		Arc& Coordinates;
		Arc& FaceDirection;
	};
};

#endif // COORDINATES__HEADER
