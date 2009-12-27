#ifndef GAME__HEADER
#define GAME__HEADER

#include <stdafx.h>

#include <Singleton.h>

class Actor;
class Pawn;
class MapCoordinates;

class Game
{
DECLARE_SINGLETON_CLASS(Game)

public:

	~Game();
	bool Init();
	bool Run();


	bool RemovePawn(Uint32 ID);
    void SpawnPawn(MapCoordinates SpawnCoordinates);

	std::vector<Pawn*> PawnList;

protected:

};

#define GAME (Game::GetInstance())

#endif // GAME__HEADER
