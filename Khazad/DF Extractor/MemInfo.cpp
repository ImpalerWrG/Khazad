#include "DFCommon.h"
#include "MemInfo.h"
#include <stdlib.h>

///TODO: turn into XML. This ini format is stretched beyond the breaking point and the interface is hard to use!!!

memory_info::memory_info()
{
    rebase = 0;
}
// copy constructor
memory_info::memory_info(const memory_info &old)
{
    rebase = old.rebase;
    data = old.data;
}

void memory_info::setRebase(int32_t old_base,int32_t new_base)
{
    printf("oldbase: 0x%X\n",old_base);
    printf("newbase: 0x%X\n",new_base);
    rebase = - old_base + new_base;
    if(rebase <0)
        printf("rebase: -0x%X = %d\n",-rebase,rebase);
    else
        printf("rebase:  0x%X = %d\n",rebase,rebase);
}

uint32_t memory_info::getBase (string key)
{
    if(hasToken(key))
    {
        int64_t evil = strtoul(data[key].c_str(),NULL,16);
        evil += rebase;
        return evil;
    }
    return 0;
}

uint32_t memory_info::getOffset (string key)
{
    if(hasToken(key))
    {
        return strtoul(data[key].c_str(),NULL,16);
    }
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
