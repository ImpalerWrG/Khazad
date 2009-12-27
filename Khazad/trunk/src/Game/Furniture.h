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

	bool Update();
	bool Draw(CameraOrientation Orientaion);

    Sint16 getFurnitureType()                   { return FurnitureType; }
    void setFurnitureType(Sint16 NewType)       { FurnitureType = NewType; }

    Sint16 getMaterialType()                    { return MaterialType; }
    void setMaterialType(Sint16 NewType)        { MaterialType = NewType; }

    Sint16 getModelType()                       { return ModelType; }
    void setModelType(Sint16 NewType)           { ModelType = NewType; }

protected:

    Sint16 FurnitureType;

    Sint16 MaterialType;

    Sint16 ModelType;
};

#endif  // FURNITURE__HEADER

