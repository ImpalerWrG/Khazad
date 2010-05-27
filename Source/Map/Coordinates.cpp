#include <Coordinates.h>
#include <stdint.h>



MapCoordinates::MapCoordinates(CellCoordinates CellCoords, CubeCoordinates CubeCoords)
{
    X = (CellCoords.X * CELLEDGESIZE) + CubeCoords.X;
    Y = (CellCoords.Y * CELLEDGESIZE) + CubeCoords.Y;
    Z = CellCoords.Z;
}

