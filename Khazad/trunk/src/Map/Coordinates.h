#ifndef COORDINATES__HEADER
#define COORDINATES__HEADER

#define CELLEDGESIZE 16
#define HALFCUBE 0.5

#include <stdint.h>

struct MapCoordinates
{
    MapCoordinates()
    {
        X = 0;
        Y = 0;
        Z = 0;
    };

    /*MapCoordinates(Vector3 Point)
    {
        X = Point.x;
        Y = Point.y;
        Z = Point.z;
    };*/

    MapCoordinates(int32_t NewX, int32_t NewY, int32_t NewZ)
    {
        Set(NewX, NewY, NewZ);
    };

    inline void Set(int32_t NewX, int32_t NewY, int32_t NewZ)
    {
        X = NewX;
        Y = NewY;
        Z = NewZ;
    };

    MapCoordinates& operator= (const MapCoordinates& ArgumentCoordinates)
    {
        X = ArgumentCoordinates.X;
        Y = ArgumentCoordinates.Y;
        Z = ArgumentCoordinates.Z;

        return *this;
    };

    bool operator == (const MapCoordinates& ArgumentCoordinates) const
    {
        return (X == ArgumentCoordinates.X && Y == ArgumentCoordinates.Y && Z == ArgumentCoordinates.Z);
    };

    bool operator < (const MapCoordinates& ArgumentCoordinates) const
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

#endif // COORDINATES__HEADER
