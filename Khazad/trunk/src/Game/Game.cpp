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
    //Clean out dead Pawns?

	for (Uint32 i = 0; i < PawnList.size(); i++)
	{
	    if (PawnList[i] != NULL)
	    {
            PawnList[i]->Update();
	    }
	}
	return true;
}

void Game::SpawnPawn(MapCoordinates SpawnCoordinates)
{
    Pawn* NewPawn = new Pawn();
    NewPawn->Init(SpawnCoordinates);
    PawnList.push_back(NewPawn);
}
