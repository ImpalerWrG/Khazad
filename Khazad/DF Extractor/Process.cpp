#include <Process.h>
#include <Paths.h>
#include <fstream>
#include <StringMagic.h>
#include <DfVector.h>

///TODO: move these out of the way, too. they belong into their own file
Uint32 memory_info::getOffset (string key)
{
    if(hasToken(key))
     return strtoul(data[key].c_str(),NULL,16);
    return 0;
}
std::string memory_info::getString (string key)
{
    if(hasToken(key))
        return data[key];
    else return string("");
}
bool memory_info::hasToken (string key)
{
    return data.count(key);
}
bool memory_info::setToken (string key, string dat)
{
    data[key] = dat;
    return true;
}
void memory_info::flush()
{
    data.clear();
}

DECLARE_SINGLETON(Process)


Process::Process()
{
    attached = false;
}

Process::~Process()
{
}

bool Process::Init()
{
    // random code for testing
    /*
    if(attach())
    cout << "Success!!!!!!" <<endl;
    else
    cout << "Fail D:" <<endl;
    */
    return true;
}

// process the .ini file
bool Process::loadDescriptors()
{
    meminfo.clear();
    Path file = Path("Assets\\Maps\\memory.ini");
    string line;
    ifstream inFile;
    inFile.open(file);
    if (!inFile)
    {
        cerr << "Unable to open file datafile.txt";
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
            cerr << "malformed line: " << line << endl;
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
    return true;
}
#ifdef LINUX_BUILD
/**
 *     LINUX PART
 */

bool Process::getProcessINFO(char *p_processname)
{
	DIR *dir_p;
	struct dirent *dir_entry_p;
	char dir_name[40];										// ??? buffer overrun potential
	char target_name[252];									// ??? buffer overrun potential
	int target_result;
	char exe_link[252];
	int errorcount;
	int result;

	errorcount=0;
	result=0;
	dir_p = opendir("/proc/"); 																// Open /proc/ directory
	while(NULL != (dir_entry_p = readdir(dir_p))) {											// Reading /proc/ entries
		if (strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name))		// Checking for numbered directories
		{
			strcpy(dir_name, "/proc/");
			strcat(dir_name, dir_entry_p->d_name);
            // Obtaining the full-path eg: /proc/24657/
			strcat(dir_name, "/");
			exe_link[0] = 0;
			strcat(exe_link, dir_name);
			// Getting the full-path of that exe link
			strcat(exe_link, "exe");
			exe = exe_link; /// assign this to the member string variable, so we can do md5 on it
            // Getting the target of the exe ie to which binary it points to
			target_result = readlink(exe_link, target_name, sizeof(target_name)-1);
			if (target_result > 0)
			{
				target_name[target_result] = 0;
				// Searching for process name in the target name -- ??? could be a better search !!!
				if (strstr(target_name, p_processname) != NULL)
				{
					result = atoi(dir_entry_p->d_name);
					printf("getProcessID(%s) :Found. id = %d\n", p_processname, result);
					closedir(dir_p);
					PID = result;/// assign this to the member pid_t variable, so we can use it for ptrace
					return true;
				}
			}
		}
	}
	closedir(dir_p);
	printf("getProcessID(%s) : id = 0 (could not find process)\n", p_processname);
	return false;
};

bool Process::attach()
{
    /// TODO: check for errors!
    loadDescriptors(); // load/reload descriptors
    if(!getProcessINFO("dwarfort.exe")) return false; // DF isn't running
    // PID and exe are filled now.
    string hash = MD5Sum((char *)exe.c_str());// get hash of the running DF process
    // iterate over the list of memory locations
    vector<memory_info>::iterator it;
    for ( it=meminfo.begin() ; it < meminfo.end(); it++ )
    {
        if("linux" == (*it).getString("system")) // is this a linux entry?
        {
            cout << "Found a linux entry :D." << endl;
            if(hash == (*it).getString("md5")) // are the md5 hashes the same?
            {
                // bingo.
                cout << "Found binary." << endl;
                current_descriptor = &*it; // set current descriptor for others to play with
                // let's try some BLACK MAGIC
                ptrace(PTRACE_ATTACH , PID, NULL, NULL);
                wait(NULL); // wait for DF to be stopped.
                attached = true;
                return true; // we are attached
            }
        }
    }
    return false; // unknown binary or error. it's crap.
}
bool Process::detach()
{
    /// TODO: check for errors.
    ptrace(PTRACE_DETACH, PID, NULL, NULL);
    attached = false;
    current_descriptor = NULL; // it's better to be sure noone tries messing with that
    return true;
}

