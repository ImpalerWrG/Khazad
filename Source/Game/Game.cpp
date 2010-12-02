#include <Game.h>

#include <Temporal.h>
#include <Pawn.h>
#include <Tree.h>
#include <Geology.h>
#include <PathManager.h>


DECLARE_SINGLETON(Game)

bool Game::Init()
{
    Geology* MapGeology = new Geology();

	MainMap = new Map();
	MainMap->Init();
	MainMap->Generate(MapGeology);

	Path = new PathManager();
	Path->Init();
	Path->CreateMapAbstraction(MainMap);
	Path->InitializeTestingSuite();

	return true;
}

Game::Game()
{
	TickRate = 10;

	Pause = false;
}

Game::~Game()
{

}

bool Game::Run()
{
	if (!Pause)
	{
		for(uint32_t i = TickRate; i > 0; i--)
		{
			TEMPORAL->UpdateTick();

			// Update map? other none actor based logics?
		}
	}

	return true;
}

Pawn* Game::SpawnPawn(MapCoordinates SpawnCoordinates)
{
	Pawn* NewPawn = new Pawn();

	NewPawn->Init(SpawnCoordinates);

	return NewPawn;
}

Tree* Game::SpawnTree(MapCoordinates SpawnCoordinates)
{
	Tree* NewTree = new Tree();

	NewTree->Init(DATA->getLabelIndex("TREE_OAK"), SpawnCoordinates, true);

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
