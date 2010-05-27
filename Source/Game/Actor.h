#ifndef ACTOR__HEADER
#define ACTOR__HEADER

#include <stdafx.h>

#include <OgreVector3.h>

class Cell;

#include <Coordinates.h>


class Actor
{
public:

	Actor();
	virtual ~Actor();

	virtual int Update() = 0;

    virtual Ogre::Vector3 getRenderPosition();
	//virtual bool Draw(CameraOrientation Orientaion) = 0;

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

    // TODO Use bitvector in the future
	bool Visible;
    bool Hidden;
};

#endif  // ACTOR__HEADER

