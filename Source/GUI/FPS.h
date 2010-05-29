#ifndef FPS__HEADER
#define FPS__HEADER

#include <stdafx.h>

#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"

//using namespace Ogre;


class FPSDisplay
{

public:

	FPSDisplay();

	bool Init();

	~FPSDisplay();

	void Update();

private:

	MyGUI::WidgetPtr PanelWidget;

	MyGUI::StaticTextPtr FPSText;

};

#endif  // GUI__HEADER
