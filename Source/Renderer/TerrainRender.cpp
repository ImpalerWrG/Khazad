#include <TerrainRender.h>

#include <TileShapes.h>
#include <Coordinates.h>
#include <Face.h>

#include <Renderer.h>

TerrainRendering::TerrainRendering(Cell* TargetCell)
{
	ParentCell = TargetCell;
	CellCoordinates Coords = ParentCell->getCellCoordinates();
    Ogre::SceneNode* ZNode = RENDERER->getZlevelNode(Coords.Z);
    CellSceneNode = ZNode->createChildSceneNode();

	float x = (float) (Coords.X * CELLEDGESIZE) + (CELLEDGESIZE / 2) - HALFCUBE;
    float y = (float) (Coords.Y * CELLEDGESIZE) + (CELLEDGESIZE / 2) - HALFCUBE;

    CellSceneNode->setPosition(x, y, 0);

    char buffer[64];
    sprintf(buffer, "Cell%i-%i-%i",  Coords.X, Coords.Y, Coords.Z);

    if (!RENDERER->getSceneManager()->hasStaticGeometry(buffer))
    {
        CellGeometry = RENDERER->getSceneManager()->createStaticGeometry(buffer);
    }
}

void TerrainRendering::BuildFaceGeometry()
{
    if (CellGeometry != NULL)
    {
        CellGeometry->reset();

        // Iterate all Faces and RefreshEntites;
        std::map<uint16_t, Face*> Faces = ParentCell->getFaceMap();
        for (std::map<uint16_t, Face*>::iterator it = Faces.begin(); it != Faces.end(); it++)
        {
            it->second->RefreshEntity(CellGeometry, ParentCell->getCellCoordinates());
        }

        CellGeometry->setCastShadows(true);
        CellGeometry->build();
    }
}

void TerrainRendering::setNeedsReRendering()
{
	RENDERER->addDirtyTerrain(this);
}

void TerrainRendering::setVisible(bool NewVisibility)
{
	Visible = NewVisibility;
	CellGeometry->setVisible(NewVisibility);
}

void TerrainRendering::DestroyAllAttachedEntities(Ogre::SceneNode* TargetNode)
{
    // Destroy all the attached objects
    Ogre::SceneNode::ObjectIterator Nodeit = TargetNode->getAttachedObjectIterator();

    while (Nodeit.hasMoreElements())
    {
        Ogre::MovableObject* Object = static_cast<Ogre::MovableObject*> (Nodeit.getNext());
        TargetNode->getCreator()->destroyMovableObject(Object);
    }

    // Recurse to child SceneNodes
    Ogre::SceneNode::ChildNodeIterator itChild = TargetNode->getChildIterator();

    while (itChild.hasMoreElements())
    {
        Ogre::SceneNode* ChildNode = static_cast<Ogre::SceneNode*> (itChild.getNext());
        DestroyAllAttachedEntities(ChildNode);
        ChildNode->getCreator()->destroySceneNode(ChildNode);
    }
}

