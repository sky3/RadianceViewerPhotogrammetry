#ifndef _XERCESIMPL_H_
#define _XERCESIMPL_H_

#include <vector>
#include <string>
#include <map>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

class XercesImpl
{
	public:

		XercesImpl();
		~XercesImpl();

		void Init(std::string file);

		std::vector<std::string> findPath(std::string attrib, bool ext=false);
		std::string getValue(std::string attrib);
		std::vector<double> getNumericalValue(std::string attrib, int size=1);
		std::vector<std::string> getValues(std::string attrib);
		std::vector<std::map<std::string,std::string> > getAttributes(std::string name);
		std::vector<std::map<std::string,std::string> > getAttributes(std::string name, std::vector<void*> &chNodeList);
		std::vector<std::map<std::string,std::string> > getAttributes(std::string name, void* node);
		std::vector<unsigned int> splitAndConvertToInt(std::vector<std::string> tab);
		static std::string checkPath(std::string path);

		const bool is_parsed();

	protected:

		xercesc::XercesDOMParser *parser;


		bool parsed;

		XMLCh* ATTR_value;
		XMLCh* ATTR_path;
		XMLCh* ATTR_ext;

	
};

#endif

