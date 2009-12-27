#include <stdafx.h>

#include <Furniture.h>


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

bool Furniture::Update()
{
	return true;
}

bool Furniture::Draw(CameraOrientation Orientation)
{
	return true;
}
