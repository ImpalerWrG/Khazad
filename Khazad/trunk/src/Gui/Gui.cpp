#include <stdafx.h>

#include <Gui.h>
//#include <ScreenManager.h>
#include <Singleton.h>
#include <ActionListeners.h>
#include <Paths.h>
#include "toggleimagebutton.hpp"
#include "KhazWindow.hpp"
#include "KhazImageButton.hpp"
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
    printf("Mystery GL Error in GUI: %d\n",glGetError());
}

void Ui::InitMainMenu()
{
    Uint16 ButtonSize = 34;

    MainMenuWindow = new gcn::KhazWindow("MAIN MENU");
    
    TopWidget->add(MainMenuWindow);
    MainMenuWindow->setPosition(250, 50);

    // Populate the Window with Buttons
    gcn::KhazImageButton* MapDumpButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/Dump.png"));
    MapDumpButton->setSize(ButtonSize, ButtonSize);
    MainMenuWindow->add(MapDumpButton, 0, 0);
    gcn::ActionListener* MapDumpListener = new MapDumpActionListener();
    MapDumpButton->addActionListener(MapDumpListener);

    // Populate the Window with Buttons
    gcn::KhazImageButton* MapLoadButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/OpenFile.png"));
    MapLoadButton->setSize(ButtonSize, ButtonSize);
    MainMenuWindow->add(MapLoadButton, ButtonSize, 0);
    gcn::ActionListener* MapLoadListener = new MapLoadActionListener();
    MapLoadButton->addActionListener(MapLoadListener);

    // Populate the Window with Buttons
    gcn::KhazImageButton* MapSaveButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/SaveFile.png"));
    MapSaveButton->setSize(ButtonSize, ButtonSize);
    MainMenuWindow->add(MapSaveButton, ButtonSize * 2, 0);
    gcn::ActionListener* MapSaveListener = new MapSaveActionListener();
    MapSaveButton->addActionListener(MapSaveListener);

    // Populate the Window with Buttons
    gcn::KhazImageButton* ExitButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/Exit.png"));
    ExitButton->setSize(ButtonSize, ButtonSize);
    MainMenuWindow->add(ExitButton, ButtonSize * 3 , 0);
    gcn::ActionListener* ExitListener = new ConfirmExitActionListener();
    ExitButton->addActionListener(ExitListener);
    MainMenuWindow->resizeToContent();
}

