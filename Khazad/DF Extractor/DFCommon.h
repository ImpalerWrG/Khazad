#ifndef DFCOMMON_H_INCLUDED
#define DFCOMMON_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <fstream>
using namespace std;
#include <stdint.h>
#include <assert.h>
#include <string.h>

#ifdef LINUX_BUILD
#include <sys/types.h>
#include <sys/ptrace.h>
#else
#include <windows.h>
#include <winnt.h>
#endif

#include "DataModel.h"
#include "ProcessManager.h"
#include "MemAccess.h"
#include "DfVector.h"
#include "Process.h"
#include "DfMap.h"


#endif // DFCOMMON_H_INCLUDED
