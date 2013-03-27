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


#include <Temporal.h>
#include <Game.h>


// TEMPORAL

Temporal::Temporal()
{
    UniqueID = GAME->getTemporal()->getNextUniqueID();
	GAME->getTemporal()->AddTemporal(this, 1);  // Will set CurrentBucket
}

Temporal::~Temporal()
{

}

void Temporal::ResetCooldown(CoolDown NewCoolDown)
{
    // GEt the actor and move to appropriate carosel

	//return true;
}

void Temporal::Retire()
{
    GAME->getTemporal()->RetireTemporal(this);
}

// BUCKET

Bucket::Bucket()
{

}

Bucket::~Bucket()
{

}

void Bucket::add(Temporal* NewTemporal)
{
    TemporalVector.push_back(NewTemporal);
}

// CARROSEL

Carrosel::Carrosel(CoolDown CarroselSize)
{
    BucketCount = CarroselSize;         // Saved for destructor
    Buckets = new Bucket*[CarroselSize];

    for (CoolDown i = 0; i < BucketCount; i++)
    {
        Buckets[i] = new Bucket;
    }
}

Carrosel::~Carrosel()
{
    for (CoolDown i = 0; i < BucketCount; i++)
    {
        delete Buckets[i];
    }
    delete Buckets;
}

// TEMPORAL MANAGER

TemporalManager::TemporalManager()
{
    CurrentGameTick = 0;
    UniqueIDCounter = 0;
}

TemporalManager::~TemporalManager()
{
    //delete CarroselMap;
}

void TemporalManager::UpdateTick()
{
	std::map<CoolDown, Carrosel*>::iterator CarroselIterator;

	for (CarroselIterator = CarroselMap.begin(); CarroselIterator != CarroselMap.end(); CarroselIterator++)
	{
		CoolDown CurrentCoolDown = CarroselIterator->first;        // CoolDown is the size of this Carosel and how long it takes to 'revolve'
		Carrosel* TargetCarosel = CarroselIterator->second;

		Bucket* TargetBucket = TargetCarosel->Buckets[(CurrentGameTick + CurrentCoolDown) % CurrentCoolDown];  // Modulus advances to the next bucket each update
		uint16_t BucketShorteningCounter = 0;

		for (uint32_t i = 0; i < TargetBucket->TemporalVector.size(); i++)
		{
		    if (TargetBucket->TemporalVector[i] != NULL)  // should Null be allowed?
		    {
                CoolDown DesiredCoolDown = TargetBucket->TemporalVector[i]->Update();  // The amount of Cooldown the Temporal desires untill its next update

                if (DesiredCoolDown != CurrentCoolDown)  // ReIndex the Temporal
                {
                    ReIndexedTemporalBuffer.push_back(TargetBucket->TemporalVector[i]);
                    ReIndexedTemporalCoolDown.push_back(DesiredCoolDown);

                    TargetBucket->TemporalVector[i] = TargetBucket->TemporalVector.back();  i--; //  Move Last Object into the current slot and remain on it for lext loop
                    TargetBucket->TemporalVector.back() = NULL;
                    BucketShorteningCounter++;
                }
		    }
            else
		    {
                //BucketShorteningCounter++; // No Nulls should slip through
		    }
		}
		TargetBucket->TemporalVector.resize(TargetBucket->TemporalVector.size() - BucketShorteningCounter);  //Shrink the Bucket to save space
	}


    // Extract and Destroy Retired



	// Insert New and Changed
	for (int i = 0; i < ReIndexedTemporalBuffer.size(); i++)
	{
		AddTemporal(ReIndexedTemporalBuffer[i], ReIndexedTemporalCoolDown[i]);
	}

	// Clear the lists now that all Reinsertions are done
	ReIndexedTemporalBuffer.clear();
	ReIndexedTemporalCoolDown.clear();


	CurrentGameTick++;   // Advance Tick count
}

Carrosel* TemporalManager::getCarosel(CoolDown DesiredCoolDown)
{
    std::map<CoolDown, Carrosel*>::iterator CarroselIterator = CarroselMap.find(DesiredCoolDown);

	if (CarroselIterator != CarroselMap.end())
	{
		return CarroselIterator->second;
	}
	else   // Create and Insert a new Carosel
	{
		Carrosel* NewCarosel = new Carrosel(DesiredCoolDown);
		CarroselMap[DesiredCoolDown] = NewCarosel;
		return NewCarosel;
	}
}

bool TemporalManager::AddTemporal(Temporal* NewTemporal, CoolDown NewCoolDown)
{
    if (NewCoolDown < 1)
    {
        NewCoolDown = 1;
    }
	Carrosel* TargetCarosel = getCarosel(NewCoolDown);
	CoolDown Bucket = ((CurrentGameTick + NewCoolDown) % NewCoolDown);   // Ensures small Tick count dose not produce false modulus

    TargetCarosel->Buckets[Bucket]->TemporalVector.push_back(NewTemporal);
    NewTemporal->CurrentBucket = TargetCarosel->Buckets[Bucket];

    return true;
}

bool TemporalManager::RetireTemporal(Temporal* TargetTemporal)
{
    Bucket* TargetBucket = TargetTemporal->CurrentBucket;

    //for
	// TODP, retreave and remove pawn
	return false;
}
