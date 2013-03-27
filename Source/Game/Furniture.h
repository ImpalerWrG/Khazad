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


/*------------------------FURNITURE---------------------------

Furniture are stationary inanimate objects that act as work-sites
for a number of activities.  */


#ifndef FURNITURE__HEADER
#define FURNITURE__HEADER

#include <stdafx.h>

#include <Actor.h>

class Furniture: Actor
{

public:

	Furniture();

	bool InitializeFurniture();
	virtual ~Furniture();

	CoolDown Update();

    int16_t getFurnitureType()                   { return FurnitureType; }
    void setFurnitureType(int16_t NewType)       { FurnitureType = NewType; }

    int16_t getMaterialType()                    { return MaterialType; }
    void setMaterialType(int16_t NewType)        { MaterialType = NewType; }

    int16_t getModelType()                       { return ModelType; }
    void setModelType(int16_t NewType)           { ModelType = NewType; }

protected:

    int16_t FurnitureType;

    int16_t MaterialType;

    int16_t ModelType;
};

#endif  // FURNITURE__HEADER

