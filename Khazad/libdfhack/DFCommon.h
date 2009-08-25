#ifndef DFCOMMON_H_INCLUDED
#define DFCOMMON_H_INCLUDED

///TODO: separate into extrenal and internal

#include <string>
#include <vector>
#include <map>
//#include <boost/bimap/bimap.hpp>
//using namespace boost::bimaps;

#include <fstream>
using namespace std;
#include <stdint.h>
#include <assert.h>
#include <string.h>

#ifdef LINUX_BUILD
#include <sys/types.h>
#include <sys/ptrace.h>
#include <dirent.h>
#else
#define WINVER 0x0500					// OpenThread(), PSAPI, Toolhelp32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winnt.h>
#include <psapi.h>
#endif

#include "Types.h"
#include "DataModel.h"
#include "ProcessManager.h"
#include "MemAccess.h"
#include "DfVector.h"
#include "DfMap.h"


#endif // DFCOMMON_H_INCLUDED
