#include <stdafx.h>

#include <Cell.h>
#include <Cube.h>
#include <Face.h>
#include <Game.h>
#include <TextureManager.h>
#include <ScreenManager.h>
#include <DataManager.h>
#include <Random.h>
#include <Map.h>


Face::Face()
{
	setType(FACE_ACTOR);

	Initalized = false;
	Visible = false;

    Constructed = false;
    Decorated = false;

    Material = 6;

    Rotation = RANDOM->Roll(0, 255);

    MAP->ChangeFaceCount(1);
	//GAME->ActorList.push_back(this);
	//ID = (Uint32) GAME->ActorList.size();
}

Face::~Face()
{
    MAP->ChangeFaceCount(-1);
    //GAME->RemoveActor(ID);
    if(Initalized)
    {
        MAP->ChangeInitedFaceCount(-1);
    }
}

bool Face::Init(Facet Type, Uint16 MaterialType)
{
    Initalized = true;

	FacetType = Type;
	Visible = true;
	Material = MaterialType;
	Texture = DATA->getMaterialData(Material)->getTexture();

    MAP->ChangeInitedFaceCount(1);

	return true;
}

bool Face::Update()
{
	return true;
}

bool Face::Draw(float xTranslate, float yTranslate)
{
    Vector3 Points[4];

    if(Visible)
    {
        switch(FacetType)
        {
            case FACET_TOP:
            {
                Points[0] = Cube::ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
                Points[1] = Cube::ConvertSpacialPoint(SPACIAL_POINT_SOUTH_TOP);
                Points[2] = Cube::ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
                Points[3] = Cube::ConvertSpacialPoint(SPACIAL_POINT_NORTH_TOP);
                break;
            }
            case FACET_BOTTOM:
            {
                Points[0] = Cube::ConvertSpacialPoint(SPACIAL_POINT_WEST_BOTTOM);
                Points[1] = Cube::ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
                Points[2] = Cube::ConvertSpacialPoint(SPACIAL_POINT_EAST_BOTTOM);
                Points[3] = Cube::ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
                break;
            }
            case FACET_NORTH_EAST:
            {
                Points[0] = Cube::ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
                Points[1] = Cube::ConvertSpacialPoint(SPACIAL_POINT_EAST_BOTTOM);
                Points[2] = Cube::ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
                Points[3] = Cube::ConvertSpacialPoint(SPACIAL_POINT_NORTH_TOP);
                break;
            }
            case FACET_SOUTH_EAST:
            {
                Points[0] = Cube::ConvertSpacialPoint(SPACIAL_POINT_EAST_BOTTOM);
                Points[1] = Cube::ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
                Points[2] = Cube::ConvertSpacialPoint(SPACIAL_POINT_SOUTH_TOP);
                Points[3] = Cube::ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
                break;
            }
            case FACET_SOUTH_WEST:
            {
                Points[0] = Cube::ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
                Points[1] = Cube::ConvertSpacialPoint(SPACIAL_POINT_WEST_BOTTOM);
                Points[2] = Cube::ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
                Points[3] = Cube::ConvertSpacialPoint(SPACIAL_POINT_SOUTH_TOP);
                break;
            }
            case FACET_NORTH_WEST:
            {
                Points[0] = Cube::ConvertSpacialPoint(SPACIAL_POINT_WEST_BOTTOM);
                Points[1] = Cube::ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
                Points[2] = Cube::ConvertSpacialPoint(SPACIAL_POINT_NORTH_TOP);
                Points[3] = Cube::ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
                break;
            }
        }

        TEXTURE->BindTexturePoint(Texture, 0 + Rotation);         glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);
        TEXTURE->BindTexturePoint(Texture, 1 + Rotation);         glVertex3f(Points[1].x + xTranslate, Points[1].y + yTranslate, Points[1].z);
        TEXTURE->BindTexturePoint(Texture, 2 + Rotation);         glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);

        TEXTURE->BindTexturePoint(Texture, 2 + Rotation);         glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);
        TEXTURE->BindTexturePoint(Texture, 3 + Rotation);         glVertex3f(Points[3].x + xTranslate, Points[3].y + yTranslate, Points[3].z);
        TEXTURE->BindTexturePoint(Texture, 0 + Rotation);         glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);

        SCREEN->IncrementTriangles(2);
    }

	return true;
}
