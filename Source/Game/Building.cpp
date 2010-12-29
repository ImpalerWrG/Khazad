#include <Building.h>

//#include <TextureManager.h>
#include <DataManager.h>
//#include <ModelManager.h>
//#include <Map.h>
//#include <Model.h>


Building::Building(MapCoordinates NewCoordinates, uint8_t Xlength, uint8_t Ylength, int16_t MaterialID, int16_t BuildingID)
{
    MapCoords = NewCoordinates;
    CubeCoords = MapCoords.Cube();

    LengthX = Xlength + 1;
    LengthY = Ylength + 1;

    BuildingType = BuildingID;
    MaterialType = MaterialID;

    ModelType = DATA->getBuildingData(BuildingType)->getModelID();

    static int16_t BuildingMaterial = DATA->getLabelIndex("MATERIAL_BUILDING");  // this will keep the Texture Image
    static int16_t BuildingSurface = DATA->getLabelIndex("SURFACETYPE_CONSTRUCTED_FLOOR");

    int16_t TextureID = DATA->getBuildingData(BuildingType)->getTextureID();
    if (TextureID != INVALID_INDEX)
    {
        //Texture = TEXTURE->MapTexture(BuildingMaterial, TextureID);
    }
    else
    {
        //Texture = TEXTURE->MapTexture(MaterialType, TEXTURE->PickImageTexture(MaterialType, BuildingSurface));
    }
}

Building::~Building()
{
    // NOP
}

bool Building::Init()
{
    return true;
}

CoolDown Building::Update()
{
    return 1;
}

/*
bool Building::Draw(CameraOrientation Orientaion)
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

    if(Validated && ModelType != INVALID_INDEX)
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
*/
