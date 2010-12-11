#include <Random.h>

#include <stdint.h>

RandomNumberGenerator::RandomNumberGenerator()
{

}

RandomNumberGenerator::~RandomNumberGenerator()
{

}

bool RandomNumberGenerator::Init()
{
	Seed((int32_t)time(0));
	return true;
}

void RandomNumberGenerator::Seed(int32_t Seed)
{
	srand(Seed);
}

int32_t RandomNumberGenerator::Roll(int32_t Min, int32_t Max)
{
    return rand() % (Max - Min + 1) + Min;
}

float RandomNumberGenerator::Roll(float Min, float Max)
{
	int r = rand();

    float x = (float)(r & 0x7fff) / (float)0x7fff;

    return (x * (Max - Min) + Min);
}

