#ifndef ACTION__HEADER
#define ACTION__HEADER

#include <stdafx.h>
#include <ScreenManager.h>
#include <Map.h>
#include <Camera.h>

class DepthChangeActionListener: public gcn::ActionListener
{
    void action(const gcn::ActionEvent& actionEvent)
    {
        gcn::Slider* Source = (gcn::Slider*) actionEvent.getSource();

        float NewLevel = Source->getValue() * MAP->getMapSizeZ();

        SCREEN->MainCamera->setViewHight((Sint16) NewLevel);
    }
};

#endif // ACTION__HEADER
