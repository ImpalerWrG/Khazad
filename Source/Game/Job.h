#ifndef JOB_HEADER
#define JOB_HEADER

#include <stdafx.h>
#include <Singleton.h>

//#include <string.h>

class Zone;
class Citizen;

enum TaskType
{
    TASK_IDLE,
    TASK_PICK_UP,
    TASK_HAUL,
    TASK_DROP_OFF,
    TASK_DIG,

    NUM_TASKTYPES,
    TASKTYPES_START = 0
};

struct Task
{

public:

    Task();
    ~Task();

    bool InitializeTask();

    TaskType Type;


};

enum JobType
{
    JOB_IDLE,
    JOB_DIG,
    JOB_HAUL,

    NUM_JOBTYPES,
    JOBTYPES_START = 0
};

class Job
{

friend class JobManager;

protected:

    Job(std::string Name);
    ~Job();

    bool InitializeJob(JobType Type, Zone* WorkSite);

    void addWorker(Citizen* NewWorker);
    void removeWorker(Citizen* DepartingWorker);

private:

    JobType Type;     // What type of Job
    bool Active;      // Is the Job active or on hold
    bool Terminate;    // Job is complete or cancelled
    Zone* WorkPlace;  // Ware is the job

    std::string Name;
    std::vector<Citizen*> Workers;
    Task ProvideNextTask();
};

class JobManager
{
    DECLARE_SINGLETON_CLASS(JobManager)

public:

    ~JobManager();

    void UpdateTick();

    Job* getJob(std::string JobName);
    std::string createJobName(JobType Type);
    Job* createJob(JobType Type);
    void pauseJob(std::string JobName);
    void changeWorkers(std::string JobName, uint16_t Change);

private:

    std::map<std::string, Job*> JobsMap;

};

#define JOBS (JobManager::GetInstance())

#endif // JOB_HEADER
