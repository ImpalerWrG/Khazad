#ifndef TERRAINRENDER__HEADER
#define TERRAINRENDER__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <Cell.h>

#include <Ogre.h>

class Cell;

class TerrainRendering
{
public:

	TerrainRendering(Cell* TargetCell);
	~TerrainRendering();

	void BuildFaceGeometry();
    Ogre::StaticGeometry* getCellGeometry()             { return CellGeometry; }

	void setVisible(bool NewVisibility);
	void setNeedsReRendering();

    void DestroyAllAttachedEntities(Ogre::SceneNode* TargetNode);

private:

	bool Visible;
	Cell* ParentCell;

    // Exact spacial Coordinates of the center of the cell, used for rendering
    Ogre::SceneNode* CellSceneNode;
    Ogre::StaticGeometry* CellGeometry;

};

#endif // TERRAINRENDER__HEADER
