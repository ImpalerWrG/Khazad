#ifndef TIMER__HEADER
#define TIMER__HEADER

#include <stdafx.h>


class Timer
{
public:

	Timer(uint16_t Size = 1);
	~Timer();

	void Start();
	const uint64_t Stop();

	void Pause();
	void Unpause();

	void SetSampleSize(uint16_t Size);

	const uint64_t getElapsed();

	void doAverage();
    double getAverage();

protected:

	uint64_t StartTime;
	uint64_t PausedTime;

    uint16_t SampleSize;
    uint16_t SampleIndex;

    uint64_t SamplingPause;

    float AverageTime;

    uint64_t* AcumulationArray;

	bool started;
	bool paused;
};

#endif // TIMER__HEADER
