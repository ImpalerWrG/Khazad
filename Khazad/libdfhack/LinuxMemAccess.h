/**
 * DO NOT USE THIS FILE DIRECTLY!
 */

inline
uint8_t MreadByte (uint32_t offset)
{
    uint8_t val;
    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( &val, sizeof(uint8_t), 1, g_ProcessMemFile );
    return val;
}
inline
uint16_t MreadWord (uint32_t offset)
{
    uint16_t val;
    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( &val, sizeof(uint16_t), 1, g_ProcessMemFile );
    return val;
}
inline
uint32_t MreadDWord (uint32_t offset)
{
    uint32_t val;
    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( &val, sizeof(uint32_t), 1, g_ProcessMemFile );
    return val;
}
inline
uint64_t MreadQuad (uint32_t offset)
{
    uint64_t val;
    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( &val, sizeof(uint32_t), 1, g_ProcessMemFile );
    return val;
}
inline
bool Mread (uint32_t offset, uint32_t size, uint8_t *target)
{

    fseek(g_ProcessMemFile, offset,SEEK_SET);
    fread ( target, 1, size, g_ProcessMemFile );
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
