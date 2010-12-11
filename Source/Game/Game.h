#ifndef GAME__HEADER
#define GAME__HEADER

#include <stdafx.h>

#include <Singleton.h>

class Actor;
class Pawn;
class Tree;
class Map;
class PathManager;
class Geology;

class MapCoordinates;

class Game
{
DECLARE_SINGLETON_CLASS(Game)

public:

	~Game();
	bool Init();
	bool Run();

    Pawn* SpawnPawn(MapCoordinates SpawnCoordinates);
    Tree* SpawnTree(MapCoordinates SpawnCoordinates);

    void changeTickRate(int32_t RateChange);
    void setTickRate(uint32_t NewRate);

    uint32_t getTickRate()       { return TickRate; }

    void togglePause()              { Pause = !Pause; }
    void setPause(bool NewState)    { Pause = NewState; }

    Map* getMap()               { return MainMap; }
    Geology* getGeology()       { return MapGeology; }
    PathManager* getPath()      { return Path; }

protected:

    uint32_t TickRate;     // Simulation Rate;
    bool Pause;

    Map* MainMap;
    Geology* MapGeology;

    PathManager* Path;
};

#define GAME (Game::GetInstance())

#endif // GAME__HEADER
