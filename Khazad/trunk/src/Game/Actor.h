#ifndef ACTOR__HEADER
#define ACTOR__HEADER

#include <stdafx.h>
#include <Vector3.h>
#include <Renderer.h>


class Actor
{
public:

	Actor();
	virtual ~Actor();

	virtual bool Update();
	virtual bool Draw(CameraOrientation Orientaion);


    Vector3 getPosition()                              { return RenderPosition; }
    void setPosition(Vector3 NewPosition)              { RenderPosition = NewPosition; }

    MapCoordinates getLocationCoordinates()            { return LocationCoordinates; }
    void setLocation(MapCoordinates NewPosition)       { LocationCoordinates = NewPosition; }


	bool isVisible()                    { return Visible; }
	void setVisible(bool NewValue)      { Visible = NewValue; }

	bool isHidden()                     { return Hidden; }
	void setHidden(bool NewValue)       { Hidden = NewValue; }

protected:

	Vector3 RenderPosition;                 // Used to Render things between Cubes
	MapCoordinates LocationCoordinates;     // The location for gameplay logic purposes

	bool Visible;
    bool Hidden;
};

#endif  // ACTOR__HEADER

