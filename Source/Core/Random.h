#ifndef RANDOM__HEADER
#define RANDOM__HEADER

#include <stdafx.h>

#include <cstdlib>
#include <ctime>
#include <Singleton.h>

class RandomNumberGenerator
{

public:

    RandomNumberGenerator();
    ~RandomNumberGenerator();

	bool Init();

	void Seed(int32_t Seed);
	int32_t Roll(int32_t Min, int32_t Max);
};

#endif // RANDOM__HEADER
