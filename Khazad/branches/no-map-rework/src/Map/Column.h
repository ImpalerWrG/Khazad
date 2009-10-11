#ifndef COLUMN__HEADER
#define COLUMN__HEADER

#include <stdafx.h>

class Cell;

class Column
{
public:

    Column();
    ~Column();
    bool Init(Sint16 X, Sint16 Y);

    Cell* getCell(Sint16 Zlevel);
    void PushCell(Cell* NewCell, Sint16 Zlevel);

    Sint16 TopLevel()       { return PositiveVector.size() - 1; }
    Sint16 BottomLevel()    { return NegativeVector.size() * -1; }
    Uint32 Size()           { return PositiveVector.size() + NegativeVector.size() - 1; }

protected:

    Sint16 XLocation;
    Sint16 YLocation;

    std::vector<Cell*> PositiveVector;
    std::vector<Cell*> NegativeVector;
};

#endif // COLUMN__HEADER
