#include <Citizen.h>

#include <DataManager.h>
#include <Map.h>
#include <Job.h>

#include <ImageManager.h>
#include <TextureManager.h>

Citizen::Citizen()
{

}

Citizen::~Citizen()
{
    // NOP
}

bool Citizen::InitializeCitizen()
{
    return false;
}

void Citizen::ChangeJob(Job* NewJob)
{
    //CurrentJob->removeWorker(this);
    //CurrentJob = NewJob;
    //CurrentJob->addWorker(this);
}

