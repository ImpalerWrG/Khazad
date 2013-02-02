#ifndef CITIZEN_HEADER
#define CITIZEN_HEADER


#include <Pawn.h>
#include <Job.h>
#include <Task.h>

class Citizen : public Pawn
{

public:

    Citizen();
    ~Citizen();

    bool InitializeCitizen();

    void ChangeJob(Job* NewJob);

protected:

    Job* CurrentJob;
    Task* CurrentTask;

    friend class Job;

};

#endif // CITIZEN_HEADER
