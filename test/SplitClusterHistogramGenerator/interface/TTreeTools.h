#ifndef TTREE_TOOLS_H
#define TTREE_TOOLS_H

#include "../interface/ConsoleColors.h"

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

#include <iostream>

class TTreeTools
{
	public:
		static int treeCheck(TTree* tree, std::string errorMessage, bool stopExecution = false);
};


#endif