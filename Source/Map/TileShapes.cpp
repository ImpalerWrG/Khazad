#include <TileShapes.h>
#include <Coordinates.h>

#include <Renderer.h>

void CreateAllEntities()
{
    CreateFlatTiles();

    CreateSlopedTiles();
}

void CreateFlatTiles()
{
    // Flat tiles need 6
    Ogre::ManualObject* ManualObject;

    ManualObject = RENDERER->getSceneManager()->createManualObject("ManualUpTile");
    ManualObject->setDynamic(false);

    ManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    {
        ManualObject->position( HALFCUBE,  HALFCUBE, -HALFCUBE);  // North East Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

        ManualObject->position(-HALFCUBE,  HALFCUBE, -HALFCUBE);  // South East Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

        ManualObject->position( HALFCUBE, -HALFCUBE, -HALFCUBE);  // North West Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

        ManualObject->position(-HALFCUBE, -HALFCUBE, -HALFCUBE);  // South West Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));


        //ManualObject->index(0);
        //ManualObject->index(1);
        //ManualObject->index(2);
        //ManualObject->index(3);

        ManualObject->triangle(0, 1, 2);
        ManualObject->triangle(3, 2, 1);
    }
    ManualObject->end();
    ManualObject->convertToMesh("UpTile");
    RENDERER->getSceneManager()->destroyManualObject(ManualObject);


    ManualObject = RENDERER->getSceneManager()->createManualObject("ManualDownTile");
    ManualObject->setDynamic(false);

    ManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    {
        ManualObject->position( HALFCUBE,  HALFCUBE, -HALFCUBE);  // North East Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

        ManualObject->position(-HALFCUBE,  HALFCUBE, -HALFCUBE);  // South East Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

        ManualObject->position( HALFCUBE, -HALFCUBE, -HALFCUBE);  // North West Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

        ManualObject->position(-HALFCUBE, -HALFCUBE, -HALFCUBE);  // South West Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

        //ManualObject->index(0);
        //ManualObject->index(1);
        //ManualObject->index(2);
        //ManualObject->index(3);
        ManualObject->triangle(0, 1, 2);
        ManualObject->triangle(3, 2, 1);
    }
    ManualObject->end();
    ManualObject->convertToMesh("DownTile");
    RENDERER->getSceneManager()->destroyManualObject(ManualObject);


    ManualObject = RENDERER->getSceneManager()->createManualObject("ManualNorthTile");
    ManualObject->setDynamic(false);

    ManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    {
        ManualObject->position( HALFCUBE, -HALFCUBE, HALFCUBE);  // East South Up
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

        ManualObject->position( HALFCUBE, -HALFCUBE, -HALFCUBE);  // East South Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

        ManualObject->position( -HALFCUBE, -HALFCUBE, HALFCUBE);  // West South Up
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

        ManualObject->position( -HALFCUBE, -HALFCUBE, -HALFCUBE);  // West South Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

        /*ManualObject->index(0);
        ManualObject->index(1);
        ManualObject->index(2);
        ManualObject->index(3);*/
        ManualObject->triangle(0, 1, 2);
        ManualObject->triangle(3, 2, 1);
    }
    ManualObject->end();
    ManualObject->convertToMesh("NorthTile");
    RENDERER->getSceneManager()->destroyManualObject(ManualObject);


    ManualObject = RENDERER->getSceneManager()->createManualObject("ManualSouthTile");
    ManualObject->setDynamic(false);

    ManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    {
        ManualObject->position(-HALFCUBE, -HALFCUBE,  HALFCUBE);  //  West South Up
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

        ManualObject->position(-HALFCUBE, -HALFCUBE,  -HALFCUBE);  // West South Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

        ManualObject->position( HALFCUBE, -HALFCUBE,  HALFCUBE);  // East South Up
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

        ManualObject->position( HALFCUBE, -HALFCUBE, -HALFCUBE);  // East South Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

        //ManualObject->index(0);
        //ManualObject->index(1);
        //ManualObject->index(2);
        //ManualObject->index(3);
        ManualObject->triangle(0, 1, 2);
        ManualObject->triangle(3, 2, 1);
    }
    ManualObject->end();
    ManualObject->convertToMesh("SouthTile");
    RENDERER->getSceneManager()->destroyManualObject(ManualObject);


    ManualObject = RENDERER->getSceneManager()->createManualObject("ManualWestTile");
    ManualObject->setDynamic(false);

    ManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    {
        ManualObject->position(-HALFCUBE,  HALFCUBE, HALFCUBE);  // West North Up
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

        ManualObject->position(-HALFCUBE,  HALFCUBE, -HALFCUBE);  // West North Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

        ManualObject->position(-HALFCUBE, -HALFCUBE,  HALFCUBE);  //  West South Up
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

        ManualObject->position(-HALFCUBE, -HALFCUBE, -HALFCUBE);  // West South Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));


        //ManualObject->index(0);
        //ManualObject->index(1);
        //ManualObject->index(2);
        //ManualObject->index(3);
        ManualObject->triangle(0, 1, 2);
        ManualObject->triangle(3, 2, 1);
    }
    ManualObject->end();
    ManualObject->convertToMesh("WestTile");
    RENDERER->getSceneManager()->destroyManualObject(ManualObject);


    ManualObject = RENDERER->getSceneManager()->createManualObject("ManualEastTile");
    ManualObject->setDynamic(false);

    ManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    {
        ManualObject->position( -HALFCUBE, -HALFCUBE, HALFCUBE);  //  West South Up
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

        ManualObject->position( -HALFCUBE, -HALFCUBE, -HALFCUBE);  // West South Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

        ManualObject->position( -HALFCUBE,  HALFCUBE, HALFCUBE);  // West North Up
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

        ManualObject->position( -HALFCUBE,  HALFCUBE, -HALFCUBE);  // West North Down
        ManualObject->colour(Ogre::ColourValue::White);
        ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

        //ManualObject->index(0);
        //ManualObject->index(1);
        //ManualObject->index(2);
        //ManualObject->index(3);
        ManualObject->triangle(0, 1, 2);
        ManualObject->triangle(3, 2, 1);
    }
    ManualObject->end();
    ManualObject->convertToMesh("EastTile");
    RENDERER->getSceneManager()->destroyManualObject(ManualObject);



    // A 3 pronged marker to show the XYZ axies
    ManualObject = RENDERER->getSceneManager()->createManualObject("ManualAxialMarker");
    ManualObject->setDynamic(false);

    ManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
    {
        float lAxeSize = 10;
        // X Red
        ManualObject->position(0.0f, 0.0f, 0.0f);
        ManualObject->colour(Ogre::ColourValue::Red);

        ManualObject->position(lAxeSize, 0.0f, 0.0f);
        ManualObject->colour(Ogre::ColourValue::Red);

        // Y Green
        ManualObject->position(0.0f, 0.0f, 0.0f);
        ManualObject->colour(Ogre::ColourValue::Green);

        ManualObject->position(0.0, lAxeSize, 0.0);
        ManualObject->colour(Ogre::ColourValue::Green);

        // Z Blue
        ManualObject->position(0.0f, 0.0f, 0.0f);
        ManualObject->colour(Ogre::ColourValue::Blue);

        ManualObject->position(0.0, 0.0, lAxeSize);
        ManualObject->colour(Ogre::ColourValue::Blue);

        ManualObject->index(0);
        ManualObject->index(1);
        ManualObject->index(2);
        ManualObject->index(3);
        ManualObject->index(4);
        ManualObject->index(5);
    }
    ManualObject->end();
    ManualObject->convertToMesh("AxialMarker");
    RENDERER->getSceneManager()->destroyManualObject(ManualObject);


    // A 3 pronged marker to show the XYZ axies
    ManualObject = RENDERER->getSceneManager()->createManualObject("ManualWireFrame");
    ManualObject->setDynamic(true);

    ManualObject->begin("BaseWhiteNoLighting",Ogre::RenderOperation::OT_LINE_LIST);
    {
        // TOP
        ManualObject->position(HALFCUBE, HALFCUBE, HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        ManualObject->position(HALFCUBE, -HALFCUBE, HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        ManualObject->position(-HALFCUBE, -HALFCUBE, HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        ManualObject->position(-HALFCUBE, HALFCUBE, HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        // BOTTOM
        ManualObject->position(HALFCUBE, HALFCUBE, -HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        ManualObject->position(HALFCUBE, -HALFCUBE, -HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        ManualObject->position(-HALFCUBE, -HALFCUBE, -HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        ManualObject->position(-HALFCUBE, HALFCUBE, -HALFCUBE);
        ManualObject->colour(Ogre::ColourValue::White);

        // TOP
        ManualObject->index(0);
        ManualObject->index(1);

        ManualObject->index(1);
        ManualObject->index(2);

        ManualObject->index(2);
        ManualObject->index(3);

        ManualObject->index(3);
        ManualObject->index(0);

        // BOTTOM
        ManualObject->index(4);
        ManualObject->index(5);

        ManualObject->index(5);
        ManualObject->index(6);

        ManualObject->index(6);
        ManualObject->index(7);

        ManualObject->index(7);
        ManualObject->index(4);

        // SIDES
        ManualObject->index(0);
        ManualObject->index(4);

        ManualObject->index(1);
        ManualObject->index(5);

        ManualObject->index(2);
        ManualObject->index(6);

        ManualObject->index(3);
        ManualObject->index(7);
    }
    ManualObject->end();
    ManualObject->convertToMesh("WhiteWireFrame");
    RENDERER->getSceneManager()->destroyManualObject(ManualObject);


    // A 3 pronged marker to show the XYZ axies
    ManualObject = RENDERER->getSceneManager()->createManualObject("ManualWireFrame");
    ManualObject->setDynamic(true);

    ManualObject->begin("BaseWhiteNoLighting",Ogre::RenderOperation::OT_LINE_LIST);
    {
        // TOP
        ManualObject->position(HALFCUBE, HALFCUBE, HALFCUBE);
        ManualObject->colour(Ogre::ColourValue(1.0, 1.0, 0.0, 1.0));

        ManualObject->position(HALFCUBE, -HALFCUBE, HALFCUBE);
        ManualObject->colour(Ogre::ColourValue(1.0, 1.0, 0.0, 1.0));

        ManualObject->position(-HALFCUBE, -HALFCUBE, HALFCUBE);
        ManualObject->colour(Ogre::ColourValue(1.0, 1.0, 0.0, 1.0));

        ManualObject->position(-HALFCUBE, HALFCUBE, HALFCUBE);
        ManualObject->colour(Ogre::ColourValue(1.0, 1.0, 0.0, 1.0));

        // BOTTOM
        ManualObject->position(HALFCUBE, HALFCUBE, -HALFCUBE);
        ManualObject->colour(Ogre::ColourValue(1.0, 1.0, 0.0, 1.0));

        ManualObject->position(HALFCUBE, -HALFCUBE, -HALFCUBE);
        ManualObject->colour(Ogre::ColourValue(1.0, 1.0, 0.0, 1.0));

        ManualObject->position(-HALFCUBE, -HALFCUBE, -HALFCUBE);
        ManualObject->colour(Ogre::ColourValue(1.0, 1.0, 0.0, 1.0));

        ManualObject->position(-HALFCUBE, HALFCUBE, -HALFCUBE);
        ManualObject->colour(Ogre::ColourValue(1.0, 1.0, 0.0, 1.0));

        // TOP
        ManualObject->index(0);
        ManualObject->index(1);

        ManualObject->index(1);
        ManualObject->index(2);

        ManualObject->index(2);
        ManualObject->index(3);

        ManualObject->index(3);
        ManualObject->index(0);

        // BOTTOM
        ManualObject->index(4);
        ManualObject->index(5);

        ManualObject->index(5);
        ManualObject->index(6);

        ManualObject->index(6);
        ManualObject->index(7);

        ManualObject->index(7);
        ManualObject->index(4);

        // SIDES
        ManualObject->index(0);
        ManualObject->index(4);

        ManualObject->index(1);
        ManualObject->index(5);

        ManualObject->index(2);
        ManualObject->index(6);

        ManualObject->index(3);
        ManualObject->index(7);
    }

    ManualObject->end();
    ManualObject->convertToMesh("YellowWireFrame");
    RENDERER->getSceneManager()->destroyManualObject(ManualObject);
}

void CreateSlopedTiles()
{
    Ogre::ManualObject* ManualObject;

    for (TileShape Shape = TILESHAPE_START; Shape < NUM_TILESHAPES; ++Shape)
    {
        ManualObject = RENDERER->getSceneManager()->createManualObject("ManualRampTile");
        ManualObject->setDynamic(false);

        uint8_t HeightData = (uint8_t) Shape;

        uint8_t SouthWestCorner = HeightData & (3 << 0);
        uint8_t SouthEastCorner = HeightData & (3 << 2);    SouthEastCorner >>= 2;
        uint8_t NorthWestCorner = HeightData & (3 << 4);    NorthWestCorner >>= 4;
        uint8_t NorthEastCorner = HeightData & (3 << 6);    NorthEastCorner >>= 6;


        ManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
        {
            ManualObject->position(-HALFCUBE,  -HALFCUBE, (SouthWestCorner / 3.0f) -HALFCUBE);  // South West
            ManualObject->colour(Ogre::ColourValue::White);
            ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

            ManualObject->position( HALFCUBE,  -HALFCUBE, (SouthEastCorner / 3.0f) -HALFCUBE);  // South East
            ManualObject->colour(Ogre::ColourValue::White);
            ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

            ManualObject->position(-HALFCUBE, HALFCUBE, (NorthWestCorner / 3.0f) -HALFCUBE);  // North West
            ManualObject->colour(Ogre::ColourValue::White);
            ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

            ManualObject->position( HALFCUBE, HALFCUBE, (NorthEastCorner / 3.0f) -HALFCUBE);  // North East
            ManualObject->colour(Ogre::ColourValue::White);
            ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

            if (((NorthEastCorner <= SouthEastCorner) && (NorthEastCorner <= NorthWestCorner))  &&  ((SouthWestCorner >= SouthEastCorner) && (SouthWestCorner >= NorthWestCorner)))  // Break quad along a NW-SE line
            {
                ManualObject->triangle(3, 2, 1);
                ManualObject->triangle(2, 0, 1);
            }
            else // Break quad along alternative SW-NE line
            {
                ManualObject->triangle(0, 3, 2);
                ManualObject->triangle(0, 1, 3);
            }
        }

        char buffer[64];
        sprintf(buffer, "Slope%i", (uint16_t) Shape);

        ManualObject->end();
        ManualObject->convertToMesh(buffer);
        RENDERER->getSceneManager()->destroyManualObject(ManualObject);
    }
}
