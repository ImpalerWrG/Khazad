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
    Ogre::SceneNode* getCellSceneNode()					{ return CellSceneNode; }

	void setVisible(bool NewVisibility);

	inline bool isDirty()										{ return Dirty; }
	inline void setDirty()										{ Dirty = true; }

    void DestroyAllAttachedEntities(Ogre::SceneNode* TargetNode);

private:

	bool Dirty;
	bool Visible;
	Cell* ParentCell;

    // Exact spacial Coordinates of the center of the cell, used for rendering
    Ogre::SceneNode* CellSceneNode;
    Ogre::StaticGeometry* CellGeometry;

};

#endif // TERRAINRENDER__HEADER
