#include "DFCommon.h"

#include <DataModel.h>
#include <MemInfo.h>

#include <tinyxml.h>
#include <iostream>

/// HACK: global variables (only one process can be attached at the same time.)
ProcessManager::Process * g_pProcess; ///< current process. non-NULL when picked
ProcessHandle g_ProcessHandle; ///< cache of handle to current process. used for speed reasons

#ifdef LINUX_BUILD
/// LINUX version of the process finder.
#include <md5.h>

ProcessManager::Process* ProcessManager::addProcess(string & exe,ProcessHandle PH)
{
    string hash = MD5Sum((char *)exe.c_str());// get hash of the running DF process
    // iterate over the list of memory locations
    vector<memory_info>::iterator it;
    for ( it=meminfo.begin() ; it < meminfo.end(); it++ )
        if(hash == (*it).getString("md5")) // are the md5 hashes the same?
        {
            memory_info * m = &*it;
            Process * ret;
            cout <<"Found process " << PH <<  ". It's DF version " << m->getVersion() << "." << endl;
            if(memory_info::OS_WINDOWS == (*it).getOS())
            {
                ret= new Process(new DMWindows40d(),m,PH);
            }
            else if (memory_info::OS_LINUX == (*it).getOS())
            {
                ret= new Process(new DMLinux40d(),m,PH);
            }
            else
            {
                // some error happened, continue with next process
                continue;
            }
            processes.push_back(ret);
            return ret;
        }
    return NULL;
}

bool ProcessManager::findProcessess()
{
    DIR *dir_p;
	struct dirent *dir_entry_p;
	string dir_name;
    string exe_link;
    string cwd_link;
    string cmdline_path;
    string cmdline;
	char target_name[1024];									// ??? buffer overrun potential
	int target_result;
	int errorcount;
	int result;

	errorcount=0;
	result=0;
	// Open /proc/ directory
	dir_p = opendir("/proc/");
	// Reading /proc/ entries
	while(NULL != (dir_entry_p = readdir(dir_p)))
	{
	    // Checking for numbered directories
		if (strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name))
		{
		    dir_name = "/proc/";
		    dir_name +=  + dir_entry_p->d_name;
		    dir_name += "/";
		    exe_link = dir_name + "exe";
			// Getting the target of the exe ie to which binary it points to
			target_result = readlink(exe_link.c_str(), target_name, sizeof(target_name)-1);
			if (target_result > 0)
			{
			    // make sure we have a null terminated string...
				target_name[target_result] = 0;
				// is this the regular linux DF?
				if (strstr(target_name, "dwarfort.exe") != NULL)
				{
				    exe_link = target_name;
				    // get PID
					result = atoi(dir_entry_p->d_name);
					/// create linux process, add it to the vector
					addProcess(exe_link,result);
				}
				// a wine process, we need to do more checking and this may break is the future
				/// FIXME: this fails when the wine process isn't started from the 'current working directory'. strip path data from cmdline
				else if(strstr(target_name, "wine-preloader")!= NULL)
				{
				    cwd_link = dir_name + "cwd";
				    // get working directory
				    target_result = readlink(cwd_link.c_str(), target_name, sizeof(target_name)-1);
				    target_name[target_result] = 0;
				    // got path to executable, do the same for its name
				    cmdline_path = dir_name + "cmdline";
				    ifstream ifs ( cmdline_path.c_str() , ifstream::in );
				    getline(ifs,cmdline);
				    if (cmdline.find("dwarfort.exe") != string::npos || cmdline.find("Dwarf Fortress.exe") != string::npos)
				    {
				        // put executable name and path together
				        exe_link = target_name;
				        exe_link += "/";
				        exe_link += cmdline;
				        // get PID
				        result = atoi(dir_entry_p->d_name);
				        /// create wine process, add it to the vector
				        addProcess(exe_link,result);
				    }
				}
			}
		}
	}
	closedir(dir_p);
	// we have some precesses stored. nice.
	if(processes.size())
        return true;
    return false;
}

