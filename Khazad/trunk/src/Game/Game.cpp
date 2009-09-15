#include <stdafx.h>

#include <Game.h>
#include <Singleton.h>
#include <Pawn.h>
#include <Vector3.h>
#include <TextureManager.h>


DECLARE_SINGLETON(Game)

bool Game::Init()
{
    /*
    Pawn* Digger1 = new Pawn;

    Vector3 Origin1 = Vector3(10, 10, 0);
    Digger1->Init(Origin1);

    Pawn* Digger2 = new Pawn;

    Vector3 Origin2 = Vector3(30, 30, 0);
    Digger2->Init(Origin2);

    Pawn* Digger3 = new Pawn;

    Vector3 Origin3 = Vector3(50, 50, 0);
    Digger3->Init(Origin3);
    */


	return true;
}

Game::Game()
{

}

Game::~Game()
{

}

bool Game::RemoveActor(Uint32 ID)
{
    /*
    if (ID < ActorList.size())
    {
        ActorList.erase(ActorList.begin() + ID);

        return true;
    }
    */
    return false;
}

bool Game::Run()
{
    /*
	for (Uint32 i = 0; i < ActorList.size(); i++)
	{
		ActorList[i]->Update();
	}
	*/
	return true;
}
