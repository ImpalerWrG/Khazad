#include <Renderer.h>

#include <Singleton.h>

#include <Camera.h>

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
    createRenderWindow();
    initializeResourceGroups();
    setupScene();

    return true;
}

Renderer::~Renderer()
{

}

void Renderer::RenderFrame()
{
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
}

void Renderer::createRenderWindow()
{
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


    Ogre::Entity *ent;
    //Light *light;
    char buffer [50];

/*
    for (uint16_t i = 0; i < 10; i++)
    {
        for (uint16_t j = 0; j < 10; j++)
        {
            sprintf(buffer, "Ninja%d", (i * 10) + j);
            ent = OgreSceneManager->createEntity(buffer, "ninja.mesh");
            ent->setCastShadows(true);
            Ogre::SceneNode* NewNode = OgreSceneManager->getRootSceneNode()->createChildSceneNode();
            NewNode->attachObject(ent);
            NewNode->setPosition(i * 100, j * 100, 0);
            NewNode->setDirection(Ogre::Vector3::UNIT_Y);
        }
    }
*/


    //OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
    //guiDbg->setCaption(OgreRoot->getAutoCreatedWindow()->getDebugText());
}

/*
void GeomCore::run( void )
{
   bShutdownSignal = false;
   unsigned long lTimeLastFrame = 0;

   mRoot->getRenderSystem()->_initRenderTargets();

   Ogre::LogManager::getSingletonPtr()->logMessage("Starting Geom main routine...");

    while( !bShutdownSignal ) {
        // Calculate time since last frame and remember current time for next frame
        unsigned long lTimeCurrentFrame = mRoot->getTimer()->getMilliseconds();
        unsigned long lTimeSinceLastFrame = lTimeCurrentFrame - lTimeLastFrame;
        lTimeLastFrame = lTimeCurrentFrame;

        // Update inputmanager
        //mInputMgr->capture();

      Ogre::WindowEventUtilities::messagePump();

        // Render next frame
        if(!mRoot->_fireFrameStarted())
         fireShutdownSignal();

        mRoot->getRenderSystem()->_updateAllRenderTargets();

        if(!mRoot->_fireFrameEnded())
         fireShutdownSignal();
    }
}
*/
