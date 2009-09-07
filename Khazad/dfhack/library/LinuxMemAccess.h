/**
 * DO NOT USE THIS FILE DIRECTLY! USE MemAccess.h INSTEAD!
 */

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
