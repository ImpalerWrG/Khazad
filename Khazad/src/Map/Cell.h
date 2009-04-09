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

	bool Draw();
	bool Update();

private:

	bool Sky;
	bool Basment;
	bool Rock;
	bool Air;

	Sint16 XOffset;
	Sint16 YOffset;
	Sint16 ZOffset;

	Cube** Cubes;

	bool Active;
};

#endif // CELL__HEADER