Uint8 Process::readByte (Uint32 offset)
{
    if(isValid())
    {
     Uint32 val = ptrace(PTRACE_PEEKDATA,PID, offset, NULL);
     Uint8 cropped;
     memcpy(&cropped, &val,sizeof(Uint8));
     return cropped;
    }
    return 0; // we fail with zeros, user should check by calling isValid first.
}

Uint16 Process::readWord (Uint32 offset)
{
    if(isValid())
    {
     Uint32 val = ptrace(PTRACE_PEEKDATA,PID, offset, NULL);
     Uint16 cropped;
     memcpy(&cropped, &val,sizeof(Uint16));
     return cropped;
    }
    return 0; // we fail with zeros, user should check by calling isValid first.
}

Uint32 Process::readDWord (Uint32 offset)
{
    if(isValid())
    {
     Uint32 val = ptrace(PTRACE_PEEKDATA,PID, offset, NULL);
     return val;
    }
    return 0; // we fail with zeros, user should check by calling isValid first.
}

Uint64 Process::readQuad (Uint32 offset)
{
    if(isValid())
    {
     Uint32 val1 = ptrace(PTRACE_PEEKDATA,PID, offset, NULL);
     Uint32 val2 = ptrace(PTRACE_PEEKDATA,PID, offset+4, NULL);
     Uint64 ret;
     memcpy(&ret,&val1,sizeof(Uint32));
     memcpy((&ret) + 4,&val2,sizeof(Uint32));
     return ret;
    }
    return 0; // we fail with zeros, user should check by calling isValid first.
}

bool Process::read (Uint32 offset, Uint32 size, Uint8 *target)
{
    Uint8 *mover = target;
    Uint32 offseter = offset;
    while (size)
    {
        if(size >= 4)
        {
            * (Uint32 *)mover = readDWord(offseter);
            mover+=4;
            offseter +=4;
            size -=4;
        }
        else if(size >= 2)
        {
            * (Uint16 *)mover = readWord(offseter);
            mover+=2;
            offseter +=2;
            size -=2;
        }
        else if(size == 1)
        {
            * (Uint8 *)mover = readByte(offseter);
            mover+=1;
            offseter ++;
            size --;
        }
    }
    return true;
}
// read a vector from memory
DfVector Process::readVector (Uint32 offset, Uint32 item_size)
{
    /**
    GNU libstdc++ vector is three pointers long
    ptr start
    ptr end
    ptr alloc_end

    we don't care about alloc_end because we don't try to add stuff
    */
    Uint32 start = readDWord(offset);
    Uint32 end = readDWord(offset+4);
    Uint32 size = (end - start) /4;
    return DfVector(start,size,item_size);
}
const string Process::readSTLString (Uint32 offset)
{
    //GNU std::string is a single pointer
    offset = readDWord(offset);
    return readCString(offset);
}
#else
/**
 *     WINDOWS PART
 */
bool Process::getProcessINFO(char *p_processname)
{
    DFProcess = FindWindow(NULL,p_processname);
    if(!DFProcess)
    {
        printf("Cannot find window with name \"%s\"\n", p_processname);
        return false;
    }

    // And Create Handle
    GetWindowThreadProcessId(DFProcess, &DFProcessID);
    printf("Window Thread Process ID [%u] \n", (unsigned int)DFProcessID);
    DFHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, DFProcessID);

    if(!DFHandle)
    {
        printf("Could not get application handle to hook. \n");
        return false;
    }
    return true;
}

