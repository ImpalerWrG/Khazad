#ifndef PROCESS_H_INCLUDED
#define PROCESS_H_INCLUDED

#include <Singleton.h>
#include <stdafx.h>
#include <map>

#ifdef LINUX_BUILD
#include <md5.h>
#include <sys/param.h>
#include <sys/user.h>
#include <sys/sysctl.h>
#include <sys/ptrace.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#else
//#include <windows.h>
#include <winnt.h>
#endif

///TODO: decouple memory_info from the Process class?

class DfVector;

class memory_info
{
protected:
    map <string, string> data;
public:
    Uint32 getOffset (string);
    string getString (string);
    bool hasToken (string);
    bool setToken (string, string);
    void flush();
};

class Process
{
DECLARE_SINGLETON_CLASS(Process)

protected:
    std::vector<memory_info> meminfo;
    memory_info * current_descriptor;
    bool attached;

    bool getProcessINFO(char *p_processname); // get information about the (hopefully) running DF process
    /**
     * LINUX version
     */
    #ifdef LINUX_BUILD
    pid_t PID; // Process ID
    string exe; // string with the path to DF binary. something like '/proc/123/exe'. exe is a link.
    /**
     * WINDOWS version
     */
    #else
    HWND DFProcess;
    HANDLE DFHandle;
    DWORD DFProcessID;
    #endif
public:
    bool Init();
    ~Process();

    // Set up stuff so we can read memory
    bool attach();
    bool detach();

    // is the process still there?
    bool isValid();
    bool loadDescriptors();
    memory_info *getDescriptor();

    // methods for reading various amounts of raw data
    Uint8 readByte (Uint32 offset);
    Uint16 readWord (Uint32 offset);
    Uint32 readDWord (Uint32 offset);
    Uint64 readQuad (Uint32 offset);
    bool read (Uint32 offset, Uint32 size, Uint8 *target);
    // read a platform-specific vector structure from memory
    DfVector readVector (Uint32 offset, Uint32 item_size);
};

#define PROCESS (Process::GetInstance())
#endif // PROCESS_H_INCLUDED