void Ui::InitCameraControlMenu()
{
    Uint16 ButtonSize = 34;

    // Window holding Buttons that control the Camera
    CameraControlWindow = new gcn::KhazWindow("CAMERA CONTROLS");
    TopWidget->add(CameraControlWindow);
    CameraControlWindow->setPosition(RENDERER->getWidth() - 300, 50);
    CameraControlWindow->setVisible(false);


    gcn::ToggleImageButton* VerticalToggleButton = new gcn::ToggleImageButton(Path("Assets/UI/Buttons/lockview.png"),Path("Assets/UI/Buttons/releaseview.png"));
    VerticalToggleButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* VerticalListener = new VerticalActionListener();
    VerticalToggleButton->addActionListener(VerticalListener);
    CameraControlWindow->add(VerticalToggleButton, ButtonSize * 0, ButtonSize * 0);

    gcn::ToggleImageButton* ShadingToggleButton = new gcn::ToggleImageButton( Path("Assets/UI/Buttons/light_uniform.png"), Path("Assets/UI/Buttons/light_gradient.png"));
    ShadingToggleButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* ShadingTobbleListener = new ShadingToggleActionListener();
    ShadingToggleButton->addActionListener(ShadingTobbleListener);
    CameraControlWindow->add(ShadingToggleButton, ButtonSize * 1, ButtonSize * 0);

    gcn::KhazImageButton* TakeScreenShotButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/TakeScreenShot.png"));
    TakeScreenShotButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* TakeScreenShotListener = new TakeScreenShotActionListener();
    TakeScreenShotButton->addActionListener(TakeScreenShotListener);
    CameraControlWindow->add(TakeScreenShotButton, ButtonSize * 2, ButtonSize * 0);

    gcn::KhazImageButton* FrameToggleButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/Frame.png"));
    FrameToggleButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* FrameToggleListener = new FrameToggleActionListener();
    FrameToggleButton->addActionListener(FrameToggleListener);
    CameraControlWindow->add(FrameToggleButton, ButtonSize * 3, ButtonSize * 0);

    gcn::ToggleImageButton* HiddenToggleButton = new gcn::ToggleImageButton( Path("Assets/UI/Buttons/Hidden.png"), Path("Assets/UI/Buttons/Hidden2.png"));
    HiddenToggleButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* HiddenToggleListener = new HiddenToggleActionListener();
    HiddenToggleButton->addActionListener(HiddenToggleListener);
    CameraControlWindow->add(HiddenToggleButton, ButtonSize * 4, ButtonSize * 0);


    gcn::KhazImageButton* ViewUpButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/UpArrow.png"));
    ViewUpButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* MoveViewUpListener = new MoveViewUpActionListener();
    ViewUpButton->addActionListener(MoveViewUpListener);
    CameraControlWindow->add(ViewUpButton, ButtonSize * 0, ButtonSize * 1);

    gcn::KhazImageButton* ViewDownButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/DownArrow.png"));
    ViewDownButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* MoveViewDownListener = new MoveViewDownActionListener();
    ViewDownButton->addActionListener(MoveViewDownListener);
    CameraControlWindow->add(ViewDownButton, ButtonSize * 0, ButtonSize * 2);

    gcn::KhazImageButton* OrbitClockwiseButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/Clockwise.png"));
    OrbitClockwiseButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* OrbitClockwiseListener = new OrbitClockwiseActionListener();
    OrbitClockwiseButton->addActionListener(OrbitClockwiseListener);
    CameraControlWindow->add(OrbitClockwiseButton, ButtonSize * 1, ButtonSize * 1);

    gcn::KhazImageButton* OrbitCounterClockwiseButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/CounterClockwise.png"));
    OrbitCounterClockwiseButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* OrbitCounterClockwiseListener = new OrbitCounterClockwiseActionListener();
    OrbitCounterClockwiseButton->addActionListener(OrbitCounterClockwiseListener);
    CameraControlWindow->add(OrbitCounterClockwiseButton, ButtonSize * 1, ButtonSize * 2);


    gcn::KhazImageButton* ExplodeLevelsButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/Explode.png"));
    ExplodeLevelsButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* ExplodeLevelsListener = new IncresseLevelSeperationActionListener();
    ExplodeLevelsButton->addActionListener(ExplodeLevelsListener);
    CameraControlWindow->add(ExplodeLevelsButton, ButtonSize * 2, ButtonSize * 1);

    gcn::KhazImageButton* ContractLevelsButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/Contract.png"));
    ContractLevelsButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* ContractLevelsListener = new DecresseLevelSeperationActionListener();
    ContractLevelsButton->addActionListener(ContractLevelsListener);
    CameraControlWindow->add(ContractLevelsButton, ButtonSize * 2, ButtonSize * 2);


    gcn::KhazImageButton* ZoomInButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/ZoomIn.png"));
    ZoomInButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* ZoomInListener = new ZoomInActionListener();
    ZoomInButton->addActionListener(ZoomInListener);
    CameraControlWindow->add(ZoomInButton, ButtonSize * 3, ButtonSize * 1);

    gcn::KhazImageButton* ZoomOutButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/ZoomOut.png"));
    ZoomOutButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* ZoomOutListener = new ZoomOutActionListener();
    ZoomOutButton->addActionListener(ZoomOutListener);
    CameraControlWindow->add(ZoomOutButton, ButtonSize * 3, ButtonSize * 2);


    gcn::KhazImageButton* TiltUpButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/TiltUp.png"));
    TiltUpButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* TiltUpListener = new TiltUpActionListener();
    TiltUpButton->addActionListener(TiltUpListener);
    CameraControlWindow->add(TiltUpButton, ButtonSize * 4, ButtonSize * 1);

    gcn::KhazImageButton* TiltDownButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/TiltDown.png"));
    TiltDownButton->setSize(ButtonSize, ButtonSize);
    gcn::ActionListener* TiltDownListener = new TiltDownActionListener();
    TiltDownButton->addActionListener(TiltDownListener);
    CameraControlWindow->add(TiltDownButton, ButtonSize * 4, ButtonSize * 2);
    CameraControlWindow->resizeToContent();
}

void Ui::InitConfirmationWindow()
{
    Uint16 ButtonSize = 34;

    ConfirmationWindow = new gcn::KhazWindow("CONFIRMATION");
    ConfirmationWindow->setSize(ButtonSize * 3 + 16, (ButtonSize * 2) + 16);
    TopWidget->add(ConfirmationWindow);
    ConfirmationWindow->setPosition(RENDERER->getWidth() / 2 - ConfirmationWindow->getWidth() / 2, RENDERER->getHeight() / 2 - ConfirmationWindow->getHeight() / 2);
    ConfirmationWindow->setVisible(false);

    gcn::ActionListener* ConfirmationListener = new ConfirmationHideListener();

    YesButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/Yes.png"));
    YesButton->setSize(ButtonSize, ButtonSize);
    YesButton->addActionListener(ConfirmationListener);
    ConfirmationWindow->add(YesButton, 16, ButtonSize / 2);

    NoButton = new gcn::KhazImageButton(Path("Assets/UI/Buttons/No.png"));
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
    DepthSlider = new gcn::KhazSlider(Path("Assets/UI/Slider/topslider.png"),
                                      Path("Assets/UI/Slider/bottomslider.png"),
                                      Path("Assets/UI/Slider/topbutton.png"),
                                      Path("Assets/UI/Slider/bottombutton.png"),
                                      Path("Assets/UI/Slider/topbutton_hover.png"),
                                      Path("Assets/UI/Slider/bottombutton_hover.png"),
                                      Path("Assets/UI/Slider/topbutton_click.png"),
                                      Path("Assets/UI/Slider/bottombutton_click.png"),
                                      Path("Assets/UI/Slider/background.png"),30);
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
                //cout << "mouse down" << endl;
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
            //cout << "mouse up" << endl;
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



