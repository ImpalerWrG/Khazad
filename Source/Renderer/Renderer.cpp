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










    Ogre::ManualObject* ManualTileObject = OgreSceneManager->createManualObject("ManualTile");
    ManualTileObject->setDynamic(false);
    ManualTileObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

    ManualTileObject->position(-0.5, -0.5, 0.0);
    ManualTileObject->colour(Ogre::ColourValue::White);
    ManualTileObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

    ManualTileObject->position( 0.5, -0.5, 0.0);
    ManualTileObject->colour(Ogre::ColourValue::White);
    ManualTileObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

    ManualTileObject->position(-0.5,  0.5, 0.0);
    ManualTileObject->colour(Ogre::ColourValue::White);
    ManualTileObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

    ManualTileObject->position( 0.5,  0.5, 0.0);
    ManualTileObject->colour(Ogre::ColourValue::White);
    ManualTileObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

    ManualTileObject->index(0);
    ManualTileObject->index(1);
    ManualTileObject->index(2);
    ManualTileObject->index(3);
    //manual->index(0);

    ManualTileObject->end();
    ManualTileObject->convertToMesh("Tile");















    Ogre::ManualObject* lManualObject = OgreSceneManager->createManualObject("ManualCube");
    lManualObject->setDynamic(false);
    //lManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);



			lManualObject->begin("BaseWhiteNoLighting",Ogre::RenderOperation::OT_LINE_LIST);
			{
				float lAxeSize = 10;
				// X Red
				lManualObject->position(0.0f, 0.0f, 0.0f);
				lManualObject->colour(Ogre::ColourValue::Red);

				lManualObject->position(lAxeSize, 0.0f, 0.0f);
				lManualObject->colour(Ogre::ColourValue::Red);

				// Y Green
				lManualObject->position(0.0f, 0.0f, 0.0f);
				lManualObject->colour(Ogre::ColourValue::Green);

				lManualObject->position(0.0, lAxeSize, 0.0);
				lManualObject->colour(Ogre::ColourValue::Green);

				// Z Blue
				lManualObject->position(0.0f, 0.0f, 0.0f);
				lManualObject->colour(Ogre::ColourValue::Blue);

				lManualObject->position(0.0, 0.0, lAxeSize);
				lManualObject->colour(Ogre::ColourValue::Blue);

				lManualObject->index(0);
				lManualObject->index(1);
				lManualObject->index(2);
				lManualObject->index(3);
				lManualObject->index(4);
				lManualObject->index(5);
			}
			lManualObject->end();

            lManualObject->convertToMesh("ColorCube");


    //Light *light;



    //OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
    //guiDbg->setCaption(OgreRoot->getAutoCreatedWindow()->getDebugText());
}

void Renderer::createCamera()
{
    Camera* newCamera = new Camera();
    Cameras.push_back(newCamera);
    ActiveCameraIndex = 0;
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
