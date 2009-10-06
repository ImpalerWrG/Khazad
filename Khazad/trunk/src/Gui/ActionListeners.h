#ifndef ACTION__HEADER
#define ACTION__HEADER

#include <stdafx.h>
#include <Renderer.h>
#include <Map.h>
#include <Camera.h>
#include <Cube.h>
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
        gcn::KhazSlider* Source = (gcn::KhazSlider*) actionEvent.getSource();
        RENDERER->MainCamera->SetSliceTop( Source->getTopSlice());
        RENDERER->MainCamera->SetSliceBottom(Source->getBottomSlice());
        //cout << "TOP: " << Source->getTopSlice() << " BOTTOM: " <<  Source->getBottomSlice() << endl;
    }
};

class CenteringActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        gcn::Button* Source = (gcn::Button*) actionEvent.getSource();
        RENDERER->MainCamera->CenterView(MAP->getMapCenter());
    }
};

class VerticalActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        gcn::Button* Source = (gcn::Button*) actionEvent.getSource();
        RENDERER->MainCamera->setVerticalMode(!RENDERER->MainCamera->isVerticalMode());
    }
};

class DebuggingToggleActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->setDebuggingDraw(!RENDERER->isDebuggingDraw());
    }
};

class ShadingToggleActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->setShadedDraw(!RENDERER->isShadedDraw());
    }
};

class TakeScreenShotActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->CaptureScreenShot();
    }
};

class FrameToggleActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->setFrameDraw(!RENDERER->isFrameDraw());
    }
};

class HiddenToggleActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->setHiddenDraw(!RENDERER->isHiddenDraw());
    }
};

class IncresseLevelSeperationActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
		RENDERER->MainCamera->changeLevelSeperation(1);
    }
};

class DecresseLevelSeperationActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
		RENDERER->MainCamera->changeLevelSeperation(-1);
    }
};

class IncreseViewLevelsActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->MainCamera->ChangeViewLevel(1);
    }
};

class DecreseViewLevelsActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->MainCamera->ChangeViewLevel(-1);
    }
};

class MoveViewUpActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->MainCamera->MoveViewVertical(1.0);
    }
};

class MoveViewDownActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->MainCamera->MoveViewVertical(-1.0);
    }
};

class OrbitClockwiseActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->MainCamera->OrbitView(CONFIG->OrbitSpeed() / 1000.0);
    }
};

class OrbitCounterClockwiseActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->MainCamera->OrbitView(CONFIG->OrbitSpeed() / -1000.0);
    }
};

class ZoomInActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->MainCamera->ZoomView(1.0 / (CONFIG->ZoomSpeed() / (50.0 + CONFIG->ZoomSpeed())));
    }
};

class ZoomOutActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->MainCamera->ZoomView((CONFIG->ZoomSpeed() / (50.0 + CONFIG->ZoomSpeed())) / 1.0);
    }
};

class TiltUpActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->MainCamera->TiltView(-10 *  (CONFIG->TiltSpeed() / 1000.0), (float)0.01, (float)10.0);
    }
};

class TiltDownActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->MainCamera->TiltView(10 *  (CONFIG->TiltSpeed() / 1000.0), (float)0.01, (float)10.0);
    }
};

class DigActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        Vector3 CubePosition = RENDERER->MainCamera->getCursor();
        Cube* TargetCube = MAP->getCube((Sint32) CubePosition.x, (Sint32) CubePosition.y, (Sint32) CubePosition.z);
        if(TargetCube != NULL)
        {
            TargetCube->Dig();
        }
    }
};

class MapDumpActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->WipeScreen();
        RENDERER->setDrawingFlat();
        RENDERER->RenderLogo();

        RENDERER->RenderTextCentered("Dumping Memory", 0, WHITE, 0);
        RENDERER->Flip();

        MAP->Extract();

        RENDERER->MainCamera->CenterView(MAP->getMapCenter());
        UI->setZSliderRange(MAP->getMapSizeZ());
        UI->setZSliders(MAP->getMapSizeZ(),0 );
        RENDERER->MainCamera->SetSliceTop(MAP->getMapSizeZ());
        RENDERER->MainCamera->SetSliceBottom(0);
        RENDERER->MainCamera->ConfineCursor();
        UI->setMapViewState();
    }
};

class MapLoadActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->WipeScreen();
        RENDERER->setDrawingFlat();
        RENDERER->RenderLogo();

        char buffer[256];
        sprintf(buffer, "Loading from File:  %s", CONFIG->LoadPath());
        RENDERER->RenderTextCentered(buffer, 0, WHITE, 0);
        RENDERER->Flip();

        MAP->Load(Path(CONFIG->LoadPath()));

        RENDERER->MainCamera->CenterView(MAP->getMapCenter());
        UI->setZSliderRange(MAP->getMapSizeZ());
        UI->setZSliders(0,MAP->getMapSizeZ() );
        RENDERER->MainCamera->SetSliceTop(MAP->getMapSizeZ());
        RENDERER->MainCamera->SetSliceBottom(0);
        RENDERER->MainCamera->ConfineCursor();

        UI->setMapViewState();
    }
};

class MapSaveActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        if (!MAP->isMapLoaded())
        {
        	// map not loaded, exit
        	return;
        }

        RENDERER->WipeScreen();
        RENDERER->setDrawingFlat();
        RENDERER->RenderLogo();

        char buffer[256];
        sprintf(buffer, "Writing to File:  %s", CONFIG->SavePath());
        RENDERER->RenderTextCentered(buffer, 0, WHITE, 0);
        RENDERER->Flip();

        MAP->Save(CONFIG->SavePath());
    }
};

#endif // ACTION__HEADER
