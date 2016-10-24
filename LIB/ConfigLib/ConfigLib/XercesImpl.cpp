#include "stdafx.h"
#include "XercesImpl.h"


using namespace xercesc;
using namespace std;

#include <iostream>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <sstream>

XercesImpl::XercesImpl()
{
	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch( XMLException& e )
	{
		char* message = XMLString::transcode( e.getMessage() );
		cerr << "XML toolkit initialization error: " << message << endl;
		XMLString::release( &message );
	}
	
	ATTR_value=XMLString::transcode("value");
	ATTR_path=XMLString::transcode("path");
	ATTR_ext=XMLString::transcode("ext");

	parser = new XercesDOMParser;
	parsed = false;
}

XercesImpl::~XercesImpl()
{
		delete parser;
		try
		{
			XMLString::release( &ATTR_value );
			XMLString::release( &ATTR_path );
			XMLString::release( &ATTR_ext );
		}
		catch( ... )
		{
			cerr << "Unknown exception encountered in TagNamesdtor" << endl;
		}
 
   
	   try
	   {
		  XMLPlatformUtils::Terminate();
	   }
	   catch( xercesc::XMLException& e )
	   {
		  char* message = xercesc::XMLString::transcode( e.getMessage() );
		  cerr << "XML ttolkit teardown error: " << message << endl;
		  XMLString::release( &message );
	   }
}

const bool XercesImpl::is_parsed()
{
	return parsed;
}

void XercesImpl::Init(std::string file)
{
	struct _stat64 ret;

	errno = 0;
	if( _stat64(file.c_str(), &ret) == -1) 
	{
		if( errno == ENOENT )
			throw (std::runtime_error(string("File ")+file+" does not exist."));
		else if( errno == ENOTDIR )
			throw (std::runtime_error("A component of the path is not a directory."));
		else if( errno == EACCES )
			throw (std::runtime_error("Permission denied."));
		else if( errno == ENAMETOOLONG )
			throw (std::runtime_error("File can not be read\n"));
	}

	parser->setValidationScheme(XercesDOMParser::Val_Never );
	parser->setDoNamespaces(false);
	parser->setDoSchema(false);
	parser->setLoadExternalDTD(false);

	try
	{

		parser->parse(file.c_str());

		xercesc::DOMDocument* xmlDoc = parser->getDocument();
		DOMElement* elementRoot = xmlDoc->getDocumentElement();

		if( !elementRoot ) throw(std::runtime_error("Empty XML document"));

		parsed = true;
		
	}
	catch(xercesc::XMLException& e)
	{
		char* message = xercesc::XMLString::transcode( e.getMessage() );
		ostringstream errBuf;
		errBuf << "Error parsing file: " << message << flush;
		XMLString::release( &message );
	}
}

std::vector<unsigned int> XercesImpl::splitAndConvertToInt(std::vector<string> tab)
{
	vector<unsigned int> ret;

	for(int i=0;i<tab.size();i++)
	{
		string val;
		int first=-1;
		int last=-1;
		for(int j=0;j<tab[i].size();j++)
		{
			if(tab[i][j]=='-')
			{
				if(first==-1)
				{
					first=atoi(val.c_str());
					val.clear();
				}
			}
			else
				val+=tab[i][j];
		}
		if(first!=-1)
		{
			if(last==-1)
				last=atoi(val.c_str());

			for(int x=first;x<=last;x++)
				ret.push_back(x);
		}
		else
			ret.push_back((unsigned int)atoi(val.c_str()));

	}
	return ret;
}

std::vector<std::string> XercesImpl::findPath(std::string attrib, bool ext)
{
	vector<string> ret;
	
	if(parsed)
	{
		xercesc::DOMDocument* xmlDoc = parser->getDocument();
		XMLCh* TAG_attrib=XMLString::transcode(attrib.c_str());
		DOMNodeList* nodeListe = xmlDoc->getElementsByTagName(TAG_attrib);
		if(nodeListe->getLength())
		{
			if(nodeListe->item(0)->getNodeType() && nodeListe->item(0)->getNodeType() == DOMNode::ELEMENT_NODE )
			{
				DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(nodeListe->item(0));
				char * data = XMLString::transcode(currentElement->getAttribute(ATTR_path));
				ret.push_back(checkPath(string(data)));
				XMLString::release(&data);
				if(ext)
				{
					char * data2 = XMLString::transcode(currentElement->getAttribute(ATTR_ext));
					ret.push_back("."+string(data2));
					XMLString::release(&data2);
				}
			}
		}else
			throw(runtime_error("No "+attrib));

		XMLString::release(&TAG_attrib);
	}

	return ret;
}

