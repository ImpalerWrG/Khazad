#ifndef FACE__HEADER
#define FACE__HEADER

#include <stdafx.h>

#include <Actor.h>
#include <Cube.h>


class Cube;
class Actor;

class Face: public Actor
{
public:

	Face();
	~Face();
	bool Init(Cube* First, Cube* Second, Facet Type, Uint16 MaterialType);

	bool CheckRemoval();

	Vector3 Points[4];
	SDL_Color Color;

	bool Update();
	bool Draw();

	void setMaterial(Uint16 MaterialID) { Material = MaterialID; }

    Cube* getFirstOwner() { return FirstOwner; }
    Cube* getSecondOwner() { return SecondOwner; }

protected:

	Cube* FirstOwner;
	Cube* SecondOwner;

	Uint16 Material;
	Facet FacetType;
};

#endif // FACE__HEADER
