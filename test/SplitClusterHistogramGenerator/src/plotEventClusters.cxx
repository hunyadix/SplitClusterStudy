// ...

// Root
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
// #include <TH1F.h>
// #include <TH2I.h>
// #include <TRandom3.h>

// C++ libraries
#include <iostream>
#include <exception>

int main(int argc, char** argv)
{
	try
	{
		TTree* eventClustersTree = new TTree("eventClustersTree", "Event cluster collection");
		std::cout << "PlotEventClusters started." << std::endl;

	}
	catch(const std::exception& e)
	{
		std::cout << "An error occured: " << e.what() << std::endl;
	}
	return 0; 
}
