#ifndef ACTOR__HEADER
#define ACTOR__HEADER

#include <stdafx.h>
#include <Vector3.h>
#include <Renderer.h>
#include <Cell.h>


class Actor
{
public:

	Actor();
	virtual ~Actor();

	virtual int Update() = 0;
	virtual bool Draw(CameraOrientation Orientaion) = 0;


    Vector3 getPosition()                              { return RenderPosition; }
    void setPosition(Vector3 NewPosition)              { RenderPosition = NewPosition; }

    MapCoordinates getLocationCoordinates()            { return LocationCoordinates; }
    void setLocation(MapCoordinates NewPosition);


	bool isVisible()                    { return Visible; }
	void setVisible(bool NewValue)      { Visible = NewValue; }

	bool isHidden()                     { return Hidden; }
	void setHidden(bool NewValue)       { Hidden = NewValue; }

    void setCellActorIndex(int NewValue)    { CellActorIndex = NewValue; }

protected:

	Vector3 RenderPosition;                 // Used to Render things between Cubes
	MapCoordinates LocationCoordinates;     // The location for gameplay logic purposes
	CellCoordinates CurrentCellCoordinates; // Cell that this Actor is currently in
	CubeCoordinates CurrentCubeCoordinates; // CubeLocation within the Cell
    int CellActorIndex;

	bool Visible;
    bool Hidden;
};

#endif  // ACTOR__HEADER

