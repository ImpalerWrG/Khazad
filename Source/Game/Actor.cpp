#include <Actor.h>

#include <Cell.h>
#include <Game.h>

#include <TextureManager.h>
#include <DataManager.h>

Actor::Actor()
{
    Visible = false;
    Hidden = false;
    CurrentCell = NULL;
}

Actor::~Actor()
{

}

bool Actor::Init(MapCoordinates SpawnLocation)
{
    setLocation(SpawnLocation);

    ActorBillboard = RENDERER->getSceneManager()->createBillboardSet(1);
    ActorBillboard->createBillboard(LocationCoordinates.X, LocationCoordinates.Y, LocationCoordinates.Z);
    ActorBillboard->setDefaultDimensions(1.0, 1.0);


    Ogre::MaterialPtr Mat = TEXTURE->getOgreMaterial(DATA->getLabelIndex("MATERIAL_DWARF"), DATA->getLabelIndex("SURFACETYPE_ROUGH_FLOOR_2"));
    ActorBillboard->setMaterialName(Mat->getName());

	Ogre::SceneNode* PawnNode = RENDERER->getSceneManager()->getRootSceneNode()->createChildSceneNode(); //"ActorNode1");
    PawnNode->attachObject(ActorBillboard);

    return true;
}

void Actor::setLocation(MapCoordinates NewPosition)
{
    LocationCoordinates = NewPosition;
    CurrentCubeCoordinates = CubeCoordinates(NewPosition);

    CellCoordinates NewCellCoords = CellCoordinates(NewPosition);
    Cell* NewCell = GAME->getMap()->getCell(NewCellCoords);

    if (CurrentCell != NewCell)
    {
        if (CurrentCell != NULL)
        {
            CurrentCell->removeActor(CellActorIndex);
        }

        if (NewCell != NULL)
        {
            CellActorIndex = NewCell->addActor(this);
            CurrentCell = NewCell;
        }
    }
}

Ogre::Vector3 Actor::getRenderPosition()
{
    return Ogre::Vector3(LocationCoordinates.X, LocationCoordinates.Y, LocationCoordinates.Z);
}

