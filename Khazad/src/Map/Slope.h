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
	bool Draw(float xTranslate, float yTranslate);

	void SetSlopeType(Slopping SlopeType);

protected:

	Vector3 Points[4];

    Slopping SlopeType;
	Cube* Owner;

    Uint16 Material;
	Uint16 Texture;

};

#endif // SLOPE__HEADER
