#ifndef TTREE_TOOLS_H
#define TTREE_TOOLS_H

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

#include <iostream>

class TTreeTools
{
	static int treeCheck(TTree* tree, std::string errorMessage);
};


#endif