#ifndef PIXEL_DATA_TREE_H
#define PIXEL_DATA_TREE_H

#include "../interface/Pixel.h"
#include "../interface/EventData.h"

#include <TROOT.h>
#include <TTree.h>

class PixelDataTree
{
	public:
		static void definePixelTreeBranches(TTree*& pixelTree, Pixel& pixelField, EventData& eventField);
		static void setPixelTreeDataFields (TTree*& pixelTree, Pixel& pixelField, EventData& eventField);
};


#endif