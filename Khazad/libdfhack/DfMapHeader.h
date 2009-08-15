#ifndef DF_MAP_HEADER_H
#define DF_MAP_HEADER_H

static const char dmh_id[] = "!!URIST!!";
static const uint8_t dmh_ver = 1U;

// a header for save files
struct DfMapHeader
{
    char identifier[10]; // !!URIST!!
    uint8_t version; // DfMap/Header version; current: 1
    uint32_t reserved; // reserved 4 bytes
};


#endif // DF_MAP_HEADER_H

