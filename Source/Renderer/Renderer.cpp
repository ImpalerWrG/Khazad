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





			Ogre::MaterialPtr lMaterial = Ogre::MaterialManager::getSingleton().create("GrassMaterial", "General", true);
			Ogre::Technique* lFirstTechnique = lMaterial->getTechnique(0);
			Ogre::Pass* lFirstPass = lFirstTechnique->getPass(0);
			lFirstPass->setLightingEnabled(false);

			// We create the TextureUnit and tell it to use a specific texture (using it's name).
			// The texture must be available in a resourcegroup at render time.
			Ogre::TextureUnitState* lTextureUnit = lFirstPass->createTextureUnitState();
			lTextureUnit->setTextureName("grass.png", Ogre::TEX_TYPE_2D);
			lTextureUnit->setTextureCoordSet(0);


    GrassMat = lMaterial;


    Ogre::ManualObject* ManualTileObject = OgreSceneManager->createManualObject("ManualTile");
    ManualTileObject->setDynamic(false);
    ManualTileObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

    ManualTileObject->position(-0.45, -0.45, 0.0);
    ManualTileObject->colour(Ogre::ColourValue::White);
    ManualTileObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

    ManualTileObject->position( 0.45, -0.45, 0.0);
    ManualTileObject->colour(Ogre::ColourValue::White);
    ManualTileObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

    ManualTileObject->position(-0.45,  0.45, 0.0);
    ManualTileObject->colour(Ogre::ColourValue::White);
    ManualTileObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

    ManualTileObject->position( 0.45,  0.45, 0.0);
    ManualTileObject->colour(Ogre::ColourValue::White);
    ManualTileObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

    ManualTileObject->index(0);
    ManualTileObject->index(1);
    ManualTileObject->index(2);
    ManualTileObject->index(3);
    //manual->index(0);

    ManualTileObject->end();
    ManualTileObject->convertToMesh("Tile");






    Ogre::ManualObject* ManualCubeObject = OgreSceneManager->createManualObject("ManualCube");
    ManualCubeObject->setDynamic(false);
    ManualCubeObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

        float lSize = 0.45f;

        float cp = 1.0f * lSize ;
        float cm = -1.0f * lSize;

        ManualCubeObject->position(cm, cp, cm);// a vertex
        ManualCubeObject->colour(Ogre::ColourValue(0.0f,1.0f,0.0f,1.0f));
        ManualCubeObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

        ManualCubeObject->position(cp, cp, cm);// a vertex
        ManualCubeObject->colour(Ogre::ColourValue(1.0f,1.0f,0.0f,1.0f));
        ManualCubeObject->position(cp, cm, cm);// a vertex
        ManualCubeObject->colour(Ogre::ColourValue(1.0f,0.0f,0.0f,1.0f));
        ManualCubeObject->position(cm, cm, cm);// a vertex
        ManualCubeObject->colour(Ogre::ColourValue(0.0f,0.0f,0.0f,1.0f));

        ManualCubeObject->position(cm, cp, cp);// a vertex
        ManualCubeObject->colour(Ogre::ColourValue(0.0f,1.0f,1.0f,1.0f));
        ManualCubeObject->position(cp, cp, cp);// a vertex
        ManualCubeObject->colour(Ogre::ColourValue(1.0f,1.0f,1.0f,1.0f));
        ManualCubeObject->position(cp, cm, cp);// a vertex
        ManualCubeObject->colour(Ogre::ColourValue(1.0f,0.0f,1.0f,1.0f));
        ManualCubeObject->position(cm, cm, cp);// a vertex
        ManualCubeObject->colour(Ogre::ColourValue(0.0f,0.0f,1.0f,1.0f));

        // face behind / front
        ManualCubeObject->triangle(0,1,2);
        ManualCubeObject->triangle(2,3,0);
        ManualCubeObject->triangle(4,6,5);
        ManualCubeObject->triangle(6,4,7);

        // face top / down
        ManualCubeObject->triangle(0,4,5);
        ManualCubeObject->triangle(5,1,0);
        ManualCubeObject->triangle(2,6,7);
        ManualCubeObject->triangle(7,3,2);

        // face left / right
        ManualCubeObject->triangle(0,7,4);
        ManualCubeObject->triangle(7,0,3);
        ManualCubeObject->triangle(1,5,6);
        ManualCubeObject->triangle(6,2,1);

    ManualCubeObject->end();
    ManualCubeObject->convertToMesh("ColorCube");


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
