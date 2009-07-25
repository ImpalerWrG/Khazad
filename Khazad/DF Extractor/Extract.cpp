// Extractor
#include "DFCommon.h"
using namespace std;

#include <Extract.h>
#include <DataModel.h>
#include <MemInfo.h>

Extractor::Extractor()
{
    df_map = NULL; // important, null pointer means we don't have a map loaded
}

Extractor::~Extractor()
{
    if(df_map !=NULL )
    {
        delete df_map;
    }
}

bool Extractor::dumpMemory( string path_to_xml)
{
    // create process manager, get first process
    ProcessManager pm(path_to_xml);
    if(!pm.findProcessess())
    {
        fprintf(stderr,"Can't find any suitable DF process\n");
        return false;
    }
    // attach to process
    printf("Attempting to Attach Process\n");
    ///FIXME: this won't do.
    Process * p = pm[0];
    DataModel * dm = p->getDataModel();
    if(!p->attach())
    {
        printf("Could not Attach Process, Aborting\n");
        return false; // couldn't attach to process, no go
    }
    printf("Process succesfully Attached\n");
    memory_info* offset_descriptor = p->getDescriptor();

    uint32_t map_loc, // location of the X array
            temp_loc, // block location
            temp_locx, // iterator for the X array
            temp_locy, // iterator for the Y array
            temp_locz; // iterator for the Z array
    unsigned blocks_read = 0U;

    // Read Map Data Blocks
    int map_offset = offset_descriptor->getAddress("map_data");;
    int x_count_offset = offset_descriptor->getAddress("x_count");
    int y_count_offset = offset_descriptor->getAddress("y_count");
    int z_count_offset = offset_descriptor->getAddress("z_count");
    int tile_type_offset = offset_descriptor->getOffset("type");
    int designation_offset = offset_descriptor->getOffset("designation");
    int occupancy_offset = offset_descriptor->getOffset("occupancy");
    int biome_stuffs = offset_descriptor->getOffset("biome_stuffs");

    // layers
    int region_x_offset = offset_descriptor->getAddress("region_x");
    int region_y_offset = offset_descriptor->getAddress("region_y");
    int region_z_offset =  offset_descriptor->getAddress("region_z");
    int world_offset =  offset_descriptor->getAddress("world");
    int world_regions_offset =  offset_descriptor->getOffset("w_regions_arr");
    int region_size =  offset_descriptor->getHexValue("region_size");
    int region_geo_index_offset =  offset_descriptor->getOffset("region_geo_index_off");
    int world_geoblocks_offset =  offset_descriptor->getOffset("w_geoblocks");
    int world_size_x = offset_descriptor->getOffset("world_size_x");
    int world_size_y = offset_descriptor->getOffset("world_size_y");
    int geolayer_geoblock_offset = offset_descriptor->getOffset("geolayer_geoblock_offset");
    // veins
    int veinvector = offset_descriptor->getOffset("v_vein");
    int veinsize = offset_descriptor->getHexValue("v_vein_size");

    // stone/soil/gem matgloss
    int stone_matgloss_offset = offset_descriptor->getAddress("mat_stone");

    bool have_geology = false;

    // tree/wood matgloss
    /// NOT YET
    printf("Map offset: 0x%.8X\n", map_offset);
    map_loc = MreadDWord(map_offset);

    if (!map_loc)
    {
        printf("Could not find DF map information in memory, Aborting\n");
        return false;
    }
    printf("Map data Found at: 0x%.8X\n", map_loc);

    if(df_map != NULL)
    {
        delete df_map;
    }
    df_map = new DfMap(MreadDWord(x_count_offset),MreadDWord(y_count_offset),MreadByte(z_count_offset));


    // read matgloss data from df if we can
    if(stone_matgloss_offset)
    {
        // set up vector of pointers to materials
        DfVector p_stone_matgloss = dm->readVector(stone_matgloss_offset, 4);
        //printf ("YAY, MATGLOSS! %d\n", p_stone_matgloss.getSize());
        // iterate over it
        for (int i = 0; i< p_stone_matgloss.getSize();i++)
        {
            uint32_t temp;
            string tmpstr;
            // read the matgloss pointer from the vector into temp
            p_stone_matgloss.read((uint32_t)i,(uint8_t *)&temp);
            // read the string pointed at by
            tmpstr = dm->readSTLString(temp); // reads a C string given an address
            // store it in the block
            df_map->stone_matgloss.push_back(tmpstr);
            printf("%d = %s\n",i,tmpstr.c_str());
        }
    }
    if(region_x_offset && region_y_offset && region_z_offset)
    {
        df_map->setRegionCoords(MreadDWord(region_x_offset),MreadDWord(region_y_offset),MreadDWord(region_z_offset));
        // we have region offset
        // extract layer geology data. we need all these to do that
        if(world_size_x && world_size_y && world_offset && world_regions_offset && world_geoblocks_offset && region_size && region_geo_index_offset && geolayer_geoblock_offset)
        {
            // get world size
            int worldSizeX = MreadWord(world_offset + world_size_x);
            int worldSizeY = MreadWord(world_offset + world_size_y);
            df_map->worldSizeX = worldSizeX;
            df_map->worldSizeY = worldSizeY;
            printf("World size. X=%d Y=%d\n",worldSizeX,worldSizeY);
            // get pointer to first part of 2d array of regions
            uint32_t regions = MreadDWord(world_offset + world_regions_offset);
            printf("regions. Offset=%d\n",regions);
            // read the 9 surrounding regions
            DfVector geoblocks = dm->readVector(world_offset + world_geoblocks_offset,4);
            // iterate over surrounding biomes. make sure we don't fall off the world
            for(int i = eNorthWest; i< eBiomeCount; i++)
            {
                // check bounds, fix them if needed
                int bioRX = df_map->regionX / 16 + (i%3) - 1;
                if( bioRX < 0) bioRX = 0;
                if( bioRX >= worldSizeX) bioRX = worldSizeX - 1;
                int bioRY = df_map->regionY / 16 + (i/3) - 1;
                if( bioRY < 0) bioRY = 0;
                if( bioRY >= worldSizeY) bioRY = worldSizeY - 1;
                /// TODO: encapsulate access to multidimensional arrays.
                // load region stuff here
                uint32_t geoX = MreadDWord(regions + bioRX*4);// get pointer to column of regions
                // geoX = base
                // bioRY = index
                // region_size = size of array objects
                // region_geo_index_off = offset into the array object
                uint16_t geoindex = MreadWord(geoX + bioRY*region_size + region_geo_index_offset);
                uint32_t geoblock_off;
                // get the geoblock from the geoblock vector using the geoindex
                geoblocks.read(geoindex,(uint8_t *) &geoblock_off);
                df_map->geoblockadresses[i]=geoblock_off;
                df_map->regionadresses[i]=geoX + bioRY*region_size + region_geo_index_offset;
                // get the layer pointer vector :D
                DfVector geolayers = dm->readVector(geoblock_off + geolayer_geoblock_offset , 4); // let's hope
                // make sure we don't load crap
                assert(geolayers.getSize() > 0 && geolayers.getSize() <= 16);
                for(int j = 0;j< geolayers.getSize();j++)
                {
                    int geol_offset;
                    // read pointer to a layer
                    geolayers.read(j, (uint8_t *) & geol_offset);
                    // read word at pointer + 2, store in our geology vectors
                    df_map->geology[i].push_back(MreadWord(geol_offset + 2));
                    df_map->geodebug[i].push_back(geol_offset);
                }
            }
            have_geology = true;
        }
    }
    else
    {
        // crap, can't get the real layer materials
        df_map->setRegionCoords(0,0,0);
    }
    //read the memory from the map blocks
    for(int x = 0; x < df_map->x_block_count; x++)
    {
        temp_locx = map_loc + ( 4 * x );
        temp_locy = MreadDWord(temp_locx);
        for(int y = 0; y < df_map->y_block_count; y++)
        {
            temp_locz = MreadDWord(temp_locy);
            temp_locy += 4;
            for(int z = 0; z < df_map->z_block_count; z++)
            {
                temp_loc = MreadDWord(temp_locz);
                temp_locz += 4;
                if (temp_loc)
                {
                    Block * b = df_map->allocBlock(x,y,z);
                    b->origin = temp_loc; // save place of block in DF's memory for later
                    //
                    Mread(
                    /*Uint32 offset*/ temp_loc + tile_type_offset,
                    /*Uint32 size*/   sizeof(uint16_t)*BLOCK_SIZE*BLOCK_SIZE,
                    /*void *target*/  (uint8_t *)&b->tile_type
                           );
                    Mread(
                    /*Uint32 offset*/ temp_loc + designation_offset,
                    /*Uint32 size*/   sizeof(uint32_t)*BLOCK_SIZE*BLOCK_SIZE,
                    /*void *target*/  (uint8_t *)&b->designation
                           );
                    Mread(
                    /*Uint32 offset*/ temp_loc + occupancy_offset,
                    /*Uint32 size*/   sizeof(uint32_t)*BLOCK_SIZE*BLOCK_SIZE,
                    /*void *target*/  (uint8_t *)&b->occupancy
                           );
                    // set all materials to -1.
                    memset(b->material, -1, sizeof(int16_t) * 256);
                    if(biome_stuffs) // we got biome stuffs! we can try loading matgloss from here
                    {
                        Mread(
                        /*Uint32 offset*/ temp_loc + biome_stuffs,
                        /*Uint32 size*/   sizeof(uint8_t)*16,
                        /*void *target*/  (uint8_t *)&b->RegionOffsets
                               );
                        // if we have geology, we can use the geolayers to determine materials
                        if(have_geology)
                        {
                            df_map->applyMatgloss(b);
                        }
                    }
                    else
                    {
                        // can't load offsets, substitute local biome everywhere
                        memset(b->RegionOffsets,eHere,sizeof(b->RegionOffsets));
                    }
                    // load veins from the game
                    if(veinvector && veinsize)
                    {
                        assert(sizeof(t_vein) == veinsize);
                        // veins are stored as a vector of pointers to veins .. at least in df 40d11 on linux
                        /*pointer is 4 bytes! we work with a 32bit program here, no matter what architecture we compile khazad for*/
                        DfVector p_veins = dm->readVector(temp_loc + veinvector, 4);
                        // read all veins
                        for (int i = 0; i< p_veins.getSize();i++)
                        {
                            t_vein v;
                            uint32_t temp;
                            // read the vein pointer from the vector
                            p_veins.read((uint32_t)i,(uint8_t *)&temp);
                            // read the vein data (dereference pointer)
                            Mread(temp, veinsize, (uint8_t *)&v);
                            // store it in the block
                            b->veins.push_back(v);
                        }
                        b->collapseVeins(); // collapse *our* vein vector into vein matgloss data
                    }
                    ++blocks_read;
                }
            }
        }
    }

    printf("Blocks read into memory: %d\n", blocks_read);
    df_map->setBlocksCount(blocks_read);


    p->detach();
    return true;
}
// wrappers!
bool Extractor::loadMap(string FileName)
{
    if(df_map == NULL)
    {
        df_map = new DfMap(FileName);
    }
    else
    {
        df_map->load(FileName);
    }
    return df_map->isValid();
}

bool Extractor::writeMap(string FileName)
{
    if(df_map == NULL)
    {
        return false;
    }
    return df_map->write(FileName);
}

bool Extractor::isMapLoaded()
{
    if(df_map != NULL)
    {
        if(df_map->isValid())
        {
            return true;
        }
    }
    return false;
}
