#include <TileShapes.h>
#include <Coordinates.h>

#include <Renderer.h>


Ogre::MeshPtr CreateFlatFace(FaceShape Shape)
{
	Ogre::ManualObject* ManualObject = RENDERER->getSceneManager()->createManualObject("ManualFlatTile");
	ManualObject->setDynamic(false);

	bool Triangle1 = false;
	bool Triangle2 = false;

	uint8_t NorthEastCorner = Shape.CubeComponent.NorthEastCorner;
	uint8_t NorthWestCorner = Shape.CubeComponent.NorthWestCorner;
	uint8_t SouthEastCorner = Shape.CubeComponent.SouthEastCorner;
	uint8_t SouthWestCorner = Shape.CubeComponent.SouthWestCorner;

	ManualObject->begin("BaseWhite", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	{
		Ogre::Vector3 Vertex0 = Ogre::Vector3(-HALFCUBE, -HALFCUBE, ((SouthWestCorner - 1) / HEIGHT_FRACTIONS) -HALFCUBE);
		Ogre::Vector3 Vertex1 = Ogre::Vector3( HALFCUBE, -HALFCUBE, ((SouthEastCorner - 1) / HEIGHT_FRACTIONS) -HALFCUBE);
		Ogre::Vector3 Vertex2 = Ogre::Vector3(-HALFCUBE,  HALFCUBE, ((NorthWestCorner - 1) / HEIGHT_FRACTIONS) -HALFCUBE);
		Ogre::Vector3 Vertex3 = Ogre::Vector3( HALFCUBE,  HALFCUBE, ((NorthEastCorner - 1) / HEIGHT_FRACTIONS) -HALFCUBE);

		if (Shape.CubeComponent.split()) // Split along NW-SE line
		{
			if ((SouthEastCorner == CUBE_BOTTOM_HEIGHT && NorthEastCorner == CUBE_BOTTOM_HEIGHT && NorthWestCorner == CUBE_BOTTOM_HEIGHT) || (SouthEastCorner == CUBE_TOP_HEIGHT && NorthEastCorner == CUBE_TOP_HEIGHT && NorthWestCorner == CUBE_TOP_HEIGHT))
			{
				ManualObject->position(Vertex1);  // South East
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Ogre::Vector3::UNIT_Z);
				ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

				ManualObject->position(Vertex3);  // North East
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Ogre::Vector3::UNIT_Z);
				ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

				ManualObject->position(Vertex2);  // North West
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Ogre::Vector3::UNIT_Z);
				ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

				ManualObject->triangle(0, 1, 2);  // SE->NE->NW
				Triangle1 = true;
			}

			if ((NorthWestCorner == CUBE_BOTTOM_HEIGHT && SouthWestCorner == CUBE_BOTTOM_HEIGHT && SouthEastCorner == CUBE_BOTTOM_HEIGHT) || (NorthWestCorner == CUBE_TOP_HEIGHT && SouthWestCorner == CUBE_TOP_HEIGHT && SouthEastCorner == CUBE_TOP_HEIGHT))
			{
				ManualObject->position(Vertex2);  // North West
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Ogre::Vector3::UNIT_Z);
				ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

				ManualObject->position(Vertex0);  // South West
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Ogre::Vector3::UNIT_Z);
				ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

				ManualObject->position(Vertex1);  // South East
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Ogre::Vector3::UNIT_Z);
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
		else // Split along SW-NE line
		{
			if ((NorthEastCorner == CUBE_BOTTOM_HEIGHT && NorthWestCorner == CUBE_BOTTOM_HEIGHT && SouthWestCorner == CUBE_BOTTOM_HEIGHT) || (NorthEastCorner == CUBE_TOP_HEIGHT && NorthWestCorner == CUBE_TOP_HEIGHT && SouthWestCorner == CUBE_TOP_HEIGHT))
			{
				ManualObject->position(Vertex3);  // North East
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Ogre::Vector3::UNIT_Z);
				ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

				ManualObject->position(Vertex2);  // North West
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Ogre::Vector3::UNIT_Z);
				ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

				ManualObject->position(Vertex0);  // South West
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Ogre::Vector3::UNIT_Z);
				ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

				ManualObject->triangle(0, 1, 2);  // NE->NW->SW
				Triangle1 = true;
			}

			if ((SouthWestCorner == CUBE_BOTTOM_HEIGHT && SouthEastCorner == CUBE_BOTTOM_HEIGHT && NorthEastCorner == CUBE_BOTTOM_HEIGHT) || (SouthWestCorner == CUBE_TOP_HEIGHT && SouthEastCorner == CUBE_TOP_HEIGHT && NorthEastCorner == CUBE_TOP_HEIGHT))
			{
				ManualObject->position(Vertex0);  // South West
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Ogre::Vector3::UNIT_Z);
				ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

				ManualObject->position(Vertex1);  // South East
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Ogre::Vector3::UNIT_Z);
				ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

				ManualObject->position(Vertex3);  // North East
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Ogre::Vector3::UNIT_Z);
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

	ManualObject->end();

	if (Triangle1 || Triangle2)
	{
		char buffer[64];
		sprintf(buffer, "FlatFace%i", Shape.Key());

		Ogre::MeshPtr NewMesh = ManualObject->convertToMesh(buffer);
		RENDERER->getSceneManager()->destroyManualObject(ManualObject);
		return NewMesh;
	} else {
		RENDERER->getSceneManager()->destroyManualObject(ManualObject);
		return Ogre::MeshPtr(NULL);
	}

	RENDERER->getSceneManager()->destroyManualObject(ManualObject);
}

