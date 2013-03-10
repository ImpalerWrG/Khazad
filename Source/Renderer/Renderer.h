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

    bool Init();

    ~Renderer();

    void RenderFrame();
	void RefreshTerrainGeometry();

	void setDirtyNonresponsive(bool newValue)		{}
	void addDirtyTerrain(TerrainRendering* Dirty) 	{ DirtyTerrain.insert(Dirty); }

    Ogre::Root* getRoot()                         { return OgreRoot; }
    Ogre::SceneManager* getSceneManager()         { return OgreSceneManager; }
    Ogre::SceneNode* getRootNode()                { return OgreSceneManager->getRootSceneNode(); }
    Ogre::RenderWindow* getWindow()               { return OgreRenderWindow; }

    Ogre::SceneNode* getZlevelNode(int32_t Zlevel);
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

    std::set<TerrainRendering*> DirtyTerrain;

    std::vector< Ogre::SceneNode* > ZLevelSpindle;   // Organizes the Root Node for each Zlevel

    Ogre::Overlay*			OgreDebugOverlay;
	Ogre::Overlay*			OgreInfoOverlay;

	Timer* RenderTimer;
};

#define RENDERER (Renderer::GetInstance())

#endif  // RENDERER__HEADER
