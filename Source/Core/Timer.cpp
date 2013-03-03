#include <stdafx.h>

#include <Timer.h>
#include <Renderer.h>

Timer::Timer(uint16_t Size)
{
	StartTime = 0;
	PausedTime = 0;
	started = false;
	paused = false;
    AverageTime = 0;
    SamplingPause = 0;

    SetSampleSize(Size);
}

Timer::~Timer()
{

}

void Timer::Start()
{
	StartTime = RENDERER->getRoot()->getTimer()->getMillisecondsCPU();
	started = true;
	paused = false;
    SampleIndex = 0;
	PausedTime = 0;
    SamplingPause = 0;
}

const uint64_t Timer::Stop()
{
    uint64_t Elapsed = getElapsed();

	started = false;
	paused = false;
	StartTime = 0;
	PausedTime = 0;
    SampleIndex = 0;

    return Elapsed;
}

const uint64_t Timer::getElapsed()
{
	if (started)
	{
		if (paused)
		{
			return PausedTime;
		}
		else
		{
			return RENDERER->getRoot()->getTimer()->getMillisecondsCPU() - StartTime;
		}
	}
	return 0;
}

void Timer::Pause()
{
	if ((started == true) && (paused == false))
	{
		paused = true;
        AcumulationVector[SampleIndex] = RENDERER->getRoot()->getTimer()->getMillisecondsCPU() - (StartTime + SamplingPause);
        PausedTime = RENDERER->getRoot()->getTimer()->getMillisecondsCPU() - StartTime;

        SampleIndex++;
        if(SampleIndex == SampleSize)
        {
            doAverage();
        }
	}
}

void Timer::Unpause()
{
	if (paused == true)
	{
		paused = false;
		StartTime = RENDERER->getRoot()->getTimer()->getMillisecondsCPU() - PausedTime;
		SamplingPause = PausedTime;
		PausedTime = 0;
	}
}

void Timer::SetSampleSize(uint16_t Size)
{
    SampleSize = Size;
    if (SampleSize == 0)
    {
        SampleSize = 1;
    }
    AcumulationVector.clear();
    AcumulationVector.reserve(SampleSize);

    SampleIndex = 0;
    SamplingPause = 0;
}

void Timer::doAverage()
{
    uint64_t Total = 0;
    for (uint16_t i = 0; i < SampleSize; i++)
    {
        Total += AcumulationVector[i];
    }
	SampleIndex = 0;
	AcumulationVector.clear();
    AverageTime = (double)Total / (double)SampleSize;
}

double Timer::getAverage()
{
    return AverageTime;
}
