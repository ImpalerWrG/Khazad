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
    // memory info entries loaded from a file
    std::vector<memory_info> meminfo;
    // vector to keep track of dynamically created memory_info entries
    std::vector<memory_info *> destroy_meminfo;
    Process * currentProcess;
    ProcessHandle currentProcessHandle;
    std::vector<Process *> processes;
    bool loadDescriptors( string path_to_xml);
#ifdef LINUX_BUILD
    Process* addProcess(string & exe,ProcessHandle PH);
#endif
    public:
    ProcessManager( string path_to_xml);
    ~ProcessManager();
    bool findProcessess();
    uint32_t size();
    Process * operator[](uint32_t index);
};


#endif // PROCESSMANAGER_H_INCLUDED
