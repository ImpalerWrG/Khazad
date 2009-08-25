#include "DFCommon.h"
#include "SimpleMapAPI.h"

// internals
ProcessManager* pm = NULL;
ProcessManager::Process* p;
DataModel* dm;

/**
 * This is a canned initialization. It hides a lot of the process search and attach specifics from you.
 * It can be done better, but this is really easy to use.
 */
bool Attach(const string path_to_xml)
{
    // detach all processes, destroy manager
    if(pm != NULL)
        delete pm;
    // find a process (ProcessManager can find multiple when used properly)
    pm = new ProcessManager(path_to_xml); // FIXME: handle bad XML better
    if(!pm->findProcessess())
        return false; // FIXME: throw exceptions to distinguish errors? provide error code?
    p = (*pm)[0]; // we just use the first found process
    if(!p->attach())
        return false; // couldn't attach to process, no go
    // process is attached, everything went just fine
    return true;
}
/*
void ReadMatgloss()
{
    // set up some stuff, determine order
}


void ReadStoneMatgloss()
{

}
void ReadWoodMatgloss()
{

}
void ReadMetalMatgloss()
{

}
void ReadPlantMatgloss()
{

}
*/
