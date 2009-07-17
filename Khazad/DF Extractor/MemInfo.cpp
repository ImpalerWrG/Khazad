#include <stdint.h>
#include <map>
#include <string>
using namespace std;
#include "MemInfo.h"
#include <stdlib.h>

uint32_t memory_info::getOffset (string key)
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
