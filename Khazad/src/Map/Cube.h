#ifndef CUBE__HEADER
#define CUBE__HEADER

#include <stdafx.h>

#include <Actor.h>

#define HALFCUBE 0.5

class Face;
class Slope;
class Cell;

class Cube: public Actor
{

public:

	Cube();
	~Cube();
	bool Init(Uint16 MaterialType);

    void SetOwner(Cell* NewOwner, Uint8 X, Uint8 Y);

	bool isSolid()                  { return Solid; }
    void setSolid(bool NewValue)    { Solid = NewValue; }

	Uint8 getLiquid()                   { return Liquid; }
	void setLiquid(Uint8 NewValue)      { Liquid = NewValue; }

    Slope* getSlope()               { return Slopage; }
	void SetSlope(Slopping Type);
	void RemoveSlope();
    void DetermineSlope();

	Uint16 getMaterial()            { return Material; }
    bool setMaterial(Uint16 MaterialType);

    bool isFaceted()                { return Faceted; }

    Face* getFacet(Facet Type);
    void setFacet(Face* NewFace, Facet Type);
    void setAllFacesVisiblity(bool NewValue);
    void DeleteFace(Facet Type);
    void CheckFaceted();

    bool InitFacesOpen();
    bool InitFacesSolid();
    bool InitFaces();

    void InitConstructedFace(Facet FacetType, Uint16 MaterialType);
    bool Open();

    Cube* getAdjacentCube(Facet Type);
    Cube* getNeiborCube(Direction Type);
    Cell* getCellOwner()                    { return Owner; }

    Cell* getAdjacentCell(Facet Type);
    static Facet OpositeFace(Facet Type);

    bool InitFace(Facet FaceType);

	bool Update();
	bool Draw(CameraOrientation Orientation, float xTranslate, float yTranslate, bool DrawHidden, bool DrawSubTerranian, bool DrawSkyView, bool DrawSunLit);

	bool isSubTerranean()                 { return SubTerranian; }
	void setSubTerranean(bool NewValue)   { SubTerranian = NewValue; }

	bool isSkyView()                 { return SkyView; }
	void setSkyView(bool NewValue)   { SkyView = NewValue; }

	bool isSunLit()                 { return SunLit; }
	void setSunLit(bool NewValue)   { SunLit = NewValue; }

    void Dig();

protected:

	Slope* Slopage;

	bool Solid;
	Uint8 Liquid;
	Uint16 Material;

    bool SubTerranian;
    bool SkyView;
    bool SunLit;
    bool Faceted;

    Cell* Owner;

    Uint8 CellX;
    Uint8 CellY;
};

#endif // CUBE__HEADER
