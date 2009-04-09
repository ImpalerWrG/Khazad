#ifndef TIMER__HEADER
#define TIMER__HEADER

#include <stdafx.h>
#include <sdl.h>

class Timer
{
public:

	Timer(Uint32 Size = 1);
	~Timer();

	void Start();
	void Stop();
	void Pause();
	void Unpause();
	void SetSampleSize(Uint32 Size);

	const Uint32 getElapsed();
	void doAverage();
    float getAverage();

private:

	Uint32 StartTime;
	Uint32 PausedTime;
	Uint32 Alarm;
    Uint32 SampleSize;
    Uint32 SampleIndex;
    Uint32 SamplingPause;
    float AverageTime;

    Uint32* AcumulationArray;

	bool started;
	bool paused;
};

#endif // TIMER__HEADER
