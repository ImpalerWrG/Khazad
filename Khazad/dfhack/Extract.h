/* Memory research
    http://dwarffortresswiki.net/index.php/User:Rick/memory.ini#A_table_of_available_settings
    http://dwarffortresswiki.net/index.php/User:Rick/Memory_research#Tile_Block
    http://dwarffortresswiki.net/index.php/User:AzureLightning/Memory_research
    http://dwarffortresswiki.net/index.php/User:Iluxan/Memory_research
 */
#ifndef EXTRACT_HEADER
#define EXTRACT_HEADER

class DfMap;

class Extractor
{
protected:
    DfMap *df_map;   // DF extracted map structure
    
public:
    bool Init();
    Extractor();
    ~Extractor();
    bool loadMap(string FileName);
    bool writeMap(string FileName);
    bool isMapLoaded();
    DfMap *getMap() {return df_map;};
    bool dumpMemory( string path_to_xml);
};
#endif // EXTRACT_HEADER
