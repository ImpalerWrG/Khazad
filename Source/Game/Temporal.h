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


/*------------------------TEMPORAL---------------------------

Temporal encapsulates the ability to be updated each game time-ticks,
and is intended as a virtual base class for game objects.

A Singlton TemporalManager tracks time ticks and should be called in the
main logic loop as many times as nessary to create the desidred 'speed',
Each UpdateTick call will advance one Tick and update the apropriate
Entities.

Only The Manager can call updates to ensure that each entity
can recive at most one update per tick. The Temporal constructor will register
it automatically with the Manager and the destructor is callable only
from the TemporalManager, use Retire to mark Entities for deletion by the Manager.
A retired Temporal will be deleted at the end of the next update not
durring the update cycle to prevent a 'shoots first' paradox.  Likewise
new Entities will recive their first update durring the next full Tick,
Entities created durring a Tick will not update on that Tick.  New Entites
are update before Retired ones are deleted so Initializing from a retired
parent Temporal is valid.

Entities require an Update function which returns the number of Ticks
untill the next desired Update called the 'Cooldown', the Temporal will need
to store  or determine the purpose of that update internally. Entities
may also change their desired Cooldown at any time but this will take effect
on the next tick and won't prevent a previously scedualed update if the
Update already is in progress.

The TemporalManager uses a system of Vectors and Arrays to efficiently update
only the desired Entities that have reached their cooldown time without
iterating over all Entites and testing for elapsed time.  The time required
to process a Tick will incresse linearly with the number of Entities being
updated on the Tick along with a second small linear incresse from the number of
different Cooldown times returned.  Lastly their is a slight overhead when a
Temporal dose not return a consistent CoolDown as it much be reindexed.  Best
performance will come from Entites that have long and consistent Cooldowns. */

// TODO 'Rank' allowing limited control of update order

#ifndef TEMPORAL__HEADER
#define TEMPORAL__HEADER

#include <stdafx.h>

#include <Singleton.h>

typedef uint16_t CoolDown; // Determines the maximum value of a cooldown
typedef uint32_t Tick;     // Determines the maximum game time ticks

class Temporal;
class Bucket;
class Carrosel;
class TemporalManager;


class Temporal
{
friend class TemporalManager;

public:

	Temporal();

    void ResetCooldown(CoolDown NewCoolDown);
    void Retire();  // Will be destructed

protected:

	~Temporal();   // Destructed only by the Temporal Manager after Retiring

	virtual CoolDown Update() = 0;
	Bucket* CurrentBucket;
};

class Bucket  // An unsorted group of Entities that will be updated together
{
friend class Carrosel;

    Bucket();
    ~Bucket();

    void add(Temporal* NewTemporal);


public:

    std::vector<Temporal*> TemporalVector;
};

class Carrosel  // A group of Buckets holding unsyncronized entities with the same CoolDown time
{
friend class TemporalManager;

public:

    Carrosel(CoolDown CarroselSize);
    ~Carrosel();

    Bucket** Buckets;
    CoolDown BucketCount; // ? nessary
};

class TemporalManager
{
DECLARE_SINGLETON_CLASS(TemporalManager)

friend class Temporal;

public:

    ~TemporalManager();

    void UpdateTick();
    Tick getCurrentTimeTick()   { return CurrentGameTick; }
    bool isTickInProgress();

protected:

    // Called only from the Temporal constructor and destructors
    bool AddTemporal(Temporal* NewTemporal, CoolDown NewCoolDown);
    bool RetireTemporal(Temporal* TargetTemporal);

    Carrosel* getCarosel(CoolDown TargetCoolDown);

    Tick CurrentGameTick;
    std::map<CoolDown, Carrosel*> CarroselMap;

    std::vector<Temporal*> ReIndexedTemporalBuffer;
    std::vector<CoolDown> ReIndexedTemporalCoolDown;

    std::vector<Temporal*> RetiredTemporalBuffer;

};

#define TEMPORAL (TemporalManager::GetInstance())

#endif  // TEMPORAL__HEADER
