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
    ModelType = ModelID;
}

Building::~Building()
{
    // NOP
}

bool Building::Draw()
{
    static Sint16 Surface = DATA->getLabelIndex("SURFACETYPE_CONSTRUCTED_FLOOR");

    //RENDERER->DrawCage(Coordinates, (float) LengthX, (float) LengthY, 1, false, 1.0, 1.0, 1.0);
/*
    glPushMatrix();

        glNormal3f(0.0, 0.0, 1.0);

        glTexCoord2f(0.0, 1.0);      glVertex3f(xa           , ya + LengthY    , -0.4);
        glTexCoord2f(1.0, 0.0);      glVertex3f(xa + LengthX , ya              , -0.4);
        glTexCoord2f(0.0, 0.0);      glVertex3f(xa           , ya              , -0.4);

        glTexCoord2f(0.0, 1.0);      glVertex3f(xa           , ya + LengthY   , -0.4);
        glTexCoord2f(1.0, 1.0);      glVertex3f(xa + LengthX , ya + LengthY   , -0.4);
        glTexCoord2f(0.0, 0.0);      glVertex3f(xa           , ya             , -0.4);

    glPopMatrix();
*/
    Model* model = MODEL->getModel(ModelType);

    glPushMatrix();

        glTranslatef(CubeCoords.X, CubeCoords.Y, -HALFCUBE);

        TEXTURE->BindTexture(TEXTURE->MapTexture(MaterialType, Surface));
        model->Render();

    glPopMatrix();
}
