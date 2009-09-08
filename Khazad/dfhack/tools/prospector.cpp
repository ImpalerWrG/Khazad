// produces a list of materials available on the map.
// TODO: use material colors to make the output prettier
// TODO: needs the tiletype filter!

#include <iostream>
#include <stdint.h>
#include <string.h> // for memset
#include <vector>
#include <map>
using namespace std;

#include <Types.h>
#include <TileTypes.h>
#include <SimpleMapAPI.h>

int main (int argc, const char* argv[])
{
    bool showhidden = false;
    for(int i = 0; i < argc; i++)
    {
        string test = argv[i];
        if(test == "-a")
        {
            showhidden = true;
            break;
        }
    }
    uint32_t x_max,y_max,z_max;
    uint16_t tiletypes[16][16];
    t_designation designations[16][16];
    map <int16_t, uint32_t> materials;
    materials.clear();
    vector<t_matgloss> stonetypes;
    
    // init the API
    SimpleAPI DF("Memory.xml");
    
    // attach
    if(!DF.Attach())
    {
        cerr << "DF not found" << endl;
        return 1;
    }
    
    // init the map
    DF.InitMap();
    DF.getSize(x_max,y_max,z_max);
    
    // get stone matgloss mapping
    if(!DF.ReadStoneMatgloss(stonetypes))
    {
        //DF.DestroyMap();
        cerr << "Can't get the materials." << endl;
        return 1; 
    }
    int16_t tempvein [16][16];
    vector <t_vein> veins;
    // walk the map!
    for(uint32_t x = 0; x< x_max;x++)
    {
        for(uint32_t y = 0; y< y_max;y++)
        {
            for(uint32_t z = 0; z< z_max;z++)
            {
                if(!DF.isValidBlock(x,y,z))
                    continue;
                
                // read data
                DF.ReadTileTypes(x,y,z, (uint16_t *) tiletypes);
                DF.ReadDesignations(x,y,z, (uint32_t *) designations);
                
                // get veins, collapse them
                memset(tempvein, -1, sizeof(tempvein));
                
                veins.clear();
                DF.ReadVeins(x,y,z,veins);
                
                for(int i = 0; i < veins.size();i++)
                {
                    if(veins[i].type >= stonetypes.size() || veins[i].type < 0)
                    {
                        cerr << "weird vein " << veins[i].type << endl;
                        continue;
                    }
                    for(uint32_t j = 0;j<16;j++)
                    {
                        //iterate through the bits
                        for (uint32_t k = 0; k< 16;k++)
                        {
                            // and the bit array with a one-bit mask, check if the bit is set
                            bool set = ((1 << k) & veins[i].assignment[j]) >> k;
                            if(set)
                            {
                                tempvein[k][j] = veins[i].type;
                            }
                        }
                    }
                }
                // process stuff
                for(uint32_t xi = 0 ; xi< 16 ; xi++)
                {
                    for(uint32_t yi = 0 ; yi< 16 ; yi++)
                    {
                        if(designations[xi][yi].bits.hidden && !showhidden || !isWallTerrain(tiletypes[xi][yi]))
                            continue;
                        if(tempvein[xi][yi] < 0)
                            continue;
                        
                        if(materials.count(tempvein[xi][yi]))
                        {
                            materials[tempvein[xi][yi]] += 1;
                            
                        }
                        else
                        {
                            materials[tempvein[xi][yi]] = 1;
                        }
                    }    
                }
            }
        }
    }
    // print report
    map<int16_t, uint32_t>::iterator p;
    for(p = materials.begin(); p != materials.end(); p++)
    {
        cout << stonetypes[p->first].id << " : " << p->second << endl;
    }
    return 0;
}