string XercesImpl::getValue(std::string attrib)
{
	string ret;
	if(parsed)
	{
		xercesc::DOMDocument* xmlDoc = parser->getDocument();
		XMLCh* TAG_attrib=XMLString::transcode(attrib.c_str());
		DOMNodeList* nodeListe = xmlDoc->getElementsByTagName(TAG_attrib);
		if(nodeListe->getLength())
		{
			if(nodeListe->item(0)->getNodeType() && nodeListe->item(0)->getNodeType() == DOMNode::ELEMENT_NODE )
			{
				DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(nodeListe->item(0));
				char * data = XMLString::transcode(currentElement->getAttribute(ATTR_value));
				ret=string(data);
				XMLString::release(&data);
			}
		}else
			throw(runtime_error("No "+attrib));

		XMLString::release(&TAG_attrib);
	}
	return ret;
}

std::vector<double> XercesImpl::getNumericalValue(std::string attrib, int size)
{
	vector<double> ret;
	if(parsed)
	{
		xercesc::DOMDocument* xmlDoc = parser->getDocument();
		XMLCh* TAG_attrib=XMLString::transcode(attrib.c_str());
		DOMNodeList* nodeListe = xmlDoc->getElementsByTagName(TAG_attrib);
		if(nodeListe->getLength())
		{
			if(nodeListe->item(0)->getNodeType() && nodeListe->item(0)->getNodeType() == DOMNode::ELEMENT_NODE )
			{
				DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(nodeListe->item(0));
				DOMNamedNodeMap* attributes = currentElement->getAttributes();
				for( unsigned int i = 0; i < attributes->getLength(); ++i )
				{
					DOMAttr* attribute = dynamic_cast<xercesc::DOMAttr*>(attributes->item(i));
					char * data = XMLString::transcode(attribute->getValue());
					ret.push_back(strtod(data,NULL));
					XMLString::release(&data);
				}
			}
		}else
			throw(runtime_error("No "+attrib));

		XMLString::release(&TAG_attrib);
	}
	return ret;
}

std::vector<string> XercesImpl::getValues(std::string attrib)
{
	vector<string> ret;
	if(parsed)
	{
		xercesc::DOMDocument* xmlDoc = parser->getDocument();
		XMLCh* TAG_attrib=XMLString::transcode(attrib.c_str());
		DOMNodeList* nodeListe = xmlDoc->getElementsByTagName(TAG_attrib);
		if(nodeListe->getLength())
		{
			if(nodeListe->item(0)->getNodeType() && nodeListe->item(0)->getNodeType() == DOMNode::ELEMENT_NODE )
			{
				DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(nodeListe->item(0));
				DOMNamedNodeMap* attributes = currentElement->getAttributes();
				for( unsigned int i = 0; i < attributes->getLength(); ++i )
				{
					DOMAttr* attribute = dynamic_cast<xercesc::DOMAttr*>(attributes->item(i));
					char * data = XMLString::transcode(attribute->getValue());
					ret.push_back(string(data));
					XMLString::release(&data);
				}
			}
		}else
			throw(runtime_error("No "+attrib));

		XMLString::release(&TAG_attrib);
	}
	return ret;
}

