#ifndef SLOPE__HEADER
#define SLOPE__HEADER

#include <stdafx.h>

#include <Actor.h>

class Cube;
class Actor;

class Slope: public Actor
{

public:

	Slope();
	~Slope();
	bool Init(Cube* Owner, Slopping SlopeType);

	void CheckRemoval();

	SDL_Color Color;

	bool Update();
	bool Draw();

	void SetSlopeType(Slopping SlopeType);

private:

	Vector3 Points[4];

    Slopping SlopeType;
	Cube* Owner;
};

#endif // SLOPE__HEADER
