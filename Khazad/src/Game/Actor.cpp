#include <stdafx.h>

#include <Actor.h>
#include <Timer.h>

Actor::Actor(ActorType NewType)
{
	Visible = false;
	DirtyDrawlist = true;
	Type = NewType;
	Dead = false;

	BirthDate = 0;
}

Actor::Actor()
{
	Visible = false;
	DirtyDrawlist = true;
}

Actor::~Actor()
{

}

ActorType Actor::getType()
{
	return Type;
}

void Actor::setType(ActorType NewType)
{
	Type = NewType;
}

bool Actor::Init()
{
	return true;
}

bool Actor::Update()
{
	return true;
}

bool Actor::Draw()
{
	return true;
}
