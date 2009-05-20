#include <stdafx.h>

#include <Gui.h>
//#include <ScreenManager.h>
#include <Singleton.h>
#include <ActionListeners.h>


DECLARE_SINGLETON(Ui)

bool Ui::Init()
{
    // Create a Top level Container
    TopWidget = new gcn::Container();
    TopWidget->setOpaque(false);
    TopWidget->setDimension(gcn::Rectangle(0, 0, SCREEN->getWidth(), SCREEN->getHight()));
    GuiChanMainObject->setTop(TopWidget);


    InitMainMenu();
    InitCameraControlMenu();
    InitDepthSlider();

	return true;
}

Ui::Ui()
{
    // Now we create the Gui object to be used with this OpenGL
    // and SDL application.
    // The Gui object needs a Graphics to be able to draw itself and an Input
    // object to be able to check for user input. In this case we provide the
    // Gui object with OpenGL and SDL implementations of these objects hence
    // making Guichan able to utilise OpenGL and SDL.

    GraphicsImplementation = new gcn::OpenGLGraphics();
    GraphicsImplementation->setTargetPlane(SCREEN->getWidth(), SCREEN->getHight());

    Input = new gcn::SDLInput();
    GuiChanMainObject = new gcn::Gui();

    GuiChanMainObject->setGraphics(GraphicsImplementation);
    GuiChanMainObject->setInput(Input);
}

void Ui::InitMainMenu()
{
    MainMenuWindow = new gcn::Window("MAIN MENU");
    MainMenuWindow->setSize(128, 48);
    TopWidget->add(MainMenuWindow);
    MainMenuWindow->setPosition(250, 50);


    // Populate the Window with Buttons
    gcn::ImageButton* MapDumpButton = new gcn::ImageButton("Assets\\Buttons\\Dump.png");
    MapDumpButton->setSize(32, 32);
    MainMenuWindow->add(MapDumpButton, 0, 0);
    gcn::ActionListener* MapDumpListener = new MapDumpActionListener();
    MapDumpButton->addActionListener(MapDumpListener);

    // Populate the Window with Buttons
    gcn::ImageButton* MapLoadButton = new gcn::ImageButton("Assets\\Buttons\\OpenFile.png");
    MapLoadButton->setSize(32, 32);
    MainMenuWindow->add(MapLoadButton, 32, 0);
    gcn::ActionListener* MapLoadListener = new MapLoadActionListener();
    MapLoadButton->addActionListener(MapLoadListener);

    // Populate the Window with Buttons
    gcn::ImageButton* MapSaveButton = new gcn::ImageButton("Assets\\Buttons\\SaveFile.png");
    MapSaveButton->setSize(32, 32);
    MainMenuWindow->add(MapSaveButton, 64, 0);
    gcn::ActionListener* MapSaveListener = new MapSaveActionListener();
    MapSaveButton->addActionListener(MapSaveListener);

}

