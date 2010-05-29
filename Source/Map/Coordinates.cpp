#include <Coordinates.h>
#ifdef _MSC_VER
#include <stdint_win.h>
#else
#include <stdint.h>
#endif



MapCoordinates::MapCoordinates(CellCoordinates CellCoords, CubeCoordinates CubeCoords)
{
    X = (CellCoords.X * CELLEDGESIZE) + CubeCoords.X;
    Y = (CellCoords.Y * CELLEDGESIZE) + CubeCoords.Y;
    Z = CellCoords.Z;
}

