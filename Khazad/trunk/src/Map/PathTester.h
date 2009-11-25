#ifndef TEST__HEADER
#define TEST__HEADER

#include <stdafx.h>
#include <Singleton.h>

#include <Timer.h>
#include <Map.h>

class PathTester
{
	DECLARE_SINGLETON_CLASS(PathTester)

public:

    ~PathTester();
    bool Init();

    void RunPathTestSuite(int Interations, vector<int> PathSystems);
    void RunHuristicTestSuite(int Interations, vector<int> PathSystems);

protected:

    Timer* PathingTimer;

    MapCoordinates* TestCoords;
    int NumTestPoints;
};


#define TESTER (PathTester::GetInstance())

#endif // TEST__HEADER
