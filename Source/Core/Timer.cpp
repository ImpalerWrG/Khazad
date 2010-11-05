#include <stdafx.h>

#include <Timer.h>
#include <Renderer.h>

Timer::Timer(uint32_t Size)
{
	StartTime = 0;
	PausedTime = 0;
	started = false;
	paused = false;
    AverageTime = 0;
    SamplingPause = 0;

    AcumulationArray = NULL;
    SetSampleSize(Size);
}

Timer::~Timer()
{
    if (AcumulationArray != NULL)
    {
        delete AcumulationArray;
    }
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

const uint32_t Timer::Stop()
{
    uint32_t Elapsed = getElapsed();

	started = false;
	paused = false;
	StartTime = 0;
	PausedTime = 0;
    SampleIndex = 0;

    return Elapsed;
}

const uint32_t Timer::getElapsed()
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
	if(( started == true ) && ( paused == false ))
	{
		paused = true;
        AcumulationArray[SampleIndex] = RENDERER->getRoot()->getTimer()->getMillisecondsCPU() - (StartTime + SamplingPause);

        PausedTime = RENDERER->getRoot()->getTimer()->getMillisecondsCPU() - StartTime;

        SampleIndex++;

        if(SampleIndex == SampleSize)
        {
            doAverage();
            SampleIndex = 0;
        }
	}
}

void Timer::Unpause()
{
	if( paused == true )
	{
		paused = false;
		StartTime = RENDERER->getRoot()->getTimer()->getMillisecondsCPU() - PausedTime;
		SamplingPause = PausedTime;
		PausedTime = 0;
	}
}

void Timer::SetSampleSize(uint32_t Size)
{
    SampleSize = Size;
    if (SampleSize == 0)
    {
        SampleSize = 1;
    }
    SampleIndex = 0;
    SamplingPause = 0;

    if (AcumulationArray != NULL)
    {
        delete AcumulationArray;
    }

    AcumulationArray = new uint32_t[SampleSize];

    for (uint32_t i = 0; i < SampleSize; i++)
    {
        AcumulationArray[i] = 0;
    }
}

void Timer::doAverage()
{
    uint32_t Total = 0;
    for(uint32_t i = 0; i < SampleSize; i++)
    {
        Total += AcumulationArray[i];
    }

    AverageTime = (float)Total / (float)SampleSize;
}

float Timer::getAverage()
{
    return AverageTime;
}
