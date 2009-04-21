#ifndef CUBE__HEADER
#define CUBE__HEADER

#include <stdafx.h>

#include <Actor.h>


class Face;
class Slope;
class Cell;

class Cube: public Actor
{

public:

	Cube();
	~Cube();
	bool Init(Uint16 MaterialType);


	bool isSolid()                  { return Solid; }
    void setSolid(bool NewValue)    { Solid = NewValue; }

	bool isLiquid()                 { return Liquid; }
	void setLiquid(bool NewValue)   { Liquid = NewValue; }

    Slope* getSlope()               { return Slopage; }
	void SetSlope(Slopping Type);

    Face* getFacet(Facet Type);
    void setFacet(Facet Type, Face* NewFace);

	Uint16 getMaterial() { return Material; }
    bool setMaterial(Uint16 MaterialType);

    void DeleteFace(Facet Type);

    bool InitAllFaces();
    void InitFace(Facet Type);
    bool Open();

    Cube* getNeiborCube(Facet Type);
    Cell* getNeiborCell(Facet Type);
    static Facet OpositeFace(Facet Type);

	bool Update();
	bool Draw();

protected:

	Face* Facets[NUM_FACETS];

	Slope* Slopage;

	bool Solid;
	bool Liquid;
	Uint16 Material;

};

#endif // CUBE__HEADER
