#include "DFCommon.h"
#ifdef LINUX_BUILD
#include <sys/wait.h>
#endif


Process::Process(DataModel * dm, memory_info* mi, ProcessHandle ph)
{
    my_datamodel = dm;
    my_descriptor = mi;
    my_handle = ph;
    attached = false;
}


Process::~Process()
{
    if(attached)
    {
        detach();
    }
    // destroy data model. this is assigned by processmanager
    delete my_datamodel;
    freeResources();
}


DataModel *Process::getDataModel()
{
    return my_datamodel;
}


memory_info * Process::getDescriptor()
{
    return my_descriptor;
}


void Process::setMemFile(const string & memf)
{
    assert(!attached);
    memFile = memf;
}


#ifdef LINUX_BUILD
/*
 *     LINUX PART
 */
bool Process::attach()
{
    // TODO: check for errors!
    if(g_pProcess != NULL)
    {
        return false;
    }
    
    ptrace(PTRACE_ATTACH , my_handle, NULL, NULL);
    wait(NULL); // wait for DF to be stopped.
    attached = true;
    
    // HACK: Set the global process variables
    g_pProcess = this;
    g_ProcessHandle = my_handle;
    g_ProcessMemFile = fopen(memFile.c_str(),"rb");
    return true; // we are attached
}


bool Process::detach()
{
    // TODO: check for errors.
    ptrace(PTRACE_DETACH, my_handle, NULL, NULL);
    attached = false;
    
    g_pProcess = NULL;
    g_ProcessHandle = 0;
    fclose(g_ProcessMemFile);// close /proc/PID/mem
    g_ProcessMemFile = NULL;
    return true;
}


void Process::freeResources()
{
    // nil
};


#else
/*
 *     WINDOWS PART
 */

//FIXME: should support stopping and resuming the process

bool Process::attach()
{
    attached = true;
    g_pProcess = this;
    g_ProcessHandle = my_handle;
    return true;
}


bool Process::detach()
{
    attached = false;
    g_pProcess = NULL;
    g_ProcessHandle = 0;
    // nothing to do here, we are not a debbuger on Windows
    return true;
}


void Process::freeResources()
{
    // opened by process manager
    CloseHandle(my_handle);
};
#endif

