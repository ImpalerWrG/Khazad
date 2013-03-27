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


/*------------------------CITIZEN---------------------------

Citizen acts as a base class for the workers/peons/minions/pets
that are considered a part of the players settlment and are thus
friendly, able to perform work and have their history, relationships,
and other in-depth statistics tracked.

Citizen derives from Pawn as all citizens are mobile sentient
creatures. */

#ifndef CITIZEN_HEADER
#define CITIZEN_HEADER

#include <stdafx.h>

#include <Pawn.h>
#include <Job.h>

class Citizen : public Pawn
{

public:

    Citizen();
    ~Citizen();

    bool InitializeCitizen();

    void ChangeJob(Job* NewJob);

protected:

    Job* CurrentJob;
    Task* CurrentTask;

    friend class Job;
};

#endif // CITIZEN_HEADER
