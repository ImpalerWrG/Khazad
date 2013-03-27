#include <Actor.h>

#include <Cell.h>
#include <Game.h>
#include <Map.h>

#include <TextureManager.h>
#include <DataManager.h>
#include <Renderer.h>

Actor::Actor()
{
    Visible = false;
    Hidden = false;
}

Actor::~Actor()
{

}

bool Actor::Init(MapCoordinates SpawnLocation, Ogre::MaterialPtr Mat, float Width, float Height)
{
    ActorBillboard = RENDERER->getSceneManager()->createBillboardSet(1);

    ActorBillboard->setDefaultDimensions(Width, Height);
    ActorBillboard->createBillboard(0, 0, ((Height - 1.0) / 2));

    ActorBillboard->setMaterialName(Mat->getName());

	ActorNode = RENDERER->getSceneManager()->createSceneNode();
    ActorNode->attachObject(ActorBillboard);
    ActorNode->setVisible(false);

    setLocation(SpawnLocation);

    return true;
}

void Actor::setLocation(MapCoordinates NewPosition)
{
	CellCoordinates OldCoordinates = CellCoordinates(LocationCoordinates);
    LocationCoordinates = NewPosition;

    CellCoordinates NewCellCoords = CellCoordinates(NewPosition);
    Cell* NewCell = GAME->getMap()->getCell(NewCellCoords);
    CubeCoordinates Cube = NewPosition.Cube();

    float X = (float) (Cube >> CELLBITSHIFT) + (float)HALFCUBE - (float)(CELLEDGESIZE / 2);
    float Y = (float) (Cube & CELLBITFLAG) + (float)HALFCUBE - (float)(CELLEDGESIZE / 2);

    if (OldCoordinates != NewCell->getCellCoordinates())
    {
		Cell* OldCell = GAME->getMap()->getCell(OldCoordinates);
		if (OldCell != NULL)
		{
			OldCell->removeActor(this);
		}

        if (NewCell != NULL)
        {
            NewCell->addActor(this);
        }
    }
    ActorNode->setPosition(X, Y, 0);
}

void Actor::setRenderPosition(Ogre::Vector3 NewPosition)
{
    ActorNode->setPosition(NewPosition);
}

void Actor::MoveRenderPosition(Ogre::Vector3 Translation)
{
    ActorNode->translate(Translation);
}

void Actor::setVisible(bool NewValue)
{
    Visible = NewValue; ActorNode->setVisible(NewValue);
}
