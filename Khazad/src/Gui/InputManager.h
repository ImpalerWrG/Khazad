#ifndef INPUT__HEADER
#define INPUT__HEADER

#include <stdafx.h>

#include <Singleton.h>

class Actor;

class InputManager
{
DECLARE_SINGLETON_CLASS(InputManager)

public:

	~InputManager();
	bool Init();
	bool HandleInput();

private:

	Actor* Focus;

};

#define INPUT (InputManager::GetInstance())

#endif // INPUT__HEADER
