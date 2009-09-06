// This program exports the entire map from DF. Takes roughly 6.6 seconds for 1000 cycles on my Linux machine.

#include <iostream>
#include <stdint.h>
#include <vector>
#include <map>
using namespace std;
#include <Types.h>
#include <ProcessManager.h>
#include <SimpleMapAPI.h>

int main (void)
{
    uint32_t x_max,y_max,z_max;
    uint32_t num_blocks = 0;
    uint32_t bytes_read = 0;
    uint16_t tiletypes[256];
    t_designation designations[256];
    t_occupancy occupancies[256];
    
    SimpleAPI DF("Memory.xml");
    if(!DF.Attach())
    {
        cerr << "DF not found" << endl;
        return 1;
    }
    DF.InitMap();
    DF.getSize(x_max,y_max,z_max);
    
    //for(int i = 0;i < 100;i++)
    for(uint32_t x = 0; x< x_max;x++)
    {
        for(uint32_t y = 0; y< y_max;y++)
        {
            for(uint32_t z = 0; z< z_max;z++)
            {
                if(DF.isValidBlock(x,y,z))
                {
                    DF.ReadTileTypes(x,y,z, tiletypes);
                    DF.ReadDesignations(x,y,z, (uint32_t *) designations);
                    DF.ReadOccupancy(x,y,z, (uint32_t *) occupancies);
                    num_blocks ++;
                    bytes_read += 256 * (4 + 4 + 2);
                }
            }
        }
    }
    cout << num_blocks << " blocks read" << endl;
    cout << bytes_read / (1024 * 1024) << " MB" << endl;
    return 0;
}