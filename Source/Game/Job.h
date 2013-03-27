/* Copyright 2010 Kenneth Ferland

This file is part of Khazad.

Khazad is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Khazad is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Khazad.  If not, see <http://www.gnu.org/licenses/> */


/*-------------------------TASK----------------------------

Tasks are the smallest logical sub-divisions of activity that
Pawns will perform.  By definition a Task can only be done by
one individual
*/

/*--------------------------JOB----------------------------

Jobs are larger aggregations of Tasks often of multiple types
all directed towards completing one overall goal, usually a
player-designated goal such as digging out a large room,
building a set of items or fighting an enemy creature.

More then one Pawn may be assigned to a Job at a time but
each will be assigned different individual Tasks.
*/

#ifndef JOB_HEADER
#define JOB_HEADER

#include <stdafx.h>
#include <Singleton.h>


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

class Task
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

protected:

	std::map<std::string, Job*> JobsMap;

};

#define JOBS (JobManager::GetInstance())

#endif // JOB_HEADER
