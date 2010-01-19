#include <stdafx.h>

#include <Furniture.h>
#include <Vector3.h>

Furniture::Furniture()
{
	Visible = false;
	Hidden = false;
}

Furniture::~Furniture()
{

}

bool Furniture::Init()
{
	return true;
}

int Furniture::Update()
{
	return true;
}

bool Furniture::Draw(CameraOrientation Orientation)
{
	return true;
}
