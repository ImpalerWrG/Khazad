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

	template<class Archive>
	void serialize(Archive & Arc, const unsigned int version)
	{
		Arc& StartTime;
		Arc& PausedTime;

		Arc& SampleSize;
		Arc& SampleIndex;

		Arc& SamplingPause;

		Arc& AverageTime;

		Arc& AcumulationVector;

		Arc& started;
		Arc& paused;
	};

protected:

	uint64_t StartTime;
	uint64_t PausedTime;

    uint16_t SampleSize;
    uint16_t SampleIndex;

    uint64_t SamplingPause;

    float AverageTime;

    std::vector<uint64_t> AcumulationVector;

	bool started;
	bool paused;
};

#endif // TIMER__HEADER
