#include "DFCommon.h"
#include "MemInfo.h"
#include <stdlib.h>

memory_info::memory_info()
{
    base = 0;
}

void memory_info::setVersion(const char * v)
{
    version = v;
}
void memory_info::setVersion(string v)
{
    version = v;
}
string memory_info::getVersion()
{
    return version;
}


void memory_info::setOS(const char *os)
{
    string oss = os;
    if(oss == "windows")
        OS = OS_WINDOWS;
    else if(oss == "linux")
        OS = OS_LINUX;
    else
        OS = OS_BAD;
}
void memory_info::setOS(string os)
{
    if(os == "windows")
        OS = OS_WINDOWS;
    else if(os == "linux")
        OS = OS_LINUX;
    else
        OS = OS_BAD;
}
void memory_info::setOS(OSType os)
{
    if(os >= OS_WINDOWS && os < OS_BAD)
    {
        OS = os;
        return;
    }
    OS = OS_BAD;
}
memory_info::OSType memory_info::getOS()
{
    return OS;
}

// copy constructor
memory_info::memory_info(const memory_info &old)
{
    version = old.version;
    OS = old.OS;
    addresses = old.addresses;
    offsets = old.offsets;
    hexvals = old.hexvals;
    strings = old.strings;
    base = old.base;
}

uint32_t memory_info::getBase ()
{
    return base;
}
void memory_info::setBase (string s)
{
    base = strtol(s.c_str(), NULL, 16);
}
void memory_info::setBase (uint32_t b)
{
    base = b;
}

void memory_info::setOffset (string key, string value)
{
    uint32_t offset = strtol(value.c_str(), NULL, 16);
    offsets[key] = offset;
}
void memory_info::setAddress (string key, string value)
{
    uint32_t address = strtol(value.c_str(), NULL, 16);
    addresses[key] = address;
}
void memory_info::setHexValue (string key, string value)
{
    uint32_t hexval = strtol(value.c_str(), NULL, 16);
    hexvals[key] = hexval;
}
void memory_info::setString (string key, string value)
{
    strings[key] = value;
}

// change base of all addresses
void memory_info::Rebase(int32_t new_base)
{
  map<string, uint32_t>::iterator iter;

  for(iter = addresses.begin(); iter != addresses.end(); iter++)
  {
      int32_t rebase = - (int32_t)base + new_base;
      addresses[iter->first] = iter->second + rebase;
  }
}

uint32_t memory_info::getAddress (string key)
{
    if(addresses.count(key))
    {
        return addresses[key];
    }
    return 0;
}

uint32_t memory_info::getOffset (string key)
{
    if(offsets.count(key))
    {
        return offsets[key];
    }
    return 0;
}
std::string memory_info::getString (string key)
{
    if(strings.count(key))
        return strings[key];
    else return string("");
}

uint32_t memory_info::getHexValue (string key)
{
    if(hexvals.count(key))
        return hexvals[key];
    else 0;
}
void memory_info::flush()
{
    base = 0;
    addresses.clear();
    offsets.clear();
    strings.clear();
    hexvals.clear();
}
