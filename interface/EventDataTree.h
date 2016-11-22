#ifndef EVENT_DATA_TREE_H
#define EVENT_DATA_TREE_H

//////////////////////
// Tree definitions //
//////////////////////

#include "../interface/EventData.h"

////////////////////
// Root libraries //
////////////////////

#include <TROOT.h>
#include <TTree.h>

class EventDataTree
{
	public:
		static void defineEventTreeBranches(TTree*& eventTree, EventData& eventField);
		static void setEventTreeDataFields (TTree*& eventTree, EventData& eventField);
};

#endif