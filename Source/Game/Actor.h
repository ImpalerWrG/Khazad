/* Copyright 2010 Kenneth Ferland

This file is part of Khazad.

Khazad is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Khazad is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Khazad.  If not, see <http://www.gnu.org/licenses/> */


/*------------------------ACTOR---------------------------

Actor is a virtual baseclass for any game object which a physical
location in the game environment, it inherits Temporal as all
objects that exist in Space are assumed to exist in Time as well.*/

#ifndef ACTOR__HEADER
#define ACTOR__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <Temporal.h>

#include <Ogre.h>

class Actor: virtual public Temporal
{
public:

	Actor();
	~Actor();

	bool Init(MapCoordinates SpawnLocation, Ogre::MaterialPtr Mat, float Width, float Height);

	virtual CoolDown Update() = 0;

	void setRenderPosition(Ogre::Vector3 NewPosition);
	void MoveRenderPosition(Ogre::Vector3 Translation);

	MapCoordinates getLocationCoordinates()				{ return LocationCoordinates; }
	void setLocation(MapCoordinates NewPosition);

	Ogre::SceneNode* getNode()			{ return ActorNode; }

	bool isVisible()					{ return Visible; }
	void setVisible(bool NewValue);

	bool isHidden()						{ return Hidden; }
	void setHidden(bool NewValue)		{ Hidden = NewValue; }

protected:

	// The location for gameplay logic purposes
	MapCoordinates LocationCoordinates;

	Ogre::SceneNode* ActorNode;
	Ogre::BillboardSet* ActorBillboard;

	// TODO Use bitvector in the future
	bool Visible;
	bool Hidden;
};

#endif  // ACTOR__HEADER
