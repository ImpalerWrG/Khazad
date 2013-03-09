#include <Cell.h>

#include <Map.h>
#include <Face.h>
#include <Actor.h>
#include <DataManager.h>
#include <Renderer.h>
#include <TextureManager.h>


Cell::Cell()
{
    CubeShape EmptyCube = CubeShape(BELOW_CUBE_HEIGHT);
    CubeShapeTypes.assign(EmptyCube);

    CubeMaterialTypes.assign(INVALID_INDEX);

    Hidden.reset();
    SubTerranean.reset();
    SkyView.reset();
    SunLit.reset();
}

Cell::~Cell()
{

}

bool Cell::InitializeCell(Map* Parent)
{
    NeedsReBuild = true;

    ParentMap = Parent;

    return true;
}

void Cell::setCellPosition(CellCoordinates Coordinates)
{
    thisCellCoordinates = Coordinates;

    float x = (float) (Coordinates.X * CELLEDGESIZE) + (CELLEDGESIZE / 2) - HALFCUBE;
    float y = (float) (Coordinates.Y * CELLEDGESIZE) + (CELLEDGESIZE / 2) - HALFCUBE;

    Ogre::SceneNode* ZNode = ParentMap->getZlevelNode(thisCellCoordinates.Z);
    CellSceneNode = ZNode->createChildSceneNode();

    CellSceneNode->setPosition(x, y, 0);

    char buffer[64];
    sprintf(buffer, "Cell%i-%i-%i",  thisCellCoordinates.X, thisCellCoordinates.Y, thisCellCoordinates.Z);

    if (!RENDERER->getSceneManager()->hasStaticGeometry(buffer))
    {
        CellGeometry = RENDERER->getSceneManager()->createStaticGeometry(buffer);
    }
}

void Cell::setCubeShape(CubeCoordinates Coordinates, CubeShape NewShape)
{
    if (NewShape != CubeShapeTypes[Coordinates])
    {
        CubeShapeTypes[Coordinates] = NewShape;
        setNeedsReBuild(true);

        Face* TargetFace = getFace(FaceCoordinates(Coordinates, DIRECTION_NONE));
        if (TargetFace != NULL)
        {
            setFaceShape(FaceCoordinates(Coordinates, DIRECTION_NONE), FaceShape(NewShape, DIRECTION_NONE));
        }
    }
}

void Cell::BuildFaceData()
{
    MapCoordinates TargetMapCoordinates;
    bool Debug = true;

    CubeCoordinates TargetCube = 0;
    do
    {
        CubeShape Shape = getCubeShape(TargetCube);
        int16_t CubeMaterial = getCubeMaterial(TargetCube);

        static int16_t NEHEMaterial = DATA->getLabelIndex("MATERIAL_UNINITIALIZED");
        static int16_t WallSurface = DATA->getLabelIndex("SURFACETYPE_ROUGH_WALL");
        static int16_t FloorSurface = DATA->getLabelIndex("SURFACETYPE_ROUGH_FLOOR_1");

        for (uint8_t i = 0, DirectionType = Direction::AXIAL_DIRECTIONS[i]; i < NUM_AXIAL_DIRECTIONS; ++i, DirectionType = Direction::AXIAL_DIRECTIONS[i])
        {
            FaceCoordinates FaceLocation = FaceCoordinates(TargetCube, DirectionType);
            MapCoordinates ModifiedCoordinates = MapCoordinates(thisCellCoordinates, TargetCube);
            ModifiedCoordinates.TranslateMapCoordinates(DirectionType);

            CubeShape AdjacentShape = ParentMap->getCubeShape(ModifiedCoordinates);

            if (!Shape.isSky())
            {
                if (ParentMap->isCubeInited(ModifiedCoordinates) && AdjacentShape.isSky())
                {
                    if (Shape.hasFace(DirectionType))
                    {
                        Face* NewFace = ParentMap->addFace(MapCoordinates(thisCellCoordinates, TargetCube), DirectionType);

                        NewFace->setFaceMaterialType(CubeMaterial);
                        NewFace->setFaceSurfaceType(WallSurface);
                        NewFace->setFaceShapeType(FaceShape(Shape, DirectionType));
                    }
                }

                if (!AdjacentShape.isEmpty())
                {
                    if (DirectionType == DIRECTION_DOWN && Shape.hasFloor())
                    {
                        Face* NewFace = ParentMap->addFace(MapCoordinates(thisCellCoordinates, TargetCube), DirectionType);

                        NewFace->setFaceMaterialType(ParentMap->getCubeMaterial(ModifiedCoordinates));
                        NewFace->setFaceSurfaceType(FloorSurface);
                        NewFace->setFaceShapeType(FaceShape(Shape, DirectionType));
                    }
                }
            }
        }

        if (!Shape.isEmpty() && !Shape.isSolid())
        {
            Face* NewFace = addFace(FaceCoordinates(TargetCube, DIRECTION_NONE));

            NewFace->setFaceMaterialType(CubeMaterial);
            NewFace->setFaceSurfaceType(FloorSurface);
            NewFace->setFaceShapeType(FaceShape(Shape, DIRECTION_NONE));
        }

        TargetCube++;

    }
    while (TargetCube != 0);  // End Loop when Byte rolls over
}

