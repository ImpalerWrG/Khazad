#include "DFCommon.h"
#include "DfMap.h"
#include "DfMapHeader.h"
#include "ZlibHelper.h"

// LOAD v1 BLOCKS
bool DfMap::loadVersion1(FILE * Decompressed,DfMapHeader & h)
{
    uint32_t x, y, z;
    uint32_t tile_block_count; // how many tile blocks to read from the data location

    // load new size information
    fread(&tile_block_count, sizeof(uint32_t), 1, Decompressed);
    fread(&x_block_count, sizeof(uint32_t), 1, Decompressed);
    fread(&y_block_count, sizeof(uint32_t), 1, Decompressed);
    fread(&z_block_count, sizeof(uint32_t), 1, Decompressed);
    
    // make sure those size variables are in sync
    updateCellCount();
    
    // alloc new space for our new size
    allocBlockArray(x_block_count,y_block_count,z_block_count);

    // let's load the blocks
    for (uint32_t tile_block = 0U; tile_block < tile_block_count; ++tile_block)
    {
        // block position - we can omit empty blocks this way
        fread(&x, sizeof(uint32_t), 1, Decompressed);
        fread(&y, sizeof(uint32_t), 1, Decompressed);
        fread(&z, sizeof(uint32_t), 1, Decompressed);
        
        Block * b = allocBlock(x,y,z);

        // read data
        fread(&b->tile_type, sizeof(b->tile_type), 1, Decompressed);
        fread(&b->designation, sizeof(b->designation), 1, Decompressed);
        fread(&b->occupancy, sizeof(b->occupancy), 1, Decompressed);
        memset(b->material, -1, sizeof(b->material));
        
        // can't load offsets, substitute local biome everywhere
        memset(b->RegionOffsets,eHere,sizeof(b->RegionOffsets));
    }

    printf("Blocks read into memory: %d\n", tile_block_count);
    return true;
}

// SAVE v1 BLOCKS
bool DfMap::writeVersion1(FILE * SaveFile)
{
    uint32_t x, y, z;
    // write size information - total blocks and map size
    fwrite(&blocks_allocated, sizeof(uint32_t), 1, SaveFile);
    fwrite(&x_block_count, sizeof(uint32_t), 1, SaveFile);
    fwrite(&y_block_count, sizeof(uint32_t), 1, SaveFile);
    fwrite(&z_block_count, sizeof(uint32_t), 1, SaveFile);
    
    // write each non-empty block
    for (x = 0; x < x_block_count; x++ )
    {
        for (y = 0; y < y_block_count; y++ )
        {
            for (z = 0; z < z_block_count; z++ )
            {
                Block *b = getBlock(x,y,z);
                if(b != NULL)
                {
                    // first goes block position
                    fwrite(&x, sizeof(uint32_t), 1, SaveFile);
                    fwrite(&y, sizeof(uint32_t), 1, SaveFile);
                    fwrite(&z, sizeof(uint32_t), 1, SaveFile);
                    // then block data
                    fwrite(&b->tile_type, sizeof(uint16_t), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                    fwrite(&b->designation, sizeof(uint32_t), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                    fwrite(&b->occupancy, sizeof(uint32_t), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                }
            }
        }
    }
    return true;
}

