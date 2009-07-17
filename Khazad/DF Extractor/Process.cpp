#include "DFCommon.h"
#include <ProcessManager.h>
#include <Process.h>
#include <Paths.h>
#include <fstream>
#include <StringMagic.h>
#include <DfVector.h>

#ifdef LINUX_BUILD
#include <md5.h>
#include <sys/wait.h>
#endif

///TODO: move these out of the way, too. they belong into their own file

Process::Process(DataModel * dm, memory_info* mi, ProcessHandle ph)
{
    my_datamodel = dm;
    my_descriptor = mi;
    my_handle = ph;
    attached = false;
}

Process::~Process()
{
}


DataModel *Process::getDataModel()
{
    return my_datamodel;
}
memory_info * Process::getDescriptor()
{
    return my_descriptor;
}

bool Process::isValid()
{
    ///TODO: check for weird states here - like crashed DF and similar crap
    return attached; // valid when attached
}

#ifdef LINUX_BUILD
/**
 *     LINUX PART
 */

bool Process::attach()
{
    /// TODO: check for errors!
    if(g_pProcess != NULL)
        return false;
    ptrace(PTRACE_ATTACH , my_handle, NULL, NULL);
    wait(NULL); // wait for DF to be stopped.
    attached = true;
    /// !!!HACK!!!
    /// Set the global process variables
    g_pProcess = this;
    g_ProcessHandle = my_handle;
    return true; // we are attached
}
bool Process::detach()
{
    /// TODO: check for errors.
    ptrace(PTRACE_DETACH, my_handle, NULL, NULL);
    attached = false;
    g_pProcess = NULL;
    g_ProcessHandle = 0;
    return true;
}


#else
/**
 *     WINDOWS PART
 */
/*bool Process::getProcessINFO(char *p_processname)
{
    HWND DFWindow;
    HANDLE DFHandle;
    DWORD DFProcessID;
    DFWindow = FindWindow(NULL,"Dwarf Fortress");
    if(!DFWindow)
    {
        printf("Cannot find window with name \"%s\"\n", p_processname);
        return false;
    }

    // And Create Handle
    GetWindowThreadProcessId(DFProcess, &DFProcessID);
    printf("Window Thread Process ID [%u] \n", (unsigned int)DFProcessID);
    ///FIXME: Add corresponding CloseHandle call
    DFHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, DFProcessID);

    if(!DFHandle)
    {
        printf("Could not get application handle to hook. \n");
        return false;
    }
    return true;
}
*/
bool Process::attach()
{
    /// TODO: check for errors!
    attached = true;
    return true;
}
bool Process::detach()
{
    attached = false;
    // nothing to do here, we are not a debbuger on Windows
    return true;
}

#endif

