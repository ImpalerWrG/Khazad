/**
 * DO NOT USE THIS FILE DIRECTLY! USE MemAccess.h INSTEAD!
 */
#include <stdint.h>

inline
uint8_t MreadByte (const uint32_t &offset)
{
    uint8_t val;
    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( &val, sizeof(uint8_t), 1, g_ProcessMemFile );
    return val;
}

inline
void MreadByte (const uint32_t &offset, uint8_t &val )
{
    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( &val, sizeof(uint8_t), 1, g_ProcessMemFile );
}

inline
uint16_t MreadWord (const uint32_t &offset)
{
    uint16_t val;
    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( &val, sizeof(uint16_t), 1, g_ProcessMemFile );
    return val;
}

inline
void MreadWord (const uint32_t &offset, uint16_t &val)
{
    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( &val, sizeof(uint16_t), 1, g_ProcessMemFile );
}

inline
uint32_t MreadDWord (const uint32_t &offset)
{
    uint32_t val;
    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( &val, sizeof(uint32_t), 1, g_ProcessMemFile );
    return val;
}
inline
void MreadDWord (const uint32_t &offset, uint32_t &val)
{
    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( &val, sizeof(uint32_t), 1, g_ProcessMemFile );
}

inline
uint64_t MreadQuad (const uint32_t &offset)
{
    uint64_t val;
    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( &val, sizeof(uint32_t), 1, g_ProcessMemFile );
    return val;
}

inline
void MreadQuad (const uint32_t &offset, uint64_t &val)
{
    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( &val, sizeof(uint32_t), 1, g_ProcessMemFile );
}

inline
void Mread (const uint32_t &offset, const uint32_t &size, uint8_t *target)
{
    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( target, 1, size, g_ProcessMemFile );
}

inline
void MwriteDWord (uint32_t offset, uint32_t data)
{
    ptrace(PTRACE_POKEDATA,g_ProcessHandle, offset, data);
}

// using these is expensive.
inline
void MwriteWord (uint32_t offset, uint16_t data)
{
    uint32_t orig = MreadDWord(offset);
    orig |= 0x0000FFFF;
    orig &= data;
    ptrace(PTRACE_POKEDATA,g_ProcessHandle, offset, orig);
}

inline
void MwriteByte (uint32_t offset, uint8_t data)
{
    uint32_t orig = MreadDWord(offset);
    orig |= 0x000000FF;
    orig &= data;
    ptrace(PTRACE_POKEDATA,g_ProcessHandle, offset, orig);
}

// blah. I hate the kernel devs for crippling /proc/PID/mem. THIS IS RIDICULOUS
inline
bool Mwrite (uint32_t offset, uint32_t size, uint8_t *source)
{
    uint32_t indexptr = 0;
    while (size > 0)
    {
        // default: we push 4 bytes
        if(size >= 4)
        {
            MwriteDWord(offset, *(uint32_t *) (source + indexptr));
            offset +=4;
            indexptr +=4;
            size -=4;
        }
        // last is either three or 2 bytes
        else if(size >= 2)
        {
            MwriteWord(offset, *(uint16_t *) (source + indexptr));
            offset +=2;
            indexptr +=2;
            size -=2;
        }
        // finishing move
        else if(size == 1)
        {
            MwriteByte(offset, *(uint8_t *) (source + indexptr));
            return true;
        }
    }
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
