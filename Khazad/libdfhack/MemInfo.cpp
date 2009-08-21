#include "DFCommon.h"
#include "MemInfo.h"
#include <stdlib.h>
#include <iostream>
memory_info::memory_info()
{
    base = 0;
    classindex = 0;
}

void memory_info::setVersion(const char * v)
{
    version = v;
}
void memory_info::setVersion(string v)
{
    version = v;
}
string memory_info::getVersion()
{
    return version;
}


void memory_info::setOS(const char *os)
{
    string oss = os;
    if(oss == "windows")
        OS = OS_WINDOWS;
    else if(oss == "linux")
        OS = OS_LINUX;
    else
        OS = OS_BAD;
}
void memory_info::setOS(string os)
{
    if(os == "windows")
        OS = OS_WINDOWS;
    else if(os == "linux")
        OS = OS_LINUX;
    else
        OS = OS_BAD;
}
void memory_info::setOS(OSType os)
{
    if(os >= OS_WINDOWS && os < OS_BAD)
    {
        OS = os;
        return;
    }
    OS = OS_BAD;
}
memory_info::OSType memory_info::getOS()
{
    return OS;
}

// copy constructor
memory_info::memory_info(const memory_info &old)
{
    version = old.version;
    OS = old.OS;
    addresses = old.addresses;
    offsets = old.offsets;
    hexvals = old.hexvals;
    strings = old.strings;
    base = old.base;
    classes = old.classes;
    classsubtypes = old.classsubtypes;
    classindex = old.classindex;
}

uint32_t memory_info::getBase ()
{
    return base;
}
void memory_info::setBase (string s)
{
    base = strtol(s.c_str(), NULL, 16);
}
void memory_info::setBase (uint32_t b)
{
    base = b;
}

void memory_info::setOffset (string key, string value)
{
    uint32_t offset = strtol(value.c_str(), NULL, 16);
    offsets[key] = offset;
}
void memory_info::setAddress (string key, string value)
{
    uint32_t address = strtol(value.c_str(), NULL, 16);
    addresses[key] = address;
}
void memory_info::setHexValue (string key, string value)
{
    uint32_t hexval = strtol(value.c_str(), NULL, 16);
    hexvals[key] = hexval;
}
void memory_info::setString (string key, string value)
{
    strings[key] = value;
}

/// FIXME: next three methods should use some kind of custom container so it doesn't have to search so much.
void memory_info::setClass (string name, string vtable)
{
    for (int i=0; i<classes.size(); i++)
    {
        if(classes[i].classname == name)
        {
            classes[i].vtable = strtol(vtable.c_str(), NULL, 16);
            return;
        }
    }
    t_class cls;
    cls.assign = classindex;
    cls.classname = name;
    cls.is_multiclass = false;
    cls.type_offset = 0;
    classindex++;
    cls.vtable = strtol(vtable.c_str(), NULL, 16);
    classes.push_back(cls);
    cout << "class " << name << ", assign " << cls.assign << ", vtable  " << cls.vtable << endl;
}

/// find old entry by name, rewrite, return its multi index. otherwise make a new one, append an empty vector of t_type to classtypes,  return its index.
uint32_t memory_info::setMultiClass (string name, string vtable, string typeoffset)
{
    for (int i=0; i<classes.size(); i++)
    {
        if(classes[i].classname == name)
        {
            classes[i].vtable = strtol(vtable.c_str(), NULL, 16);
            classes[i].type_offset = strtol(typeoffset.c_str(), NULL, 16);
            return classes[i].multi_index;
        }
    }
    t_class cls;
    cls.assign = classindex;
    cls.classname = name;
    cls.is_multiclass = true;
    cls.type_offset = strtol(typeoffset.c_str(), NULL, 16);
    cls.vtable = strtol(vtable.c_str(), NULL, 16);
    cls.multi_index = classsubtypes.size();
    classes.push_back(cls);
    classindex++;

    vector<t_type> thistypes;
    classsubtypes.push_back(thistypes);
    //cout << "multiclass " << name << ", assign " << cls.assign << ", vtable  " << cls.vtable << endl;
    return classsubtypes.size() - 1;
}
void memory_info::setMultiClassChild (uint32_t multi_index, string name, string type)
{
    vector <t_type>& vec = classsubtypes[multi_index];
    for (int i=0; i<vec.size(); i++)
    {
        if(vec[i].classname == name)
        {
            vec[i].type = strtol(type.c_str(), NULL, 16);
            return;
        }
    }
    // new multiclass child
    t_type mcc;
    mcc.assign = classindex;
    classindex++;
    mcc.classname = name;
    mcc.type = strtol(type.c_str(), NULL, 16);
    vec.push_back(mcc);
    //cout << "    classtype " << name << ", assign " << mcc.assign << ", vtable  " << mcc.type << endl;
}

bool memory_info::resolveClassId(uint32_t address, uint32_t & classid)
{
    uint32_t vtable = MreadDWord(address);
    /// FIXME: stupid search. we need a better container
    for(int i = 0;i< classes.size();i++)
    {
        if(classes[i].vtable == vtable) // got class
        {
            // if it is a multiclass, try resolving it
            if(classes[i].is_multiclass)
            {
                vector <t_type>& vec = classsubtypes[classes[i].multi_index];
                uint32_t type = MreadWord(address + classes[i].type_offset);
                //printf ("class %d:%s offset 0x%x\n", i , classes[i].classname.c_str(), classes[i].type_offset);
                // return typed building if successful
                for (int k = 0; k < vec.size();k++)
                {
                    if(vec[k].type == type)
                    {
                        //cout << " multi " <<  address + classes[i].type_offset << " " << vec[k].classname << endl;
                        classid = vec[k].assign;
                        return true;
                    }
                }
            }
            // otherwise return the class we found
            classid = classes[i].assign;
            return true;
        }
    }
    // we failed to find anything that would match
    return false;
}

void memory_info::copyBuildings(vector<string> & v_buildingtypes)
{
    for(int i = 0;i< classes.size();i++)
    {
        v_buildingtypes.push_back(classes[i].classname);
        if(classes[i].is_multiclass)
        {
            vector <t_type>& vec = classsubtypes[classes[i].multi_index];
            for (int k = 0; k < vec.size();k++)
            {
                v_buildingtypes.push_back(vec[k].classname);
            }
        }
    }
}

// change base of all addresses
void memory_info::Rebase(int32_t new_base)
{
  map<string, uint32_t>::iterator iter;

  for(iter = addresses.begin(); iter != addresses.end(); iter++)
  {
      int32_t rebase = - (int32_t)base + new_base;
      addresses[iter->first] = iter->second + rebase;
  }
}

// change all vtable entries by offset
void memory_info::RebaseVTable(int32_t offset)
{
  vector<t_class>::iterator iter;

  for(iter = classes.begin(); iter != classes.end(); iter++)
  {
      iter->vtable += offset;
  }
}

uint32_t memory_info::getAddress (string key)
{
    if(addresses.count(key))
    {
        return addresses[key];
    }
    return 0;
}

uint32_t memory_info::getOffset (string key)
{
    if(offsets.count(key))
    {
        return offsets[key];
    }
    return 0;
}
std::string memory_info::getString (string key)
{
    if(strings.count(key))
        return strings[key];
    else return string("");
}

uint32_t memory_info::getHexValue (string key)
{
    if(hexvals.count(key))
        return hexvals[key];
    return 0;
}
void memory_info::flush()
{
    base = 0;
    addresses.clear();
    offsets.clear();
    strings.clear();
    hexvals.clear();
}
