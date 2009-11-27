#ifndef RENDERER__HEADER
#define RENDERER__HEADER

#include <stdafx.h>

#include <Singleton.h>
#include <Map.h>

class ClipImage;
class ImagePage;
class Camera;
class Vector3;
class Plane;
class Cell;

// typedefs for VBO GL functions
typedef void (APIENTRY * GL_BindBuffer_Func) (GLenum target, GLuint buffer);
typedef void (APIENTRY * GL_DeleteBuffer_Func) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * GL_GenBuffers_Func) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY * GL_BufferData_Func) (GLenum target, int size, const GLvoid *data, GLenum usage);
typedef void * (APIENTRY * GL_MapBuffer_Func) ( GLenum target, GLenum access);
typedef GLboolean (APIENTRY * GL_UnmapBuffer_Func) ( GLenum target );

/*typedef void (APIENTRY * GL_)
void glVertexPointer(   GLint   size,
                        GLenum      type,
                        GLsizei     stride,
                        const GLvoid *      pointer);*/

#ifndef GL_ARRAY_BUFFER_ARB
    #define GL_ARRAY_BUFFER_ARB 0x8892
#endif
#ifndef GL_STATIC_DRAW_ARB
    #define GL_STATIC_DRAW_ARB 0x88E4
#endif

struct RenderObject
{
    GLuint gfxHandle;
    GLuint count; // stores number of vertices if applicable
};

struct s3f
{
    float x;
    float y;
    float z;
};

inline s3f CalculateNormal(const s3f &v1, const s3f &v2, const s3f &v3)
{
    s3f va, vb, n;
    float modulo;
    // Normal N=(v2-v1)x(v3-v1)
    va.x = v2.x - v1.x;  // va = (v2-v1)
    va.y = v2.y - v1.y;
    va.z = v2.z - v1.z;

    vb.x = v3.x - v1.x;  // vb = (v3-v1)
    vb.y = v3.y - v1.y;
    vb.z = v3.z - v1.z;

    n.x = (va.y * vb.z) - (va.z * vb.y);
    n.y = (va.z * vb.x) - (va.x * vb.z);
    n.z = (va.x * vb.y) - (va.y * vb.x);
    modulo = sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
    n.x = n.x / modulo;
    n.y = n.y / modulo;
    n.z = n.z / modulo;
    return n;
}

typedef s3f normal3f;
typedef s3f vertex3f;

struct s2f
{
    float u;
    float v;
};

typedef s2f texcoord2f;

#pragma pack(push, 1)
struct vertex
{
    inline vertex (float x_, float y_, float z_, float u_, float v_, float nx_, float ny_, float nz_ ):
    x(x_),y(y_),z(z_),u(u_),v(v_),nx(nx_),ny(ny_),nz(nz_){};
    inline void translate (float tx, float ty, float tz)
    {
        x += tx;
        y += ty;
        z += tz;
    };
    inline void translate (float tx, float ty)
    {
        x += tx;
        y += ty;
    };
    inline vertex (vertex3f vert, texcoord2f t, normal3f n)
    {
        x = vert.x;
        y = vert.y;
        z = vert.z;
        u = t.u;
        v = t.v;
        nx = n.x;
        ny = n.y;
        nz = n.z;
    };
    // 3x float for vertex
    float x;
    float y;
    float z;
    // 2x float for texture map
    float u;
    float v;
    // 3x float for normal vector
    float nx;
    float ny;
    float nz;
};
#pragma pack(pop)

inline void MixVertexVectors (vector <vertex> * input, vector <vertex> * output)
{
    output->insert( output->end(), input->begin(), input->end());
};

inline void MixVertexVectorsOffset (vector <vertex> * input, vector <vertex> * output, float xoffset, float yoffset)
{
    uint32_t size = input->size();
    output->reserve(size + output->size());
    for(uint32_t i = 0; i < size; i++)
    {
        vertex temp = input->at(i);
        temp.x += xoffset;
        temp.y += yoffset;
        output->push_back(temp);
    }
};

class Renderer
{
DECLARE_SINGLETON_CLASS(Renderer)

public:

    ~Renderer();
    bool Init();

    // check for OpenGL extension
    bool IsExtensionSupported( char* extesion );

    /// VBOs
    // VBO Name Generation Procedure
    GL_BindBuffer_Func glBindBuffer;
    // VBO Bind Procedure
    GL_DeleteBuffer_Func glDeleteBuffers;
    // VBO Data Loading Procedure
    GL_GenBuffers_Func glGenBuffers;
    // VBO Deletion Procedure
    GL_BufferData_Func glBufferData;

