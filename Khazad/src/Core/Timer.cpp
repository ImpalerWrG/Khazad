#include <stdafx.h>

#include <Timer.h>

Timer::Timer(Uint32 Size)
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
	StartTime = SDL_GetTicks();
	started = true;
	paused = false;
    SampleIndex = 0;
	PausedTime = 0;
    SamplingPause = 0;
}

void Timer::Stop()
{
	started = false;
	paused = false;
	StartTime = 0;
	PausedTime = 0;
    SampleIndex = 0;
}

const Uint32 Timer::getElapsed()
{
	if (started)
	{
		if (paused)
		{
			return PausedTime;
		}
		else
		{
			return SDL_GetTicks() - StartTime;
		}
	}
	return 0;
}

void Timer::Pause()
{
	if(( started == true ) && ( paused == false ))
	{
		paused = true;
        AcumulationArray[SampleIndex] = SDL_GetTicks() - (StartTime + SamplingPause);

        PausedTime = SDL_GetTicks() - StartTime;

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
		StartTime = SDL_GetTicks() - PausedTime;
		SamplingPause = PausedTime;
		PausedTime = 0;
	}
}

void Timer::SetSampleSize(Uint32 Size)
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

    AcumulationArray = new Uint32[SampleSize];

    for (uint32_t i = 0; i < SampleSize; i++)
    {
        AcumulationArray[i] = 0;
    }
}

void Timer::doAverage()
{
    Uint32 Total = 0;
    for(Uint32 i = 0; i < SampleSize; i++)
    {
        Total += AcumulationArray[i];
    }

    AverageTime = (float)Total / (float)SampleSize;
}

float Timer::getAverage()
{
    return AverageTime;
}
