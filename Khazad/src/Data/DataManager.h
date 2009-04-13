#ifndef DATA__HEADER
#define DATA__HEADER

#include <stdafx.h>
#include <Singleton.h>
#include <DataTypes.h>

class DataManager
{
	DECLARE_SINGLETON_CLASS(DataManager)

public:

	~DataManager();
	bool Init();
	bool LoadDataClass(char* Path, char* Entry);

    std::vector<MaterialData*> Materials;

private:

};


#define DATA (DataManager::GetInstance())

#endif // DATA__HEADER
