#ifndef PROCESSMANAGER_H_INCLUDED
#define PROCESSMANAGER_H_INCLUDED

#ifdef LINUX_BUILD
typedef pid_t ProcessHandle;
#else
typedef HANDLE ProcessHandle;
#endif

class Process;
class memory_info;

/// HACK: global variables (only one process can be attached at the same time.)
extern Process * g_pProcess; ///< current process. non-NULL when picked
extern ProcessHandle g_ProcessHandle; ///< cache of handle to current process. used for speed reasons

class ProcessManager
{
    private:
    std::vector<memory_info> meminfo;
    Process * currentProcess;
    ProcessHandle currentProcessHandle;
    std::vector<Process *> processes;
    bool loadDescriptors();
#ifdef LINUX_BUILD
    Process* addWineProcess(string & exe,ProcessHandle PH);
    Process* addLinuxProcess(string & exe,ProcessHandle PH);
#else
    Process* addWindowsProcess(string & exe,ProcessHandle PH);
#endif

    public:
    ProcessManager();
    bool findProcessess();
    uint32_t size();
    Process * operator[](uint32_t index);
    Process* getCurrentProcess();
    ProcessHandle getCurrentProcessHandle();
};


#endif // PROCESSMANAGER_H_INCLUDED
