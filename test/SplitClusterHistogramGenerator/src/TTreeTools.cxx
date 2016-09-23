#include "../interface/TTreeTools.h"
#include "../interface/CommonActors.h"

int TTreeTools::treeCheck(TTree* tree, std::string errorMessage, bool stopExecution)
{
	if(tree == nullptr)
	{
		std::cerr << error_prompt << errorMessage << std::endl;
		if(stopExecution) exit(-1);
		return(-1);
	}
	return 0;
}