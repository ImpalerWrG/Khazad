#include <Furniture.h>

#include <OgreVector3.h>

Furniture::Furniture()
{
	Visible = false;
	Hidden = false;
}

Furniture::~Furniture()
{

}

bool Furniture::InitializeFurniture()
{
	return true;
}

CoolDown Furniture::Update()
{
	return true;
}

/*
bool Furniture::Draw(CameraOrientation Orientation)
{
	return true;
}
*/
