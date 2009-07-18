#ifndef MEMINFO_H_INCLUDED
#define MEMINFO_H_INCLUDED

class memory_info
{
public:
    int32_t rebase;
    map <string, string> data;
    memory_info();
    memory_info(const memory_info&);
    void setRebase(int32_t old_base,int32_t new_base);
    uint32_t getBase (string);
    uint32_t getOffset (string);
    string getString (string);
    bool hasToken (string);
    bool setToken (string, string);
    void flush();
};
#endif // MEMINFO_H_INCLUDED
