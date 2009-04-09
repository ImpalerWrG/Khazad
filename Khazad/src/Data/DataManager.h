#ifndef DATA__HEADER
#define DATA__HEADER

#include <stdafx.h>
#include <Singleton.h>

class DataManager
{
	DECLARE_SINGLETON_CLASS(DataManager)

public:

	~DataManager();
	bool Init();


private:

};

#define DATA (DataManager::GetInstance())

#endif // DATA__HEADER
