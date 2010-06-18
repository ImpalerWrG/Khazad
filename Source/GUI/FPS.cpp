#include <FPS.h>

#include <GUI.h>

#include <Game.h>
#include <Renderer.h>



FPSDisplay::FPSDisplay()
{

}

bool FPSDisplay::Init()
{

	//PanelWidget = GUI->getGUI()->createWidget<MyGUI::Widget>("Window", MyGUI::IntCoord(10, 10, 200, 200), MyGUI::Align::HCenter, "Main");

	//MyGUI::StaticTextPtr Title = PanelWidget->createWidget<MyGUI::StaticText>("StaticText", 5, 5, 200, 32, MyGUI::Align::Center , "Main");
	//Title->setCaption("FPS");

	//FPSText = PanelWidget->createWidget<MyGUI::StaticText>("StaticText", 5, 32, 200, 150, MyGUI::Align::Center , "Main");
	return true;
}

FPSDisplay::~FPSDisplay()
{

}

void FPSDisplay::Update()
{
    /*
	Ogre::RenderTarget::FrameStats stats = RENDERER->getWindow()->getStatistics();
	std::ostringstream oss;
	Ogre::String s;

	oss << "FPS: " << std::fixed << std::setprecision(1) << stats.lastFPS << "\n";
	oss << "Avg FPS: " << std::fixed << std::setprecision(1) << stats.avgFPS << "\n";
	oss << "Best FPS: " << std::fixed << std::setprecision(1) << stats.bestFPS << "\n";
	oss << "Worst FPS: " << std::fixed << std::setprecision(1) << stats.worstFPS << "\n";
	oss << "Tricount: " << std::fixed << std::setprecision(1) << stats.triangleCount << "\n";
	oss << "Batch Count: " << std::fixed << std::setprecision(1) << stats.batchCount << "\n";
	s = oss.str();
	FPSText->setCaption(s);
	*/

}
