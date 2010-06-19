#include <Renderer.h>

#include <Singleton.h>

#include <Camera.h>

#include <GUI.h>
//using namespace Ogre;

DECLARE_SINGLETON(Renderer)

Renderer::Renderer()
{

}

bool Renderer::Init()
{
    OgreRoot = new Ogre::Root();

    defineResources();
    setupRenderSystem();
    initializeResourceGroups();
    setupScene();

    return true;
}

Renderer::~Renderer()
{

}

void Renderer::RenderFrame()
{
	//GUI->NewFPSDisplay->Update();
    OgreRoot->renderOneFrame();
    OgreRoot->_fireFrameStarted();
}

void Renderer::defineResources()
{
    Ogre::String secName, typeName, archName;
    Ogre::ConfigFile cf;
    cf.load("resources.cfg");

    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
}

void Renderer::setupRenderSystem()
{
    if (!OgreRoot->restoreConfig() && !OgreRoot->showConfigDialog())
    {
        throw Ogre::Exception(52, "User canceled the config dialog!", "Application::setupRenderSystem()");
    }

    OgreRoot->initialise(true, "Khazad");
}

void Renderer::initializeResourceGroups()
{
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void Renderer::setupScene()
{
    OgreSceneManager = OgreRoot->createSceneManager(Ogre::ST_GENERIC, "Default SceneManager");
    OgreRenderWindow = OgreRoot->getAutoCreatedWindow();

    Camera* newCamera = new Camera();
    Cameras.push_back(newCamera);
    ActiveCameraIndex = 0;
    newCamera->Init();


    OgreSceneManager->setAmbientLight(Ogre::ColourValue(1, 1, 1));
    OgreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);






    Ogre::ManualObject* manual = OgreSceneManager->createManualObject("manual");
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

    manual->position(-0.5, -0.5, 0.0);
    manual->position( 0.5, -0.5, 0.0);
    manual->position(-0.5,  0.5, 0.0);
    manual->position( 0.5,  0.5, 0.0);

    manual->index(0);
    manual->index(1);
    manual->index(2);
    manual->index(3);
    //manual->index(0);

    manual->end();
    //OgreSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(manual);


    Ogre::MeshPtr TileMesh = manual->convertToMesh ("Tile");

    //manual->convertToMesh (const String& meshName, const String& groupName = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);


    //Light *light;



    //OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
    //guiDbg->setCaption(OgreRoot->getAutoCreatedWindow()->getDebugText());
}
