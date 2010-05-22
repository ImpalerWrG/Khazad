#include <Paths.h>


Path::Path()
{

}

Path::~Path()
{

}
// properly encapsulated fail, It'll work for now. /px
void Path::Crunch()
{
    #ifdef LINUX_BUILD
    char src = '\\';
    const char *target = "/";
    #else
    char src = '/';
    const char *target = "\\";
    #endif
    int len = internal.length();
    for( int i = 0; i < len; i++ )
    {
          if ( internal[i]==src)
          {
              internal.replace(i,1,target);
          }
    }
}
Path::Path(char * path)
{
    internal = path;
    Crunch();
}

Path::Path(const char * path)
{
    internal = path;
    Crunch();
}


Path::Path(string path)
{
    internal = path;
    Crunch();
}

Path::operator const char * ()
{
    return internal.c_str();
}

/// DANGEROUS!
Path::operator char * ()
{
    return (char *) internal.c_str();
}

Path::operator string ()
{
    return internal;
}

// yay for operators
ostream &operator<<(ostream &stream, Path obj)
{
  stream << obj.internal;
  return stream;  // return the stream
}
