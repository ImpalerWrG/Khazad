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
	void Seed(Sint32);
	Sint32 Roll(Sint32 Min, Sint32 Max);

};

#define RANDOM (RandomNumberGenerator::GetInstance())

#endif // RANDOM__HEADER