bool Process::attach()
{
    /// TODO: check for errors!
    loadDescriptors();
    if(!getProcessINFO("Dwarf Fortress")) return false; // DF isn't running
        int Base = 0x400000;

    // New Method
    int TempOffset;
    ReadProcessMemory(DFHandle, (int*) (Base + 60), &TempOffset, sizeof(int), NULL);

    int TimeStamp;
    ReadProcessMemory(DFHandle, (int*) (Base + TempOffset + 8), &TimeStamp, sizeof(int), NULL);

    printf("TimeStampFound [%x] at Offset [%x].\n", TimeStamp, Base + TempOffset + 8);


    vector<memory_info>::iterator it;
    for ( it=meminfo.begin() ; it < meminfo.end(); it++ )
    {
        if("windows" == (*it).getString("system")) // is this a windows entry?
        {
            Uint32 pe_timestamp = (*it).getOffset("pe_timestamp");
            // Old Method
            int OldTimeStamp;
            ReadProcessMemory(DFHandle, (int*) (*it).getOffset("pe_offset"), &OldTimeStamp, sizeof(int), NULL);
            if (pe_timestamp == TimeStamp || pe_timestamp == OldTimeStamp)
            {
                printf("Match found! Using version %s.\n", (*it).getString("version").c_str());
                current_descriptor = &*it;
                attached = true;
                return true;
            }
        }
    }
    return false;
}
bool Process::detach()
{
    attached = false;
    // nothing to do here, we are not a debbuger on Windows
    return true;
}

Uint8 Process::readByte (Uint32 offset)
{
    if(isValid())
    {
     Uint8 result;
     ReadProcessMemory(DFHandle, (int*) offset, &result, sizeof(Uint8), NULL);
     return result;
    }
    return 0; // we fail with zeros, user should check by calling isValid first.
}

Uint16 Process::readWord (Uint32 offset)
{
    if(isValid())
    {
     Uint16 result;
     ReadProcessMemory(DFHandle, (int*) offset, &result, sizeof(Uint16), NULL);
     return result;
    }
    return 0; // we fail with zeros, user should check by calling isValid first.
}

Uint32 Process::readDWord (Uint32 offset)
{
    if(isValid())
    {
     Uint32 result;
     ReadProcessMemory(DFHandle, (int*) offset, &result, sizeof(Uint32), NULL);
     return result;
    }
    return 0; // we fail with zeros, user should check by calling isValid first.
}

Uint64 Process::readQuad (Uint32 offset)
{
    if(isValid())
    {
     Uint64 result;
     ReadProcessMemory(DFHandle, (int*) offset, &result, sizeof(Uint64), NULL);
     return result;
    }
    return 0; // we fail with zeros, user should check by calling isValid first.
}

// This is a terrible cludge. I need to get the memory remapping working.
bool Process::read (Uint32 offset, Uint32 size, Uint8 *target)
{
    if(isValid())
    {
        Uint64 result;
        ReadProcessMemory(DFHandle, (int*) offset, target, size, NULL);
        return true;
    }
    return false; // we fail with zeros, user should check by calling isValid first.
}

// read a vector from memory
DfVector Process::readVector (Uint32 offset, Uint32 item_size)
{
    /**
    MSVC++ vector is four pointers long
    ptr allocator
    ptr start
    ptr end
    ptr alloc_end

    we don't care about alloc_end because we don't try to add stuff
    we also don't care about the allocator thing in front
    */
    Uint32 start = readDWord(offset+4);
    Uint32 end = readDWord(offset+8);
    Uint32 size = (end - start) /4;
    return DfVector(start,size,item_size);
}

const string Process::readSTLString (Uint32 offset)
{
    /**
    MSVC++ string
    ptr allocator
    union{
        char[16] start;
        char * start_ptr
    }
    Uint32 length
    Uint32 capacity
    */
    Uint32 start_offset = offset + 4;
    Uint32 length = readDWord(offset + 20);
    Uint32 capacity = readDWord(offset + 24);
    char * temp = new char[capacity+1];
    // read data from inside the string structure
    if(capacity < 16)
    {
        read(start_offset, capacity, (Uint8 *)temp);
    }
    else // read data from what the offset + 4 dword points to
    {
        start_offset = readDWord(start_offset);// dereference the start offset
        read(start_offset, capacity, (Uint8 *)temp);
    }
    temp[length] = 0;
    string ret = temp;
    delete temp;
    return ret;
}

#endif

const string Process::readCString (Uint32 offset)
{
    string temp;
    char temp_c[256];
    int counter = 0;
    char r;
    do
    {
        r = readByte(offset+counter);
        temp_c[counter] = r;
        counter++;
    } while (r);
    temp_c[counter] = 0;
    temp = temp_c;
    return temp;
}

// get current memory descriptor. NULL if not attached
memory_info * Process::getDescriptor()
{
    if (isValid()) return current_descriptor;
    return NULL;
}

bool Process::isValid()
{
    ///TODO: check for weird states here - like crashed DF and similar crap
    return attached; // valid when attached
}

