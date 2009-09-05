#include "DFCommon.h"
#include "DfMap.h"
#include "DfMapHeader.h"
#include "ZlibHelper.h"


bool DfMap::write(string FilePath)
{
    FILE *SaveFile;
    SaveFile = fopen(FilePath.c_str(),"wb");
    DfMapHeader df_map_header;

    if(SaveFile == NULL)
    {
        printf("Can\'t create file for write.\n");
        return false;
    }
    else
    {
        // gather information to fill dfmapheader
        strcpy(df_map_header.identifier, dmh_id);
        df_map_header.version = dmh_ver;
        df_map_header.reserved = 0/*sizeof(DfMapHeader)*/;
        
        // save map header
        fwrite(&df_map_header, sizeof(DfMapHeader), 1, SaveFile);
        
        // save map
        writeVersion1(SaveFile);
    }

    // reopen file for reading
    freopen (FilePath.c_str(),"rb",SaveFile);
    if(SaveFile == NULL)
    {
        printf("Can\'t create file for read.\n");
        return false;
    }

    FILE *SaveCompressedFile;
    string CompressedFilePath = FilePath + ".comp";

    SaveCompressedFile = fopen(CompressedFilePath.c_str(),"wb");
    if(SaveCompressedFile == NULL)
    {
        printf("Can\'t create a compressed file for write\n");
        return false;
    }

    // compress
    printf("Compressing... ");
    int ret = def(SaveFile, SaveCompressedFile, Z_BEST_COMPRESSION);
    if (ret != Z_OK)
    {
        zerr(ret);
    }
    printf("DONE\n");

    fclose(SaveFile);
    fclose(SaveCompressedFile);
    remove(FilePath.c_str());
    rename(CompressedFilePath.c_str(), FilePath.c_str());
    return true;
}

bool DfMap::load(string FilePath)
{
    string DecompressedFilePath = FilePath + ".decomp";
    FILE *ToDecompress;
    FILE *Decompressed;
    DfMapHeader df_map_header;
    bool isok = false;

    // open file for writing decompressed data
    Decompressed = fopen(DecompressedFilePath.c_str(), "wb");
    if  (Decompressed == NULL)
    {
        printf("Can\'t open a decompressed file for write.\n");
        return false;
    }
    
    // open compressed file
    ToDecompress = fopen(FilePath.c_str(),"rb");
    if  (ToDecompress == NULL)
    {
        printf("Can\'t open file for read.\n");
        return false;
    }

    // decompress
    printf("Decompressing... ");
    int ret = inf(/*source*/ToDecompress,/*destination*/Decompressed);

    printf("DONE\n");

    if (ret != Z_OK)
    {
        zerr(ret);
    }

    // OK, close file with compressed data
    fclose(ToDecompress);
    
    // reopen decompressed file for reading
    freopen(DecompressedFilePath.c_str(), "rb", Decompressed);
    if  (Decompressed == NULL)
    {
        printf("Can\'t create decompressed file for read.\n");
        return false;
    }

    // check, if the file is big enough to contain the header
    fseek(Decompressed, 0, SEEK_END);

    if (ftell(Decompressed) < (int32_t)sizeof(DfMapHeader))
    {
    	printf("This Khazad map file is corrupted - file too small.\n");
    	return false;
    }

    // read the header
    fseek(Decompressed, 0, SEEK_SET);
    fread(&df_map_header, sizeof(DfMapHeader), 1, Decompressed);

    // check, if it's a Khazad map file
    if (strcmp(df_map_header.identifier,dmh_id) != 0)
    {
    	printf("This file is not a Khazad map file.\n");
    	return false;
    }
    
    // ALERT: flush all map data. This is very important.
    clear();
    
    switch(df_map_header.version)
    {
        /*
            Basic format without matgloss. Kept for compatibility reasons.
            Saved from version 0.0.5
        */
        case 1:
            isok = loadVersion1(Decompressed, df_map_header);
            break;
        
        /*
            v2 format
            Saved from some SVN version. this format is dead
        */
        case 2:
            //isok = loadVersion2(Decompressed, df_map_header);
            isok = false;
            break;
        
        default:
            printf("Unknown Khazad map file version(%3d).\n", df_map_header.version);
            isok = false;
            break;
    }
    
    // close decompressed file and delete it
    fclose(Decompressed);
    remove(DecompressedFilePath.c_str());
    return isok;
}
