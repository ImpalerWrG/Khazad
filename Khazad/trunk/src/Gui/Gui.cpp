#include <stdafx.h>

#include <Gui.h>
//#include <ScreenManager.h>
#include <Singleton.h>
#include <ActionListeners.h>
#include <Paths.h>

/**
FIXME: This leaks some bytes. None of those gcn:: things is free'd.
*/

DECLARE_SINGLETON(Ui)

bool Ui::Init()
{
    // Create a Top level Container
    TopWidget = new gcn::Container();
    TopWidget->setOpaque(false);
    TopWidget->setDimension(gcn::Rectangle(0, 0, RENDERER->getWidth(), RENDERER->getHeight()));
    GuiChanMainObject->setTop(TopWidget);

    // Initializing
    ConfirmationAction = NULL; // No actions yet linked to confirmation window
    done = false;  // quit program command recived by UI
    ExitListener = new ExitActionListener();

    // Core Widget Initialization
    InitMainMenu();
    InitCameraControlMenu();
    InitConfirmationWindow();
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
    GraphicsImplementation->setTargetPlane(RENDERER->getWidth(), RENDERER->getHeight());

    Input = new gcn::SDLInput();
    GuiChanMainObject = new gcn::Gui();

    GuiChanMainObject->setGraphics(GraphicsImplementation);
    GuiChanMainObject->setInput(Input);
    // FIXME: why is next line needed on linux to not crash?
    glGetError();
}

void Ui::InitMainMenu()
{
    Uint16 ButtonSize = 32;

    MainMenuWindow = new gcn::Window("MAIN MENU");
    MainMenuWindow->setSize((ButtonSize + 1) * 4, (ButtonSize * 1) + 16);
    TopWidget->add(MainMenuWindow);
    MainMenuWindow->setPosition(250, 50);


    // Populate the Window with Buttons
    gcn::ImageButton* MapDumpButton = new gcn::ImageButton(Path("Assets\\Buttons\\Dump.png"));
    MapDumpButton->setSize(ButtonSize, ButtonSize);
    MainMenuWindow->add(MapDumpButton, 0, 0);
    gcn::ActionListener* MapDumpListener = new MapDumpActionListener();
    MapDumpButton->addActionListener(MapDumpListener);

    // Populate the Window with Buttons
    gcn::ImageButton* MapLoadButton = new gcn::ImageButton(Path("Assets\\Buttons\\OpenFile.png"));
    MapLoadButton->setSize(ButtonSize, ButtonSize);
    MainMenuWindow->add(MapLoadButton, 32, 0);
    gcn::ActionListener* MapLoadListener = new MapLoadActionListener();
    MapLoadButton->addActionListener(MapLoadListener);

    // Populate the Window with Buttons
    gcn::ImageButton* MapSaveButton = new gcn::ImageButton(Path("Assets\\Buttons\\SaveFile.png"));
    MapSaveButton->setSize(ButtonSize, ButtonSize);
    MainMenuWindow->add(MapSaveButton, 64, 0);
    gcn::ActionListener* MapSaveListener = new MapSaveActionListener();
    MapSaveButton->addActionListener(MapSaveListener);

    // Populate the Window with Buttons
    gcn::ImageButton* ExitButton = new gcn::ImageButton(Path("Assets\\Buttons\\Exit.png"));
    ExitButton->setSize(ButtonSize, ButtonSize);
    MainMenuWindow->add(ExitButton, 96, 0);
    gcn::ActionListener* ExitListener = new ConfirmExitActionListener();
    ExitButton->addActionListener(ExitListener);

}