void Ui::InitCameraControlMenu()
{
    Uint16 ButtonSize = 32;

    // New Window created under the Top object
    CameraControlWindow = new gcn::Window("CAMERA CONTROLS");
    CameraControlWindow->setSize((ButtonSize + 1) * 6, (ButtonSize * 4) - 16);
    TopWidget->add(CameraControlWindow);
    CameraControlWindow->setPosition(SCREEN->getWidth() - 300, 50);

/*
    // Populate the Window with Buttons
    gcn::ImageButton* CenterButton = new gcn::ImageButton("Assets\\Icons\\Center.png");
    CenterButton->setSize(32, 32);
    CameraControlWindow->add(CenterButton, 0, 0);
    gcn::ActionListener* CenterListener = new CenteringActionListener();
    CenterButton->addActionListener(CenterListener);


    gcn::ImageButton* VerticalButton = new gcn::ImageButton("Assets\\Icons\\Vertical.png");
    VerticalButton->setSize(32, 32);
    CameraControlWindow->add(VerticalButton, 32, 0);
    gcn::ActionListener* VerticalListener = new VerticalActionListener();
    VerticalButton->addActionListener(VerticalListener);
*/



    gcn::ImageButton* ViewUpButton = new gcn::ImageButton("Assets\\Buttons\\UpArrow.png");
    ViewUpButton->setSize(32, 32);
    gcn::ActionListener* MoveViewUpListener = new MoveViewUpActionListener();
    ViewUpButton->addActionListener(MoveViewUpListener);
    CameraControlWindow->add(ViewUpButton, ButtonSize * 0, ButtonSize * 1);

    gcn::ImageButton* ViewDownButton = new gcn::ImageButton("Assets\\Buttons\\DownArrow.png");
    ViewDownButton->setSize(32, 32);
    gcn::ActionListener* MoveViewDownListener = new MoveViewDownActionListener();
    ViewDownButton->addActionListener(MoveViewDownListener);
    CameraControlWindow->add(ViewDownButton, ButtonSize * 0, ButtonSize * 2);


    gcn::ImageButton* IncresseLevelsButton = new gcn::ImageButton("Assets\\Buttons\\IncresseLevels.png");
    IncresseLevelsButton->setSize(32, 32);
    gcn::ActionListener* IncresseViewLevelsListener = new IncreseViewLevelsActionListener();
    IncresseLevelsButton->addActionListener(IncresseViewLevelsListener);
    CameraControlWindow->add(IncresseLevelsButton, ButtonSize * 1, ButtonSize * 1);

    gcn::ImageButton* DecreseLevelsButton = new gcn::ImageButton("Assets\\Buttons\\DecresseLevels.png");
    DecreseLevelsButton->setSize(32, 32);
    gcn::ActionListener* DecreseLevelsListener = new DecreseViewLevelsActionListener();
    DecreseLevelsButton->addActionListener(DecreseLevelsListener);
    CameraControlWindow->add(DecreseLevelsButton, ButtonSize * 1, ButtonSize * 2);


    gcn::ImageButton* OrbitClockwiseButton = new gcn::ImageButton("Assets\\Buttons\\Clockwise.png");
    OrbitClockwiseButton->setSize(32, 32);
    gcn::ActionListener* OrbitClockwiseListener = new OrbitClockwiseActionListener();
    OrbitClockwiseButton->addActionListener(OrbitClockwiseListener);
    CameraControlWindow->add(OrbitClockwiseButton, ButtonSize * 2, ButtonSize * 1);

    gcn::ImageButton* OrbitCounterClockwiseButton = new gcn::ImageButton("Assets\\Buttons\\CounterClockwise.png");
    OrbitCounterClockwiseButton->setSize(32, 32);
    gcn::ActionListener* OrbitCounterClockwiseListener = new OrbitCounterClockwiseActionListener();
    OrbitCounterClockwiseButton->addActionListener(OrbitCounterClockwiseListener);
    CameraControlWindow->add(OrbitCounterClockwiseButton, ButtonSize * 2, ButtonSize * 2);


    gcn::ImageButton* ExplodeLevelsButton = new gcn::ImageButton("Assets\\Buttons\\Explode.png");
    ExplodeLevelsButton->setSize(32, 32);
    gcn::ActionListener* ExplodeLevelsListener = new IncresseLevelSeperationActionListener();
    ExplodeLevelsButton->addActionListener(ExplodeLevelsListener);
    CameraControlWindow->add(ExplodeLevelsButton, ButtonSize * 3, ButtonSize * 1);

    gcn::ImageButton* ContractLevelsButton = new gcn::ImageButton("Assets\\Buttons\\Contract.png");
    ContractLevelsButton->setSize(32, 32);
    gcn::ActionListener* ContractLevelsListener = new DecresseLevelSeperationActionListener();
    ContractLevelsButton->addActionListener(ContractLevelsListener);
    CameraControlWindow->add(ContractLevelsButton, ButtonSize * 3, ButtonSize * 2);


    gcn::ImageButton* ZoomInButton = new gcn::ImageButton("Assets\\Buttons\\ZoomIn.png");
    ZoomInButton->setSize(32, 32);
    gcn::ActionListener* ZoomInListener = new ZoomInActionListener();
    ZoomInButton->addActionListener(ZoomInListener);
    CameraControlWindow->add(ZoomInButton, ButtonSize * 4, ButtonSize * 1);

    gcn::ImageButton* ZoomOutButton = new gcn::ImageButton("Assets\\Buttons\\ZoomOut.png");
    ZoomOutButton->setSize(32, 32);
    gcn::ActionListener* ZoomOutListener = new ZoomOutActionListener();
    ZoomOutButton->addActionListener(ZoomOutListener);
    CameraControlWindow->add(ZoomOutButton, ButtonSize * 4, ButtonSize * 2);


    gcn::ImageButton* TiltUpButton = new gcn::ImageButton("Assets\\Buttons\\TiltUp.png");
    TiltUpButton->setSize(32, 32);
    gcn::ActionListener* TiltUpListener = new TiltUpActionListener();
    TiltUpButton->addActionListener(TiltUpListener);
    CameraControlWindow->add(TiltUpButton, ButtonSize * 5, ButtonSize * 1);

    gcn::ImageButton* TiltDownButton = new gcn::ImageButton("Assets\\Buttons\\TiltDown.png");
    TiltDownButton->setSize(32, 32);
    gcn::ActionListener* TiltDownListener = new TiltDownActionListener();
    TiltDownButton->addActionListener(TiltDownListener);
    CameraControlWindow->add(TiltDownButton, ButtonSize * 5, ButtonSize * 2);

}

