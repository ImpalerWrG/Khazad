#include <DataManager.h>
#include <Singleton.h>
#include <XMLManager.h>

DECLARE_SINGLETON(DataManager)

DataManager::DataManager()
{

}

DataManager::~DataManager()
{

}

bool DataManager::Init()
{
	return true;
}
