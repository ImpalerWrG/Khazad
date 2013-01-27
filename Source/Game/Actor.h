#ifndef ACTOR__HEADER
#define ACTOR__HEADER

#include <stdafx.h>


class Cell;

#include <Coordinates.h>
#include <Temporal.h>


class Actor: virtual public Temporal
{
public:

	Actor();
    ~Actor();

    bool Init(MapCoordinates SpawnLocation, Ogre::MaterialPtr Mat, float Width, float Height);

	virtual CoolDown Update() = 0;

    void setRenderPosition(Ogre::Vector3 NewPosition);
    void MoveRenderPosition(Ogre::Vector3 Translation);

    MapCoordinates getLocationCoordinates()            { return LocationCoordinates; }
    void setLocation(MapCoordinates NewPosition);

    Ogre::SceneNode* getNode()          { return ActorNode; }

	bool isVisible()                    { return Visible; }
	void setVisible(bool NewValue);

	bool isHidden()                     { return Hidden; }
	void setHidden(bool NewValue)       { Hidden = NewValue; }

    void setCellActorIndex(int NewValue)    { CellActorIndex = NewValue; }

protected:

	MapCoordinates LocationCoordinates;     // The location for gameplay logic purposes

    Cell* CurrentCell;

    int CellActorIndex;

    Ogre::SceneNode* ActorNode;
    Ogre::BillboardSet* ActorBillboard;

    // TODO Use bitvector in the future
	bool Visible;
    bool Hidden;
};

#endif  // ACTOR__HEADER
