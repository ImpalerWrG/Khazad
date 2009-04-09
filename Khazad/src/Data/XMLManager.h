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

	bool loadFile(const char* Filename);

	template <class T>
	void getSIntValue( const char* Rootname, const char* Element, const char* Attribute, T& defaultvalue )
	{
		TiXmlDocument* Doc;
		for (Uint32 i = 0; i < XMLDocLibrary.size(); i++)
		{
			if (XMLDocLibrary[i])
			{
				TiXmlElement* Root = XMLDocLibrary[i]->RootElement();
				printf(Root->Value());
				if (strcmp( Rootname, Root->Value() ) == 0)
				{
					Doc = XMLDocLibrary[i];

					TiXmlHandle DocHandle( Doc );
					TiXmlElement* child = DocHandle.FirstChild( Rootname ).FirstChild( Element ).ToElement();
					if ( child )
					{
						child->QueryIntAttribute(Attribute, &defaultvalue);
					}
					break;
				}
			}
		}
	};

	template <class T>
	void getUIntValue( const char* Rootname, const char* Element, const char* Attribute, T& defaultvalue )
	{
		TiXmlDocument* Doc;
		for (Uint32 i = 0; i < XMLDocLibrary.size(); i++)
		{
			if (XMLDocLibrary[i])
			{
				TiXmlElement* Root = XMLDocLibrary[i]->RootElement();
				printf(Root->Value());
				if (strcmp( Rootname, Root->Value() ) == 0)
				{
					Doc = XMLDocLibrary[i];

					TiXmlHandle DocHandle( Doc );
					TiXmlElement* child = DocHandle.FirstChild( Rootname ).FirstChild( Element ).ToElement();
					if ( child )
					{
						int temp = defaultvalue;
						child->QueryIntAttribute(Attribute, &temp);
						if (temp > 0)
						{
							defaultvalue = temp;
						}
					}
					break;
				}
			}
		}
	};

	const char * getIndentAlt( unsigned int numIndents );
	const char * getIndent( unsigned int numIndents );

private:

};

#define XML (XMLManager::GetInstance())

#endif // XML__HEADER
