#include "stdafx.h"
#include "PipelineManager.h"
#include "XercesImpl.h"
#define cimg_display 0
#include "CImg.h"

#include <iostream>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <sstream>

using namespace std;
using namespace xercesc;

typedef struct RetValues
{
	std::string parent;
	std::vector<string> vals;
};

class XercesImplPipeline : public XercesImpl
{
	public:

		XercesImplPipeline(){};
		~XercesImplPipeline(){};

		vector<RetValues> getValues(std::string attrib);
};

vector<RetValues> XercesImplPipeline::getValues(std::string attrib)
{
	vector<RetValues> ret;
	if(parsed)
	{
		xercesc::DOMDocument* xmlDoc = parser->getDocument();
		XMLCh* TAG_attrib=XMLString::transcode(attrib.c_str());
		XMLCh* TAG_attrib_parent=XMLString::transcode("value");
		DOMNodeList* nodeListe = xmlDoc->getElementsByTagName(TAG_attrib);
		for(int nodeIt=0;nodeIt<nodeListe->getLength();nodeIt++)
		{
			if(nodeListe->item(nodeIt)->getNodeType() && nodeListe->item(nodeIt)->getNodeType() == DOMNode::ELEMENT_NODE )
			{
				DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(nodeListe->item(nodeIt));
				DOMNode* parentNode=currentElement->getParentNode();
				DOMNamedNodeMap *parentAttribMap = parentNode->getAttributes();
				RetValues val;
				for(int i=0;i<parentAttribMap->getLength();i++)
				{
					DOMAttr* attribute = dynamic_cast<xercesc::DOMAttr*>(parentAttribMap->item(i));
					char * attribName = XMLString::transcode(attribute->getName());
					string s_attribName(attribName);
					if(s_attribName==string("value"))
					{
						char * data = XMLString::transcode(attribute->getValue());
						val.parent=string(data);
						i=parentAttribMap->getLength();
						XMLString::release(&data);
					}
					XMLString::release(&attribName);
				}
				DOMNamedNodeMap* attributes = currentElement->getAttributes();
				for( unsigned int i = 0; i < attributes->getLength(); ++i )
				{
					DOMAttr* attribute = dynamic_cast<xercesc::DOMAttr*>(attributes->item(i));
					char * data = XMLString::transcode(attribute->getValue());
					val.vals.push_back(string(data));
					XMLString::release(&data);
				}
				ret.push_back(val);
			}
		}
		if(nodeListe->getLength()<=0)
			throw(runtime_error("No "+attrib));

		XMLString::release(&TAG_attrib);
		XMLString::release(&TAG_attrib_parent);
	}
	return ret;
}

PipelineManager	*PipelineManager::pipelineManager	=	_NULLPTR_;
XercesImpl		*PipelineManager::pImpl		=	new XercesImplPipeline;

string PipelineManager::xmlFile;
map<string, vector<unsigned int> > PipelineManager::datasetConfig;
std::map<int, std::string> PipelineManager::datasetId;
unsigned int PipelineManager::nbDataset=0;

PipelineManager::PipelineManager(void)
{
}


PipelineManager::~PipelineManager(void)
{
	if(pipelineManager)
	{
	   delete pImpl;
	}
}

void PipelineManager::loadConfigFile(std::string file) throw(runtime_error)
{
	if(!pipelineManager)
	{
		pipelineManager = new PipelineManager;
	}

	try
	{
		((XercesImplPipeline*)pImpl)->Init(file);
		vector<map<string,string> > map = ((XercesImplPipeline*)pImpl)->getAttributes("Dataset");
		for(int i=0;i<map.size();i++)
		{
			string ds = map[i]["value"];
			if(ds!=string(""))
			{
				datasetConfig[ds]=vector<unsigned int>(0);
				datasetId[i]=ds;
				nbDataset++;
			}
		}
		vector<RetValues> val = ((XercesImplPipeline*)pImpl)->getValues("Step");
		for(int i=0;i<val.size();i++)
		{
			datasetConfig[val[i].parent]=pImpl->splitAndConvertToInt(val[i].vals);
		}
	}
	catch(const std::exception & e)
	{
		if(e.what()!=string("No Step"))
			throw(e);
	}
}
void PipelineManager::option(int argc, char **argv)
{
	cimg_usage("Retrieve command line arguments");
	/*
	const char* filename = cimg_option("-i","image.gif","Input image file");
	const char* output   = cimg_option("-o",(char*)0,"Output image file");
	const double sigma   = cimg_option("-s",1.0,"Standard variation of the gaussian smoothing");
	const  int nblevels  = cimg_option("-n",16,"Number of quantification levels");
	*/
	xmlFile = std::string(cimg_option("-xml","","Input xml config file"));
	if(!xmlFile.empty())
		loadConfigFile(xmlFile);

	_assert();
}
void PipelineManager::_assert()
{
	assert(xmlFile!="");
}
int  PipelineManager::getNbDataset()
{
	return nbDataset;
}
string  PipelineManager::getDatasetConfigFile(unsigned int id)
{
	if(id>=0 && id<nbDataset)
		return datasetId[id];

	return "";
}
std::vector<unsigned int>  PipelineManager::getDatasetSteps(unsigned int id)
{
	if(id>=0 && id<nbDataset)
		return datasetConfig[datasetId[id]];

	return std::vector<unsigned int>();
}