void Ui::InitCameraControlMenu()
{
    Uint16 ButtonSize = 32;

    // Window holding Buttons that control the Camera
    CameraControlWindow = new gcn::Window("CAMERA CONTROLS");
    CameraControlWindow->setSize((ButtonSize + 1) * 6, (ButtonSize * 3) + 16);
    TopWidget->add(CameraControlWindow);
    CameraControlWindow->setPosition(RENDERER->getWidth() - 300, 50);
    CameraControlWindow->setVisible(false);


    gcn::ImageButton* VerticalToggleButton = new gcn::ImageButton(Path("Assets\\Buttons\\Vertical.png"));
    VerticalToggleButton->setSize(32, 32);
    gcn::ActionListener* VerticalListener = new VerticalActionListener();
    VerticalToggleButton->addActionListener(VerticalListener);
    CameraControlWindow->add(VerticalToggleButton, ButtonSize * 0, ButtonSize * 0);

    gcn::ImageButton* DebuggingToggleButton = new gcn::ImageButton(Path("Assets\\Buttons\\Debugging.png"));
    DebuggingToggleButton->setSize(32, 32);
    gcn::ActionListener* DebuggingToggleListener = new DebuggingToggleActionListener();
    DebuggingToggleButton->addActionListener(DebuggingToggleListener);
    CameraControlWindow->add(DebuggingToggleButton, ButtonSize * 1, ButtonSize * 0);

    gcn::ImageButton* ShadingToggleButton = new gcn::ImageButton(Path("Assets\\Buttons\\Shading.png"));
    ShadingToggleButton->setSize(32, 32);
    gcn::ActionListener* ShadingTobbleListener = new ShadingToggleActionListener();
    ShadingToggleButton->addActionListener(ShadingTobbleListener);
    CameraControlWindow->add(ShadingToggleButton, ButtonSize * 2, ButtonSize * 0);

    gcn::ImageButton* TakeScreenShotButton = new gcn::ImageButton(Path("Assets\\Buttons\\TakeScreenShot.png"));
    TakeScreenShotButton->setSize(32, 32);
    gcn::ActionListener* TakeScreenShotListener = new TakeScreenShotActionListener();
    TakeScreenShotButton->addActionListener(TakeScreenShotListener);
    CameraControlWindow->add(TakeScreenShotButton, ButtonSize * 3, ButtonSize * 0);

    gcn::ImageButton* FrameToggleButton = new gcn::ImageButton(Path("Assets\\Buttons\\Frame.png"));
    FrameToggleButton->setSize(32, 32);
    gcn::ActionListener* FrameToggleListener = new FrameToggleActionListener();
    FrameToggleButton->addActionListener(FrameToggleListener);
    CameraControlWindow->add(FrameToggleButton, ButtonSize * 4, ButtonSize * 0);


//    gcn::Image::load(filename)

    HiddenOff = gcn::Image::load(Path("Assets\\Buttons\\Hidden2.png"));
    HiddenOn = gcn::Image::load(Path("Assets\\Buttons\\Hidden.png"));

    //HiddenOff = loader.load(Path("Assets\\Buttons\\Hidden2.png"));
    //HiddenOn = loader.load(Path("Assets\\Buttons\\Hidden.png"));
    HiddenToggleButton = new gcn::ImageButton(HiddenOff);
    HiddenToggleButton->setSize(32, 32);
    gcn::ActionListener* HiddenToggleListener = new HiddenToggleActionListener();
    HiddenToggleButton->addActionListener(HiddenToggleListener);
    CameraControlWindow->add(HiddenToggleButton, ButtonSize * 5, ButtonSize * 0);


    gcn::ImageButton* ViewUpButton = new gcn::ImageButton(Path("Assets\\Buttons\\UpArrow.png"));
    ViewUpButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* MoveViewUpListener = new MoveViewUpActionListener();
    ViewUpButton->addActionListener(MoveViewUpListener);
    CameraControlWindow->add(ViewUpButton, ButtonSize * 0, ButtonSize * 1);

    gcn::ImageButton* ViewDownButton = new gcn::ImageButton(Path("Assets\\Buttons\\DownArrow.png"));
    ViewDownButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* MoveViewDownListener = new MoveViewDownActionListener();
    ViewDownButton->addActionListener(MoveViewDownListener);
    CameraControlWindow->add(ViewDownButton, ButtonSize * 0, ButtonSize * 2);


    gcn::ImageButton* IncresseLevelsButton = new gcn::ImageButton(Path("Assets\\Buttons\\IncresseLevels.png"));
    IncresseLevelsButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* IncresseViewLevelsListener = new IncreseViewLevelsActionListener();
    IncresseLevelsButton->addActionListener(IncresseViewLevelsListener);
    CameraControlWindow->add(IncresseLevelsButton, ButtonSize * 1, ButtonSize * 1);

    gcn::ImageButton* DecreseLevelsButton = new gcn::ImageButton(Path("Assets\\Buttons\\DecresseLevels.png"));
    DecreseLevelsButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* DecreseLevelsListener = new DecreseViewLevelsActionListener();
    DecreseLevelsButton->addActionListener(DecreseLevelsListener);
    CameraControlWindow->add(DecreseLevelsButton, ButtonSize * 1, ButtonSize * 2);


    gcn::ImageButton* OrbitClockwiseButton = new gcn::ImageButton(Path("Assets\\Buttons\\Clockwise.png"));
    OrbitClockwiseButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* OrbitClockwiseListener = new OrbitClockwiseActionListener();
    OrbitClockwiseButton->addActionListener(OrbitClockwiseListener);
    CameraControlWindow->add(OrbitClockwiseButton, ButtonSize * 2, ButtonSize * 1);

    gcn::ImageButton* OrbitCounterClockwiseButton = new gcn::ImageButton(Path("Assets\\Buttons\\CounterClockwise.png"));
    OrbitCounterClockwiseButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* OrbitCounterClockwiseListener = new OrbitCounterClockwiseActionListener();
    OrbitCounterClockwiseButton->addActionListener(OrbitCounterClockwiseListener);
    CameraControlWindow->add(OrbitCounterClockwiseButton, ButtonSize * 2, ButtonSize * 2);


    gcn::ImageButton* ExplodeLevelsButton = new gcn::ImageButton(Path("Assets\\Buttons\\Explode.png"));
    ExplodeLevelsButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* ExplodeLevelsListener = new IncresseLevelSeperationActionListener();
    ExplodeLevelsButton->addActionListener(ExplodeLevelsListener);
    CameraControlWindow->add(ExplodeLevelsButton, ButtonSize * 3, ButtonSize * 1);

    gcn::ImageButton* ContractLevelsButton = new gcn::ImageButton(Path("Assets\\Buttons\\Contract.png"));
    ContractLevelsButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* ContractLevelsListener = new DecresseLevelSeperationActionListener();
    ContractLevelsButton->addActionListener(ContractLevelsListener);
    CameraControlWindow->add(ContractLevelsButton, ButtonSize * 3, ButtonSize * 2);


    gcn::ImageButton* ZoomInButton = new gcn::ImageButton(Path("Assets\\Buttons\\ZoomIn.png"));
    ZoomInButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* ZoomInListener = new ZoomInActionListener();
    ZoomInButton->addActionListener(ZoomInListener);
    CameraControlWindow->add(ZoomInButton, ButtonSize * 4, ButtonSize * 1);

    gcn::ImageButton* ZoomOutButton = new gcn::ImageButton(Path("Assets\\Buttons\\ZoomOut.png"));
    ZoomOutButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* ZoomOutListener = new ZoomOutActionListener();
    ZoomOutButton->addActionListener(ZoomOutListener);
    CameraControlWindow->add(ZoomOutButton, ButtonSize * 4, ButtonSize * 2);


    gcn::ImageButton* TiltUpButton = new gcn::ImageButton(Path("Assets\\Buttons\\TiltUp.png"));
    TiltUpButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* TiltUpListener = new TiltUpActionListener();
    TiltUpButton->addActionListener(TiltUpListener);
    CameraControlWindow->add(TiltUpButton, ButtonSize * 5, ButtonSize * 1);

    gcn::ImageButton* TiltDownButton = new gcn::ImageButton(Path("Assets\\Buttons\\TiltDown.png"));
    TiltDownButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* TiltDownListener = new TiltDownActionListener();
    TiltDownButton->addActionListener(TiltDownListener);
    CameraControlWindow->add(TiltDownButton, ButtonSize * 5, ButtonSize * 2);

}

