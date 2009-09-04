#ifndef PROCESSMANAGER_H_INCLUDED
#define PROCESSMANAGER_H_INCLUDED

#ifdef LINUX_BUILD
typedef pid_t ProcessHandle;
#else
typedef HANDLE ProcessHandle;
#endif

class memory_info;
class DataModel;
class TiXmlElement;

/*
 * Process manager
 */
class ProcessManager
{
public:
    class Process
    {
        friend class ProcessManager;
    protected:
        DataModel* my_datamodel;
        memory_info * my_descriptor;
        ProcessHandle my_handle;
        string memFile;
        bool attached;
        void freeResources();
        void setMemFile(const string & memf);
    public:
        Process(DataModel * dm, memory_info* mi, ProcessHandle ph);
        ~Process();
        // Set up stuff so we can read memory
        bool attach();
        bool detach();
        // is the process still there?
        bool isAttached();
        memory_info *getDescriptor();
        DataModel *getDataModel();
    };

    ProcessManager( string path_to_xml);
    ~ProcessManager();
    bool findProcessess();
    uint32_t size();
    Process * operator[](uint32_t index);
private:
    // memory info entries loaded from a file
    std::vector<memory_info> meminfo;
    // vector to keep track of dynamically created memory_info entries
    std::vector<memory_info *> destroy_meminfo;
    Process * currentProcess;
    ProcessHandle currentProcessHandle;
    std::vector<Process *> processes;
    bool loadDescriptors( string path_to_xml);
    void ParseVTable(TiXmlElement* vtable, memory_info& mem);
    void ParseEntry (TiXmlElement* entry, memory_info& mem, map <string ,TiXmlElement *>& knownEntries);
#ifdef LINUX_BUILD
    Process* addProcess(const string & exe,ProcessHandle PH,const string & memFile);
#endif
};

/*
 * Currently attached process and its handle
 */
extern ProcessManager::Process * g_pProcess; ///< current process. non-NULL when picked
extern ProcessHandle g_ProcessHandle; ///< cache of handle to current process. used for speed reasons
extern FILE * g_ProcessMemFile; ///< opened /proc/PID/mem, valid when attached

#endif // PROCESSMANAGER_H_INCLUDED
