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

    Uint16 getMaterial()            { return Material; }
    bool setMaterial(Uint16 MaterialType);

    Cube* getAdjacentCube(Facet Type);
    bool getAdjacentCubeOutOfBounds(Facet Type);
    Cube* getNeighborCube(Direction Type);
    Cell* getCellOwner()                    { return Owner; }

    Cell* getAdjacentCell(Facet Type);

    //void setFacetSurfaceType(Facet FacetType, Sint16 SurfaceType);
    Sint16 getFacetSurfaceType(Facet FacetType);
    
    //void setFacetMaterialType(Facet FacetType, Sint16 MaterialType);
    Sint16 getFacetMaterialType(Facet FacetType);
    
    bool Update();
    bool Draw(float xTranslate, float yTranslate, map<int16_t, vector < vertex > * >& normal, map<int16_t, vector < vertex > * > & tops);
    bool DrawFaces(float xTranslate, float yTranslate, map<int16_t, vector < vertex > * >& normal, map<int16_t, vector < vertex > * > & tops);
    bool DrawSlope(float xTranslate, float yTranslate, map<int16_t, vector < vertex > * >& normal, map<int16_t, vector < vertex > * > & tops);
    //bool DrawLiquid(float xTranslate, float yTranslate);

    bool isSubTerranean()                 { return data.SubTerranian; }
    void setSubTerranean(bool NewValue)   { data.SubTerranian = NewValue; }

    bool isSkyView()                 { return data.SkyView; }
    void setSkyView(bool NewValue)   { data.SkyView = NewValue; }

    bool isSunLit()                 { return data.SunLit; }
    void setSunLit(bool NewValue)   { data.SunLit = NewValue; }

    static Vector3 ConvertSpacialPoint(SpacialPoint Point);

    void Dig();
    void DigChannel();
    void DigSlope();

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