void Ui::InitDepthSlider()
{
    DepthSlider = new gcn::Slider(0.0, 1.0);
    DepthSlider->setOrientation(gcn::Slider::VERTICAL);
    TopWidget->add(DepthSlider);

    DepthSlider->setSize(25, SCREEN->getHight());
    DepthSlider->setPosition(SCREEN->getWidth() - 25, 0);
    DepthSlider->setValue(0.5);

    gcn::ActionListener* actionListener = new DepthChangeActionListener();
    DepthSlider->addActionListener(actionListener);
}

Ui::~Ui()
{
    delete GuiChanMainObject;
    delete Input;
    delete GraphicsImplementation;
}

bool Ui::ProcessEvent(SDL_Event event, Sint32 RelativeX, Sint32 RelativeY)
{
    Input->pushInput(event);

    if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN)
    {
        return false; // Not consumed in the UI
    }

    if(event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN /*|| event.type == SDL_MOUSEBUTTONUP*/)
    {
        Sint32 RealX;
        Sint32 RealY;
        Uint8 MouseState = SDL_GetMouseState(&RealX, &RealY);

        Sint32 OriginX = RealX - RelativeX;
        Sint32 OriginY = RealY - RelativeY;

        if(isWidgetCollision(DepthSlider, OriginX, OriginY))
        {
            return true;
        }

        if(isWidgetCollision(MainMenuWindow, OriginX, OriginY))
        {
            return true;
        }

        if(isWidgetCollision(CameraControlWindow, OriginX, OriginY))
        {
            return true;
        }
    }

    return false; // Default not consumed
}

bool Ui::isWidgetCollision(gcn::Widget* TestWidget, Uint16 RealX, Uint16 RealY)
{
    if(RealX >= TestWidget->getX() && RealX < (TestWidget->getX() + TestWidget->getWidth()))
    {
        if(RealY >= TestWidget->getY() && RealY < (TestWidget->getY() + TestWidget->getHeight()))
        {
           return true;
        }
    }
    return false;
}

bool Ui::Draw()
{
    GuiChanMainObject->logic();
    GuiChanMainObject->draw();

    return true;
}



