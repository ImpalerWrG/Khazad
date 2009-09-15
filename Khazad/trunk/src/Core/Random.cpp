#include <Singleton.h>
#include <Random.h>

DECLARE_SINGLETON(RandomNumberGenerator)

RandomNumberGenerator::RandomNumberGenerator()
{

}

RandomNumberGenerator::~RandomNumberGenerator()
{

}

bool RandomNumberGenerator::Init()
{
	Seed((Uint32)time(0));
	return true;
}

void RandomNumberGenerator::Seed(Sint32 Seed)
{
	srand(Seed);
}

Sint32 RandomNumberGenerator::Roll(Sint32 Min, Sint32 Max)
{
    int range = (Max - Min) + 1;
    return (Sint32) Min + (range * rand() / (RAND_MAX + 1));
}