Ogre::MeshPtr CreateSideFace(FaceShape Shape)
{
	Ogre::ManualObject* ManualObject = RENDERER->getSceneManager()->createManualObject("ManualFaceTile");
	ManualObject->setDynamic(false);

	bool Triangle = false;
	CubeShape Cube = Shape.CubeComponent;

	ManualObject->begin("BaseWhite", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	{
		float XLeft; float XRight; float YLeft; float YRight;
		uint8_t LeftCorner, RightCorner;
		Ogre::Vector3 Normal;

		switch (Shape.FaceDirection)
		{
			case DIRECTION_SOUTH:
				XLeft = -HALFCUBE;  YLeft = -HALFCUBE;  XRight =  HALFCUBE;  YRight = -HALFCUBE;
				LeftCorner = Cube.SouthWestCorner;   RightCorner = Cube.SouthEastCorner;
				Normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
				break;

			case DIRECTION_NORTH:
				XLeft =  HALFCUBE;  YLeft =  HALFCUBE;  XRight = -HALFCUBE;  YRight =  HALFCUBE;
				LeftCorner = Cube.NorthEastCorner;  RightCorner = Cube.NorthWestCorner;
				Normal = Ogre::Vector3::UNIT_Y;
				break;

			case DIRECTION_WEST:
				XLeft = -HALFCUBE;  YLeft =  HALFCUBE;  XRight = -HALFCUBE;  YRight = -HALFCUBE;
				LeftCorner = Cube.NorthWestCorner;  RightCorner = Cube.SouthWestCorner;
				Normal = Ogre::Vector3::NEGATIVE_UNIT_X;
				break;

			case DIRECTION_EAST:
				XLeft =  HALFCUBE;  YLeft = -HALFCUBE;  XRight =  HALFCUBE;  YRight =  HALFCUBE;
				LeftCorner = Cube.SouthEastCorner;  RightCorner = Cube.NorthEastCorner;
				Normal = Ogre::Vector3::UNIT_X;
				break;

			default:
				break;
		}

		float Left = (min(max(LeftCorner, (uint8_t) CUBE_BOTTOM_HEIGHT), (uint8_t) CUBE_TOP_HEIGHT) - 1.0f) / HEIGHT_FRACTIONS;
		float Right = (min(max(RightCorner, (uint8_t) CUBE_BOTTOM_HEIGHT), (uint8_t) CUBE_TOP_HEIGHT) - 1.0f) / HEIGHT_FRACTIONS;

		ManualObject->position(XLeft,  YLeft, -HALFCUBE);  // Left Bottom
		ManualObject->colour(Ogre::ColourValue::White);
		ManualObject->normal(Normal);
		ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

		ManualObject->position(XLeft, YLeft, Left - HALFCUBE);  // Left Top
		ManualObject->colour(Ogre::ColourValue::White);
		ManualObject->normal(Normal);
		ManualObject->textureCoord(Ogre::Vector2(0.0f, Left));

		ManualObject->position(XRight, YRight, -HALFCUBE);  // Right Bottom
		ManualObject->colour(Ogre::ColourValue::White);
		ManualObject->normal(Normal);
		ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

		ManualObject->position(XRight, YRight, Right - HALFCUBE);  // Right Top
		ManualObject->colour(Ogre::ColourValue::White);
		ManualObject->normal(Normal);
		ManualObject->textureCoord(Ogre::Vector2(1.0f, Right));

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

	ManualObject->end();

	if (Triangle)
	{
		char buffer[64];
		sprintf(buffer, "SideFace%i", Shape.Key());

		Ogre::MeshPtr NewMesh = ManualObject->convertToMesh(buffer);
		RENDERER->getSceneManager()->destroyManualObject(ManualObject);
		return NewMesh;
	} else {
		RENDERER->getSceneManager()->destroyManualObject(ManualObject);
		return Ogre::MeshPtr(NULL);
	}
}

Ogre::MeshPtr CreateSlopeFace(FaceShape Shape)
{
    Ogre::ManualObject* ManualObject = RENDERER->getSceneManager()->createManualObject("ManualSlopeTile");
	ManualObject->setDynamic(false);

	bool Triangle1 = false;
	bool Triangle2 = false;

	uint8_t NorthEastCorner = Shape.CubeComponent.NorthEastCorner;
	uint8_t NorthWestCorner = Shape.CubeComponent.NorthWestCorner;
	uint8_t SouthEastCorner = Shape.CubeComponent.SouthEastCorner;
	uint8_t SouthWestCorner = Shape.CubeComponent.SouthWestCorner;

	ManualObject->begin("BaseWhite", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	{
		Ogre::Vector3 Vertex0 = Ogre::Vector3(-HALFCUBE, -HALFCUBE, (((float) SouthWestCorner - 1) / HEIGHT_FRACTIONS) -HALFCUBE);
		Ogre::Vector3 Vertex1 = Ogre::Vector3( HALFCUBE, -HALFCUBE, (((float) SouthEastCorner - 1) / HEIGHT_FRACTIONS) -HALFCUBE);
		Ogre::Vector3 Vertex2 = Ogre::Vector3(-HALFCUBE,  HALFCUBE, (((float) NorthWestCorner - 1) / HEIGHT_FRACTIONS) -HALFCUBE);
		Ogre::Vector3 Vertex3 = Ogre::Vector3( HALFCUBE,  HALFCUBE, (((float) NorthEastCorner - 1) / HEIGHT_FRACTIONS) -HALFCUBE);

		Ogre::Vector3 Normal;

		if (SouthWestCorner == ABOVE_CUBE_HEIGHT)
		{
			Vertex0 = Ogre::Vector3( -HALFCUBE, -HALFCUBE, HALFCUBE);
		}
		if (SouthEastCorner == ABOVE_CUBE_HEIGHT)
		{
			Vertex1 = Ogre::Vector3( HALFCUBE, -HALFCUBE, HALFCUBE);
		}
		if (NorthWestCorner == ABOVE_CUBE_HEIGHT)
		{
			Vertex2 = Ogre::Vector3( -HALFCUBE, HALFCUBE, HALFCUBE);
		}
		if (NorthEastCorner == ABOVE_CUBE_HEIGHT)
		{
			Vertex3 = Ogre::Vector3( HALFCUBE, HALFCUBE, HALFCUBE);
		}


		if (Shape.CubeComponent.split()) // Split along the NW-SE line
		{
			// Triangle1 SE->NE->NW
			if (SouthEastCorner > BELOW_CUBE_HEIGHT && NorthEastCorner > BELOW_CUBE_HEIGHT && NorthWestCorner > BELOW_CUBE_HEIGHT)
			{
				if (SouthEastCorner > CUBE_BOTTOM_HEIGHT || NorthEastCorner > CUBE_BOTTOM_HEIGHT || NorthWestCorner > CUBE_BOTTOM_HEIGHT)
				{
					if (SouthEastCorner < CUBE_TOP_HEIGHT || NorthEastCorner < CUBE_TOP_HEIGHT || NorthWestCorner < CUBE_TOP_HEIGHT)
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
			}

			// Triangle2 NW->SW->SE
			if (NorthWestCorner > BELOW_CUBE_HEIGHT && SouthWestCorner > BELOW_CUBE_HEIGHT && SouthEastCorner > BELOW_CUBE_HEIGHT)
			{
				if (NorthWestCorner > CUBE_BOTTOM_HEIGHT || SouthWestCorner > CUBE_BOTTOM_HEIGHT || SouthEastCorner > CUBE_BOTTOM_HEIGHT)
				{
					if (NorthWestCorner < CUBE_TOP_HEIGHT || SouthWestCorner < CUBE_TOP_HEIGHT || SouthEastCorner < CUBE_TOP_HEIGHT)
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
			}

			// Vertical face inside Cube when only one triangle is drawn
			if ((Triangle1 ^ Triangle2) && ((NorthWestCorner > CUBE_BOTTOM_HEIGHT || SouthEastCorner > CUBE_BOTTOM_HEIGHT)))
			{
				if ((Triangle1 && SouthWestCorner < CUBE_TOP_HEIGHT) || (Triangle2 && NorthEastCorner < CUBE_TOP_HEIGHT))
				{
					if (Triangle1)
					{
						Normal = Ogre::Vector3::NEGATIVE_UNIT_X + Ogre::Vector3::NEGATIVE_UNIT_Y;
					}
					if (Triangle2)
					{
						Normal = Ogre::Vector3::UNIT_X + Ogre::Vector3::UNIT_Y;
					}

					Ogre::Vector3 Vertex4 = Ogre::Vector3( -HALFCUBE, HALFCUBE, -HALFCUBE);
					Ogre::Vector3 Vertex5 = Ogre::Vector3( HALFCUBE, -HALFCUBE, -HALFCUBE);

					ManualObject->position(Vertex2);  // North West  3
					ManualObject->colour(Ogre::ColourValue::White);
					ManualObject->normal(Normal);
					ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

					ManualObject->position(Vertex1);  // South East  4
					ManualObject->colour(Ogre::ColourValue::White);
					ManualObject->normal(Normal);
					ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

					ManualObject->position(Vertex4);  // North West Bottom  5
					ManualObject->colour(Ogre::ColourValue::White);
					ManualObject->normal(Normal);
					ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

					ManualObject->position(Vertex5);  // South East Bottom  6
					ManualObject->colour(Ogre::ColourValue::White);
					ManualObject->normal(Normal);
					ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));


					if (Triangle1)
					{
						if (NorthWestCorner > CUBE_BOTTOM_HEIGHT)
						{
							ManualObject->triangle(4, 3, 5);  // SE->NW->NW Bottom
						}
						if (SouthEastCorner > CUBE_BOTTOM_HEIGHT)
						{
							ManualObject->triangle(5, 6, 4);  // NW Bottom->SE Bottom->SE
						}
					}

					if (Triangle2)
					{
						if (SouthEastCorner > CUBE_BOTTOM_HEIGHT)
						{
							ManualObject->triangle(3, 4, 6);  // NW->SE->SE Bottom
						}
						if (NorthWestCorner > CUBE_BOTTOM_HEIGHT)
						{
							ManualObject->triangle(6, 5, 3);  // SE Bottom->NW Bottom->NW
						}
					}
				}
			}

			// Vertical bisector through whole cube
			if (!Triangle1 && !Triangle2 && NorthWestCorner >= CUBE_TOP_HEIGHT && SouthEastCorner >= CUBE_TOP_HEIGHT)
			{
				if (NorthEastCorner < CUBE_BOTTOM_HEIGHT)
				{
					Normal =  Ogre::Vector3::UNIT_X + Ogre::Vector3::UNIT_Y;
				}
				if (SouthWestCorner < CUBE_BOTTOM_HEIGHT)
				{
					Normal = Ogre::Vector3::NEGATIVE_UNIT_X + Ogre::Vector3::NEGATIVE_UNIT_Y;
				}

				Ogre::Vector3 Vertex4 = Ogre::Vector3( -HALFCUBE, HALFCUBE, -HALFCUBE);
				Ogre::Vector3 Vertex5 = Ogre::Vector3( HALFCUBE, -HALFCUBE, -HALFCUBE);

				ManualObject->position(Vertex2);  // North West  0
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Normal);
				ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

				ManualObject->position(Vertex1);  // South East  1
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Normal);
				ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

				ManualObject->position(Vertex4);  // North West Bottom  2
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Normal);
				ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

				ManualObject->position(Vertex5);  // South East Bottom  3
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Normal);
				ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

				if (NorthEastCorner < CUBE_BOTTOM_HEIGHT)
				{
					ManualObject->triangle(2, 0, 1);  // NW Bottom-> NW-> SE
					ManualObject->triangle(1, 3, 2);  // SE ->SE Bottom-> NW Bottom
				}
				else
				{
					ManualObject->triangle(1, 0, 2);  // SE-> NW-> NW Bottom
					ManualObject->triangle(2, 3, 1);  // NW Bottom ->SE Bottom-> SE
				}

				Triangle1 = true;
				Triangle2 = true;
			 }
		}
		else // Split along the SW-NE line
		{
			// Triangle1 NE->NW->SW
			if (NorthEastCorner > BELOW_CUBE_HEIGHT && NorthWestCorner > BELOW_CUBE_HEIGHT && SouthWestCorner > BELOW_CUBE_HEIGHT)
			{
				if (NorthEastCorner > CUBE_BOTTOM_HEIGHT || NorthWestCorner > CUBE_BOTTOM_HEIGHT || SouthWestCorner > CUBE_BOTTOM_HEIGHT)
				{
					if (NorthEastCorner < CUBE_TOP_HEIGHT || NorthWestCorner < CUBE_TOP_HEIGHT || SouthWestCorner < CUBE_TOP_HEIGHT)
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
			}

			// Triangle2 SW->SE->NE
			if (SouthWestCorner > BELOW_CUBE_HEIGHT && SouthEastCorner > BELOW_CUBE_HEIGHT && NorthEastCorner > BELOW_CUBE_HEIGHT)
			{
				if (SouthWestCorner > CUBE_BOTTOM_HEIGHT || SouthEastCorner > CUBE_BOTTOM_HEIGHT || NorthEastCorner > CUBE_BOTTOM_HEIGHT)
				{
					if (true) //(SouthWestCorner < ABOVE_CUBE_HEIGHT && SouthEastCorner < ABOVE_CUBE_HEIGHT && NorthEastCorner < ABOVE_CUBE_HEIGHT)
					{
						if (SouthWestCorner < CUBE_TOP_HEIGHT || SouthEastCorner < CUBE_TOP_HEIGHT || NorthEastCorner < CUBE_TOP_HEIGHT)
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

			// Vertical face inside Cube when only one triangle is drawn
			if ((Triangle1 ^ Triangle2) && ((NorthEastCorner > CUBE_BOTTOM_HEIGHT || SouthWestCorner > CUBE_BOTTOM_HEIGHT)))
			{
				if ((Triangle1 && SouthEastCorner < CUBE_TOP_HEIGHT) || (Triangle2 && NorthWestCorner < CUBE_TOP_HEIGHT))
				{
					Ogre::Vector3 Normal;

					if (Triangle1)
					{
						Normal = Ogre::Vector3::UNIT_X + Ogre::Vector3::NEGATIVE_UNIT_Y;
					}
					if (Triangle2)
					{
						Normal = Ogre::Vector3::NEGATIVE_UNIT_X + Ogre::Vector3::UNIT_Y;
					}

					Ogre::Vector3 Vertex4 = Ogre::Vector3( HALFCUBE, HALFCUBE, -HALFCUBE);
					Ogre::Vector3 Vertex5 = Ogre::Vector3( -HALFCUBE, -HALFCUBE, -HALFCUBE);

					ManualObject->position(Vertex3);  // North East  3
					ManualObject->colour(Ogre::ColourValue::White);
					ManualObject->normal(Normal);
					ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

					ManualObject->position(Vertex0);  // South West  4
					ManualObject->colour(Ogre::ColourValue::White);
					ManualObject->normal(Normal);
					ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

					ManualObject->position(Vertex4);  // North East Bottom  5
					ManualObject->colour(Ogre::ColourValue::White);
					ManualObject->normal(Normal);
					ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

					ManualObject->position(Vertex5);  // South West Bottom  6
					ManualObject->colour(Ogre::ColourValue::White);
					ManualObject->normal(Normal);
					ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));


					if (Triangle1)
					{
						if (NorthEastCorner > CUBE_BOTTOM_HEIGHT)
						{
							ManualObject->triangle(6, 5, 3);  // SW Bottom->NE Bottom->NE
						}
						if (SouthWestCorner > CUBE_BOTTOM_HEIGHT)
						{
							ManualObject->triangle(3, 4, 6);  // NE->SW->SW Bottom
						}
					}
					if (Triangle2)
					{
						if (NorthEastCorner > CUBE_BOTTOM_HEIGHT)
						{
							ManualObject->triangle(4, 3, 5);  // SW->NE->NE Bottom
						}
						if (SouthWestCorner > CUBE_BOTTOM_HEIGHT)
						{
							ManualObject->triangle(5, 6, 4);  // NE Bottom->SW Bottom->SW
						}
					}
				}
			}

			// Vertical bisector through whole cube
			if (!Triangle1 && !Triangle2 && SouthWestCorner >= CUBE_TOP_HEIGHT && NorthEastCorner >= CUBE_TOP_HEIGHT)
			{
				if (SouthEastCorner < CUBE_BOTTOM_HEIGHT)
				{
					Normal = Ogre::Vector3::NEGATIVE_UNIT_X + Ogre::Vector3::NEGATIVE_UNIT_Y;
				}
				if (NorthWestCorner < CUBE_BOTTOM_HEIGHT)
				{
					Normal = Ogre::Vector3::UNIT_X + Ogre::Vector3::UNIT_Y;
				}

				Ogre::Vector3 Vertex4 = Ogre::Vector3( HALFCUBE, HALFCUBE, -HALFCUBE);
				Ogre::Vector3 Vertex5 = Ogre::Vector3( -HALFCUBE, -HALFCUBE, -HALFCUBE);

				ManualObject->position(Vertex3);  // North East  3
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Normal);
				ManualObject->textureCoord(Ogre::Vector2(0.0f, 0.0f));

				ManualObject->position(Vertex0);  // South West  4
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Normal);
				ManualObject->textureCoord(Ogre::Vector2(0.0f, 1.0f));

				ManualObject->position(Vertex4);  // North East Bottom  5
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Normal);
				ManualObject->textureCoord(Ogre::Vector2(1.0f, 0.0f));

				ManualObject->position(Vertex5);  // South West Bottom  6
				ManualObject->colour(Ogre::ColourValue::White);
				ManualObject->normal(Normal);
				ManualObject->textureCoord(Ogre::Vector2(1.0f, 1.0f));

				if (SouthEastCorner < CUBE_BOTTOM_HEIGHT)
				{
					ManualObject->triangle(2, 0, 1);  // NW Bottom-> NW-> SE
					ManualObject->triangle(1, 3, 2);  // SE ->SE Bottom-> NW Bottom
				}
				else
				{
					ManualObject->triangle(1, 0, 2);  // SE-> NW-> NW Bottom
					ManualObject->triangle(2, 3, 1);  // NW Bottom ->SE Bottom-> SE
				}

				Triangle1 = true;
				Triangle2 = true;
			 }
		}
	}

	ManualObject->end();

	if (Triangle1 || Triangle2)
	{
		char buffer[64];
		sprintf(buffer, "SlopeFace%i", Shape.Key());

		Ogre::MeshPtr NewMesh = ManualObject->convertToMesh(buffer);
		RENDERER->getSceneManager()->destroyManualObject(ManualObject);
		return NewMesh;
	} else {
		RENDERER->getSceneManager()->destroyManualObject(ManualObject);
		return Ogre::MeshPtr(NULL);
	}
}
