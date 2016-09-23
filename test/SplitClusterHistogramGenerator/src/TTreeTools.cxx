#include "../interface/TTreeTools.h"

int TTreeTools::treeCheck(TTree* tree, std::string errorMessage)
{
	if(tree == NULL)
	{
		// std::cerr << error_prompt << errorMessage << std::endl;
		std::cerr << errorMessage << std::endl;
		return(-1);
	}
	return 0;
}