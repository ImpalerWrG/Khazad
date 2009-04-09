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
	bool Init(Cube* First, Cube* Second, Facet Type);

	void CheckRemoval();

	Vector3 Points[4];
	SDL_Color Color;
	Uint16 Texture;

	bool Update();
	bool Draw();

private:

	Cube* FirstOwner;
	Cube* SecondeOwner;

	Facet FacetType;
};

#endif // FACE__HEADER