#else

/// some black magic
bool EnableDebugPriv()
{
   bool               bRET = FALSE;
   TOKEN_PRIVILEGES   tp;
   HANDLE             hToken;

   if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
   {
      if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
      {
         if (hToken != INVALID_HANDLE_VALUE)
         {
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            tp.PrivilegeCount = 1;
            if (AdjustTokenPrivileges(hToken, FALSE, &tp, 0, 0, 0))
               bRET = TRUE;
            CloseHandle(hToken);
         }
      }
   }
   return bRET;
}

/// WINDOWS version of the process finder
bool ProcessManager::findProcessess()
{
    // Get the list of process identifiers.
    ///TODO: make this dynamic. (call first to get the array size and second to really get process handles)
    DWORD ProcArray[512], memoryNeeded, numProccesses;
    HMODULE hmod = NULL;
    DWORD junk;
    HANDLE hProcess;
    bool found = false;

    IMAGE_NT_HEADERS32 pe_header;
	IMAGE_SECTION_HEADER sections[16];

    EnableDebugPriv();
    if ( !EnumProcesses( ProcArray, sizeof(ProcArray), &memoryNeeded ) )
        return false;

    // Calculate how many process identifiers were returned.
    numProccesses = memoryNeeded / sizeof(DWORD);

    // iterate through processes
    for ( int i = 0; i < numProccesses; i++ )
    {
        found = false;
        // open process
        hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, ProcArray[i] );
        if (NULL == hProcess)
            continue;
        // we've got some process, look at its first module
        if(EnumProcessModules(hProcess, &hmod, 1 * sizeof(HMODULE), &junk))
        {
            /// TODO: check module filename to verify that it's DF!
            // got base ;)
            uint32_t base = (uint32_t)hmod;
            // read from this process
            g_ProcessHandle = hProcess;
            uint32_t pe_offset = MreadDWord(base+0x3C);
            Mread(base + pe_offset                   , sizeof(pe_header), (uint8_t *)&pe_header);
            Mread(base + pe_offset+ sizeof(pe_header), sizeof(sections) , (uint8_t *)&sections );
            // see if there's a version entry that matches this process
            vector<memory_info>::iterator it;
            for ( it=meminfo.begin() ; it < meminfo.end(); it++ )
            {
                // filter by OS
                if(memory_info::OS_WINDOWS == (*it).getOS())
                {
                    uint32_t pe_timestamp = (*it).getHexValue("pe_timestamp");
                    if (pe_timestamp == pe_header.FileHeader.TimeDateStamp)
                    {
                        printf("Match found! Using version %s.\n", (*it).getVersion().c_str());
                        // give the process a data model and memory layout fixed to the base of first module
                        memory_info *m = new memory_info(*it);
                        m->Rebase(base);
                        // keep track of created memory_info objects so we can destroy them later
                        destroy_meminfo.push_back(m);
                        // process is responsible for destroying its data model
                        Process *ret= new Process(new DMWindows40d(),m,hProcess);
                        processes.push_back(ret);
                        found = true;
                        break; // break the iterator loop
                    }
                }
            }
            // close handle of processes that aren't DF
            if(!found)
            {
                CloseHandle(hProcess);
            }
        }
    }
	if(processes.size())
        return true;
    return false;
}
#endif


