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


/*-------------------------SETTLMENT----------------------------

Settlment is the embodyment of the players growing comunity, it
holds lists of the settlments members (Citizens), the Zones they
live and work in and the Jobs they are assigned to perform, and
overall statistics about the health of the Community overall.  */

#ifndef SETTLMENT_HEADER
#define SETTLMENT_HEADER

#include <stdafx.h>

#include <DataManager.h>
#include <Citizen.h>
#include <Coordinates.h>

class Settlment
{
public:
	Settlment();
	virtual ~Settlment();
	void InitializeSettlment();

protected:

	std::vector<Citizen*> SettlmentMembers;

	void addMember(Citizen* NewMember);
	void removeMember(Citizen* DepartingMember);

private:

};

#endif // SETTLMENT_HEADER
