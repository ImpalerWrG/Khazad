#ifndef SCREEN__HEADER
#define SCREEN__HEADER

#include <stdafx.h>

#include <Singleton.h>
#include <SDL_ttf.h>


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

	bool ReSizeScreen(Uint16 Width, Uint16 Hight);

    void RenderText(char* text, Sint8 FontIndex, SDL_Color color, SDL_Rect *location);
    void RenderTextCentered(char* text, Sint8 FontIndex, SDL_Color color, Sint16 Verticaladjust);

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

	Uint16 getWidth();
	Uint16 getHight();

	void DirtyAllLists();
	bool Render();

	void RefreshDrawlist(Cell* TargetCell, GLuint DrawlistID, CameraOrientation Orientation);

	void IncrementTriangles(Uint32 Triangles);

	void PrintDebugging();

	void ShowAxis(void);
	void DrawPoint(Vector3 Point, float Length = 1.0);
    void DrawCage(Vector3 Point, float x, float y, float z);

    Uint32 getTriangleCount() { return TotalTriangles; }
    void ToggleFullScreen();

    void setDrawing3D();
    void setDrawingFlat();

	SDL_Color getPickingColor();

	SDL_Surface* MainScreen() { return ScreenSurface; }

	Camera* MainCamera;


    void binarysprintf(char* buffer, int Input); // TODO move this to some global place;

protected:

	Uint16 ScreenWidth;
	Uint16 ScreenHight;
	Uint8 ScreenBPP;

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

    Uint32 TriangleCounter;
    Uint32 TotalTriangles;

    gcn::OpenGLSDLImageLoader* imageLoader;

};

#define SCREEN (ScreenManager::GetInstance())

#endif  // SCREEN__HEADER