    // map/unmap VBO to system memory
    GL_MapBuffer_Func glMapBuffer;
    GL_UnmapBuffer_Func glUnmapBuffer;

    bool isDebuggingDraw()                      { return DebuggingDraw; }
    void setDebuggingDraw(bool NewValue)        { DebuggingDraw = NewValue; }

    bool isFrameDraw()                          { return FrameDraw; }
    void setFrameDraw(bool NewValue)            { FrameDraw = NewValue; }

    bool isShadedDraw()                         { return ShadedDraw; }
    void setShadedDraw(bool NewValue)           { ShadedDraw = NewValue; }

    bool isHiddenDraw()                         { return HiddenDraw; }
    void setHiddenDraw(bool NewValue);

    bool isSubterraneanDraw()                   { return SubterraneanDraw; }
    void setSubterraneanDraw(bool NewValue);

    bool isSkyViewDraw()                        { return SkyViewDraw; }
    void setSkyViewDraw(bool NewValue);

    bool isSunLitDraw()                         { return SunLitDraw; }
    void setSunLitDraw(bool NewValue);

    bool ReSizeScreen(Uint16 Width, Uint16 Hight, bool fullscreen);

    void RenderText(const char* text, Sint8 FontIndex, SDL_Color color, SDL_Rect *location);
    void RenderTextCentered(const char* text, Sint8 FontIndex, SDL_Color color, Sint16 Verticaladjust);

    void RenderSurface(SDL_Surface* RenderSurface, SDL_Rect* location);
    void RenderTexture(GLuint texture, SDL_Rect* Size, SDL_Rect* location, bool invert);

    void RenderLogo();

    void CaptureScreenShot();

    int round(double x);
    int nextpoweroftwo(int x);

    bool ClearDevice();
    bool WipeScreen();
    bool Flip();

    inline Uint16 getWidth()            { return CurrentWidth; };
    inline Uint16 getHeight()           { return CurrentHeight; };

    void DirtyAllLists();
    bool Render();
    void RenderCell(CellCoordinates Coordinates, float ZTranslate, float Shading);

    RenderObject* CreateRenderObject(vector <vertex>* source);
    void CallRenderObject(RenderObject* obj);
    void DeleteRenderObject(RenderObject* obj);

    void PrintDebugging();

    void ShowAxis(void);
    void DrawPoint(Vector3 Point, float Length = 1.0);
    void DrawPlane(Plane ArgumentPlane, float Length = 10);

    void DrawCage(MapCoordinates Coodinates, float x, float y, float z, bool Inflated, float red, float green, float blue);
    void DrawCage(MapCoordinates Coodinates, float x, float y, float z, bool Inflated);

    //void DrawStreamers(Vector3 Point, float x, float y, float z, float Length, float red, float green, float blue);
    //void DrawStreamers(Vector3 Point, float x, float y, float z, float Length);

    void DrawDiamond(MapCoordinates Coodinates, float red, float green, float blue);


    void IncrementTriangles(Uint32 Triangles);
    Uint32 getTriangleCount()                   { return TotalTriangles; }

    void ToggleFullScreen();

    void setDrawing3D();
    void setDrawingFlat();

    bool isCubeDrawn(MapCoordinates Coordinates);

    SDL_Surface* MainScreen()               { return ScreenSurface; }

    Camera* MainCamera;

    MapCoordinates getCursor()                     { return Cursor; }

    void setCursor(MapCoordinates Coordinates);
    void ConfineCursor();

    uint32_t TotalTriangles;
    void binarysprintf(char* buffer, int Input); // TODO move this to some global place;

protected:

    bool haveVBO;
    bool haveSHADOW;

    Uint16 CurrentWidth;
    Uint16 CurrentHeight;
    Uint8 CurrentBPP;

    Uint16 ScreenWidth;
    Uint16 ScreenHeight;
    Uint8 ScreenBPP;

    Uint16 WindowWidth;
    Uint16 WindowHeight;
    Uint8 WindowBPP;

    SDL_Surface* ScreenSurface;

    Uint16 ScreenShotCounter;

    bool FullScreen;

    bool FrameDraw;
    bool ShadedDraw;

    bool HiddenDraw;
    bool SubterraneanDraw;
    bool SkyViewDraw;
    bool SunLitDraw;

    bool FlatDraw;
    bool DebuggingDraw;

    MapCoordinates Cursor;

    std::vector<GLuint*> DrawList;

    gcn::OpenGLSDLImageLoader* imageLoader;
};

#define RENDERER (Renderer::GetInstance())

#endif  // RENDERER__HEADER
