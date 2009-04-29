#ifndef ACTOR__HEADER
#define ACTOR__HEADER

#include <stdafx.h>
#include <Vector3.h>
#include <ScreenManager.h>

class ClipImage;

class Actor
{

public:

	Actor(ActorType Type);
	Actor();
	bool Init();
	virtual ~Actor();


	virtual bool Update();
	virtual bool Draw(CameraOrientation Orientaion);

	Vector3 Position;

	bool Dead;


	ActorType getType()                { return Type; }
	void setType(ActorType NewType)    { Type = NewType; }

	bool Visible;

	bool isInitalized()                { return Initalized; }
	void setInitalized(bool NewValue)  { Initalized = NewValue; }

	bool isVisible()                { return Visible; }
	void setVisible(bool NewValue)  { Visible = NewValue; }

	bool DirtyDrawlist;
	GLuint DrawListID;

	bool isHidden()                 { return Hidden; }
	void setHidden(bool NewValue)   { Hidden = NewValue; }

protected:

	ActorType Type;

    bool Initalized;
    bool Hidden;
};

#endif  // ACTOR__HEADER

