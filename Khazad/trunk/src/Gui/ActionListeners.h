#ifndef ACTION__HEADER
#define ACTION__HEADER

#include <stdafx.h>
#include <Renderer.h>
#include <Map.h>
#include <Camera.h>
#include <ConfigManager.h>
#include <DataManager.h>
#include <PathManager.h>
#include <PathTester.h>
#include <Game.h>

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

class GamePauseActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        GAME->togglePause();
    }
};

class SlowSpeedActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        GAME->setPause(false);
        GAME->setTickRate(10);
    }
};

class MediumSpeedActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        GAME->setPause(false);
        GAME->setTickRate(30);
    }
};

class FastSpeedActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        GAME->setPause(false);
        GAME->setTickRate(90);
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
    }
};

class CenteringActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        gcn::Button* Source = (gcn::Button*) actionEvent.getSource();

        MapCoordinates Coordinate = MAP->getMapCenter();

        Vector3 CenterPoint;
        CenterPoint.x = Coordinate.X;
        CenterPoint.y = Coordinate.Y;
        CenterPoint.z = Coordinate.Z;

        RENDERER->MainCamera->CenterView(CenterPoint);
    }
};

class AngleLockToggleActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        gcn::Button* Source = (gcn::Button*) actionEvent.getSource();
        RENDERER->MainCamera->setAngleLock(!RENDERER->MainCamera->isAngleLock());
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
        RENDERER->TakeScreenShot();
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

class SubterraneanToggleActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->setSubterraneanDraw(!RENDERER->isSubterraneanDraw());
    }
};

class SkyViewToggleActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->setSkyViewDraw(!RENDERER->isSkyViewDraw());
    }
};

class SunLitToggleActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->setSunLitDraw(!RENDERER->isSunLitDraw());
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
        RENDERER->MainCamera->TiltView(10 *  (CONFIG->TiltSpeed() / 1000.0), (float)0.01, (float)10.0);
    }
};

class TiltDownActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->MainCamera->TiltView(-10 *  (CONFIG->TiltSpeed() / 1000.0), (float)0.01, (float)10.0);
    }
};

class DigActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        MapCoordinates TargetCoordinates = RENDERER->getCursor();
        MAP->Dig(TargetCoordinates);
    }
};

class RampActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        MapCoordinates TargetCoordinates = RENDERER->getCursor();
        MAP->DigSlope(TargetCoordinates);
    }
};

class ChannelActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        MapCoordinates TargetCoordinates = RENDERER->getCursor();
        MAP->DigChannel(TargetCoordinates);
    }
};

class MapDumpActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        RENDERER->WipeScreen();
        RENDERER->setDrawingFlat();
        RENDERER->RenderLogo();

        static const SDL_Color WHITE = {255, 255, 255};

        RENDERER->RenderTextCentered("Dumping Memory", 0, WHITE, 0);
        RENDERER->Flip();

        if(MAP->Extract())
        {
            MapCoordinates Coordinate = MAP->getMapCenter();

            Vector3 CenterPoint;
            CenterPoint.x = Coordinate.X;
            CenterPoint.y = Coordinate.Y;
            CenterPoint.z = Coordinate.Z;

            RENDERER->MainCamera->CenterView(CenterPoint);
            RENDERER->setCursor(MAP->getMapCenter());

            UI->setZSliderRange(MAP->getMapSizeZ());
            UI->setZSliders(MAP->getMapSizeZ(),0 );

            RENDERER->MainCamera->SetSliceTop(MAP->getMapSizeZ());
            RENDERER->MainCamera->SetSliceBottom(0);
            RENDERER->ConfineCursor();

            UI->setMapViewState();

            PATH->CreateMapAbstraction();
            TESTER->CreateTestSuite(0, 500);
        }
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

        static const SDL_Color WHITE = {255, 255, 255};

        RENDERER->RenderTextCentered(buffer, 0, WHITE, 0);
        RENDERER->Flip();

        if(MAP->Load(Path(CONFIG->LoadPath())))
        {
            MapCoordinates Coordinate = MAP->getMapCenter();

            Vector3 CenterPoint;

            CenterPoint.x = Coordinate.X;
            CenterPoint.y = Coordinate.Y;
            CenterPoint.z = Coordinate.Z;

            RENDERER->MainCamera->CenterView(CenterPoint);

            UI->setZSliderRange(MAP->getMapSizeZ());
            UI->setZSliders(0, MAP->getMapSizeZ());

            RENDERER->MainCamera->SetSliceTop(MAP->getMapSizeZ());
            RENDERER->MainCamera->SetSliceBottom(0);
            RENDERER->ConfineCursor();

            UI->setMapViewState();
        }
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

        static const SDL_Color WHITE = {255, 255, 255};

        RENDERER->RenderTextCentered(buffer, 0, WHITE, 0);
        RENDERER->Flip();

        MAP->Save(CONFIG->SavePath());
    }
};

#endif // ACTION__HEADER
