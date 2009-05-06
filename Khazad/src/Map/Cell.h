#ifndef CELL__HEADER
#define CELL__HEADER
#include <stdafx.h>

#include <Actor.h>

class Cube;

class Cell: public Actor
{
public:

	Cell();
	Cell(Sint32 X, Sint32 Y, Sint32 Z);
	~Cell();
	bool Init();
	Cube* getCube(Uint16 x, Uint16 y);

    Uint16 getTriangleCount(CameraOrientation Orientation) { return TriangleCount[Orientation]; }
    void setTriangleCount(CameraOrientation Orientation, Uint16 Triangles) { TriangleCount[Orientation] = Triangles; }

	bool Draw(CameraOrientation Orientation, bool DrawHidden = false);
	bool Update();

	//void SetBasment(bool Value)         { Basment = Value; }
	//bool isBasment()                    { return Basment; }

    bool isActive()                     { return Active; }
    void setActive(bool NewValue)       { Active = NewValue; }

protected:

	//bool Sky;
	//bool Basment;
	//bool Rock;
	//bool Air;

	Sint16 XOffset;
	Sint16 YOffset;
	Sint16 ZOffset;

    Uint16 TriangleCount[5];

	Cube** Cubes;

	bool Active;
};

#endif // CELL__HEADER
