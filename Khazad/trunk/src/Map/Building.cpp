#include <Building.h>

#include <TextureManager.h>
#include <DataManager.h>
#include <ModelManager.h>
//#include <Map.h>
#include <Model.h>


Building::Building(MapCoordinates NewCoordinates, Uint8 Xlength, Uint8 Ylength, Sint16 MaterialID, Sint16 BuildingID)
{
    MapCoords = NewCoordinates;
    CubeCoords = CubeCoordinates(MapCoords);

    LengthX = Xlength + 1;
    LengthY = Ylength + 1;

    static Sint16 ModelID = DATA->getLabelIndex("MODEL_BIN");

    BuildingType = BuildingID;
    MaterialType = MaterialID;
    ModelType = DATA->getBuildingData(BuildingType)->getModelID();
}

Building::~Building()
{
    // NOP
}

bool Building::Draw()
{
    static Sint16 Surface = DATA->getLabelIndex("SURFACETYPE_SMOOTH_FLOOR");

    if(ModelType != -1)
    {
        Model* model = MODEL->getModel(ModelType);

        if(model != NULL)
        {
            glPushMatrix();

                glTranslatef(CubeCoords.X, CubeCoords.Y, -HALFCUBE);

                TEXTURE->BindTexture(TEXTURE->MapTexture(MaterialType, Surface));
                model->Render();

            glPopMatrix();
        }
    }
}
