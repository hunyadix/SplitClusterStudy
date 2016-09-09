#include "../interface/PixelDataTree.h"

////////////////////
// Tree branching //
////////////////////

void PixelDataTree::definePixelTreeBranches(TTree*& pixelTree, Pixel& pixelField, EventData& eventField)
{
	pixelTree -> Branch("event",     &eventField,          eventField.list.c_str());
	pixelTree -> Branch("module",    &pixelField.mod,      ModuleData::list.c_str());
	pixelTree -> Branch("module_on", &pixelField.mod_on,   ModuleData::list.c_str());
	// Position of the digi
	pixelTree -> Branch("col",       &pixelField.col,      "col/I");
	pixelTree -> Branch("row",       &pixelField.row,      "row/I");
	// Charge deposit in the digi
	pixelTree -> Branch("adc",       &pixelField.adc,      "adc/I");
	// Marker data for dcol lost digis
	pixelTree -> Branch("isMarked",  &pixelField.isMarked, "isMarked/I");
}

////////////////////////////////////
// Tree branch address assignment //
////////////////////////////////////

void PixelDataTree::setPixelTreeDataFields(TTree*& pixelTree, Pixel& pixelField, EventData& eventField)
{
	pixelTree -> SetBranchAddress("event",     &eventField);
	pixelTree -> SetBranchAddress("module",    &pixelField.mod);
	pixelTree -> SetBranchAddress("module_on", &pixelField.mod_on);
	// Position of the digi
	pixelTree -> SetBranchAddress("col",       &pixelField.col);
	pixelTree -> SetBranchAddress("row",       &pixelField.row);
	// Charge deposit in the digi
	pixelTree -> SetBranchAddress("adc",       &pixelField.adc);
	// Marker data for dcol lost digis
	pixelTree -> SetBranchAddress("isMarked",  &pixelField.isMarked);
}