void ProcessManager::ParseVTable(TiXmlElement* vtable, memory_info& mem)
{
    TiXmlElement* pClassEntry;
    TiXmlElement* pClassSubEntry;
    const char * rebase = vtable->Attribute("rebase");
    if(rebase)
    {
        int32_t rebase_offset = strtol(rebase, NULL, 16);
        mem.RebaseVTable(rebase_offset);
    }
    pClassEntry = vtable->FirstChildElement();
    for(;pClassEntry;pClassEntry=pClassEntry->NextSiblingElement())
    {
        const char *cstr_type = pClassEntry->Value();
        const char *cstr_name = pClassEntry->Attribute("name");
        const char *cstr_vtable = pClassEntry->Attribute("vtable");
        string type = cstr_type;
        string name = cstr_name;
        string vtable = cstr_vtable;
        if(type== "class")
        {
            mem.setClass(name, vtable);
        }
        else if (type == "multiclass")
        {
            const char *cstr_typeoffset = pClassEntry->Attribute("typeoffset");
            string typeoffset = cstr_typeoffset;
            int mclass = mem.setMultiClass(name, vtable, typeoffset);
            pClassSubEntry = pClassEntry->FirstChildElement();
            for(;pClassSubEntry;pClassSubEntry=pClassSubEntry->NextSiblingElement())
            {
                cstr_type = pClassSubEntry->Value();
                type = cstr_type;
                if(type== "class")
                {
                    cstr_name = pClassSubEntry->Attribute("name");
                    const char *cstr_value = pClassSubEntry->Attribute("type");
                    name = cstr_name;
                    string value = cstr_value;
                    mem.setMultiClassChild(mclass,name,value);
                }
            }
        }
    }
}



void ProcessManager::ParseEntry (TiXmlElement* entry, memory_info& mem, map <string ,TiXmlElement *>& knownEntries)
{
    TiXmlElement* pMemEntry;
    const char *cstr_version = entry->Attribute("version");
    const char *cstr_os = entry->Attribute("os");
    const char *cstr_base = entry->Attribute("base");
    const char *cstr_rebase = entry->Attribute("rebase");
    if(cstr_base)
    {
        string base = cstr_base;
        ParseEntry(knownEntries[base], mem, knownEntries);
    }
    // mandatory attributes missing?
    if(!(cstr_version && cstr_os))
    {
        cerr << "Bad entry in memory.xml detected, version or os attribute is missing.";
        // skip if we don't have valid attributes
        return;
    }
    string os = cstr_os;
    mem.setVersion(cstr_version);
    mem.setOS(cstr_os);

    // offset inherited addresses by 'rebase'.
    int32_t rebase = 0;
    if(cstr_rebase)
    {
        rebase = mem.getBase() + strtol(cstr_rebase, NULL, 16);
        mem.Rebase(rebase);
    }

    //set base to default, we're overwriting this because the previous rebase could cause havoc on Vista/7
    if(os == "windows")
    {
        // set default image base. this is fixed for base relocation later
        mem.setBase(0x400000);
    }
    else if(os == "linux")
    {
        // this is wrong... I'm not going to do base image relocation on linux though.
        // users are free to use a sane kernel that doesn't do this kind of ****
        mem.setBase(0x0);
    }
    else
    {
        cerr << "unknown operating system " << os << endl;
        return;
    }
    // process additional entries
    cout << "Entry " << cstr_version << " " <<  cstr_os << endl;
    pMemEntry = entry->FirstChildElement()->ToElement();
    for(;pMemEntry;pMemEntry=pMemEntry->NextSiblingElement())
    {
        // only elements get processed
        const char *cstr_type = pMemEntry->Value();
        const char *cstr_name = pMemEntry->Attribute("name");
        const char *cstr_value = pMemEntry->GetText();
        // check for missing parts
        string type, name, value;
        type = cstr_type;
        if(type == "VTable")
        {
            ParseVTable(pMemEntry, mem);
            continue;
        }
        if( !(cstr_name && cstr_value))
        {
            cerr << "underspecified MemInfo entry" << endl;
            continue;
        }
        name = cstr_name;
        value = cstr_value;
        if (type == "HexValue")
        {
            mem.setHexValue(name, value);
        }
        else if (type == "Address")
        {
            mem.setAddress(name, value);
        }
        else if (type == "Offset")
        {
            mem.setOffset(name, value);
        }
        else if (type == "String")
        {
            mem.setString(name, value);
        }
        else
        {
            cerr << "Unknown MemInfo type: " << type << endl;
        }
    } // for
} // method


