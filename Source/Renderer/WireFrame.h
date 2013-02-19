#ifndef WIREFRAME__HEADER
#define WIREFRAME__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <Ogre.h>

class WireFrame
{
public:

	WireFrame(MapCoordinates Location);
	~WireFrame();

	void setColor(float R, float G, float B);
	//void setShape(MapCoordinates Max);
	void setShape(MapCoordinates Min, MapCoordinates Max, bool update = true);
	void draw(bool update);

private:

	MapCoordinates Min;
	MapCoordinates Max;

	Ogre::SceneNode* Node;
	Ogre::ManualObject* ManualObject;
	Ogre::ColourValue Color;
};

#endif // WIREFRAME__HEADER
