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
    void setSolid(bool NewValue)    { Solid = NewValue; /*Liquid = !NewValue;*/}

	Uint8 getLiquid()                 { return Liquid; }
	void setLiquid(Uint8 NewValue)      { Liquid = NewValue; }

    Slope* getSlope()               { return Slopage; }
	void SetSlope(Slopping Type);
	void RemoveSlope();
    void DetermineSlope();

	Uint16 getMaterial()            { return Material; }
    bool setMaterial(Uint16 MaterialType);

    Sint16 FaceMaterial(Facet Type);

    Face* getFacet(Facet Type);
    void setFacet(Facet Type, Face* NewFace);
    void setAllFacesVisiblity(bool NewValue);
    void DeleteFace(Facet Type);

    bool InitAllFaces();
    void InitFace(Facet Type);
    void InitConstructedFace(Facet FacetType, Uint16 MaterialType);
    bool Open();

    Cube* getAdjacentCube(Facet Type);
    Cube* getNeiborCube(Direction Type);
    Cell* getCellOwner();

    Cell* getAdjacentCell(Facet Type);
    static Facet OpositeFace(Facet Type);


	bool Update();
	bool Draw(CameraOrientation Orientation, float xTranslate, float yTranslate, bool DrawHidden = false);

protected:

	Face* Facets[NUM_FACETS];

	Slope* Slopage;

	bool Solid;
	Uint8 Liquid;
	Uint16 Material;

};

#endif // CUBE__HEADER
