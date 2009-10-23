#include <stdafx.h>

#include <Pawn.h>
#include <Random.h>
#include <Renderer.h>
#include <Game.h>
#include <Map.h>
#include <Cube.h>


Pawn::Pawn()
{
	setType(PAWN_ACTOR);

	GAME->ActorList.push_back(this);
	//ID = (Uint32) GAME->ActorList.size();
}

Pawn::~Pawn()
{

}

bool Pawn::Init(Vector3 SpawnLocation)
{
	Position = SpawnLocation;

    return true;
}

bool Pawn::Move(Vector3 Direction)
{
    Position += Direction;

    if(Position.x > MAP->getMapSizeX())
    {
        Position.x = MAP->getMapSizeX();
    }
    else
    {
         if(Position.x < 0)
        {
            Position.x = 0;
        }
    }

    if(Position.y > MAP->getMapSizeY())
    {
        Position.y = MAP->getMapSizeY();
    }
    else
    {
         if(Position.y < 0)
        {
            Position.y = 0;
        }
    }

    if(Position.z > MAP->getMapSizeZ())
    {
        Position.z = MAP->getMapSizeZ();
    }
    else
    {
         if(Position.z < 0)
        {
            Position.z = 0;
        }
    }

    //Cube* NewCube = MAP->getCube((Uint32) Position.x, (Uint32) Position.y, (Uint32) Position.z);
/*
    if (NewCube)
    {
        if (NewCube->isInitalized() != true)
        {
            //NewCube->Init();
            //NewCube->setVisible(true);
        }
    }
*/
    return true;
}

bool Pawn::Update()
{
    if(RANDOM->Roll(0, 20) == 20)
    {
        switch (RANDOM->Roll(0, 5))
        {
            case 0:
            {
                Vector3 Direction = Vector3(1, 0, 0);
                Move(Direction);
                break;
            }
            case 1:
            {
                Vector3 Direction = Vector3(0, 1, 0);
                Move(Direction);
                break;
            }
            case 2:
            {
                Vector3 Direction = Vector3(-1, 0, 0);
                Move(Direction);
                break;
            }
            case 3:
            {
                Vector3 Direction = Vector3(0, -1, 0);
                Move(Direction);
                break;
            }
            case 4:
            {
                Vector3 Direction = Vector3(0, 0, -1);
                Move(Direction);
                break;
            }
            case 5:
            {
                Vector3 Direction = Vector3(0, 0, 1);
                Move(Direction);
                break;
            }
        }
    }

    return true;
}

bool Pawn::Draw()
{
	glColor3f(1.0, 1.0, 1.0);

  	glBegin(GL_QUADS);

		glVertex3f(Position.x - 0.5, Position.y + 0.5, Position.z + 0.5);
		glVertex3f(Position.x + 0.5, Position.y + 0.5, Position.z + 0.5);
		glVertex3f(Position.x + 0.5, Position.y - 0.5, Position.z + 0.5);
		glVertex3f(Position.x - 0.5, Position.y - 0.5, Position.z + 0.5);

        /*
		glVertex3f(Position.x - 0.5, Position.y + 0.5, Position.z - 0.5);
		glVertex3f(Position.x + 0.5, Position.y + 0.5, Position.z - 0.5);
		glVertex3f(Position.x + 0.5, Position.y - 0.5, Position.z - 0.5);
		glVertex3f(Position.x - 0.5, Position.y - 0.5, Position.z - 0.5);
		*/

	glEnd();


    return true;
}
