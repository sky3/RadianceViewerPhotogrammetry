#ifndef _PIPELINEMANAGER_H_
#define _PIPELINEMANAGER_H_

#include "CommonInclude.h"
#include <vector>
#include <string>
#include <map>

class CONFIGLIB_API PipelineManager
{
	public:
		~PipelineManager(void);

		static void loadConfigFile(std::string file);
		static void option(int argc, char **argv);
		static void _assert();

		static int getNbDataset();
		static std::string getDatasetConfigFile(unsigned int id);
		static std::vector<unsigned int> getDatasetSteps(unsigned int id);

	private:
		PipelineManager(void);
		static PipelineManager *pipelineManager;
		static XercesImpl* pImpl;

		static unsigned int nbDataset;
		static std::string xmlFile;
		static std::map<std::string, std::vector<unsigned int> > datasetConfig;
		static std::map<int, std::string> datasetId;
};

#endif

