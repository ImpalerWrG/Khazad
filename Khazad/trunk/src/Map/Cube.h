#ifndef CUBE__HEADER
#define CUBE__HEADER

#include <stdafx.h>

#include <Cell.h>

#define HALFCUBE 0.5


class Cube
{

public:

	Cube();
	~Cube();
	//bool Init();

    //void setOwner(Cell* NewOwner, Uint8 X, Uint8 Y);

    //bool hasFace(Facet FacetType);


    //Vector3 getAdjacentCubePosition(Facet FacetType);
    //Cube* getAdjacentCube(Facet Type);
    //Cube* getNeighborCube(Direction Type);

    //Cell* getCellOwner()                    { return Owner; }
    //Cell* getAdjacentCell(Facet Type);

	//bool Update();

    //bool Draw(float xTranslate, float yTranslate);
    //bool DrawFaces(float xTranslate, float yTranslate);
    //bool DrawSlope(float xTranslate, float yTranslate);
    //bool DrawLiquid(float xTranslate, float yTranslate);

    //void setShape(Sint16 TileShape);
    //Sint16 getShape()                       { return CubeShapeType; }

    //void setMaterial(Sint16 MaterialID);
    //Sint16 getMaterial()                    { return CubeMaterialType; }

    //void setCubeSurface(Sint16 SurfaceID)   { CubeSurfaceType = SurfaceID; }
    //Sint16 getCubeSurfaceType()             { return CubeSurfaceType; }

    void setFacetSurfaceType(Facet FacetType, Sint16 SurfaceType);
    Sint16 getFacetSurfaceType(Facet FacetType);

    void setFacetMaterialType(Facet FacetType, Sint16 MaterialType);
    Sint16 getFacetMaterialType(Facet FacetType);

    void RefreshFacetData();

    //bool isSolid()                     { Owner->isCubeSolid(CellX, CellY); }
    //bool setSolid(bool NewValue)       { Owner->setCubeSolid(CellX, CellY, NewValue); }

    bool isSlope();

    void Dig();
    void DigChannel();
    void DigSlope();

protected:

    //Cell* Owner;

    //Sint16 CubeMaterialType;
    //Sint16 CubeShapeType;
    //Sint16 CubeSurfaceType;
    //Sint16 SlopeSurfaceType;

    Sint16 FacetSurfaceTypes[NUM_FACETS];
    Sint16 FacetMaterialTypes[3];

    Uint8 CellX;
    Uint8 CellY;
};

#endif // CUBE__HEADER
