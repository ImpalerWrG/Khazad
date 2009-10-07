#ifndef CUBE__HEADER
#define CUBE__HEADER

#include <stdafx.h>

#include <Actor.h>

#define HALFCUBE 0.5

/*class Face;
class Slope;*/
class Cell;

class Cube: public Actor
{

public:

	Cube();
	~Cube();
	bool Init();

    void SetOwner(Cell* NewOwner, Uint8 X, Uint8 Y);

    bool isSolid();
    bool isSlope();

    bool hasFace(Facet FacetType);

    Uint8 getLiquid()                   { return data.liquid; }
    void setLiquid(Uint8 liquidtype, Uint8 NewValue);

//    Slope* getSlope()               { return Slopage; }
//	void SetSlope(Slopping Type);
//	void RemoveSlope();
    //void DetermineSlope();

    /*bool InitFacesOpen();
    bool InitFacesSolid();
    bool InitFaces();

    void InitConstructedFace(Facet FacetType, Uint16 MaterialType);
    bool Open();
*/
    Cube* getAdjacentCube(Facet Type);
    Cube* getNeighborCube(Direction Type);

    Cell* getCellOwner()                    { return Owner; }
    Cell* getAdjacentCell(Facet Type);

	bool Update();

    bool Draw(float xTranslate, float yTranslate);
    bool DrawFaces(float xTranslate, float yTranslate);
    bool DrawSlope(float xTranslate, float yTranslate);
    //bool DrawLiquid(float xTranslate, float yTranslate);

    void setShape(Sint16 TileShape);
    Sint16 getShape()                       { return CubeShapeType; }

    void setMaterial(Sint16 MaterialID);
    Sint16 getMaterial()                    { return CubeMaterialType; }


    void setFacetSurfaceType(Facet FacetType, Sint16 SurfaceType);
    Sint16 getFacetSurfaceType(Facet FacetType);

    void setFacetMaterialType(Facet FacetType, Sint16 MaterialType);
    Sint16 getFacetMaterialType(Facet FacetType);



    bool isSubTerranean()                   { return data.SubTerranian; }
    void setSubTerranean(bool NewValue)     { data.SubTerranian = NewValue; }

    bool isSkyView()                        { return data.SkyView; }
    void setSkyView(bool NewValue)          { data.SkyView = NewValue; }

    bool isSunLit()                         { return data.SunLit; }
    void setSunLit(bool NewValue)           { data.SunLit = NewValue; }

    static Vector3 ConvertSpacialPoint(SpacialPoint Point);

    void Dig();

protected:

    union
    {
       struct
       {
            bool SubTerranian : 1;
            bool SkyView : 1;
            bool SunLit : 1;
            bool snow : 1;
            unsigned int liquid : 3;
            bool liquidtype :1;
        };
        uint16_t whole;
    }data;

    Cell* Owner;

    Sint16 CubeMaterialType;
    Sint16 CubeShapeType;
    Sint16 SlopeSurfaceType;

    Sint16 FacetSurfaceTypes[NUM_FACETS];
    Sint16 FacetMaterialTypes[3];

    Uint8 CellX;
    Uint8 CellY;
};

#endif // CUBE__HEADER
