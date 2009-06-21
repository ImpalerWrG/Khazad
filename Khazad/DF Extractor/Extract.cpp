// Extractor
#include <Extract.h>

DECLARE_SINGLETON(Extractor)

Extractor::Extractor()
{
    df_map = NULL; // important, null pointer means we don't have a map loaded
}

bool Extractor::Init()
{
    return true;
}

Extractor::~Extractor()
{
    if(df_map)
        delete df_map;
}


// //ReadProcessMemory( DFHandle, (int*)(temp_locy), &temp_locz, sizeof(int), NULL);
bool Extractor::dumpMemory()
{
    // attach to process
    printf("Here we go...\n");
    Process * p = PROCESS;
    if(!p->attach())
        return 1; // couldn't attach to process, no go
    memory_info * offset_descriptor = p->getDescriptor();

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

    map_loc = p->readDWord(map_offset);

    if (!map_loc)
    {
        printf("Could not find DF map information in memory...\n");
        return 1;
    }
    printf("map data : 0x%.8X\n", map_loc);

    if(df_map)
        delete df_map;
    df_map = new DfMap(p->readDWord(x_count_offset),p->readDWord(y_count_offset),p->readByte(z_count_offset));
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
                    /*void *target*/  &b->tile_type
                           );
                    p->read(
                    /*Uint32 offset*/ temp_loc + designation_offset,
                    /*Uint32 size*/   sizeof(int)*BLOCK_SIZE*BLOCK_SIZE,
                    /*void *target*/  &b->designation
                           );
                    p->read(
                    /*Uint32 offset*/ temp_loc + occupancy_offset,
                    /*Uint32 size*/   sizeof(int)*BLOCK_SIZE*BLOCK_SIZE,
                    /*void *target*/  &b->occupancy
                           );
                    ++blocks_read;
                }
            }
        }
    }
    printf("Blocks read into memory: %d\n", blocks_read);
    p->detach();
    return 1;
}
// wrappers!
bool Extractor::loadMap(string FileName)
{
    if(!df_map)
        df_map = new DfMap(FileName);
    else
        df_map->load(FileName);
    return df_map->isValid();
}

bool Extractor::writeMap(string FileName)
{
    if(!df_map)
        return false;
    return df_map->write(FileName);
}
bool Extractor::isMapLoaded()
{
    if(df_map)
        if(df_map->isValid())
            return true;
    return false;
}
