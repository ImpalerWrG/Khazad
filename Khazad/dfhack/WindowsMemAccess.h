/**
 * DO NOT USE THIS FILE DIRECTLY! USE MemAccess.h INSTEAD!
 */

    /*
    char buffer[256];
    DWORD oldProtect = 0;
    DWORD numRead = 0;
    VirtualProtectEx( hProc, (LPVOID)0x77810F34, 256, PAGE_EXECUTE_READWRITE, &oldProtect );
    ReadProcessMemory( hProc, (LPVOID)0x77810F34, buffer, 256, &numRead );
    VirtualProtectEx( hProc, (LPVOID)0x77810F34, 256, oldProtect, NULL ); //restore the original protection when you're done
    */



inline
uint8_t MreadByte (const uint32_t &offset)
{
    uint8_t result;
    ReadProcessMemory(g_ProcessHandle, (int*) offset, &result, sizeof(uint8_t), NULL);
    return result;
}


inline
void MreadByte (const uint32_t &offset,uint8_t &result)
{
    ReadProcessMemory(g_ProcessHandle, (int*) offset, &result, sizeof(uint8_t), NULL);
}


inline
uint16_t MreadWord (const uint32_t &offset)
{
    uint16_t result;
    ReadProcessMemory(g_ProcessHandle, (int*) offset, &result, sizeof(uint16_t), NULL);
    return result;
}


inline
void MreadWord (const uint32_t &offset, uint16_t &result)
{
    ReadProcessMemory(g_ProcessHandle, (int*) offset, &result, sizeof(uint16_t), NULL);
}


inline
uint32_t MreadDWord (const uint32_t &offset)
{
    uint32_t result;
    ReadProcessMemory(g_ProcessHandle, (int*) offset, &result, sizeof(uint32_t), NULL);
    return result;
}


inline
void MreadDWord (const uint32_t &offset, uint32_t &result)
{
    ReadProcessMemory(g_ProcessHandle, (int*) offset, &result, sizeof(uint32_t), NULL);
}


inline
uint64_t MreadQuad (const uint32_t &offset)
{
    uint64_t result;
    ReadProcessMemory(g_ProcessHandle, (int*) offset, &result, sizeof(uint64_t), NULL);
    return result;
}


inline
void MreadQuad (const uint32_t &offset, uint64_t &result)
{
    ReadProcessMemory(g_ProcessHandle, (int*) offset, &result, sizeof(uint64_t), NULL);
}


inline
void Mread (const uint32_t &offset, uint32_t size, uint8_t *target)
{
    ReadProcessMemory(g_ProcessHandle, (int*) offset, target, size, NULL);
}


///FIXME: reduce use of temporary objects
inline
const string MreadCString (const uint32_t &offset)
{
    string temp;
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
