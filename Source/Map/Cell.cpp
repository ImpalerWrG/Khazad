#include <Cell.h>

#include <Map.h>
#include <Face.h>
#include <Actor.h>
#include <DataManager.h>
#include <Renderer.h>
#include <TextureManager.h>


Cell::Cell()
{
    Active = false;
    ActiveLiquid = false;
    Initialized = false;

    for(uint8_t i = 0; i < CELLEDGESIZE; i++)
    {
        for(uint8_t j = 0; j < CELLEDGESIZE; j++)
        {
            CubeShapeTypes[(i * CELLEDGESIZE) + j] = TILESHAPE_EMPTY;
            CubeMaterialTypes[(i * CELLEDGESIZE) + j] = INVALID_INDEX;
        }
    }

    Hidden.reset();
    SubTerranean.reset();
    SkyView.reset();
    SunLit.reset();
    LiquidType.reset();

    CellSceneNode = RENDERER->getRootNode()->createChildSceneNode();
}

Cell::~Cell()
{

}

bool Cell::InitializeCell(Map* Parent)
{
    Initialized = true;
    NeedsReBuild = true;

    ParentMap = Parent;

    return true;
}

void Cell::setCellPosition(CellCoordinates Coordinates)
{
    float x = (float) (Coordinates.X * CELLEDGESIZE) + (CELLEDGESIZE / 2) - HALFCUBE;
    float y = (float) (Coordinates.Y * CELLEDGESIZE) + (CELLEDGESIZE / 2) - HALFCUBE;
    float z = (float) Coordinates.Z;

    CellSceneNode->setPosition(x, y, z);
    thisCellCoordinates = Coordinates;
}

bool Cell::Update()
{
    return true;
}

void Cell::setCubeShape(CubeCoordinates Coordinates, TileShape NewShape)
{
    if (NewShape != CubeShapeTypes[Coordinates.Index])
    {
        CubeShapeTypes[Coordinates.Index] = NewShape;
        setNeedsReBuild(true);
    }
}

void Cell::BuildFaceData()
{
    MapCoordinates TargetMapCoordinates;
    bool Debug = true;

    for (CubeCoordinates TargetCubeCoordinates; TargetCubeCoordinates.Index < (CUBESPERCELL - 1); ++TargetCubeCoordinates)
    {
        TileShape CubeShape = getCubeShape(TargetCubeCoordinates);
        int16_t CubeMaterial = getCubeMaterial(TargetCubeCoordinates);

        static int16_t CubeSurface = DATA->getLabelIndex("SURFACETYPE_ROUGH_WALL");

        if (CubeShape != TILESHAPE_EMPTY)
        {
            for (Direction DirectionType = AXIAL_DIRECTIONS_START; DirectionType < NUM_AXIAL_DIRECTIONS; ++DirectionType)
            {
                FaceCoordinates FaceLocation = FaceCoordinates(TargetCubeCoordinates, DirectionType);

                if (ParentMap->isCubeInited(TargetMapCoordinates))
                {
                    if (ParentMap->getCubeShape(TargetMapCoordinates) == TILESHAPE_EMPTY)
                    {
                        //Face* NewFace = addFace(TargetCubeCoordinates, DirectionType);

                        //NewFace->setFaceMaterialType(CubeMaterial);
                        //NewFace->setFaceSurfaceType(CubeSurface, DirectionType);
                    }
                }
            }
        }

        if (CubeShape > TILESHAPE_EMPTY && CubeShape < TILESHAPE_SOLID)
        {
            Face* NewFace = addFace(FaceCoordinates(TargetCubeCoordinates, DIRECTION_NONE));

            NewFace->setFaceMaterialType(CubeMaterial);
            NewFace->setFaceSurfaceType(CubeSurface);
            NewFace->setShapeType(CubeShape);
        }
    }
    Debug = false;
}

void Cell::BuildStaticGeometry()
{
    char buffer[64];
    sprintf(buffer, "Cell%i-%i-%i",  thisCellCoordinates.X, thisCellCoordinates.Y, thisCellCoordinates.Z);

    Ogre::StaticGeometry* CellGeometry;

    if (!RENDERER->getSceneManager()->hasStaticGeometry(buffer))
    {
        CellGeometry = RENDERER->getSceneManager()->createStaticGeometry(buffer);
    }
    else
    {
        CellGeometry = RENDERER->getSceneManager()->getStaticGeometry(buffer);
        CellGeometry->destroy();
    }

    // Iterate all Faces and RefreshEntites;
    for (std::map<uint16_t, Face*>::iterator it = Faces.begin(); it != Faces.end(); it++)
    {
        it->second->RefreshEntity();
    }


    CellGeometry->addSceneNode(CellSceneNode);
    CellGeometry->setCastShadows(false);

    CellGeometry->build();

    DestroyAllAttachedEntities(CellSceneNode);

    NeedsReBuild = false;
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
    }
}

int16_t Cell::getFaceMaterialType(FaceCoordinates TargetCoordinates)
{
    Face* TargetFace = getFace(TargetCoordinates);
    return TargetFace != NULL ? TargetFace->getFaceMaterialType() : INVALID_INDEX;
}

int16_t Cell::getFaceSurfaceType(FaceCoordinates TargetCoordinates)
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
        Face* NewFace = new Face(CellSceneNode, TargetCoordinates.Coordinates);
        Faces[Key] = NewFace;

        return NewFace;
    }
    else
    {
        return Faces.find(Key)->second;
    }
}

Ogre::Vector3 Cell::getCubePosition(CubeCoordinates Coordinates)
{
    float X = CellSceneNode->getPosition().x - (float)(CELLEDGESIZE / 2) + (float)Coordinates.X + (float)HALFCUBE;
    float Y = CellSceneNode->getPosition().y - (float)(CELLEDGESIZE / 2) + (float)Coordinates.Y + (float)HALFCUBE;

	return Ogre::Vector3(X, Y, CellSceneNode->getPosition().z);
}

MapCoordinates Cell::TranslateCubeToMap(CubeCoordinates Coordinates)
{
    MapCoordinates NewCoordinates;

    NewCoordinates.X = (thisCellCoordinates.X * CELLEDGESIZE) + Coordinates.X;
    NewCoordinates.Y = (thisCellCoordinates.Y * CELLEDGESIZE) + Coordinates.Y;
    NewCoordinates.Z = thisCellCoordinates.Z;

    return NewCoordinates;
}

bool Cell::isCubeSloped(CubeCoordinates Coordinates)
{
    TileShape CubeShape = getCubeShape(Coordinates);
    return (CubeShape > TILESHAPE_EMPTY && CubeShape < TILESHAPE_SOLID);
}

void Cell::DrawCellCage()
{
    //RENDERER->DrawCage(AdjustedPoint, CELLEDGESIZE, CELLEDGESIZE, 1.0, true, 1, 1, 1);
}

int Cell::addActor(Actor* NewActor)
{
    LocalActors.push_back(NewActor);
    return LocalActors.size() - 1;
}

void Cell::removeActor(int Index)
{
    if (LocalActors.size() != 0)
    {
        Actor* LastActor = LocalActors.back();

        if (LastActor != NULL)
        {
            LastActor->setCellActorIndex(Index);
        }
        LocalActors[Index] = LastActor;

        LocalActors.pop_back();
    }
}

