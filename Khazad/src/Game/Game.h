#ifndef GAME__HEADER
#define GAME__HEADER

#include <stdafx.h>
#include <Singleton.h>
#include <Actor.h>

class Actor;

class Game
{
DECLARE_SINGLETON_CLASS(Game)

public:
	~Game();
	bool Init();
	bool Run();

	std::vector<Actor*> ActorList;

	bool RemoveActor(Uint32 ID);

private:



};

#define GAME (Game::GetInstance())

#endif // GAME__HEADER