void Ui::InitConfirmationWindow()
{
    Uint16 ButtonSize = 32;

    ConfirmationWindow = new gcn::Window("CONFIRMATION");
    ConfirmationWindow->setSize(ButtonSize * 3 + 16, (ButtonSize * 2) + 16);
    TopWidget->add(ConfirmationWindow);
    ConfirmationWindow->setPosition(RENDERER->getWidth() / 2 - ConfirmationWindow->getWidth() / 2, RENDERER->getHeight() / 2 - ConfirmationWindow->getHeight() / 2);
    ConfirmationWindow->setVisible(false);

    gcn::ActionListener* ConfirmationListener = new ConfirmationHideListener();

    YesButton = new gcn::ImageButton(Path("Assets\\Buttons\\Yes.png"));
    YesButton->setSize(ButtonSize, ButtonSize);
    YesButton->addActionListener(ConfirmationListener);
    ConfirmationWindow->add(YesButton, 16, ButtonSize / 2);

    NoButton = new gcn::ImageButton(Path("Assets\\Buttons\\No.png"));
    NoButton->setSize(ButtonSize, ButtonSize);
    NoButton->addActionListener(ConfirmationListener);
    ConfirmationWindow->add(NoButton, ButtonSize * 2, ButtonSize / 2);
}

