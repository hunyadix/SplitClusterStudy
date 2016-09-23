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
		std::vector<int>* pixelsCol    = new std::vector<int>;
		std::vector<int>* pixelsRow    = new std::vector<int>;
		std::vector<int>* pixelsAdc    = new std::vector<int>;
		std::vector<int>* pixelsMarker = new std::vector<int>;
		ClustData() { init(); }
		~ClustData()
		{
			delete pixelsCol;
			delete pixelsRow;
			delete pixelsAdc;
			delete pixelsMarker;
		}
		void init()
		{
			x            = NOVAL_F;
			y            = NOVAL_F;
			sizeX        = NOVAL_I;
			sizeY        = NOVAL_I;
			clusterIndex = NOVAL_I;
			clusterSize  = 0;
			charge       = NOVAL_F;
			pixelsCol    -> clear();
			pixelsRow    -> clear();
			pixelsAdc    -> clear();
			pixelsMarker -> clear();
		};
};

class EventClustersDataArrays
{
	public:
		int                            size;
		std::vector<float>*            x;
		std::vector<float>*            y;
		std::vector<int>*              sizeX;
		std::vector<int>*              sizeY;
		std::vector<int>*              clusterIndex;
		std::vector<int>*              clusterSize;
		std::vector<float>*            charge;
		std::vector<std::vector<int>>* pixelsCol;
		std::vector<std::vector<int>>* pixelsRow;
		std::vector<std::vector<int>>* pixelsAdc;
		std::vector<std::vector<int>>* pixelsMarker;
		// Module
		std::vector<int>*              det;
		std::vector<int>*              layer;
		std::vector<int>*              ladder;
		std::vector<int>*              module;
		std::vector<int>*              half;
		std::vector<int>*              outer;
		std::vector<int>*              side;
		std::vector<int>*              disk;
		std::vector<int>*              blade;
		std::vector<int>*              panel;
		std::vector<int>*              ring;
		std::vector<int>*              shl;
		std::vector<int>*              federr;

		EventClustersDataArrays()
		{
			size         = 0;
			x            = new std::vector<float>();
			y            = new std::vector<float>();
			sizeX        = new std::vector<int>();
			sizeY        = new std::vector<int>();
			clusterIndex = new std::vector<int>();
			clusterSize  = new std::vector<int>();
			charge       = new std::vector<float>();
			pixelsCol    = new std::vector<std::vector<int>>();
			pixelsRow    = new std::vector<std::vector<int>>();
			pixelsAdc    = new std::vector<std::vector<int>>();
			pixelsMarker = new std::vector<std::vector<int>>();
			det          = new std::vector<int>();
			layer        = new std::vector<int>();
			ladder       = new std::vector<int>();
			module       = new std::vector<int>();
			half         = new std::vector<int>();
			outer        = new std::vector<int>();
			side         = new std::vector<int>();
			disk         = new std::vector<int>();
			blade        = new std::vector<int>();
			panel        = new std::vector<int>();
			ring         = new std::vector<int>();
			shl          = new std::vector<int>();
			federr       = new std::vector<int>();
		}
		~EventClustersDataArrays()
		{
			delete x;
			delete y;
			delete sizeX;
			delete sizeY;
			delete clusterIndex;
			delete clusterSize;
			delete charge;
			delete pixelsCol;
			delete pixelsRow;
			delete pixelsAdc;
			delete pixelsMarker;
			// Module
			delete det;
			delete layer;
			delete ladder;
			delete module;
			delete half;
			delete outer;
			delete side;
			delete disk;
			delete blade;
			delete panel;
			delete ring;
			delete shl;
			delete federr;
		}
		void clear()
		{
			size = 0;
			x             -> clear();
			y             -> clear();
			sizeX         -> clear();
			sizeY         -> clear();
			clusterIndex  -> clear();
			clusterSize   -> clear();
			charge        -> clear();
			pixelsCol     -> clear();
			pixelsRow     -> clear();
			pixelsAdc     -> clear();
			pixelsMarker  -> clear();
			// Module
			det          -> clear();
			layer        -> clear();
			ladder       -> clear();
			module       -> clear();
			half         -> clear();
			outer        -> clear();
			side         -> clear();
			disk         -> clear();
			blade        -> clear();
			panel        -> clear();
			ring         -> clear();
			shl          -> clear();
			federr       -> clear();
		}
		void reserve(int capacity)
		{
			x            -> reserve(capacity);
			y            -> reserve(capacity);
			sizeX        -> reserve(capacity);
			sizeY        -> reserve(capacity);
			clusterIndex -> reserve(capacity);
			clusterSize  -> reserve(capacity);
			charge       -> reserve(capacity);
			pixelsCol    -> reserve(capacity);
			pixelsRow    -> reserve(capacity);
			pixelsAdc    -> reserve(capacity);
			pixelsMarker -> reserve(capacity);
			det          -> reserve(capacity);
			layer        -> reserve(capacity);
			ladder       -> reserve(capacity);
			module       -> reserve(capacity);
			half         -> reserve(capacity);
			outer        -> reserve(capacity);
			side         -> reserve(capacity);
			disk         -> reserve(capacity);
			blade        -> reserve(capacity);
			panel        -> reserve(capacity);
			ring         -> reserve(capacity);
			shl          -> reserve(capacity);
			federr       -> reserve(capacity);	
		}
		void fill(const ClustData& clustData, const ModuleData& module_on)
		{
			x            -> push_back(clustData.x);
			y            -> push_back(clustData.y);
			sizeX        -> push_back(clustData.sizeX);
			sizeY        -> push_back(clustData.sizeY);
			clusterIndex -> push_back(clustData.clusterIndex);
			clusterSize  -> push_back(clustData.clusterSize);
			charge       -> push_back(clustData.charge);
			pixelsCol    -> push_back(*clustData.pixelsCol);
			pixelsRow    -> push_back(*clustData.pixelsRow);
			pixelsAdc    -> push_back(*clustData.pixelsAdc);
			pixelsMarker -> push_back(*clustData.pixelsMarker);	
			// Module
			det          -> push_back(module_on.det);
			layer        -> push_back(module_on.layer);
			ladder       -> push_back(module_on.ladder);
			module       -> push_back(module_on.module);
			half         -> push_back(module_on.half);
			outer        -> push_back(module_on.outer);
			side         -> push_back(module_on.side);
			disk         -> push_back(module_on.disk);
			blade        -> push_back(module_on.blade);
			panel        -> push_back(module_on.panel);
			ring         -> push_back(module_on.ring);
			shl          -> push_back(module_on.shl);
			federr       -> push_back(module_on.federr);								
			++size;
		}
};

#endif