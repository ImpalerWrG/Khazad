#include <stdafx.h>

#include <Game.h>

#include <Actor.h>
#include <Pawn.h>


DECLARE_SINGLETON(Game)

bool Game::Init()
{
	return true;
}

Game::Game()
{
    TickCounter = 0;
    TickRate = 1;
    Pause = false;
}

Game::~Game()
{

}

bool Game::RemovePawn(Uint32 ID)
{

    return false;
}

bool Game::Run()
{
    if (!Pause)
    {
        for(uint32_t i = TickRate; i > 0; i--)
        {
            TickCounter++;

            //Clean out dead Actors?
            UpdateActors();

            // Update map? other none actor based logics?
        }
    }

	return true;
}

bool Game::setActorCooldown(Actor* TargetActor, int CoolDown)
{

}

bool Game::UpdateActors()
{
    // Insert Buffered Actors
    for (int i = 0; i < ActorBuffer.size(); i++)
    {
        AddActor(ActorBuffer[i].first, ActorBuffer[i].second);
    }
    ActorBuffer.clear(); // Clear the list now that all Actors are inserted


    std::map<uint32_t, std::vector<Actor*>*>::iterator CoolDownGroupIterator;

    for (CoolDownGroupIterator = ActorUpdateGroups.begin(); CoolDownGroupIterator != ActorUpdateGroups.end(); CoolDownGroupIterator++)
    {
        int CoolDown = CoolDownGroupIterator->first;
        std::vector<Actor*>* TargetCarosel = CoolDownGroupIterator->second;
        std::vector<Actor*>* TargetBucket = &TargetCarosel[TickCounter % CoolDown];

        for (std::vector<Actor*>::iterator BucketIterator = TargetBucket->begin(); BucketIterator != TargetBucket->end(); BucketIterator++)
        {
            int ActorDesiredCoolDown = (*BucketIterator)->Update();

            if (ActorDesiredCoolDown != CoolDown)
            {
                std::pair<Actor*, int> MovingActor = make_pair(*BucketIterator, ActorDesiredCoolDown);
                ActorBuffer.push_back(MovingActor);

                // Move last element into vacant spot
                *BucketIterator = *TargetBucket->end();
                TargetBucket->pop_back();
            }
        }
    }
}

std::vector<Actor*>* Game::getCarosel(int CoolDown)
{
    std::map<uint32_t, std::vector<Actor*>*>::iterator it;
    it = ActorUpdateGroups.find(CoolDown);

    if (it != ActorUpdateGroups.end())
    {
        return it->second;
    }
    else
    {
        // Create and Insert a new Carosel
        std::vector<Actor*>* NewCarosel = new std::vector<Actor*>[CoolDown];
        ActorUpdateGroups[CoolDown] = NewCarosel;
        return NewCarosel;
    }
}

bool Game::AddActor(Actor* NewActor, int CoolDown)
{
    // Add actor to the
    std::vector<Actor*>* TargetCarosel = getCarosel(CoolDown);
    int Bucket = (TickCounter % CoolDown);

    if (Bucket > 0)
    {
        TargetCarosel[Bucket - 1].push_back(NewActor);
    }
    else
    {
        TargetCarosel[0].push_back(NewActor);
    }
}

void Game::SpawnPawn(MapCoordinates SpawnCoordinates)
{
    Pawn* NewPawn = new Pawn();
    NewPawn->Init(SpawnCoordinates);
    AddActor(NewPawn, 1);
}

void Game::changeTickRate(int32_t RateChange)
{
    TickRate += RateChange;

    if (TickRate < 0)
    {
        TickRate = 0;
    }
}