/// OS independent part
bool ProcessManager::loadDescriptors(string path_to_xml)
{
    TiXmlDocument doc( path_to_xml.c_str() );
    bool loadOkay = doc.LoadFile();
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
    memory_info mem;

    if ( loadOkay )
    {
        // block: name
        {
            pElem=hDoc.FirstChildElement().Element();
            // should always have a valid root but handle gracefully if it does
            if (!pElem)
            {
                cerr << "no pElem found" << endl;
                return false;
            }
            string m_name=pElem->Value();
            if(m_name != "DFExtractor")
            {
                cerr << "DFExtractor != " << m_name << endl;
                return false;
            }
            cout << "got DFExtractor XML!" << endl;
            // save this for later
            hRoot=TiXmlHandle(pElem);
        }
        // transform elements
        {
            meminfo.clear(); // trash existing list

            TiXmlElement* pMemInfo=hRoot.FirstChild( "MemoryDescriptors" ).FirstChild( "Entry" ).Element();
            map <string ,TiXmlElement *> map_pNamedEntries;
            vector <TiXmlElement *> v_pEntries;
            for( ; pMemInfo; pMemInfo=pMemInfo->NextSiblingElement("Entry"))
            {
                v_pEntries.push_back(pMemInfo);
                const char *id;
                if(id= pMemInfo->Attribute("id"))
                {
                    string str_id = id;
                    map_pNamedEntries[str_id] = pMemInfo;
                }
            }
            for(uint32_t i = 0; i< v_pEntries.size();i++)
            {
                memory_info mem;
                ///FIXME: add a set of entries processed in a step of this cycle, use it to check for infinite loops
                /* recursive */ParseEntry( v_pEntries[i] , mem , map_pNamedEntries);
                meminfo.push_back(mem);
            }
            // process found things here
        }
        return true;
    }
    else
    {
        // load failed
        cerr << "Can't load memory offsets from memory.xml" << endl;
        return false;
    }
/*
    meminfo.clear();
    Path file = "Assets\\Maps\\memory.ini";
    string line;
    ifstream inFile;
    inFile.open(file);
    if (!inFile)
    {
        cerr << "Unable to open file " << file << endl;
        return false;
    }
    memory_info mem;
    while(std::getline(inFile, line))
    {
        vector<string> tokens; // tokens go here
        // trim crap
        string t1 = trim(line);
        // ignore comments
        if(t1[0] == ';') continue;
        // we got data
        tokenize(t1,tokens,"=");
        if(tokens.size() != 2)
        {
//            cerr << "malformed line: " << line << endl;
            continue;
        }
        // assume two tokens
        string ta = trim(tokens[0]);
        string tb = trim(tokens[1]);
        if(ta == "version")// we got a version line, this means a new entry
        {
            if(mem.hasToken("version"))// we have one already
            {
                // push the sucker on the vector
                meminfo.push_back(mem);
            }
            //make a clean new mem
            mem.flush();
            mem.setToken(ta,tb);// set its version
        }
        else
        {
            mem.setToken(ta,tb);// ordinary line with a '=', add
        }
    }
    if(mem.hasToken("version")) // one last entry remaining
        meminfo.push_back(mem); // add it to the vector
    return true;*/
}

uint32_t ProcessManager::size()
{
    return processes.size();
};
ProcessManager::Process * ProcessManager::operator[](uint32_t index)
{
    assert(index < processes.size());
    return processes[index];
};
ProcessManager::ProcessManager( string path_to_xml )
{
    currentProcess = NULL;
    currentProcessHandle = 0;
    loadDescriptors( path_to_xml );
}
ProcessManager::~ProcessManager()
{
    // delete all processes
    for(uint32_t i = 0;i < processes.size();i++)
    {
        delete processes[i];
    }
    //delete all generated memory_info stuff
    for(uint32_t i = 0;i < destroy_meminfo.size();i++)
    {
        delete destroy_meminfo[i];
    }
}
