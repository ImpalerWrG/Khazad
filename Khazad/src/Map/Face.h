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
	bool Draw(float xTranslate, float yTranslate);

    Uint16 getMaterial()                    { return Material; }
	void setMaterial(Uint16 MaterialID)     { Material = MaterialID; }

    Cube* getFirstOwner()                   { return FirstOwner; }
    Cube* getSecondOwner()                  { return SecondOwner; }

    bool isConstructed()                    { return Constructed;}
    void setConstructed(bool NewValue)       { Constructed = NewValue;}

    bool isDecorated()                      { return Decorated;}
    void setDecorated(bool NewValue)        { Decorated = NewValue;}

protected:

    bool Constructed; // Face will be independent of Cube addition or removal
    bool Decorated;   // Face apearance is different from Owner Cube

	Cube* FirstOwner;
	Cube* SecondOwner;

	Uint16 Material;
	Facet FacetType;

	Uint8 Rotation;
};

#endif // FACE__HEADER
