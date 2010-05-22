#ifndef GAME__HEADER
#define GAME__HEADER

#include <stdafx.h>

#include <Singleton.h>

class Actor;
class Pawn;
class Tree;
class Map;

class MapCoordinates;

class Game
{
DECLARE_SINGLETON_CLASS(Game)

public:

	~Game();
	bool Init();
	bool Run();


	bool RemovePawn(uint32_t ID);
    Pawn* SpawnPawn(MapCoordinates SpawnCoordinates);
    Tree* SpawnTree(MapCoordinates SpawnCoordinates, int16_t TreeType, bool isAlive);

	//std::vector<Pawn*> PawnList;

	bool setActorCooldown(Actor* TargetActor, int CoolDown);
    std::vector<Actor*>* getCarosel(int CoolDown);
    bool UpdateActors();
    bool AddActor(Actor* NewActor, int CoolDown);

    void changeTickRate(int32_t RateChange);
    void setTickRate(uint32_t NewRate);

    uint32_t getTickCount()      { return TickCounter; }
    uint32_t getTickRate()       { return TickRate; }

    void togglePause()              { Pause = !Pause; }
    void setPause(bool NewState)    { Pause = NewState; }

    Map* getMap()               { return MainMap; }

protected:

    uint32_t TickCounter;  // Simulation time units
    uint32_t TickRate;     // Simulation Rate;
    bool Pause;

    std::map< uint32_t, std::vector< Actor* >* > ActorUpdateGroups;

    std::vector<Actor*> ReIndexedActorBuffer;
    std::vector<int> ReIndexedActorCoolDown;

    Map* MainMap;
};

#define GAME (Game::GetInstance())

#endif // GAME__HEADER
