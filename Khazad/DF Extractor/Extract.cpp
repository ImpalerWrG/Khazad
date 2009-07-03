// Extractor
#include <Extract.h>
#include <DfVector.h>

DECLARE_SINGLETON(Extractor)

Extractor::Extractor()
{
    df_map = NULL; // important, null pointer means we don't have a map loaded
}

bool Extractor::Init()
{
    df_map = NULL;
    return true;
}

Extractor::~Extractor()
{
    if(df_map !=NULL )
    {
        delete df_map;
    }
}

bool Extractor::dumpMemory()
{
    // attach to process
    printf("Attempting to Attach Process\n");
    Process * p = PROCESS;
    if(!p->attach())
    {
        printf("Could not Attach Process, Aborting\n");
        return false; // couldn't attach to process, no go
    }
    printf("Process succesfully Attached\n");
    memory_info* offset_descriptor = p->getDescriptor();

    Uint32 map_loc, // location of the X array
           temp_loc, // block location
           temp_locx, // iterator for the X array
           temp_locy, // iterator for the Y array
           temp_locz; // iterator for the Z array
    unsigned blocks_read = 0U;

    // Read Map Data Blocks
    int map_offset = offset_descriptor->getOffset("map_data");;
    int x_count_offset = offset_descriptor->getOffset("x_count");
    int y_count_offset = offset_descriptor->getOffset("y_count");
    int z_count_offset = offset_descriptor->getOffset("z_count");
    int tile_type_offset = offset_descriptor->getOffset("type");
    int designation_offset = offset_descriptor->getOffset("designation");
    int occupancy_offset = offset_descriptor->getOffset("occupancy");

    // layers
    int region_x_offset = offset_descriptor->getOffset("region_x");
    int region_y_offset = offset_descriptor->getOffset("region_y");
    int region_z_offset =  offset_descriptor->getOffset("region_z");
    int world_offset =  offset_descriptor->getOffset("world");
    int world_regions_offset =  offset_descriptor->getOffset("world_regions");
    int sizeof_region =  offset_descriptor->getOffset("region_size");
    int region_geoindex_offset =  offset_descriptor->getOffset("region_geoindex_offset");
    int world_geoblocks_offset =  offset_descriptor->getOffset("world_geoblocks");
    int biome_stuffs = offset_descriptor->getOffset("biome_stuffs");

    // veins
    int veinvector = offset_descriptor->getOffset("v_vein");
    int veinsize = offset_descriptor->getOffset("v_vein_size");

    // stone/soil/gem matgloss
    int stone_matgloss_offset = offset_descriptor->getOffset("mat_stone");

    // tree/wood matgloss
    /// NOT YET

    map_loc = p->readDWord(map_offset);

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
    df_map = new DfMap(p->readDWord(x_count_offset),p->readDWord(y_count_offset),p->readByte(z_count_offset));


    // read matgloss data from df if we can
    if(stone_matgloss_offset)
    {
        // set up vector of pointers to materials
        DfVector p_stone_matgloss = p->readVector(stone_matgloss_offset, 4);
        printf ("YAY, MATGLOSS! %d\n", p_stone_matgloss.getSize());
        // iterate over it
        for (int i = 0; i< p_stone_matgloss.getSize();i++)
        {
            Uint32 temp;
            string tmpstr;
            // read the matgloss pointer from the vector into temp
            p_stone_matgloss.read((Uint32)i,(Uint8 *)&temp);
            // read the string pointed at by
            tmpstr = p->readSTLString(temp); // reads a C string given an address
            // store it in the block
            df_map->stone_matgloss.push_back(tmpstr);
            printf("%d = %s\n",i,tmpstr.c_str());
        }
    }

    if(region_x_offset && region_y_offset && region_z_offset)
    {
        df_map->setRegionCoords(p->readDWord(region_x_offset),p->readDWord(region_y_offset),p->readDWord(region_z_offset));
        // we have region offset
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
        temp_locy = p->readDWord(temp_locx);
        for(int y = 0; y < df_map->y_block_count; y++)
        {
            temp_locz = p->readDWord(temp_locy);
            temp_locy += 4;
            for(int z = 0; z < df_map->z_block_count; z++)
            {
                temp_loc = p->readDWord(temp_locz);
                temp_locz += 4;
                if (temp_loc)
                {
                    Block * b = df_map->allocBlock(x,y,z);
                    b->origin = temp_loc; // save place of block in DF's memory for later
                    //
                    p->read(
                    /*Uint32 offset*/ temp_loc + tile_type_offset,
                    /*Uint32 size*/   sizeof(short int)*BLOCK_SIZE*BLOCK_SIZE,
                    /*void *target*/  (Uint8 *)&b->tile_type
                           );
                    p->read(
                    /*Uint32 offset*/ temp_loc + designation_offset,
                    /*Uint32 size*/   sizeof(int)*BLOCK_SIZE*BLOCK_SIZE,
                    /*void *target*/  (Uint8 *)&b->designation
                           );
                    p->read(
                    /*Uint32 offset*/ temp_loc + occupancy_offset,
                    /*Uint32 size*/   sizeof(int)*BLOCK_SIZE*BLOCK_SIZE,
                    /*void *target*/  (Uint8 *)&b->occupancy
                           );
                    // set all materials to -1.
                    memset(b->material, -1, sizeof(int16_t) * 256);
                    if(biome_stuffs) // we got biome stuffs! we can try loading matgloss from here
                    {
                        p->read(
                        /*Uint32 offset*/ temp_loc + biome_stuffs,
                        /*Uint32 size*/   sizeof(Uint8)*16,
                        /*void *target*/  (Uint8 *)&b->RegionOffsets
                               );
                    }
                    // load veins from the game
                    if(veinvector && veinsize)
                    {
                        assert(sizeof(t_vein) == veinsize);
                        // veins are stored as a vector of pointers to veins .. at least in df 40d11 on linux
                        /*pointer is 4 bytes! we work with a 32bit program here, no matter what architecture we compile khazad for*/
                        DfVector p_veins = p->readVector(temp_loc + veinvector, 4);
                        // read all veins
                        for (int i = 0; i< p_veins.getSize();i++)
                        {
                            t_vein v;
                            Uint32 temp;
                            // read the vein pointer from the vector
                            p_veins.read((Uint32)i,(Uint8 *)&temp);
                            // read the vein data (dereference pointer)
                            p->read(temp, veinsize, (Uint8 *)&v);
                            // store it in the block
                            b->veins.push_back(v);
                        }
                        b->collapseVeins(); // collapse *our* vein vector into vein matgloss data
                    }
                    // load layer matgloss
/*                    for(int x_b = 0; x_b < BLOCK_SIZE; x_b++)
                    {
                        for(int y_b = 0; y_b < BLOCK_SIZE; y_b++)
                        {
                            if(vein_matgloss[x_b][y_b] != -1)
                            {
                                int geolayer = b->designation[x_b][y_b].geolayer_index;
                                int biome = b->designation[x_b][y_b].geolayer_index;
                            }
                        }
                    }*/
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
