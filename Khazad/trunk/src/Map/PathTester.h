#ifndef TEST__HEADER
#define TEST__HEADER

#include <stdafx.h>
#include <Singleton.h>

#include <Timer.h>

class PathTester
{
	DECLARE_SINGLETON_CLASS(PathTester)

public:

    ~PathTester();
    bool Init();

    void RunTestSuite();

protected:

    Timer* PathingTimer;
};


#define TESTER (PathTester::GetInstance())

#endif // TEST__HEADER
