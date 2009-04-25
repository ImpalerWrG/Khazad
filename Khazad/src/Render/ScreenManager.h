#ifndef SCREEN__HEADER
#define SCREEN__HEADER

#include <stdafx.h>

#include <Singleton.h>


class ClipImage;
class ImagePage;
class Camera;
class Vector3;
class Face;
class Cube;
class Cell;


class ScreenManager
{
DECLARE_SINGLETON_CLASS(ScreenManager)

public:

	~ScreenManager();
	bool Init();

	bool isFrameDraw()                  { return FrameDraw; }
	bool setFrameDraw(bool NewValue)    { FrameDraw = NewValue; }

	bool isShadedDraw()                 { return ShadedDraw; }
	bool setShadedDraw(bool NewValue);

	bool ReSizeScreen(Uint16 Width, Uint16 Hight);

	void applyClipCentered(SDL_Rect Offset, ClipImage* Clip);
	void applyClipAt(SDL_Rect Offset, ClipImage* Clip);


    bool ClearDevice();
	bool WipeScreen();
	bool Flip();

	Uint16 getWidth();
	Uint16 getHight();

	void DirtyAllLists();
	bool Render();

	void RefreshDrawlist(Cell* TargetCell, GLuint DrawlistID, Direction Orientation, bool Execute);

	void IncrementTriangles(Uint32 Triangles);

	bool InSlice(float Zlevel);
	float getShading(float Zlevel);

	void PrintDebugging();

	void ShowAxis(void);
	void DrawPoint(Vector3 Point, float Length = 1.0);

	//void DrawColoredFacet(Face* DrawingFace, float Shading = 1.0, SDL_Color color);
	//void DrawCube(Cube* DrawCube, float Shading = 1.0);

    void DrawCage(Vector3 Point, float x, float y, float z);

    Uint32 getTriangleCount() { return TotalTriangles; }
    void ToggleFullScreen();
    void Enable2D();
    void Disable2D();

	SDL_Color getPickingColor();

	SDL_Surface* MainScreen() { return ScreenSurface; }

	Camera* MainCamera;

protected:

	Uint16 ScreenWidth;
	Uint16 ScreenHight;
	Uint8 ScreenBPP;

	SDL_Surface* ScreenSurface;
	bool FullScreen;

	bool FrameDraw;
	bool ShadedDraw;

	std::vector<GLuint*> DrawList;

	Uint8 RedPickingValue;
	Uint8 GreenPickingValue;
	Uint8 BluePickingValue;

    Uint32 TriangleCounter;
    Uint32 TotalTriangles;

    gcn::OpenGLSDLImageLoader* imageLoader;

};

#define SCREEN (ScreenManager::GetInstance())

#endif  // SCREEN__HEADER

