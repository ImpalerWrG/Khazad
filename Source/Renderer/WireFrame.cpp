#include <WireFrame.h>
#include <TileShapes.h>
#include <Coordinates.h>

#include <Renderer.h>

WireFrame::WireFrame(MapCoordinates Location)
{
	Color = Ogre::ColourValue::White;
	Min = Max = Location;

	Node = RENDERER->getRootNode()->createChildSceneNode();
	ManualObject = RENDERER->getSceneManager()->createManualObject();

    ManualObject->setDynamic(true);
    ManualObject->setCastShadows(false);

	Node->attachObject(ManualObject);
	setShape(Min, Max, false);
}

WireFrame::~WireFrame()
{
	RENDERER->getSceneManager()->destroyManualObject(ManualObject);
	RENDERER->getSceneManager()->destroySceneNode(Node);
}

void WireFrame::setColor(float R, float G, float B)
{
	Color.r = R;
	Color.g = G;
	Color.b = B;
	Color.a = 1;
}

void WireFrame::setShape(MapCoordinates NewMin, MapCoordinates NewMax, bool update)
{
	Min.X = min(NewMin.X, NewMax.X);
	Min.Y = min(NewMin.Y, NewMax.Y);
	Min.Z = min(NewMin.Z, NewMax.Z);

	Max.X = max(NewMin.X, NewMax.X) + 1;
	Max.Y = max(NewMin.Y, NewMax.Y) + 1;
	Max.Z = max(NewMin.Z, NewMax.Z) + 1;

	Node->setPosition(Min.X - HALFCUBE, Min.Y - HALFCUBE, Min.Z - HALFCUBE);

	draw(update);
}

void WireFrame::draw(bool Update)
{
    if (Update)
    {
        ManualObject->beginUpdate(0);
    } else {
        ManualObject->begin("BaseWhiteNoLighting",Ogre::RenderOperation::OT_LINE_LIST);
    }

    {
        // TOP
        ManualObject->position(Max.X - Min.X, Max.Y - Min.Y, Max.Z - Min.Z);
        ManualObject->colour(Color);

        ManualObject->position(Max.X - Min.X, 0, Max.Z - Min.Z);
        ManualObject->colour(Color);

        ManualObject->position(0, 0, Max.Z - Min.Z);
        ManualObject->colour(Color);

        ManualObject->position(0, Max.Y - Min.Y, Max.Z - Min.Z);
        ManualObject->colour(Color);

        // BOTTOM
        ManualObject->position(Max.X - Min.X, Max.Y - Min.Y, 0);
        ManualObject->colour(Color);

        ManualObject->position(Max.X - Min.X, 0, 0);
        ManualObject->colour(Color);

        ManualObject->position(0, 0, 0);
        ManualObject->colour(Color);

        ManualObject->position(0, Max.Y - Min.Y, 0);
        ManualObject->colour(Color);

        // TOP
        ManualObject->index(0);
        ManualObject->index(1);

        ManualObject->index(1);
        ManualObject->index(2);

        ManualObject->index(2);
        ManualObject->index(3);

        ManualObject->index(3);
        ManualObject->index(0);

        // BOTTOM
        ManualObject->index(4);
        ManualObject->index(5);

        ManualObject->index(5);
        ManualObject->index(6);

        ManualObject->index(6);
        ManualObject->index(7);

        ManualObject->index(7);
        ManualObject->index(4);

        // SIDES
        ManualObject->index(0);
        ManualObject->index(4);

        ManualObject->index(1);
        ManualObject->index(5);

        ManualObject->index(2);
        ManualObject->index(6);

        ManualObject->index(3);
        ManualObject->index(7);
    }

    ManualObject->end();
}
