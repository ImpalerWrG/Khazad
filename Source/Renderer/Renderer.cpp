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
