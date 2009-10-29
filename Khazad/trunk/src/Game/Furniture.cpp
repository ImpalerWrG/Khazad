#include <stdafx.h>

#include <Furniture.h>

Furniture::Furniture(ActorType NewType)
{
	Visible = false;
	Type = NewType;

	//BirthDate = 0;
}

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
