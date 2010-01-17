#include <stdafx.h>

#include <Actor.h>
#include <Cell.h>

Actor::Actor()
{
    Visible = false;
    Hidden = false;
    CurrentCell = NULL;
}

Actor::~Actor()
{

}

void Actor::setLocation(MapCoordinates NewPosition)
{
    LocationCoordinates = NewPosition;
    CurrentCubeCoordinates = CubeCoordinates(NewPosition);

    CellCoordinates NewCellCoords = CellCoordinates(NewPosition);
    Cell* NewCell = MAP->getCell(NewCellCoords);

    if (CurrentCell != NewCell)
    {
        if (CurrentCell != NULL)
        {
            CurrentCell->removeActor(CellActorIndex);
        }

        if (NewCell != NULL)
        {
            CellActorIndex = NewCell->addActor(this);
            CurrentCell = NewCell;
        }
    }
};
