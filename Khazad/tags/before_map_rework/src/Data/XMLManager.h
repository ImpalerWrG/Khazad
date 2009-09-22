#ifndef XML__HEADER
#define XML__HEADER

#include <stdafx.h>
#include <Singleton.h>
#include <tinystr.h>
#include <tinyxml.h>

class XMLManager
{
	DECLARE_SINGLETON_CLASS(XMLManager)

public:

	~XMLManager();
	bool Init();

	std::vector<TiXmlDocument*> XMLDocLibrary;

	void DumpToConsole(const char* Filename);
	void dump_to_stdout(const char* pFilename);
	void dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 );
	bool dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent);

	TiXmlDocument* loadFile(const char* Filename);

	template <class T>
    void QueryUIntValue(TiXmlElement* Entry, const char* Element, const char* Attribute, T& defaultvalue)
    {
        TiXmlElement* child = Entry->FirstChildElement(Element);
        if (child)
        {
            int temp;
            child->QueryIntAttribute(Attribute, &temp);
            if (temp >= 0)
            {
                //Error
                defaultvalue = temp;
            }
        }
    };

    template <class T>
    void QuerySIntValue(TiXmlElement* Entry, const char* Element, const char* Attribute, T& defaultvalue)
    {
        TiXmlElement* child = Entry->FirstChildElement(Element);
        if (child)
        {
            int temp;
            child->QueryIntAttribute(Attribute, &temp);
            defaultvalue = temp;
        }
    };

    bool QueryTextValue(TiXmlElement* Entry, const char* Element, const char* Attribute, string& String)
    {
        TiXmlElement* child = Entry->FirstChildElement(Element);
        if (child)
        {
            const char * attr = child->Attribute(Attribute);
            if(attr != NULL)
            {
                String = attr;
                return true;
            }
        }
        return false;
    };

    template <class T>
    void QuerySIntArray(TiXmlElement* Entry, const char* Element, const char* SubElement, const char* Attribute, std::vector<T>& StorageVector)
    {
        TiXmlElement* child = Entry->FirstChildElement(Element);
        if (child)
        {
            TiXmlElement* Iterator = child->FirstChildElement(SubElement);
            for(; Iterator != NULL; Iterator = Iterator->NextSiblingElement(SubElement))
            {
                int temp;
                Iterator->QueryIntAttribute(Attribute, &temp);
                StorageVector.push_back(temp);
            }
        }
    };

    template <class T>
    void QueryUIntArray(TiXmlElement* Entry, const char* Element, const char* SubElement, const char* Attribute, std::vector<T>& StorageVector)
    {
        TiXmlElement* child = Entry->FirstChildElement(Element);
        if (child)
        {
            TiXmlElement* Iterator = child->FirstChildElement(SubElement);
            for(; Iterator != NULL; Iterator = Iterator->NextSiblingElement(SubElement))
            {
                int temp;
                Iterator->QueryIntAttribute(Attribute, &temp);
                if (temp >= 0)
                {
                    StorageVector.push_back(temp);
                }
            }
        }
    };
    bool QueryExists(TiXmlElement* Entry, const char* Element)
    {
        TiXmlElement* child = Entry->FirstChildElement(Element);
        if(child)
            return true;
        return false;
    }
	const char * getIndentAlt( unsigned int numIndents );
	const char * getIndent( unsigned int numIndents );

protected:

};

#define XML (XMLManager::GetInstance())

#endif // XML__HEADER
