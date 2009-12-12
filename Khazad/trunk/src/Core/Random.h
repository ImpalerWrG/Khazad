#ifndef RANDOM__HEADER
#define RANDOM__HEADER

#include <stdafx.h>

#include <cstdlib>
#include <ctime>
#include <Singleton.h>

class RandomNumberGenerator
{

DECLARE_SINGLETON_CLASS(RandomNumberGenerator)

public:

	bool Init();
	~RandomNumberGenerator();

	void Seed(int32_t Seed);
	int32_t Roll(int32_t Min, int32_t Max);
};

#define RANDOM (RandomNumberGenerator::GetInstance())

#endif // RANDOM__HEADER
