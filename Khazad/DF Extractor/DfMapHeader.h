#ifndef DF_MAP_HEADER_H
#define DF_MAP_HEADER_H

#include <string>

static const char dmh_id[] = "!!URIST!!";
static const Uint8 dmh_ver = 1U;

// a header for the DfMap structure
// size:
struct DfMapHeader
{
    char identifier[10]; // !!URIST!!
    Uint8 version; // DfMap/Header version; current: 1

    Uint32 map_data_location; // where does the data start
    Uint32 tile_block_count; // how many tile blocks to read from the data location

    Uint32 x_block_count, y_block_count, z_block_count; // DF's map block count

//    DfMapHeader()
//    :   identifier("!!URIST!!"),
//        version(1U)
//    {
//    }
};


#endif // DF_MAP_HEADER_H

