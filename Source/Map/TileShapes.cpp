#include <TileShapes.h>
#include <Coordinates.h>

#include <Renderer.h>

void CreateAllEntities()
{
    CreateFaceTiles();
    CreateSlopedTiles();
}

void CreateFaceTiles()
{
    Ogre::ManualObject* ManualObject;


    for (Direction DirectionType = CARDINAL_DIRECTIONS_START; DirectionType < NUM_CARDINAL_DIRECTIONS; ++DirectionType)
    {
        for (uint8_t LeftCorner = BELOW_CUBE_HEIGHT; LeftCorner < ABOVE_CUBE_HEIGHT; LeftCorner++)
        {
            for (uint8_t RightCorner = BELOW_CUBE_HEIGHT; RightCorner < ABOVE_CUBE_HEIGHT; RightCorner++)
            {
                ManualObject = RENDERER->getSceneManager()->createManualObject("ManualFaceTile");
                ManualObject->setDynamic(false);

                bool Triangle = false;
                CubeShape Shape = CubeShape(false);

                ManualObject->begin("BaseWhite", Ogre::RenderOperation::OT_TRIANGLE_LIST);
                {
                    float XLeft; float XRight; float YLeft; float YRight;

                    switch (DirectionType)
                    {
                        case DIRECTION_SOUTH:
                            XLeft = -HALFCUBE;  YLeft = -HALFCUBE;  XRight =  HALFCUBE;  YRight = -HALFCUBE;
                            Shape.SouthWestCorner = LeftCorner;  Shape.SouthEastCorner = RightCorner;
                            break;

                        case DIRECTION_NORTH:
                            XLeft =  HALFCUBE;  YLeft =  HALFCUBE;  XRight = -HALFCUBE;  YRight =  HALFCUBE;
                            Shape.NorthEastCorner = LeftCorner;  Shape.NorthWestCorner = RightCorner;
                            break;

                        case DIRECTION_WEST:
                            XLeft = -HALFCUBE;  YLeft =  HALFCUBE;  XRight = -HALFCUBE;  YRight = -HALFCUBE;
                            Shape.NorthWestCorner = LeftCorner;  Shape.SouthEastCorner = RightCorner;
                            break;

                        case DIRECTION_EAST:
                            XLeft =  HALFCUBE;  YLeft = -HALFCUBE;  XRight =  HALFCUBE;  YRight =  HALFCUBE;
                            Shape.SouthEastCorner = LeftCorner;  Shape.NorthEastCorner = RightCorner;
                            break;

                        default:
                            break;
                    }

                    ManualObject->position(XLeft,  YLeft, -HALFCUBE);  // Left Bottom
                    ManualObject->colour(Ogre::ColourValue::White);
                    ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

                    ManualObject->position(XLeft, YLeft, (LeftCorner / HEIGHT_FRACTIONS) -HALFCUBE);  // Left Top
                    ManualObject->colour(Ogre::ColourValue::White);
                    ManualObject->textureCoord(Ogre::Vector2(0.0f, (LeftCorner / HEIGHT_FRACTIONS)));


                    ManualObject->position(XRight, YRight, -HALFCUBE);  // Right Bottom
                    ManualObject->colour(Ogre::ColourValue::White);
                    ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

                    ManualObject->position(XRight, YRight, (RightCorner / HEIGHT_FRACTIONS) -HALFCUBE);  // Right Top
                    ManualObject->colour(Ogre::ColourValue::White);
                    ManualObject->textureCoord(Ogre::Vector2(1.0f, (RightCorner / HEIGHT_FRACTIONS)));

                    if (LeftCorner != BELOW_CUBE_HEIGHT)
                    {
                        ManualObject->triangle(3, 1, 0);
                        Triangle = true;
                    }

                    if (LeftCorner != BELOW_CUBE_HEIGHT && RightCorner != BELOW_CUBE_HEIGHT)
                    {
                        ManualObject->triangle(0, 2, 3);
                        Triangle = true;
                    }
                }

                if (Triangle)
                {
                    FaceShape NewFace = FaceShape(Shape, DirectionType);

                    char buffer[64];
                    NewFace.getName(buffer);

                    //sprintf(buffer, "Face%i", FaceShape(Shape, DirectionType).Key());

                    ManualObject->end();
                    ManualObject->convertToMesh(buffer);
                }

                RENDERER->getSceneManager()->destroyManualObject(ManualObject);
            }
        }
    }
}

