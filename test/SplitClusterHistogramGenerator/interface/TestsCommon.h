#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

void printUsage(int& argc, char** argv, bool killProcess, int argsType = 2)
{
	switch(argsType)
	{
		case 2:
			std::cout << "Usage: " << argv[0] << " <Ntuple path> <optional: --savePlots>" << std::endl;
			break;
		case 3:
			std::cout << "Usage: " << argv[0] << " <Ntuple path> <num. events to merge> <optional: --savePlots>" << std::endl;
			break;
		default:
			std::cout << "Undefined usage use-case." << std::endl;
	}
	if(killProcess) exit(-1);
}

void processArgs(int& argc, char** argv, std::string& inputFileName, unsigned int& numEventsToMerge, int& savePlots)
{
	if(argc != 3 && argc != 4)
	{
		printUsage(argc, argv, true);
	}
	inputFileName = argv[1];
	numEventsToMerge = atoi(argv[2]);
	if(argc == 4)
	{
		if(std::string(argv[3]) == std::string("--savePlots"))
		{
			savePlots = 1;
		}
		else
		{
			printUsage(argc, argv, true, 3);
		}
	}
	else
	{
		savePlots = 0;
	}
}
void processArgs(int& argc, char** argv, std::string& inputFileName, int& savePlots)
{
	if(argc != 2 && argc != 3)
	{
		printUsage(argc, argv, true);
	}
	inputFileName = argv[1];
	if(argc == 3)
	{
		if(std::string(argv[2]) == std::string("--savePlots"))
		{
			savePlots = 1;
		}
		else
		{
			printUsage(argc, argv, true);
		}
	}
	else
	{
		savePlots = 0;
	}
}

std::map<int, std::vector<Cluster>> getClusterCollectionSortedByEvtnum(TTree* clusterTree, EventData& eventField, Cluster& clusterField)
{
	std::map<int, std::vector<Cluster>> eventClustersMap;
	Int_t totalNumEntries = clusterTree -> GetEntries();
	for(Int_t entryIndex = 0; entryIndex < totalNumEntries; ++entryIndex) 
	{
		clusterTree -> GetEntry(entryIndex);
		int eventNum = eventField.evt;
		auto eventClustersIt = eventClustersMap.find(eventNum);
		// If key does not exist yet: add key
		if(eventClustersIt == eventClustersMap.end())
		{
			eventClustersIt = eventClustersMap.emplace(eventField.evt, std::vector<Cluster>()).first;
		}
		eventClustersIt -> second.push_back(clusterField);
	}
	return eventClustersMap;
}

void printClusterFieldInfo(const Cluster& clusterField)
{
	std::cout << "--- Begin cluster field info ---" << std::endl;

	std::cout << "det: "          << clusterField.mod_on.det    << "." << std::endl;
	std::cout << "layer: "        << clusterField.mod_on.layer  << "." << std::endl;
	std::cout << "module: "       << clusterField.mod_on.module << "." << std::endl;
	std::cout << "ladder: "       << clusterField.mod_on.ladder << "." << std::endl;
	std::cout << "x: "            << clusterField.x                << "." << std::endl;
	std::cout << "y: "            << clusterField.y                << "." << std::endl;
	std::cout << "clustSizeX: "   << clusterField.sizeX        << "." << std::endl;
	std::cout << "clustSizeY: "   << clusterField.sizeY        << "." << std::endl;
	std::cout << "clustIndex: "   << clusterField.clusterIndex       << "." << std::endl;
	std::cout << "clustSize: "    << clusterField.clusterSize        << "." << std::endl;
	std::cout << "charge: "       << clusterField.charge        << "." << std::endl;

	std::cout << "pixelsCol:    vector of size: "  << clusterField.pixelsCol.size()    << ". First element: " << clusterField.pixelsCol[0]    << "." << std::endl;
	std::cout << "pixelsRow:    vector of size: "  << clusterField.pixelsRow.size()    << ". First element: " << clusterField.pixelsRow[0]    << "." << std::endl;
	std::cout << "pixelsAdc:    vector of size: "  << clusterField.pixelsAdc.size()    << ". First element: " << clusterField.pixelsAdc[0]    << "." << std::endl;
	std::cout << "pixelsMarker: vector of size: "  << clusterField.pixelsMarker.size() << ". First element: " << clusterField.pixelsMarker[0] << "." << std::endl;

	std::cout << "--- End cluster field info ---" << std::endl;
}

#endif // TESTS_COMMON_H
