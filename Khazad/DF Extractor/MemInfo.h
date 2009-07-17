#ifndef MEMINFO_H_INCLUDED
#define MEMINFO_H_INCLUDED

class memory_info
{
protected:
    map <string, string> data;
public:
    uint32_t getOffset (string);
    string getString (string);
    bool hasToken (string);
    bool setToken (string, string);
    void flush();
};
#endif // MEMINFO_H_INCLUDED