std::vector<std::map<std::string,std::string> > XercesImpl::getAttributes(std::string name, std::vector<void*> &chNodeList)
{
	vector<std::map<std::string,std::string> > ret;
	if(parsed)
	{
		xercesc::DOMDocument* xmlDoc = parser->getDocument();
		XMLCh* TAG_attrib=XMLString::transcode(name.c_str());
		DOMNodeList* nodeListe = xmlDoc->getElementsByTagName(TAG_attrib);
		if(nodeListe->getLength())
		{
			
			for(int idNode=0;idNode<nodeListe->getLength();idNode++)
			{
				std::map<std::string,std::string> attribMap;
				if(nodeListe->item(idNode)->getNodeType() && nodeListe->item(idNode)->getNodeType() == DOMNode::ELEMENT_NODE )
				{
					DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(nodeListe->item(idNode));
					chNodeList.push_back((void*)currentElement);
					DOMNamedNodeMap* attributes = currentElement->getAttributes();
					for( unsigned int i = 0; i < attributes->getLength(); ++i )
					{
						DOMAttr* attribute = dynamic_cast<xercesc::DOMAttr*>(attributes->item(i));
						char * data = XMLString::transcode(attribute->getValue());
						char * name = XMLString::transcode(attribute->getName());

						if(string(name)==string("path"))
							attribMap[string(name)]=checkPath(string(data));
						else
							attribMap[string(name)]=string(data);

						//attribVec.push_back(make_(string(name),string(data)));
						

						XMLString::release(&data);
						XMLString::release(&name);
					}
				}
				ret.push_back(attribMap);
			}
		}else
			throw(runtime_error("No "+name));

		XMLString::release(&TAG_attrib);
	}
	return ret;
}

std::vector<std::map<std::string,std::string> > XercesImpl::getAttributes(std::string name, void* node)
{
	vector<std::map<std::string,std::string> > ret;
	if(parsed && node)
	{
		XMLCh* TAG_attrib=XMLString::transcode(name.c_str());
		DOMNodeList* nodeListe = ((DOMElement*)node)->getElementsByTagName(TAG_attrib);
		if(nodeListe->getLength())
		{
			
			for(int idNode=0;idNode<nodeListe->getLength();idNode++)
			{
				std::map<std::string,std::string> attribMap;
				if(nodeListe->item(idNode)->getNodeType() && nodeListe->item(idNode)->getNodeType() == DOMNode::ELEMENT_NODE )
				{
					DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(nodeListe->item(idNode));
					DOMNamedNodeMap* attributes = currentElement->getAttributes();
					for( unsigned int i = 0; i < attributes->getLength(); ++i )
					{
						DOMAttr* attribute = dynamic_cast<xercesc::DOMAttr*>(attributes->item(i));
						char * data = XMLString::transcode(attribute->getValue());
						char * name = XMLString::transcode(attribute->getName());

						if(string(name)==string("path"))
							attribMap[string(name)]=checkPath(string(data));
						else
							attribMap[string(name)]=string(data);

						//attribVec.push_back(make_(string(name),string(data)));
						

						XMLString::release(&data);
						XMLString::release(&name);
					}
				}
				ret.push_back(attribMap);
			}
		}else
			throw(runtime_error("No "+name));

		XMLString::release(&TAG_attrib);
	}
	return ret;
}

std::vector<std::map<std::string,std::string> > XercesImpl::getAttributes(std::string name)
{
	vector<std::map<std::string,std::string> > ret;
	if(parsed)
	{
		xercesc::DOMDocument* xmlDoc = parser->getDocument();
		XMLCh* TAG_attrib=XMLString::transcode(name.c_str());
		DOMNodeList* nodeListe = xmlDoc->getElementsByTagName(TAG_attrib);
		if(nodeListe->getLength())
		{
			
			for(int idNode=0;idNode<nodeListe->getLength();idNode++)
			{
				std::map<std::string,std::string> attribMap;
				if(nodeListe->item(idNode)->getNodeType() && nodeListe->item(idNode)->getNodeType() == DOMNode::ELEMENT_NODE )
				{
					DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(nodeListe->item(idNode));
					DOMNamedNodeMap* attributes = currentElement->getAttributes();
					for( unsigned int i = 0; i < attributes->getLength(); ++i )
					{
						DOMAttr* attribute = dynamic_cast<xercesc::DOMAttr*>(attributes->item(i));
						char * data = XMLString::transcode(attribute->getValue());
						char * name = XMLString::transcode(attribute->getName());

						if(string(name)==string("path"))
							attribMap[string(name)]=checkPath(string(data));
						else
							attribMap[string(name)]=string(data);

						//attribVec.push_back(make_(string(name),string(data)));
						

						XMLString::release(&data);
						XMLString::release(&name);
					}
				}
				ret.push_back(attribMap);
			}
		}else
			throw(runtime_error("No "+name));

		XMLString::release(&TAG_attrib);
	}
	return ret;
}

string XercesImpl::checkPath(std::string path)
{	

	if(path[path.size()-1]!='/' && path[path.size()-1]!='\\')
		path+="/";

	return path;	
}
