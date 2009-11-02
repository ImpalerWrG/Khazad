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

    BuildingType = BuildingID;
    MaterialType = MaterialID;

    ModelType = DATA->getBuildingData(BuildingType)->getModelID();

    static Sint16 BuildingMaterial = DATA->getLabelIndex("MATERIAL_BUILDING");  // this will keep the Texture Image
    static Sint16 BuildingSurface = DATA->getLabelIndex("SURFACETYPE_CONSTRUCTED_FLOOR");

    Sint16 TextureID = DATA->getBuildingData(BuildingType)->getTextureID();
    if (TextureID != -1)
    {
        Texture = TEXTURE->MapTexture(BuildingMaterial, TextureID);
    }
    else
    {
        Texture = TEXTURE->MapTexture(MaterialType, TEXTURE->PickImageTexture(MaterialType, BuildingSurface));
    }
}

Building::~Building()
{
    // NOP
}

bool Building::Draw()
{
    MapCoordinates TargetCoordinates = MapCoords;
    bool Validated = false;

    for(; TargetCoordinates.X < (MapCoords.X + LengthX); TargetCoordinates.X += 1)
    {
        for(; TargetCoordinates.Y < (MapCoords.Y + LengthY); TargetCoordinates.Y += 1)
        {
            if (RENDERER->isCubeDrawn(TargetCoordinates))
            {
                Validated = true;
                break;
                break;
            }
        }
    }

    if(Validated && ModelType != -1)
    {
        Model* model = MODEL->getModel(ModelType);
        float Scale = DATA->getModelData(ModelType)->getScalar();

        if(model != NULL)
        {
            glPushMatrix();

                glTranslatef(CubeCoords.X + LengthX / 2, CubeCoords.Y + LengthY / 2, -HALFCUBE);
                glScalef(Scale, Scale, Scale);

                TEXTURE->BindTexture(Texture);
                model->Render();

            glPopMatrix();
        }
    }
}
