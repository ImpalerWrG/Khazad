/**
 * DO NOT USE THIS DIRECTLY!
 */
inline
uint8_t MreadByte (uint32_t offset)
{
    uint8_t result;
    ReadProcessMemory(g_ProcessHandle, (int*) offset, &result, sizeof(uint8_t), NULL);
    return result;
}
inline
uint16_t MreadWord (uint32_t offset)
{
    uint16_t result;
    ReadProcessMemory(g_ProcessHandle, (int*) offset, &result, sizeof(uint16_t), NULL);
    return result;
}
inline
uint32_t MreadDWord (uint32_t offset)
{
    uint32_t result;
    ReadProcessMemory(g_ProcessHandle, (int*) offset, &result, sizeof(uint32_t), NULL);
    return result;
}
inline
uint64_t MreadQuad (uint32_t offset)
{
    uint64_t result;
    ReadProcessMemory(g_ProcessHandle, (int*) offset, &result, sizeof(uint64_t), NULL);
    return result;
}
inline
bool Mread (uint32_t offset, uint32_t size, uint8_t *target)
{
    ReadProcessMemory(g_ProcessHandle, (int*) offset, target, size, NULL);
    return true;
}
inline
const string MreadCString (uint32_t offset)
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
