#ifndef RENDERER__HEADER
#define RENDERER__HEADER

#include <stdafx.h>
#include <Singleton.h>

#include <Ogre.h>

class TerrainRendering;
class Camera;
class Timer;

class Renderer
{
DECLARE_SINGLETON_CLASS(Renderer)

public:

    bool InitializeRenderer();

    ~Renderer();

    void RenderFrame();
	void RefreshTerrainGeometry();
	void registerTerrainRender(TerrainRendering* NewTerrain);

    Ogre::Root* getRoot()                         { return OgreRoot; }
    Ogre::SceneManager* getSceneManager()         { return OgreSceneManager; }
    Ogre::SceneNode* getRootNode()                { return OgreSceneManager->getRootSceneNode(); }
    Ogre::RenderWindow* getWindow()               { return OgreRenderWindow; }

    void setSliceLevels(int32_t Top, int32_t Bottom);

    Camera* getActiveCamera()                     { return Cameras[ActiveCameraIndex]; }

    void ToggleOverlay();
    void UpdateOverlay();

    void FocusActiveCameraAt(Ogre::Vector3 Point);

    void TakeScreenShoot();
    Timer* getRenderTimer()                       { return RenderTimer; }

private:

    Ogre::Root* OgreRoot;

    Ogre::SceneManager* OgreSceneManager;
    Ogre::RenderWindow* OgreRenderWindow;

    void defineResources();
    void setupRenderSystem();
    void createSunLight();

    void createRenderWindow();
    void initializeResourceGroups();

    void setupScene();
    void createCamera();

    std::vector<Camera*> Cameras;
    uint16_t ActiveCameraIndex;

	std::vector<TerrainRendering*> TerrainRenders;

    Ogre::Overlay*			OgreDebugOverlay;
	Ogre::Overlay*			OgreInfoOverlay;

	Timer* RenderTimer;
};

#define RENDERER (Renderer::GetInstance())

#endif  // RENDERER__HEADER
