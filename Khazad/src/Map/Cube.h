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
	bool isLiquid() { return Liquid; }
	void setLiquid(bool NewValue) { Liquid = NewValue; }

	Face* Facets[NUM_FACETS];

    Slope* getSlope() { return Slopage; }
	void SetSlope(Slopping Type);


	Uint16 getMaterial() { return Material; }

	bool Update();
	bool Draw();

protected:

	Slope* Slopage;

	bool Solid;
	bool Liquid;
	Uint16 Material;

};

#endif // CUBE__HEADER
