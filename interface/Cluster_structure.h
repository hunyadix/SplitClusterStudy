#ifndef CLUSTER_STRUCTURE_H
#define CLUSTER_STRUCTURE_H

#include <string>
#include <vector>
#include <cstring>
#include "../ModuleDataFetcher/interface/ModuleData.h"

#ifndef NOVAL_I
#define NOVAL_I -9999
#endif
#ifndef NOVAL_F
#define NOVAL_F -9999.0
#endif

class ClustData
{
	public:
		float x;
		float y;
		int sizeX;
		int sizeY;
		int clusterIndex;
		int clusterSize;
		float charge;
		int pixelsCol[100];
		int pixelsRow[100];
		int pixelsAdc[100];
		int pixelsMarker[100];
		ClustData() { init(); }
		void init()
		{
			x            = NOVAL_F;
			y            = NOVAL_F;
			sizeX        = NOVAL_I;
			sizeY        = NOVAL_I;
			clusterIndex = NOVAL_I;
			clusterSize  = 0;
			charge       = NOVAL_F;
			for(size_t j = 0; j < 100; j++)
			{
				pixelsCol[j] = NOVAL_I;
				pixelsRow[j] = NOVAL_I;
				pixelsAdc[j] = NOVAL_I;
				pixelsMarker[j] = NOVAL_I;
			}
		};
};

class EventClustersDataArrays
{
	public:
		int size = 0;
		std::vector<float> x;
		std::vector<float> y;
		std::vector<int>   sizeX;
		std::vector<int>   sizeY;
		std::vector<int>   clusterIndex;
		std::vector<int>   clusterSize;
		std::vector<float> charge;
		int pixelsCol[200000][100];
		int pixelsRow[200000][100];
		int pixelsAdc[200000][100];
		int pixelsMarker[200000][100];
		// Module
		std::vector<ModuleData> mod;
		std::vector<ModuleData> mod_on;
		void clear()
		{
			size = 0;
			x            .clear();
			y            .clear();
			sizeX        .clear();
			sizeY        .clear();
			clusterIndex .clear();
			clusterSize  .clear();
			charge       .clear();
			for(int numClust = 0; numClust < 200000; ++numClust)
			{
				for(int numPix = 0; numPix < 100; ++numPix)
				{
					pixelsCol[numClust][numPix]    = NOVAL_I;
					pixelsRow[numClust][numPix]    = NOVAL_I;
					pixelsAdc[numClust][numPix]    = NOVAL_I;
					pixelsMarker[numClust][numPix] = NOVAL_I;
				}
			}
			// Module
			mod          .clear();
			mod_on       .clear();
		}
		void fill(const ClustData& clustData, const ModuleData& module, const ModuleData& module_on)
		{
			if(size == 200000)
			{
				std::cerr << "Error in EventClustersDataArrays: more than 200000 clusters registered for an event. Resize the storage class!" << std::endl;
				++size;
				return;
			}
			x            .push_back(clustData.x);
			y            .push_back(clustData.y);
			sizeX        .push_back(clustData.sizeX);
			sizeY        .push_back(clustData.sizeY);
			clusterIndex .push_back(clustData.clusterIndex);
			clusterSize  .push_back(clustData.clusterSize);
			charge       .push_back(clustData.charge);
			for(int numPix = 0; numPix < 100; ++numPix)
			{
				pixelsCol[size][numPix]    = clustData.pixelsCol[numPix];
				pixelsRow[size][numPix]    = clustData.pixelsCol[numPix];
				pixelsAdc[size][numPix]    = clustData.pixelsCol[numPix];
				pixelsMarker[size][numPix] = clustData.pixelsCol[numPix];	
			}
			// Module
			mod          .push_back(module);
			mod_on       .push_back(module_on);
			++size;
		}
};

#endif