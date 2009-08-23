#ifndef CELL__HEADER
#define CELL__HEADER
#include <stdafx.h>

#include <Actor.h>

#define CELLEDGESIZE 16

class Cube;
class Building;
class Tree;

class Cell: public Actor
{
public:

	Cell();
	Cell(Sint32 X, Sint32 Y, Sint32 Z);
	~Cell();
	bool Init();

	Cube* getCube(Uint8 x, Uint8 y);
	void setCube(Cube* NewCube, Uint8 x, Uint8 y);


    Face* getFace(Uint8 x, Uint8 y, Facet FaceType);
	void setFace(Face* NewFace, Uint8 x, Uint8 y, Facet FaceType);

    Uint16 getTriangleCount(CameraOrientation Orientation) { return TriangleCount[Orientation]; }

    void setTriangleCount(CameraOrientation Orientation, Uint16 Triangles) { TriangleCount[Orientation] = Triangles; }

	//bool Draw(CameraOrientation Orientation);
	bool DrawSolids(CameraOrientation Orientation);
	bool DrawLiquids();

	bool Update();

    void CreateIndices();

	//void SetBasment(bool Value)         { Basment = Value; }
	//bool isBasment()                    { return Basment; }

    void DrawCellCage();

    inline bool isActive()                     { return Active; }
    void setActive(bool NewValue)       { Active = NewValue; }

    inline bool isLiquidActive()                     { return ActiveLiquid; }
    void setLiquidActive(bool NewValue)       { ActiveLiquid = NewValue; }

    GLuint getDrawListID()                  { return DrawListID; }
    bool isDirtyDrawList()                  { return DirtyDrawlist; }
    void setDirtyDrawList(bool NewValue)    { DirtyDrawlist = NewValue; }

    void addBuilding(Building *);
    void addTree(Tree *);
protected:

	bool DirtyDrawlist;
	GLuint DrawListID;

    Uint16* VertIndices[5];
    Uint16* TextIndices[5];

	//bool Sky;
	//bool Basment;
	//bool Rock;
	//bool Air;

	Sint16 XOffset;
	Sint16 YOffset;
	Sint16 ZOffset;

    Uint16 TriangleCount[5];

	Cube* Cubes[CELLEDGESIZE][CELLEDGESIZE];
	Face* Facets[CELLEDGESIZE][CELLEDGESIZE][NUM_FACETS];

    vector <Building *> buildings;
    vector <Tree *> trees;
	bool Active;
	bool ActiveLiquid;
};

#endif // CELL__HEADER
