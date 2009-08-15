#ifndef DF_MAP_HEADER_H
#define DF_MAP_HEADER_H

static const char dmh_id[] = "!!URIST!!";
static const uint8_t dmh_ver = 1U;

// a header for the DfMap structure
// size:
struct DfMapHeader
{
    char identifier[10]; // !!URIST!!
    uint8_t version; // DfMap/Header version; current: 1

    uint32_t reserved; // reserved 4 bytes
    /*uint32_t tile_block_count; // how many tile blocks to read from the data location

    uint32_t x_block_count, y_block_count, z_block_count; // DF's map block count
    */

//    DfMapHeader()
//    :   identifier("!!URIST!!"),
//        version(1U)
//    {
//    }
};


#endif // DF_MAP_HEADER_H

