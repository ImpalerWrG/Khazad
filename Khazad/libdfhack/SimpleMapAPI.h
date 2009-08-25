#ifndef SIMPLEAPI_H_INCLUDED
#define SIMPLEAPI_H_INCLUDED

/**
 * This is a canned initialization. It hides a lot of the process search and attach specifics from you.
 * It can be done better, but this is really easy to use.
 */

class memory_info;

class SimpleAPI
{
    ///TODO: give this the pimpl treatment?
private:
    // internals
    uint32_t * block;
    uint32_t x_block_count, y_block_count, z_block_count;
    uint32_t regionX, regionY, regionZ;
    uint32_t worldSizeX, worldSizeY;
    ProcessManager* pm;
    ProcessManager::Process* p;
    DataModel* dm;
    memory_info* offset_descriptor;
    string xml;
public:
    SimpleAPI(const string path_to_xml);

    bool Attach();
    bool Detach();
    bool isAttached();
    // read all known matgloss
    bool ReadMatgloss(uint32_t &count, vector< vector <t_matgloss> > &Matgloss);

    // read region surroundings, get their vectors of geolayers

    // TBD

    /*
     * BLOCK DATA
     */
    /// allocate and read pointers to map blocks
    bool InitMap();
    /// destroy the mapblock cache
    bool DestroyMap();
    /// get size of the map in tiles
    void getSize(uint32_t& x, uint32_t& y, uint32_t& z);

    //Return false/0 on failure, buffer allocated by me, 256 long of course.
    bool ReadTileTypes(uint32_t blockx, uint32_t blocky, uint32_t blockz, uint16_t *buffer); // 256 * sizeof(uint16_t)
    bool ReadDesignations(uint32_t blockx, uint32_t blocky, uint32_t blockz, uint32_t *buffer); // 256 * sizeof(uint32_t)
    bool ReadOccupancy(uint32_t blockx, uint32_t blocky, uint32_t blockz, uint32_t *buffer); // 256 * sizeof(uint32_t)
    //16 of them? IDK... there's probably just 7. Reading more doesn't cause errors
    bool ReadRegionOffsets(uint32_t blockx, uint32_t blocky, uint32_t blockz, uint32_t *buffer); // 16 * sizeof(uint8_t)
    // aggregated veins of a block
    t_vein *ReadVeins(unsigned int blockx, unsigned int blocky, unsigned int &count);
    /*
     * Buildings, constructions, plants
     */

    // TBD
};
#endif // SIMPLEAPI_H_INCLUDED
