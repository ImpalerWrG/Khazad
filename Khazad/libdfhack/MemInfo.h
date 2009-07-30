#ifndef MEMINFO_H_INCLUDED
#define MEMINFO_H_INCLUDED

class memory_info
{
public:
    enum OSType
	{
		OS_WINDOWS,
		OS_LINUX,
		OS_BAD
	};

    memory_info();
    memory_info(const memory_info&);

    void Rebase(int32_t new_base);
    uint32_t getBase ();
    void setBase (string);
    void setBase (uint32_t);

    uint32_t getOffset (string);
    uint32_t getAddress (string);
    uint32_t getHexValue (string);
    string getString (string);

    void setVersion(const char *);
    void setVersion(string);
    string getVersion();

    void setOS(const char *);
    void setOS(string);
    void setOS(OSType);
    OSType getOS();

    void setOffset (string, int32_t);
    void setAddress (string, uint32_t);
    void setHexValue (string, uint32_t);

    void setOffset (string, const char *);
    void setAddress (string, const char *);
    void setHexValue (string, const char *);
    void setString (string, const char *);

    void setOffset (string, string);
    void setAddress (string, string);
    void setHexValue (string, string);
    void setString (string, string);

    void flush();
private:
    map <string, uint32_t> addresses;
    map <string, uint32_t> offsets;
    map <string, uint32_t> hexvals;
    map <string, string> strings;
    int32_t base;
    string version;
    OSType OS;
};
#endif // MEMINFO_H_INCLUDED
