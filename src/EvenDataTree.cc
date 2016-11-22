#include "../interface/EventDataTree.h"

// Normal event tree

void EventDataTree::defineEventTreeBranches(TTree*& eventTree, EventData& eventField)
{
	eventTree -> Branch("event", &eventField, eventField.list.c_str());
}

void EventDataTree::setEventTreeDataFields(TTree*& eventTree, EventData& eventField)
{
	eventTree -> SetBranchAddress("event",                          &eventField);
}
