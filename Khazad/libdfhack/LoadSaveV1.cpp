#include "DFCommon.h"
#include "DfMap.h"
#include "DfMapHeader.h"
#include "ZlibHelper.h"

bool DfMap::loadVersion1(FILE * Decompressed,DfMapHeader & h)
{
    uint32_t x, y, z;
    // load new size information
    x_block_count = h.x_block_count;
    y_block_count = h.y_block_count;
    z_block_count = h.z_block_count;
    // make sure those size variables are in sync
    updateCellCount();
    // alloc new space for our new size
    allocBlockArray(x_block_count,y_block_count,z_block_count);

    fseek(Decompressed, h.map_data_location, SEEK_SET);

    for (uint32_t tile_block = 0U; tile_block < h.tile_block_count; ++tile_block)
    {
        fread(&x, sizeof(uint32_t), 1, Decompressed);
        fread(&y, sizeof(uint32_t), 1, Decompressed);
        fread(&z, sizeof(uint32_t), 1, Decompressed);

        Block * b = allocBlock(x,y,z);

        fread(&b->tile_type, sizeof(b->tile_type), 1, Decompressed);
        fread(&b->designation, sizeof(b->designation), 1, Decompressed);
        fread(&b->occupancy, sizeof(b->occupancy), 1, Decompressed);
        memset(b->material, -1, sizeof(b->material));
        // can't load offsets, substitute local biome everywhere
        memset(b->RegionOffsets,eHere,sizeof(b->RegionOffsets));
    }

    printf("Blocks read into memory: %d\n", h.tile_block_count);
    return true;
}

// DEPRECATED
/*
bool DfMap::writeVersion1(FILE * SaveFile)
{
    uint32_t x, y, z;

    for (x = 0; x < x_block_count; x++ )
    {
        for (y = 0; y < y_block_count; y++ )
        {
            for (z = 0; z < z_block_count; z++ )
            {
                Block *b = getBlock(x,y,z);
                if(b != NULL)
                {
                    // which block it is
                    fwrite(&x, sizeof(uint32_t), 1, SaveFile);
                    fwrite(&y, sizeof(uint32_t), 1, SaveFile);
                    fwrite(&z, sizeof(uint32_t), 1, SaveFile);
                    // block data
                    fwrite(&b->tile_type, sizeof(uint16_t), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                    fwrite(&b->designation, sizeof(uint32_t), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                    fwrite(&b->occupancy, sizeof(uint32_t), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                }
            }
        }
    }
    return true;
}
*/
