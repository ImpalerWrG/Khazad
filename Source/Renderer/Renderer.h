#ifndef RENDERER__HEADER
#define RENDERER__HEADER

#include <stdafx.h>
#include <Singleton.h>

#include <Ogre.h>
#include "Camera.h"

//using namespace Ogre;

class Renderer
{
DECLARE_SINGLETON_CLASS(Renderer)

public:

    bool Init();

    ~Renderer();

    void RenderFrame();

    Ogre::Root* getRoot()                         { return OgreRoot; }
    Ogre::SceneManager* getSceneManager()         { return OgreSceneManager; }
    Ogre::SceneNode* getRootNode()                { return OgreSceneManager->getRootSceneNode(); }
    Ogre::RenderWindow* getWindow()               { return OgreRenderWindow; }

    Camera* getActiveCamera()                     { return Cameras[ActiveCameraIndex]; }

private:

    Ogre::Root* OgreRoot;
    Ogre::SceneManager* OgreSceneManager;
    Ogre::RenderWindow* OgreRenderWindow;

    void defineResources();
    void setupRenderSystem();

    void createRenderWindow();
    void initializeResourceGroups();

    void setupScene();

    vector<Camera*> Cameras;
    uint16_t ActiveCameraIndex;
};

#define RENDERER (Renderer::GetInstance())

#endif  // RENDERER__HEADER
