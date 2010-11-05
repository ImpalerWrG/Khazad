#ifndef TIMER__HEADER
#define TIMER__HEADER

#include <stdafx.h>


class Timer
{
public:

	Timer(uint32_t Size = 1);
	~Timer();

	void Start();
	const uint32_t Stop();

	void Pause();
	void Unpause();

	void SetSampleSize(uint32_t Size);

	const uint32_t getElapsed();

	void doAverage();
    float getAverage();

protected:

	uint32_t StartTime;
	uint32_t PausedTime;

    uint32_t SampleSize;
    uint32_t SampleIndex;
    uint32_t SamplingPause;

    float AverageTime;

    uint32_t* AcumulationArray;

	bool started;
	bool paused;
};

#endif // TIMER__HEADER
