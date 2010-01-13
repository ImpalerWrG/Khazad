#include <stdafx.h>

#include <Actor.h>
#include <Cell.h>

Actor::Actor()
{
    Visible = false; Hidden = false;
}

Actor::~Actor()
{

}

void Actor::setLocation(MapCoordinates NewPosition)
{
    LocationCoordinates = NewPosition;
    CurrentCubeCoordinates = CubeCoordinates(NewPosition);

    CellCoordinates NewCellCoords = CellCoordinates(NewPosition);
    if (NewCellCoords != CurrentCellCoordinates)
    {
        MAP->getCell(CurrentCellCoordinates)->removeActor(CellActorIndex);
        CellActorIndex = MAP->getCell(NewCellCoords)->addActor(this);
        CurrentCellCoordinates = NewCellCoords;
    }
};

