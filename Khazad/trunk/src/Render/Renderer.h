#ifndef SCREEN__HEADER
#define SCREEN__HEADER

#include <stdafx.h>

#include <Singleton.h>


class ClipImage;
class ImagePage;
class Camera;
class Vector3;
class Plane;
class Cube;
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

    bool isSubTerranianDraw()                   { return SubTerranianDraw; }
    void setSubTerranianDraw(bool NewValue);

    bool isSkyViewDraw()                        { return SkyViewDraw; }
    void setSkyViewDraw(bool NewValue);

    bool isSunLitDraw()                         { return SunLitDraw; }
    void setSunLitDraw(bool NewValue);

    bool ReSizeScreen(Uint16 Width, Uint16 Hight, bool fullscreen);

    void RenderText(const char* text, Sint8 FontIndex, SDL_Color color, SDL_Rect *location);
    void RenderTextCentered(const char* text, Sint8 FontIndex, SDL_Color color, Sint16 Verticaladjust);

    void RenderSurface(SDL_Surface* RenderSurface, SDL_Rect* location);
    void RenderTexture(GLuint texture, SDL_Rect* Size, SDL_Rect* location);

    void RenderLogo();

    void CaptureScreenShot();

    void applyClipCentered(SDL_Rect Offset, ClipImage* Clip);
    void applyClipAt(SDL_Rect Offset, ClipImage* Clip);

    int round(double x);
    int nextpoweroftwo(int x);

    bool ClearDevice();
    bool WipeScreen();
    bool Flip();

    inline Uint16 getWidth()
    {
        return CurrentWidth;
    };
    inline Uint16 getHeight()
    {
        return CurrentHeight;
    };

    void DirtyAllLists();
    bool Render();
    void RenderCell(Sint16 Zlevel, Sint32 SizeX, Sint32 SizeY, float ZTranslate, float Shading, bool drawtops);

    RenderObject *CreateRenderObject (vector <vertex> * source);
    void CallRenderObject(RenderObject * obj);
    void DeleteRenderObject(RenderObject * obj);

    void PrintDebugging();

    void ShowAxis(void);
    void DrawPoint(Vector3 Point, float Length = 1.0);
    void DrawPlane(Plane ArgumentPlane, float Length = 10);

    void DrawCage(Vector3 Point, float x, float y, float z, bool Inflated, float red,float green,float blue);
    void DrawCage(Vector3 Point, float x, float y, float z, bool Inflated);
    void DrawStreamers(Vector3 Point, float x, float y, float z, float Length, float red,float green,float blue);
    void DrawStreamers(Vector3 Point, float x, float y, float z, float Length);

    void IncrementTriangles(Uint32 Triangles);
    Uint32 getTriangleCount() { return TotalTriangles; }

    void ToggleFullScreen();

    void setDrawing3D();
    void setDrawingFlat();

    bool isCubeDrawn(Cube* TestCube);

    SDL_Color getPickingColor();

    SDL_Surface* MainScreen() { return ScreenSurface; }

    Camera* MainCamera;

    uint32_t TotalTriangles;
    void binarysprintf(char* buffer, int Input); // TODO move this to some global place;

protected:
    bool haveVBO;

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

    SDL_Surface* LogoSurface;  //Temporary TODO remove

    Uint16 ScreenShotCounter;

    bool FullScreen;

    bool FrameDraw;
    bool ShadedDraw;

    bool HiddenDraw;
    bool SubTerranianDraw;
    bool SkyViewDraw;
    bool SunLitDraw;

    bool FlatDraw;
    bool DebuggingDraw;

    std::vector<GLuint*> DrawList;

    Uint8 RedPickingValue;
    Uint8 GreenPickingValue;
    Uint8 BluePickingValue;

    gcn::OpenGLSDLImageLoader* imageLoader;

};

#define RENDERER (Renderer::GetInstance())

#endif  // SCREEN__HEADER

