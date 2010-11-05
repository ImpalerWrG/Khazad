#ifndef ACTOR__HEADER
#define ACTOR__HEADER

#include <stdafx.h>

#include <OgreVector3.h>

class Cell;

#include <Coordinates.h>
#include <Temporal.h>


class Actor: virtual public Temporal
{
public:

	Actor();
    ~Actor();

    bool Init(MapCoordinates SpawnLocation);

	virtual CoolDown Update() = 0;

    virtual Ogre::Vector3 getRenderPosition();

    MapCoordinates getLocationCoordinates()            { return LocationCoordinates; }
    void setLocation(MapCoordinates NewPosition);


	bool isVisible()                    { return Visible; }
	void setVisible(bool NewValue)      { Visible = NewValue; }

	bool isHidden()                     { return Hidden; }
	void setHidden(bool NewValue)       { Hidden = NewValue; }

    void setCellActorIndex(int NewValue)    { CellActorIndex = NewValue; }

protected:

	MapCoordinates LocationCoordinates;     // The location for gameplay logic purposes

    Cell* CurrentCell;
	//CellCoordinates CurrentCellCoordinates; // Cell that this Actor is currently in
	CubeCoordinates CurrentCubeCoordinates; // CubeLocation within the Cell

    int CellActorIndex;

    Ogre::SceneNode* ActorNode;
    Ogre::BillboardSet* ActorBillboard;

    // TODO Use bitvector in the future
	bool Visible;
    bool Hidden;
};

#endif  // ACTOR__HEADER
