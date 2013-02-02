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

#include <stdafx.h>

#include <Job.h>

#include <Citizen.h>

// TASK

Task::Task()
{

}

Task::~Task()
{
    // NOP
}

bool Task::InitializeTask()
{
    return false;
}

// JOB

Job::Job(std::string NewName)
{
    Name = NewName;
    Terminate = false;
}

Job::~Job()
{

}

bool Job::InitializeJob(JobType NewType, Zone* WorkSite)
{
    Type = NewType;
    WorkPlace = WorkSite;

    return true;
}

void Job::addWorker(Citizen* NewWorker)
{
    for(std::vector<Citizen*>::iterator it = Workers.begin(); it != Workers.end(); it++)
    {
        if (*it == NewWorker)
        {
            return;  // Abort if Worker already works on this Job
        }
    }

    Workers.push_back(NewWorker);
}

void Job::removeWorker(Citizen* DepartingWorker)
{
    for(std::vector<Citizen*>::iterator it = Workers.begin(); it != Workers.end(); it++)
    {
        if (*it == DepartingWorker)
        {
            (*it)->CurrentJob == NULL;
            (*it)->CurrentTask == NULL;  //Abort Task possibly doing something??
            Workers.erase(it);
        }
    }
}

// JOBMANAGER

DECLARE_SINGLETON(JobManager)

JobManager::JobManager()
{

}

JobManager::~JobManager()
{
    JobsMap.clear();
}

Job* JobManager::getJob(std::string JobName)
{
    std::map<std::string, Job*>::iterator it = JobsMap.find(JobName);

    if(it != JobsMap.end())
    {
        return it->second;
    } else {
        return NULL;
    }
}

Job* JobManager::createJob(JobType Type)
{
    std::string NewJobName = createJobName(Type);
    std::map<std::string, Job*>::iterator it = JobsMap.find(NewJobName);

    if (it == JobsMap.end())  // Sanity Check
    {
        Job* NewJob = new Job(NewJobName);
        JobsMap.insert(std::pair<std::string, Job*>(NewJobName, NewJob));
        return NewJob;
    }
    return NULL;
}

std::string JobManager::createJobName(JobType Type)
{
    uint16_t Counter = 0;

    for (std::map<std::string, Job*>::iterator it = JobsMap.begin(); it != JobsMap.end(); it++)
    {
        if (it->second->Type == Type)
        {
            Counter++;
        }
    }
    char buffer[32];


    switch (Type)
    {
        case JOB_DIG:
            sprintf(buffer, "DIG %i", Counter + 1);
            return std::string(buffer);
            break;

		default:
            return std::string("UnKnown Job");
    }
}

void JobManager::pauseJob(std::string JobName)
{
    std::map<std::string, Job*>::iterator it = JobsMap.find(JobName);

    if(it == JobsMap.end())
    {
        it->second->Active = false;
    }
}

void JobManager::changeWorkers(std::string JobName, uint16_t Change)
{

}

void JobManager::UpdateTick()
{
    // Find idle Citizens
    // Allow Jobs to release excess workers or request more
    // Allocate workers

    for(std::map<std::string, Job*>::iterator it = JobsMap.begin(); it != JobsMap.end(); it++)
    {
        //it->second->
    }
}
