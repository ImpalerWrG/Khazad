#ifndef FURNITURE__HEADER
#define FURNITURE__HEADER

#include <stdafx.h>
#include <Actor.h>

class Furniture: Actor
{

public:

	Furniture();

	bool Init();
	virtual ~Furniture();

	int Update();
	//bool Draw(CameraOrientation Orientaion);

    int16_t getFurnitureType()                   { return FurnitureType; }
    void setFurnitureType(int16_t NewType)       { FurnitureType = NewType; }

    int16_t getMaterialType()                    { return MaterialType; }
    void setMaterialType(int16_t NewType)        { MaterialType = NewType; }

    int16_t getModelType()                       { return ModelType; }
    void setModelType(int16_t NewType)           { ModelType = NewType; }

protected:

    int16_t FurnitureType;

    int16_t MaterialType;

    int16_t ModelType;
};

#endif  // FURNITURE__HEADER

