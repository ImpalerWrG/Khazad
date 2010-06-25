#include <Game.h>

#include <Actor.h>
#include <Pawn.h>
#include <Tree.h>
#include <Geology.h>


DECLARE_SINGLETON(Game)

bool Game::Init()
{
    Geology* MapGeology = new Geology();

	MainMap = new Map();
	MainMap->Init();
	MainMap->Generate(MapGeology);

	return true;
}

Game::Game()
{
	TickCounter = 0;
	TickRate = 10;

	Pause = false;
}

Game::~Game()
{

}

bool Game::RemovePawn(uint32_t ID)
{
	// TODP, retreave and remove pawn
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

	return true;
}

bool Game::UpdateActors()
{
	// Insert Buffered Actors
	for (int i = 0; i < ReIndexedActorBuffer.size(); i++)
	{
		AddActor(ReIndexedActorBuffer[i], ReIndexedActorCoolDown[i]);
	}

	// Clear the list now that all Actors are inserted
	ReIndexedActorBuffer.clear();
	ReIndexedActorCoolDown.clear();

	std::map<uint32_t, std::vector<Actor*>*>::iterator CoolDownGroupIterator;

	for (CoolDownGroupIterator = ActorUpdateGroups.begin(); CoolDownGroupIterator != ActorUpdateGroups.end(); CoolDownGroupIterator++)
	{
		int CoolDown = CoolDownGroupIterator->first;
		std::vector<Actor*>* TargetCarosel = CoolDownGroupIterator->second;
		std::vector<Actor*>* TargetBucket = &TargetCarosel[TickCounter % CoolDown];

		for (int i = 0; i < TargetBucket->size(); i++)
		{
			int ActorDesiredCoolDown = (*TargetBucket)[i]->Update();

			if (ActorDesiredCoolDown != CoolDown)
			{
				ReIndexedActorBuffer.push_back((*TargetBucket)[i]);
				ReIndexedActorCoolDown.push_back(ActorDesiredCoolDown);

				// Grab the last Actor, update it and replace the moved actor
				if (TargetBucket->size() > 1)
				{
					((*TargetBucket).back())->Update();
					(*TargetBucket)[i] = (*TargetBucket).back();
				}
				(*TargetBucket).pop_back();
			}
		}
	}
	return true;
}

std::vector<Actor*>* Game::getCarosel(int CoolDown)
{
	std::map<uint32_t, std::vector<Actor*>*>::iterator it = ActorUpdateGroups.find(CoolDown);
	//it = ActorUpdateGroups.find(CoolDown);

	if (it != ActorUpdateGroups.end())
	{
		return it->second;
	}
	else
	{
		// Create and Insert a new Carosel
		std::vector<Actor*>* NewCarosel = new std::vector<Actor*>[CoolDown];
		//NewCarosel->reserve(1000);
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
	return true;
}

Pawn* Game::SpawnPawn(MapCoordinates SpawnCoordinates)
{
	Pawn* NewPawn = new Pawn();
	NewPawn->Init(SpawnCoordinates);
	AddActor(NewPawn, 1);

	return NewPawn;
}

Tree* Game::SpawnTree(MapCoordinates SpawnCoordinates, int16_t TreeType, bool isAlive)
{
	Tree* NewTree = new Tree(TreeType, SpawnCoordinates, isAlive);
	//NewTree->Init(SpawnCoordinates);
	AddActor(NewTree, 1);
	return NewTree;
}

void Game::changeTickRate(int32_t RateChange)
{
	TickRate += RateChange;

	if (TickRate < 0)
	{
		TickRate = 0;
	}
}

void Game::setTickRate(uint32_t NewRate)
{
	TickRate = NewRate;
}
