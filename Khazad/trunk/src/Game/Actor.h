#ifndef ACTOR__HEADER
#define ACTOR__HEADER

#include <stdafx.h>
#include <Vector3.h>
#include <Renderer.h>


class Actor
{

public:

	Actor(ActorType Type);
	Actor();

	bool Init();
	virtual ~Actor();

	virtual bool Update();
	virtual bool Draw(CameraOrientation Orientaion);

    Vector3 getPosition()                       { return Position; }
    void setPosition(Vector3 NewPosition)       { Position = NewPosition; }
    void setPosition(float x, float y, float z);

	ActorType getType()                { return Type; }
	void setType(ActorType NewType)    { Type = NewType; }

	bool isInitalized()                 { return Initalized; }
	void setInitalized(bool NewValue)   { Initalized = NewValue; }

	bool isVisible()                    { return Visible; }
	void setVisible(bool NewValue)      { Visible = NewValue; }

	bool isHidden()                 { return Hidden; }
	void setHidden(bool NewValue)   { Hidden = NewValue; }

protected:

	Vector3 Position;
	ActorType Type;

	bool Visible;
    bool Initalized;
    bool Hidden;
};

#endif  // ACTOR__HEADER

