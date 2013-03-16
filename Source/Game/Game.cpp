/* Copyright 2010 Kenneth Ferland

This file is part of Khazad.

Khazad is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Khazad is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Khazad.  If not, see <http://www.gnu.org/licenses/> */


#include <Game.h>

#include <Temporal.h>
#include <Pawn.h>
#include <Tree.h>
#include <Geology.h>
#include <PathManager.h>
#include <Cell.h>
#include <Timer.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/version.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <GUI.h>

#define BOOST_THREAD_USE_LIB

DECLARE_SINGLETON(Game)

bool Game::InitializeGame(uint16_t X, uint16_t Y, const char* SeedString)
{
    MapGeology = new Geology();
    MapGeology->Init(SeedString);
    MapGeology->GenerateWorldHeightMap(X, Y);

	MainMap = new Map();
	MainMap->Init();

 	Path = new PathManager();
	Path->Init();

	boost::thread_group Builder;
	Builder.create_thread( boost::bind(&Game::BuildMapChunk, this, 0, 0, X, Y));

    Zoneing = false;
    GameTimer->Start();

	return true;
}

Game::Game()
{
	TickRate = 1000;
    GameTimer = new Timer(50);
	Pause = true;
}

Game::~Game()
{

}

bool Game::BuildMapChunk(int16_t X, int16_t Y, int8_t Width, int8_t Height)
{
    int16_t SizeX = X + Width;
    int16_t SizeY = Y + Height;

    float ProgressSize = SizeX * SizeY;
    float ProgressCount = 0;

    // Create and add Cells with shape and material data
    for (int32_t x = X; x < SizeX; x++)
    {
        for (int32_t y = Y; y < SizeY; y++)
        {
            MapGeology->GenerateCellHeight(x, y, 10.0, 2.0);

            for (int16_t z = MapGeology->getCellBottomZLevel() - 1; z <= MapGeology->getCellTopZLevel() + 1; z++)
            {
                CellCoordinates TargetCellCoordinates = CellCoordinates(x, y, z);
                Cell* NewCell = new Cell();

                NewCell->setCellPosition(TargetCellCoordinates);
                MainMap->insertCell(NewCell, TargetCellCoordinates);
                MapGeology->LoadCellData(NewCell);
            }
        }
    }

    // Initialize Faces for the cells
	ProgressSize = MainMap->getCellMap()->size();
    for (std::map<uint64_t, Cell*>::iterator CellIterator = MainMap->getCellMap()->begin() ; CellIterator != MainMap->getCellMap()->end(); ++CellIterator )
    {
        CellIterator->second->BuildFaceData();
		ProgressCount++;
		ProgressAmount = ProgressCount / ProgressSize;
		GUI->DirtyActiveScreen();
    }

	Path->CreateMapAbstraction(MainMap);
	Path->InitializeTestingSuite();

    RENDERER->FocusActiveCameraAt(Ogre::Vector3(X * CELLEDGESIZE / 2, Y * CELLEDGESIZE / 2, 0));
    MainMap->RegisterWithRendering();
    GUI->DirtyActiveScreen();

    return true;
}

bool Game::Run()
{
    GameTimer->Unpause();

	if (!Pause)
	{
		for(uint32_t i = TickRate; i > 0; i--)
		{
			TEMPORAL->UpdateTick();

			// Update map? other none actor based logics?
		}
	}

    GameTimer->Pause();

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