void Ui::HideConfirmationWindow()
{
    ConfirmationWindow->setVisible(false);
}

void Ui::GetConfirmation(const char* Question, gcn::ActionListener* Listener)
{
    if(ConfirmationAction)
    {
        YesButton->removeActionListener(ConfirmationAction);
    }

    ConfirmationWindow->setCaption(Question);
    YesButton->addActionListener(Listener);
    ConfirmationAction = Listener;

    ConfirmationWindow->setVisible(true);
}

void Ui::InitDepthSlider()
{
    DepthSlider = new gcn::KhazSlider(Path("Assets\\Buttons\\gemmed_handle2.png"),Path("Assets\\Buttons\\gemmed_handle.png"),30);
    TopWidget->add(DepthSlider);

    DepthSlider->setSize(32, RENDERER->getHeight());
    DepthSlider->setPosition(RENDERER->getWidth() - 32, 0);

    gcn::ActionListener* actionListener = new DepthChangeActionListener();
    DepthSlider->addActionListener(actionListener);
    DepthSlider->setVisible(false);
}

void Ui::setZSliders(int16_t A, int16_t B)
{
    DepthSlider->setTopSlice(A);
    DepthSlider->setBottomSlice(B);
}

void Ui::setZSliderRange(int16_t Z)
{
    DepthSlider->setMaxZ(Z);
}

void Ui::setMapViewState()
{
    DepthSlider->setVisible(true);
    CameraControlWindow->setVisible(true);
}

Ui::~Ui()
{
    delete GuiChanMainObject;
    delete Input;
    delete GraphicsImplementation;
}

bool Ui::ProcessEvent(SDL_Event event, Sint32 RelativeX, Sint32 RelativeY)
{
    // does the GUI hold the mouse?
    static bool guimousecapture = false;
    if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN)
    {
        return false; // Not consumed in the UI
    }
    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
        Sint32 RealX;
        Sint32 RealY;
        Uint8 MouseState = SDL_GetMouseState(&RealX, &RealY);

        Sint32 OriginX = RealX - RelativeX;
        Sint32 OriginY = RealY - RelativeY;
        if( event.button.button == SDL_BUTTON_LEFT )
        {
            if(  isWidgetCollision(DepthSlider, OriginX, OriginY)
               ||isWidgetCollision(MainMenuWindow, OriginX, OriginY)
               ||isWidgetCollision(CameraControlWindow, OriginX, OriginY)
               ||isWidgetCollision(ConfirmationWindow, OriginX, OriginY) )
            {
                cout << "mouse down" << endl;
                Input->pushInput(event);
                guimousecapture = true;
                return true;
            }
        }
        else if(event.button.button == SDL_BUTTON_WHEELDOWN)
        {
            if(isWidgetCollision(DepthSlider, OriginX, OriginY))
            {
                RENDERER->MainCamera->ChangeViewLevel(-1);
                return true;
            }
        }
        else if(event.button.button == SDL_BUTTON_WHEELUP)
        {
            if(isWidgetCollision(DepthSlider, OriginX, OriginY))
            {
                RENDERER->MainCamera->ChangeViewLevel(1);
                return true;
            }
        }
    }
    else if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
    {
        if(guimousecapture == true)
        {
            cout << "mouse up" << endl;
            guimousecapture = false;
            Input->pushInput(event);
            return true;
        }
    }
    else if(event.type == SDL_MOUSEMOTION)
    {
        // guichan internal mouse position has to be always updated no matter what.
        Input->pushInput(event);
        return guimousecapture;
    }

    return false; // Default not consumed
}

void Ui::updateSizing()
{
    TopWidget->setDimension(gcn::Rectangle(0, 0, RENDERER->getWidth(), RENDERER->getHeight()));
    GraphicsImplementation->setTargetPlane(RENDERER->getWidth(), RENDERER->getHeight());
    DepthSlider->setSize(32, RENDERER->getHeight());
    DepthSlider->setPosition(RENDERER->getWidth() - 32, 0);
    ///FIXME: move windows when viewport shrinks? Maybe make their position scale with the main window...
}


bool Ui::isWidgetCollision(gcn::Widget* TestWidget, Uint16 RealX, Uint16 RealY)
{
    if(TestWidget->isVisible())
    {
        if(RealX >= TestWidget->getX() && RealX < (TestWidget->getX() + TestWidget->getWidth()))
        {
            if(RealY >= TestWidget->getY() && RealY < (TestWidget->getY() + TestWidget->getHeight()))
            {
               return true;
            }
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



