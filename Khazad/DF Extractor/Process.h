#ifndef PROCESS_H_INCLUDED
#define PROCESS_H_INCLUDED
class DataModel;
class memory_info;

class Process
{
protected:
    DataModel* my_datamodel;
    memory_info * my_descriptor;
    ProcessHandle my_handle;
    bool attached;
public:
    Process(DataModel * dm, memory_info* mi, ProcessHandle ph);
    ~Process();
    // Set up stuff so we can read memory
    bool attach();
    bool detach();
    // is the process still there?
    bool isValid();
    memory_info *getDescriptor();
    DataModel *getDataModel();
};

#endif // PROCESS_H_INCLUDED
