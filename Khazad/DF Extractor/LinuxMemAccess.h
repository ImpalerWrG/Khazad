/**
 * DO NOT USE THIS DIRECTLY!
 */

/// FIXME: PURGE!
#include <stdint.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <sys/types.h>
using namespace std;
#include <ProcessManager.h>
#include <sys/ptrace.h>
#include <string.h>
inline
uint8_t MreadByte (uint32_t offset)
{
    uint32_t val = ptrace(PTRACE_PEEKDATA,g_ProcessHandle, offset, NULL);
    uint8_t cropped;
    memcpy(&cropped, &val,sizeof(uint8_t));
    return cropped;
}
inline
uint16_t MreadWord (uint32_t offset)
{
    uint32_t val = ptrace(PTRACE_PEEKDATA,g_ProcessHandle, offset, NULL);
    uint16_t cropped;
    memcpy(&cropped, &val,sizeof(uint16_t));
    return cropped;
}
inline
uint32_t MreadDWord (uint32_t offset)
{
    uint32_t val = ptrace(PTRACE_PEEKDATA,g_ProcessHandle, offset, NULL);
    return val;
}
inline
uint64_t MreadQuad (uint32_t offset)
{
    uint32_t val1 = ptrace(PTRACE_PEEKDATA,g_ProcessHandle, offset, NULL);
    uint32_t val2 = ptrace(PTRACE_PEEKDATA,g_ProcessHandle, offset+4, NULL);
    uint64_t ret;
    memcpy(&ret,&val1,sizeof(uint32_t));
    memcpy((&ret) + 4,&val2,sizeof(uint32_t));
    return ret;
}
inline
bool Mread (uint32_t offset, uint32_t size, uint8_t *target)
{
    uint8_t *mover = target;
    uint32_t offseter = offset;
    while (size)
    {
        if(size >= 4)
        {
            * (uint32_t *)mover = MreadDWord(offseter);
            mover+=4;
            offseter +=4;
            size -=4;
        }
        else if(size >= 2)
        {
            * (uint16_t *)mover = MreadWord(offseter);
            mover+=2;
            offseter +=2;
            size -=2;
        }
        else if(size == 1)
        {
            * (uint8_t *)mover = MreadByte(offseter);
            mover+=1;
            offseter ++;
            size --;
        }
    }
    return true;
}
inline
const std::string MreadCString (uint32_t offset)
{
    std::string temp;
    char temp_c[256];
    int counter = 0;
    char r;
    do
    {
        r = MreadByte(offset+counter);
        temp_c[counter] = r;
        counter++;
    } while (r);
    temp_c[counter] = 0;
    temp = temp_c;
    return temp;
}
