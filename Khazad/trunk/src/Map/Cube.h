#ifndef CUBE__HEADER
#define CUBE__HEADER

#include <stdafx.h>

#include <Actor.h>

#define HALFCUBE 0.5

/*class Face;
class Slope;*/
class Cell;

enum geometry_type{
    GEOM_EMPTY,
    GEOM_WALL,
    GEOM_SLOPE,
    GEOM_FLOOR
};

class Cube: public Actor
{

public:

	Cube();
	~Cube();
	bool Init(Uint16 MaterialType);

    void SetOwner(Cell* NewOwner, Uint8 X, Uint8 Y);

    bool isSolid()                  { return data.geometry == GEOM_WALL; }
    bool isEmpty()                  { return data.geometry == GEOM_EMPTY; }
    bool isSlope()                  { return data.geometry == GEOM_SLOPE; }
    bool isFloor()                  { return data.geometry == GEOM_FLOOR; }
    void setGeometry(geometry_type NewValue);
    geometry_type getGeometry() {return data.geometry;};
    bool hasFace(Facet FacetType);

    Uint8 getLiquid()                   { return data.liquid; }
    void setLiquid(Uint8 liquidtype,Uint8 NewValue);

//    Slope* getSlope()               { return Slopage; }
//	void SetSlope(Slopping Type);
//	void RemoveSlope();
    //void DetermineSlope();

	Uint16 getMaterial()            { return Material; }
    bool setMaterial(Uint16 MaterialType);

   /*
    bool isFaceted()                { return Faceted; }

    Face* getFacet(Facet Type);
    void setFacet(Face* NewFace, Facet Type);
    void setAllFacesVisiblity(bool NewValue);
    void DeleteFace(Facet Type);
    void CheckFaceted();
    */

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
    //static Facet OpositeFace(Facet Type);

   // bool InitFace(Facet FaceType);

	bool Update();
    bool Draw(CameraOrientation Orientation, float xTranslate, float yTranslate);
    //bool DrawFace(float xTranslate, float yTranslate,Facet FacetType, bool draw_vis_invis);
    bool DrawFaces(float xTranslate, float yTranslate, map<int16_t, vector < vertex > * >& normal, map<int16_t, vector < vertex > * > & tops);
    bool DrawSlope(float xTranslate, float yTranslate);
    bool DrawLiquid(float xTranslate, float yTranslate);

    bool isSubTerranean()                 { return data.SubTerranian; }
    void setSubTerranean(bool NewValue)   { data.SubTerranian = NewValue; }

    bool isSkyView()                 { return data.SkyView; }
    void setSkyView(bool NewValue)   { data.SkyView = NewValue; }

    bool isSunLit()                 { return data.SunLit; }
    void setSunLit(bool NewValue)   { data.SunLit = NewValue; }

    static Vector3 ConvertSpacialPoint(SpacialPoint Point);

    void Dig();

protected:
    union
    {
       struct {
        bool SubTerranian : 1;
        bool SkyView : 1;
        bool SunLit : 1;
        bool snow : 1;
        unsigned int facets : 6;
        unsigned int facets_visible : 6;
        unsigned int liquid : 3;
        bool liquidtype :1;
        geometry_type geometry : 3;
        unsigned int empty : 8;
        };
        uint32_t whole;
    }data;

    Cell* Owner;
    Uint16 Material;
    // packed and ready to go
    Uint8 CellX;
    Uint8 CellY;
};

#endif // CUBE__HEADER
