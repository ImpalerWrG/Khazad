#ifndef CUBE__HEADER
#define CUBE__HEADER

#include <stdafx.h>

#include <Actor.h>


class Face;
class Slope;

class Cube: public Actor
{

public:

	Cube();
	~Cube();
	bool Init(Uint16 MaterialType);


	bool isSolid() { return Solid; }
    void setSolid(bool NewValue) { Solid = NewValue; }

	Face* Facets[NUM_FACETS];
	Slope* Slopage;

	void SetSlope(Slopping Type);

	bool Update();
	bool Draw();

	Uint16 getMaterial() { return Material; }

protected:

	bool Solid;
	Uint16 Material;

};

#endif // CUBE__HEADER