void Cell::BuildStaticGeometry()
{
    if (CellGeometry != NULL)
    {
        CellGeometry->reset();

        // Iterate all Faces and RefreshEntites;
        for (std::map<uint16_t, Face*>::iterator it = Faces.begin(); it != Faces.end(); it++)
        {
            it->second->RefreshEntity(CellGeometry, thisCellCoordinates);
        }

        CellGeometry->setCastShadows(true);
        CellGeometry->build();
        NeedsReBuild = false;
    }
}

void Cell::DestroyAllAttachedEntities(Ogre::SceneNode* TargetNode)
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

void Cell::setNeedsReBuild(bool NewValue)
{
	NeedsReBuild = NewValue;
	if (NewValue)
	{
		ParentMap->setNeedsReBuild(true);
	}
}

int16_t Cell::getFaceMaterialType(FaceCoordinates TargetCoordinates) const
{
    Face* TargetFace = getFace(TargetCoordinates);
    return TargetFace != NULL ? TargetFace->getFaceMaterialType() : INVALID_INDEX;
}

int16_t Cell::getFaceSurfaceType(FaceCoordinates TargetCoordinates) const
{
    Face* TargetFace = getFace(TargetCoordinates);
    return TargetFace != NULL ? TargetFace->getFaceSurfaceType() : INVALID_INDEX;
}

bool Cell::setFaceMaterialType(FaceCoordinates TargetCoordinates, int16_t MaterialTypeID)
{
    Face* TargetFace = getFace(TargetCoordinates);

    if (TargetFace != NULL)
    {
        TargetFace->setFaceMaterialType(MaterialTypeID);
        setNeedsReBuild(true);
        return true;
    }
    return false;
}

bool Cell::setFaceSurfaceType(FaceCoordinates TargetCoordinates, int16_t SurfaceTypeID)
{
    Face* TargetFace = getFace(TargetCoordinates);

    if (TargetFace != NULL)
    {
        TargetFace->setFaceSurfaceType(SurfaceTypeID);
        setNeedsReBuild(true);
        return true;
    }
    return false;
}

FaceShape Cell::getFaceShape(FaceCoordinates TargetCoordinates) const
{
    Face* TargetFace = getFace(TargetCoordinates);
    return TargetFace != NULL ? TargetFace->getFaceShapeType() : FaceShape(CubeShape(false), DIRECTION_NONE);
}

bool Cell::setFaceShape(FaceCoordinates TargetCoordinates, FaceShape NewShape)
{
    Face* TargetFace = getFace(TargetCoordinates);

    if (TargetFace != NULL)
    {
        TargetFace->setFaceShapeType(NewShape);
        setNeedsReBuild(true);
        return true;
    }
    return false;
}

bool Cell::removeFace(FaceCoordinates TargetCoordinates)
{
    uint16_t Key = TargetCoordinates.FaceKey();

    if (Faces.find(Key) != Faces.end())
    {
        delete Faces.find(Key)->second;
        Faces.erase(Key);

        setNeedsReBuild(true);
        return true;
    }
    return false;
}

Face* Cell::addFace(FaceCoordinates TargetCoordinates)
{
    uint16_t Key = TargetCoordinates.FaceKey();

    if (Faces.find(Key) == Faces.end())
    {
        Face* NewFace = new Face(TargetCoordinates.Coordinates, TargetCoordinates.FaceDirection);
        Faces[Key] = NewFace;

        return NewFace;
    }
    else
    {
        return Faces.find(Key)->second;
    }
}

bool Cell::isCubeSloped(CubeCoordinates Coordinates) const
{
    CubeShape Shape = getCubeShape(Coordinates);
    return (!Shape.isEmpty() && !Shape.isSolid());
}

void Cell::addActor(Actor* NewActor)
{
    CellSceneNode->addChild(NewActor->getNode());
    NewActor->setVisible(Visible);
}

void Cell::removeActor(Actor* DepartingActor)
{
    CellSceneNode->removeChild(DepartingActor->getNode());
}
