#ifndef SCREEN__HEADER
#define SCREEN__HEADER

#include <stdafx.h>

#include <PrimTypes.h>
#include <Singleton.h>


class ClipImage;
class ImagePage;
class Camera;
class Vector3;
class Face;
class Cube;


class ScreenManager
{
DECLARE_SINGLETON_CLASS(ScreenManager)

public:

	~ScreenManager();
	bool Init();

	bool Draw();
	bool ReSizeScreen(Uint16 Width, Uint16 Hight);
	void FillRect(SDL_Rect* Rectangle);
	void applyClipCentered(SDL_Rect Offset, ClipImage* Clip);
	void applyClipAt(SDL_Rect Offset, ClipImage* Clip);
	void BlitText(const char* Text, SDL_Color Color, SDL_Rect Offset, Uint8 FontIndex = 0);
	bool Flip();

	void DrawPrim( DiffusePrim* pVertices, Uint32 count );
	void DrawPrim( DiffuseTexPrim* pVertices, Uint32 count );
	void DrawPrim( NormalDiffuseTexPrim* pVertices, Uint32 count );
	void DrawQuads( NormalDiffuseTexPrim* pVertices, Uint32 count );

	bool ClearDevice();
	bool ColorBackground();
	Uint16 getWidth();
	Uint16 getHight();

	void DirtyAllLists();
	bool Render();
	void IncrementTriangles(Uint32 Triangles);
	float getShading(float Zlevel);
	void PrintDebugging();
	void ShowAxis(void);
	void DrawPoint(Vector3 Point, float Length = 1.0);
	void DrawColoredFacet(Face* DrawingFace, float Shading = 1.0, SDL_Color color);
	void DrawCube(Cube* DrawCube, float Shading = 1.0);

    Uint32 getTriangleCount() { return TotalTriangles; }
    void ToggleFullScreen();
    void Enable2D();
    void Disable2D();

	SDL_Color getPickingColor();

	SDL_Surface* MainScreen() { return ScreenSurface; }

private:

	Uint16 SCREEN_WIDTH;
	Uint16 SCREEN_HEIGHT;
	Uint8 SCREEN_BPP;

	SDL_Surface* ScreenSurface;

	std::vector<GLuint*> DrawList;

	Uint8 RedPickingValue;
	Uint8 GreenPickingValue;
	Uint8 BluePickingValue;

    Uint32 TriangleCounter;
    Uint32 TotalTriangles;

    gcn::OpenGLSDLImageLoader* imageLoader;

public:

	Camera* MainCamera;
};

#define SCREEN (ScreenManager::GetInstance())

#endif  // SCREEN__HEADER

