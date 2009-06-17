#ifndef ACTION__HEADER
#define ACTION__HEADER

#include <stdafx.h>
#include <ScreenManager.h>
#include <Map.h>
#include <Camera.h>
#include <Extract.h>
#include <ColorManager.h>
#include <ConfigManager.h>

class MainKeyListener : public gcn::KeyListener
{
    void keyPressed(gcn::KeyEvent& keyEvent)
    {
        std::cout << "Key pressed: " << keyEvent.getKey().getValue() << std::endl;
    }

    void keyReleased(gcn::KeyEvent& keyEvent)
    {
        std::cout << "Key released: " << keyEvent.getKey().getValue() << std::endl;
    }
};

class ExitActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        UI->setDone(true);
    }
};

class ConfirmExitActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        UI->GetConfirmation("CONFIRM EXIT", UI->getExitListener());
    }
};

class ConfirmationHideListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        UI->HideConfirmationWindow();
    }
};

class DepthChangeActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        gcn::Slider* Source = (gcn::Slider*) actionEvent.getSource();

        float NewLevel = Source->getValue() * MAP->getMapSizeZ();

        SCREEN->MainCamera->setViewHight((Sint16) NewLevel);
    }
};

class CenteringActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        gcn::Button* Source = (gcn::Button*) actionEvent.getSource();
        SCREEN->MainCamera->CenterView();
    }
};

class VerticalActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        gcn::Button* Source = (gcn::Button*) actionEvent.getSource();
        SCREEN->MainCamera->setVerticalMode(!SCREEN->MainCamera->isVerticalMode());
    }
};

class DebuggingToggleActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->setDebuggingDraw(!SCREEN->isDebuggingDraw());
    }
};

class ShadingToggleActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->setShadedDraw(!SCREEN->isShadedDraw());
    }
};

class TakeScreenShotActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->CaptureScreenShot();
    }
};

class FrameToggleActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->setFrameDraw(!SCREEN->isFrameDraw());
    }
};

class HiddenToggleActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->setHiddenDraw(!SCREEN->isHiddenDraw());
    }
};

class IncresseLevelSeperationActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
		SCREEN->MainCamera->changeLevelSeperation(1);
    }
};

class DecresseLevelSeperationActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
		SCREEN->MainCamera->changeLevelSeperation(-1);
    }
};

class IncreseViewLevelsActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->MainCamera->ChangeViewLevels(1);
    }
};

class DecreseViewLevelsActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->MainCamera->ChangeViewLevels(-1);
    }
};

class MoveViewUpActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->MainCamera->MoveViewVertical(1.0);
    }
};

class MoveViewDownActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->MainCamera->MoveViewVertical(-1.0);
    }
};

class OrbitClockwiseActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->MainCamera->OrbitView(CONFIG->OrbitSpeed() / -1000.0);
    }
};

class OrbitCounterClockwiseActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->MainCamera->OrbitView(CONFIG->OrbitSpeed() / 1000.0);
    }
};

class ZoomInActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
         SCREEN->MainCamera->ZoomView(1.0 / (CONFIG->ZoomSpeed() / (50.0 + CONFIG->ZoomSpeed())));
    }
};

class ZoomOutActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->MainCamera->ZoomView((CONFIG->ZoomSpeed() / (50.0 + CONFIG->ZoomSpeed())) / 1.0);
    }
};

class TiltUpActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->MainCamera->TiltView(-10 *  (CONFIG->TiltSpeed() / 1000.0), (float)0.01, (float)10.0);
    }
};

class TiltDownActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->MainCamera->TiltView(10 *  (CONFIG->TiltSpeed() / 1000.0), (float)0.01, (float)10.0);
    }
};

class MapDumpActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->WipeScreen();
        SCREEN->setDrawingFlat();
        SCREEN->RenderLogo();

        SCREEN->RenderTextCentered("Dumping Memory", 0, WHITE, 0);
        SCREEN->Flip();

        if(!MAP->isInitialized())
        {
            EXTRACT->dumpMemory();
            MAP->LoadExtract();
            SCREEN->MainCamera->CenterView();
        }
        else
        {
            EXTRACT->dumpMemory();
            MAP->LoadExtract();
        }
        UI->setMapViewState();
    }
};

class MapLoadActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->WipeScreen();
        SCREEN->setDrawingFlat();
        SCREEN->RenderLogo();

        char buffer[256];
        sprintf(buffer, "Loading from File:  %s", CONFIG->LoadPath());
        SCREEN->RenderTextCentered(buffer, 0, WHITE, 0);
        SCREEN->Flip();

        if(!MAP->isInitialized())
        {
            EXTRACT->loadMap(CONFIG->LoadPath());
            MAP->LoadExtract();
            SCREEN->MainCamera->CenterView();
        }
        else
        {
            EXTRACT->loadMap(CONFIG->LoadPath());
            MAP->LoadExtract();
        }
        UI->setMapViewState();
    }
};

class MapSaveActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        SCREEN->WipeScreen();
        SCREEN->setDrawingFlat();
        SCREEN->RenderLogo();

        char buffer[256];
        sprintf(buffer, "Writing to File:  %s", CONFIG->SavePath());
        SCREEN->RenderTextCentered(buffer, 0, WHITE, 0);
        SCREEN->Flip();

        EXTRACT->writeMap(CONFIG->SavePath());
    }
};

#endif // ACTION__HEADER
