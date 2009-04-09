#ifndef ACTOR__HEADER
#define ACTOR__HEADER

#include <stdafx.h>
#include <Vector3.h>
#include <ScreenManager.h>

class ClipImage;

class Actor
{
    friend class ScreenManager;

public:

	Actor(ActorType Type);
	Actor();
	bool Init();
	~Actor();

    bool Initalized;

	virtual bool Update();
	virtual bool Draw();

	Vector3 Position;
	Uint32 BirthDate;
	Uint32 ID;

	bool Dead;

	ActorType getType();
	void setType(ActorType NewType);

	bool Visible;
	void setVisible(bool NewValue) { Visible = NewValue; }

	bool DirtyDrawlist;
	GLuint DrawListID;

private:

	ActorType Type;
	SDL_Rect BoundingBox;
	SDL_Rect DirtyBox;

	ClipImage* Sprite;
};

#endif  // ACTOR__HEADER

