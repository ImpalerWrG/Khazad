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

    void QueryTextValue(TiXmlElement* Entry, const char* Element, const char* Attribute, char* String)
    {
        TiXmlElement* child = Entry->FirstChildElement(Element);
        if (child)
        {
            const char* temp;
            temp = child->Attribute(Attribute);
            int i;
            for(i = 0; temp[i] != NULL; i++)
            {
                String[i] = temp[i];
            }
            String[i] = NULL;
        }
    };

	const char * getIndentAlt( unsigned int numIndents );
	const char * getIndent( unsigned int numIndents );

protected:

};

#define XML (XMLManager::GetInstance())

#endif // XML__HEADER
