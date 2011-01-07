#include <Coordinates.h>
#ifdef _MSC_VER
#include <stdint_win.h>
#else
#include <stdint.h>
#endif



MapCoordinates::MapCoordinates(CellCoordinates CellCoords, CubeCoordinates CubeCoords)
{
    X = (CellCoords.X * CELLEDGESIZE) + (CubeCoords >> CELLBITSHIFT);
    Y = (CellCoords.Y * CELLEDGESIZE) + (CubeCoords & CELLBITFLAG);
    Z = CellCoords.Z;
}