void CreateSlopedTiles()
{
    Ogre::ManualObject* ManualObject;

    for (uint8_t SW = BELOW_CUBE_HEIGHT; SW <= ABOVE_CUBE_HEIGHT; ++SW)
    {
        for (uint8_t SE = BELOW_CUBE_HEIGHT; SE <= ABOVE_CUBE_HEIGHT; ++SE)
        {
            for (uint8_t NW = BELOW_CUBE_HEIGHT; NW <= ABOVE_CUBE_HEIGHT; ++NW)
            {
                for (uint8_t NE = BELOW_CUBE_HEIGHT; NE <= ABOVE_CUBE_HEIGHT; ++NE)
                {
                    for (uint8_t Split = 0; Split <= 1; Split++)
                    {
                        ManualObject = RENDERER->getSceneManager()->createManualObject("ManualRampTile");
                        ManualObject->setDynamic(false);

                        bool Triangle1 = false;
                        bool Triangle2 = false;

                        float NorthEastCorner = NE;
                        float NorthWestCorner = NW;
                        float SouthEastCorner = SE;
                        float SouthWestCorner = SW;


                        ManualObject->begin("BaseWhite", Ogre::RenderOperation::OT_TRIANGLE_LIST);
                        {
                            Ogre::Vector3 Vertex0 = Ogre::Vector3(-HALFCUBE, -HALFCUBE, ((SouthWestCorner - 1) / HEIGHT_FRACTIONS) -HALFCUBE);
                            Ogre::Vector3 Vertex1 = Ogre::Vector3( HALFCUBE, -HALFCUBE, ((SouthEastCorner - 1) / HEIGHT_FRACTIONS) -HALFCUBE);
                            Ogre::Vector3 Vertex2 = Ogre::Vector3(-HALFCUBE,  HALFCUBE, ((NorthWestCorner - 1) / HEIGHT_FRACTIONS) -HALFCUBE);
                            Ogre::Vector3 Vertex3 = Ogre::Vector3( HALFCUBE,  HALFCUBE, ((NorthEastCorner - 1) / HEIGHT_FRACTIONS) -HALFCUBE);

                            if (Split)
                            {
                                if (SouthEastCorner > CUBE_BOTTOM_HEIGHT || NorthEastCorner > CUBE_BOTTOM_HEIGHT || NorthWestCorner > CUBE_BOTTOM_HEIGHT)
                                {
                                    if (SouthEastCorner < ABOVE_CUBE_HEIGHT && NorthEastCorner < ABOVE_CUBE_HEIGHT && NorthWestCorner < ABOVE_CUBE_HEIGHT)
                                    {
                                        ManualObject->position(Vertex1);  // South East
                                        ManualObject->colour(Ogre::ColourValue::White);
                                        ManualObject->normal( ( Vertex3 - Vertex1 ).crossProduct( ( Vertex2 - Vertex1 ) ).normalisedCopy());
                                        ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

                                        ManualObject->position(Vertex3);  // North East
                                        ManualObject->colour(Ogre::ColourValue::White);
                                        ManualObject->normal( ( Vertex2 - Vertex3 ).crossProduct( ( Vertex1 - Vertex3 ) ).normalisedCopy());
                                        ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

                                        ManualObject->position(Vertex2);  // North West
                                        ManualObject->colour(Ogre::ColourValue::White);
                                        ManualObject->normal( ( Vertex1 - Vertex2 ).crossProduct( ( Vertex3 - Vertex2 ) ).normalisedCopy());
                                        ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

                                        ManualObject->triangle(0, 1, 2);  // SE->NE->NW
                                        Triangle1 = true;
                                    }
                                }

                                if (NorthWestCorner > CUBE_BOTTOM_HEIGHT || SouthWestCorner > CUBE_BOTTOM_HEIGHT || SouthEastCorner > CUBE_BOTTOM_HEIGHT)
                                {
                                    if (NorthWestCorner < ABOVE_CUBE_HEIGHT && SouthWestCorner < ABOVE_CUBE_HEIGHT && SouthEastCorner < ABOVE_CUBE_HEIGHT)
                                    {
                                        ManualObject->position(Vertex2);  // North West
                                        ManualObject->colour(Ogre::ColourValue::White);
                                        ManualObject->normal( ( Vertex0 - Vertex2 ).crossProduct( ( Vertex1 - Vertex2 ) ).normalisedCopy());
                                        ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

                                        ManualObject->position(Vertex0);  // South West
                                        ManualObject->colour(Ogre::ColourValue::White);
                                        ManualObject->normal( ( Vertex1 - Vertex0 ).crossProduct( ( Vertex2 - Vertex0 ) ).normalisedCopy());
                                        ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

                                        ManualObject->position(Vertex1);  // South East
                                        ManualObject->colour(Ogre::ColourValue::White);
                                        ManualObject->normal( ( Vertex2 - Vertex1 ).crossProduct( ( Vertex0 - Vertex1 ) ).normalisedCopy());
                                        ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

                                        if (Triangle1)
                                        {
                                            ManualObject->triangle(3, 4, 5);  // NW->SW->SE
                                        }
                                        else
                                        {
                                            ManualObject->triangle(0, 1, 2);  // NW->SW->SE
                                        }

                                        Triangle2 = true;
                                    }
                                }

                                if ((Triangle1 ^ Triangle2) && ((NorthWestCorner > CUBE_BOTTOM_HEIGHT || SouthEastCorner > CUBE_BOTTOM_HEIGHT) && (NorthWestCorner < CUBE_TOP_HEIGHT || SouthEastCorner < CUBE_TOP_HEIGHT)))  // One True and One False
                                {
                                    Ogre::Vector3 Normal;

                                    if (Triangle1)
                                    {
                                        Normal = Ogre::Vector3::NEGATIVE_UNIT_X + Ogre::Vector3::NEGATIVE_UNIT_Y;
                                    }
                                    else
                                    {
                                        Normal = Ogre::Vector3::UNIT_X + Ogre::Vector3::UNIT_Y;
                                    }

                                    Ogre::Vector3 Vertex4 = Ogre::Vector3( HALFCUBE, -HALFCUBE, -HALFCUBE);
                                    Ogre::Vector3 Vertex5 = Ogre::Vector3(-HALFCUBE,  HALFCUBE, -HALFCUBE);

                                    ManualObject->position(Vertex2);  // North West  3
                                    ManualObject->colour(Ogre::ColourValue::White);
                                    ManualObject->normal(Normal);
                                    ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

                                    ManualObject->position(Vertex1);  // South East  4
                                    ManualObject->colour(Ogre::ColourValue::White);
                                    ManualObject->normal(Normal);
                                    ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

                                    ManualObject->position(Vertex4);  // North West Bottom  5
                                    ManualObject->colour(Ogre::ColourValue::White);
                                    ManualObject->normal(Normal);
                                    ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

                                    ManualObject->position(Vertex5);  // South East Bottom  6
                                    ManualObject->colour(Ogre::ColourValue::White);
                                    ManualObject->normal(Normal);
                                    ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));


                                    if (Triangle1)
                                    {
                                        if (NorthWestCorner > 1)
                                        {
                                            ManualObject->triangle(4, 3, 5);  // NW->SW->NW Bottom
                                        }
                                        else
                                        {
                                            ManualObject->triangle(4, 3, 6);  // NW->SW->SW Bottom
                                        }

                                        if (SouthEastCorner > 1)
                                        {
                                            ManualObject->triangle(3, 6, 5);  // SW->SW Bottom->NW Bottom
                                        }
                                    }
                                    else
                                    {
                                        if (SouthEastCorner > 1)
                                        {
                                            ManualObject->triangle(3, 4, 6);  // NW->SW->NW Bottom
                                        }
                                        else
                                        {
                                            ManualObject->triangle(3, 4, 5);  // NW->SW->SW Bottom
                                        }

                                        if (NorthWestCorner > 1)
                                        {
                                            ManualObject->triangle(4, 5, 6);  // SW->SW Bottom->NW Bottom
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (NorthEastCorner > CUBE_BOTTOM_HEIGHT || NorthWestCorner > CUBE_BOTTOM_HEIGHT || SouthWestCorner > CUBE_BOTTOM_HEIGHT)
                                {
                                    if (NorthEastCorner < ABOVE_CUBE_HEIGHT && NorthWestCorner < ABOVE_CUBE_HEIGHT && SouthWestCorner < ABOVE_CUBE_HEIGHT)
                                    {
                                        ManualObject->position(Vertex3);  // North East
                                        ManualObject->colour(Ogre::ColourValue::White);
                                        ManualObject->normal( ( Vertex2 - Vertex3 ).crossProduct( ( Vertex0 - Vertex3 ) ).normalisedCopy());
                                        ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

                                        ManualObject->position(Vertex2);  // North West
                                        ManualObject->colour(Ogre::ColourValue::White);
                                        ManualObject->normal( ( Vertex0 - Vertex2 ).crossProduct( ( Vertex3 - Vertex2 ) ).normalisedCopy());
                                        ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

                                        ManualObject->position(Vertex0);  // South West
                                        ManualObject->colour(Ogre::ColourValue::White);
                                        ManualObject->normal( ( Vertex3 - Vertex0 ).crossProduct( ( Vertex2 - Vertex0 ) ).normalisedCopy());
                                        ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

                                        ManualObject->triangle(0, 1, 2);  // NE->NW->SW
                                        Triangle1 = true;
                                    }
                                }

                                if (SouthWestCorner > CUBE_BOTTOM_HEIGHT || SouthEastCorner > CUBE_BOTTOM_HEIGHT || NorthEastCorner > CUBE_BOTTOM_HEIGHT)
                                {
                                    if (SouthWestCorner < ABOVE_CUBE_HEIGHT && SouthEastCorner < ABOVE_CUBE_HEIGHT && NorthEastCorner < ABOVE_CUBE_HEIGHT)
                                    {
                                        ManualObject->position(Vertex0);  // South West
                                        ManualObject->colour(Ogre::ColourValue::White);
                                        ManualObject->normal( ( Vertex1 - Vertex0 ).crossProduct( ( Vertex3 - Vertex0 ) ).normalisedCopy());
                                        ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

                                        ManualObject->position(Vertex1);  // South East
                                        ManualObject->colour(Ogre::ColourValue::White);
                                        ManualObject->normal(( Vertex3 - Vertex1 ).crossProduct( ( Vertex0 - Vertex1 ) ).normalisedCopy());
                                        ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

                                        ManualObject->position(Vertex3);  // North East
                                        ManualObject->colour(Ogre::ColourValue::White);
                                        ManualObject->normal(( Vertex0 - Vertex3 ).crossProduct( ( Vertex1 - Vertex3 ) ).normalisedCopy());
                                        ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

                                        if (Triangle1)
                                        {
                                            ManualObject->triangle(3, 4, 5);  // SW->SE->NE
                                        }
                                        else
                                        {
                                            ManualObject->triangle(0, 1, 2);  // SW->SE->NE
                                        }
                                        Triangle2 = true;
                                    }
                                }
                            }
                        }

                        if (Triangle1 || Triangle2)
                        {
                            FaceShape NewFace = FaceShape(CubeShape(SW, SE, NW, NE, Split), DIRECTION_NONE);

                            char buffer[64];
                            NewFace.getName(buffer);

                            ManualObject->end();
                            ManualObject->convertToMesh(buffer);
                        }

                        RENDERER->getSceneManager()->destroyManualObject(ManualObject);
                    }
                }
            }
        }
    }
}
