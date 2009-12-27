#include <stdafx.h>

#include <Actor.h>

Actor::Actor()
{
    Visible = false; Hidden = false;
};

Actor::~Actor()
{

};

bool Actor::Update()
{
    return true;
};

bool Actor::Draw(CameraOrientation Orientaion)
{
    return true;
};

