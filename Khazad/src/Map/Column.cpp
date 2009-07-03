#include <stdafx.h>

#include <Column.h>

Column::Column()
{
    XLocation = 0;
    YLocation = 0;
}

Column::~Column()
{
    for(Uint32 i = 0; i < NegativeVector.size(); i++)
    {
        delete NegativeVector[i];
    }

    for(Uint32 i = 0; i < PositiveVector.size(); i++)
    {
        delete PositiveVector[i];
    }
}

bool Column::Init(Sint16 X, Sint16 Y)
{
    XLocation = X;
    YLocation = Y;
}

Cell* Column::getCell(Sint16 Zlevel)
{
    if(Zlevel < 0)
    {
        if((Zlevel * -1) < NegativeVector.size() + 1)
        {
            return NegativeVector[(Zlevel * -1) - 1];
        }
    }
    else
    {
        if(Zlevel < PositiveVector.size())
        {
            return PositiveVector[Zlevel];
        }
    }
    return NULL;
}

void Column::PushCell(Cell* NewCell, Sint16 Zlevel)
{
    if(Zlevel < 0)
    {
        if(Zlevel > NegativeVector.size())
        {
            NegativeVector.resize(Zlevel, NULL);
        }
        NegativeVector[(Zlevel * -1) - 1] = NewCell;
    }
    else
    {
        if(Zlevel >= PositiveVector.size())
        {
            PositiveVector.resize(Zlevel + 1, NULL);
        }
        PositiveVector[Zlevel] = NewCell;
    }
}
