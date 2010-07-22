#include <Renderer.h>

#include <Singleton.h>

#include <Camera.h>

#include <GUI.h>
//#include <ImageManager.h>
//#include <DataManager.h>
#include <Camera.h>

//using namespace Ogre;

DECLARE_SINGLETON(Renderer)

Renderer::Renderer()
{
    OgreDebugOverlay = NULL;
}

bool Renderer::Init()
{
    // TODO replace with entirely manual plugin loading
    #ifdef LINUX_BUILD
        OgreRoot = new Ogre::Root("plugins.linux.cfg");
    #else
        OgreRoot = new Ogre::Root("plugins.windows.cfg");
    #endif

    //OgreRoot = new Ogre::Root();

    defineResources();
    setupRenderSystem();
    initializeResourceGroups();
    setupScene();
    CreateManualObjects();
    createCamera();

    return true;
}

Renderer::~Renderer()
{

}

void Renderer::RenderFrame()
{
    if (OgreDebugOverlay != NULL && OgreDebugOverlay->isVisible())
    {
        UpdateOverlay();
    }

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

    OgreSceneManager->setAmbientLight(Ogre::ColourValue(1, 1, 1));
    OgreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
}

void Renderer::CreateManualObjects()
{
    // Flat tiles need 6
    Ogre::ManualObject* ManualObject;

    ManualObject = OgreSceneManager->createManualObject("ManualDownTile");
    ManualObject->setDynamic(false);

    ManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
    {
        ManualObject->position(-0.5, -0.5, -0.5);
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

        ManualObject->position( 0.5, -0.5, -0.5);
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

        ManualObject->position(-0.5,  0.5, -0.5);
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

        ManualObject->position( 0.5,  0.5, -0.5);
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

        ManualObject->index(0);
        ManualObject->index(1);
        ManualObject->index(2);
        ManualObject->index(3);
    }
    ManualObject->end();
    ManualObject->convertToMesh("DownTile");






    // A 3 pronged marker to show the XYZ axies
    ManualObject = OgreSceneManager->createManualObject("ManualAxialMarker");
    ManualObject->setDynamic(false);

    ManualObject->begin("BaseWhiteNoLighting",Ogre::RenderOperation::OT_LINE_LIST);
    {
        float lAxeSize = 10;
        // X Red
        ManualObject->position(0.0f, 0.0f, 0.0f);
        ManualObject->colour(Ogre::ColourValue::Red);

        ManualObject->position(lAxeSize, 0.0f, 0.0f);
        ManualObject->colour(Ogre::ColourValue::Red);

        // Y Green
        ManualObject->position(0.0f, 0.0f, 0.0f);
        ManualObject->colour(Ogre::ColourValue::Green);

        ManualObject->position(0.0, lAxeSize, 0.0);
        ManualObject->colour(Ogre::ColourValue::Green);

        // Z Blue
        ManualObject->position(0.0f, 0.0f, 0.0f);
        ManualObject->colour(Ogre::ColourValue::Blue);

        ManualObject->position(0.0, 0.0, lAxeSize);
        ManualObject->colour(Ogre::ColourValue::Blue);

        ManualObject->index(0);
        ManualObject->index(1);
        ManualObject->index(2);
        ManualObject->index(3);
        ManualObject->index(4);
        ManualObject->index(5);
    }
    ManualObject->end();
    ManualObject->convertToMesh("AxialMarker");





    // A 3 pronged marker to show the XYZ axies
    ManualObject = OgreSceneManager->createManualObject("ManualWireFrame");
    ManualObject->setDynamic(true);

    ManualObject->begin("BaseWhiteNoLighting",Ogre::RenderOperation::OT_LINE_LIST);
    {
        float HALFCUBE = 0.5;
        // TOP
        ManualObject->position(HALFCUBE, HALFCUBE, HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        ManualObject->position(HALFCUBE, -HALFCUBE, HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        ManualObject->position(-HALFCUBE, -HALFCUBE, HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        ManualObject->position(-HALFCUBE, HALFCUBE, HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        // BOTTOM
        ManualObject->position(HALFCUBE, HALFCUBE, -HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        ManualObject->position(HALFCUBE, -HALFCUBE, -HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        ManualObject->position(-HALFCUBE, -HALFCUBE, -HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        ManualObject->position(-HALFCUBE, HALFCUBE, -HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        // TOP
        ManualObject->index(0);
        ManualObject->index(1);

        ManualObject->index(1);
        ManualObject->index(2);

        ManualObject->index(2);
        ManualObject->index(3);

        ManualObject->index(3);
        ManualObject->index(0);

        // BOTTOM
        ManualObject->index(4);
        ManualObject->index(5);

        ManualObject->index(5);
        ManualObject->index(6);

        ManualObject->index(6);
        ManualObject->index(7);

        ManualObject->index(7);
        ManualObject->index(4);

        // SIDES
        ManualObject->index(0);
        ManualObject->index(4);

        ManualObject->index(1);
        ManualObject->index(5);

        ManualObject->index(2);
        ManualObject->index(6);

        ManualObject->index(3);
        ManualObject->index(7);
    }
    ManualObject->end();
    ManualObject->convertToMesh("WireFrame");
}

void Renderer::createCamera()
{
    Camera* newCamera = new Camera();
    Cameras.push_back(newCamera);
    ActiveCameraIndex = Cameras.size() - 1;

    newCamera->Init();
}

void Renderer::TakeScreenShoot()
{
    OgreRenderWindow->writeContentsToFile("ScreenShoot.png");
}

void Renderer::ToggleOverlay()
{
    OgreDebugOverlay = Ogre::OverlayManager::getSingleton().getByName("Core/DebugOverlay");

    if (OgreDebugOverlay->isVisible())
    {
        OgreDebugOverlay->hide();
    }
    else
    {
        OgreDebugOverlay->show();
    }
}

void Renderer::UpdateOverlay()
{
    static Ogre::String currFps = "Current FPS: ";
    static Ogre::String avgFps = "Average FPS: ";
    static Ogre::String bestFps = "Best FPS: ";
    static Ogre::String worstFps = "Worst FPS: ";
    static Ogre::String tris = "Triangle Count: ";
    static Ogre::String batches = "Batch Count: ";

    Ogre::OverlayElement* guiAvg = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
    Ogre::OverlayElement* guiCurr = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
    Ogre::OverlayElement* guiBest = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
    Ogre::OverlayElement* guiWorst = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

    const Ogre::RenderTarget::FrameStats& stats = RENDERER->getWindow()->getStatistics();

    guiAvg->setCaption(avgFps + Ogre::StringConverter::toString(stats.avgFPS));
    guiCurr->setCaption(currFps + Ogre::StringConverter::toString(stats.lastFPS));
    guiBest->setCaption(bestFps + Ogre::StringConverter::toString(stats.bestFPS) + " " + Ogre::StringConverter::toString(stats.bestFrameTime) + " ms");
    guiWorst->setCaption(worstFps + Ogre::StringConverter::toString(stats.worstFPS) + " " + Ogre::StringConverter::toString(stats.worstFrameTime) + " ms");

    Ogre::OverlayElement* guiTris = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
    guiTris->setCaption(tris + Ogre::StringConverter::toString(stats.triangleCount));

    Ogre::OverlayElement* guiBatches = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
    guiBatches->setCaption(batches + Ogre::StringConverter::toString(stats.batchCount));

	Ogre::OverlayElement* guiDbg = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
	guiDbg->setCaption("");
}
