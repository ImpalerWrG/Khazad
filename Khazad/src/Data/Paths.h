#ifndef PATHS_H_INCLUDED
#define PATHS_H_INCLUDED

#include <string>
#include <vector>

using namespace std;

class Path
{
protected:
    string internal;
    void Crunch();
public:
    Path();
    ~Path();
    Path(char *path);
    Path(const char *path);
    Path(string path);
    operator const char *();
    operator char *();
    operator string();
    friend ostream &operator<<(ostream &stream, Path obj);
};


// translate filesystem path (if needed). It's a HACK and will fail when stretched too far.
char * makePath(char * path);

// free translated path
void freePath (char * path);

#endif // PATHS_H_INCLUDED
