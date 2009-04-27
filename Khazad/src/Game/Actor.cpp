#include <stdafx.h>

#include <Actor.h>
#include <Timer.h>

Actor::Actor(ActorType NewType)
{
	Visible = false;
	DirtyDrawlist = true;
	Type = NewType;
	Dead = false;

	//BirthDate = 0;
}

Actor::Actor()
{
	Visible = false;
	Hidden = false;
	DirtyDrawlist = true;
}

Actor::~Actor()
{

}

bool Actor::Init()
{
	return true;
}

bool Actor::Update()
{
	return true;
}

bool Actor::Draw(Direction CameraDirection)
{
	return true;
}
