#ifndef EVENT_PLOT_CREATION_H
#define EVENT_PLOT_CREATION_H

#include "../ModuleDataFetcher/interface/ModuleData.h"

// Chip size: 80 rows, 52 columns
// Ladder coordinate range:
// Layer 1: [-10, 10];
// Layer 2: [-16, 16];
// Layer 3: [-22, 22];
// Pixels per module in ladder coordinate direction: 80 * 2 = 160
// Histo definition:
// Layer 1: [-10, 10] -> [1679, 1679]; 
// Layer 2: [-16, 16] -> [2639, 2639];
// Layer 3: [-22, 22] -> [3599, 3599];
// Module coordinate range:
// All layers: [-4, 4] + extra for 0
// Pixels per module in ladder coordinate direction: 52 * 8 = 416
// Histo definition:
// All layers: [-1871, 1871]


class LayerEventPlotTriplet
{
	public:
		TH2D layer1;
		TH2D layer2;
		TH2D layer3;
	LayerEventPlotTriplet(int nameIndex) : 
		layer1(("event_plot_layer_1_" + std::to_string(nameIndex)).c_str(), ("Digis on layer 1" + std::to_string(nameIndex) + ";module pix. (col);ladder pix. (row)").c_str(), 3743, -1871.5, 1871.5, 3359, -1679.5, 1679.5),
		layer2(("event_plot_layer_2_" + std::to_string(nameIndex)).c_str(), ("Digis on layer 2" + std::to_string(nameIndex) + ";module pix. (col);ladder pix. (row)").c_str(), 3743, -1871.5, 1871.5, 5279, -2639.5, 2639.5),
		layer3(("event_plot_layer_3_" + std::to_string(nameIndex)).c_str(), ("Digis on layer 3" + std::to_string(nameIndex) + ";module pix. (col);ladder pix. (row)").c_str(), 3743, -1871.5, 1871.5, 7199, -3599.5, 3599.5) {}
	LayerEventPlotTriplet(const std::string& name, const std::string& title) : 
		layer1((name + "_layer_1").c_str(), (title + "_layer_1" + ";pixel column;pixel row").c_str(), 3743, -1871.5, 1871.5, 3359, -1679.5, 1679.5),
		layer2((name + "_layer_2").c_str(), (title + "_layer_2" + ";pixel column;pixel row").c_str(), 3743, -1871.5, 1871.5, 5279, -2639.5, 2639.5),
		layer3((name + "_layer_3").c_str(), (title + "_layer_3" + ";pixel column;pixel row").c_str(), 3743, -1871.5, 1871.5, 7199, -3599.5, 3599.5) {}
	private:
		static int  moduleAndColToXCoordinate(const int& module, const int& col);
		static int  ladderAndRowToYCoordinate(const int& ladder, const int& row);
		static void markerToRowColModifierArrays(const int& markerState, std::vector<int>& colModifiers, std::vector<int>& rowModifiers);
	public:
		void fillEventPlot(const ModuleData& mod_on, const int& col, const int& row, const int& markerState, bool fillMissingPixels = false);
};

int LayerEventPlotTriplet::moduleAndColToXCoordinate(const int& module, const int& col)
{
	int moduleCoordinate = NOVAL_I;
	if(module < 0) moduleCoordinate = (module - 0.5) * 416 + col + 1;
	if(0 < module) moduleCoordinate = (module - 0.5) * 416 + col;
	return moduleCoordinate;
}

int LayerEventPlotTriplet::ladderAndRowToYCoordinate(const int& ladder, const int& row)
{
	int ladderCoordinate = NOVAL_I;
	int isReversedModule = (ladder + (0 < ladder)) % 2;
	if(isReversedModule)
	{
		if(ladder < 0) ladderCoordinate = (ladder + 0.5) * 160 - row;
		if(0 < ladder) ladderCoordinate = (ladder + 0.5) * 160 - row - 1;
		if(ladder == 1)  ladderCoordinate += 80;
		if(ladder == -1) ladderCoordinate -= 80;
	}
	else
	{
		if(ladder < 0) ladderCoordinate = (ladder - 0.5) * 160 + row + 1;
		if(0 < ladder) ladderCoordinate = (ladder - 0.5) * 160 + row;
		if(ladder == 1)  ladderCoordinate += 80;
		if(ladder == -1) ladderCoordinate -= 80;
	}
	return ladderCoordinate;
}

void LayerEventPlotTriplet::markerToRowColModifierArrays(const int& markerState, std::vector<int>& colModifiers, std::vector<int>& rowModifiers)
{
	colModifiers.clear();
	rowModifiers.clear();
	static auto checkInsertIndexPair = [&colModifiers, &rowModifiers] (const int& pixelIsMarked, const int& col, const int& row)
	{
		if(pixelIsMarked)
		{
			colModifiers.push_back(col);
			rowModifiers.push_back(row);
		}
	};
	checkInsertIndexPair(markerState & (1 << 0), -1, -1);
	checkInsertIndexPair(markerState & (1 << 1), -1,  0);
	checkInsertIndexPair(markerState & (1 << 2), -1, +1);
	checkInsertIndexPair(markerState & (1 << 3),  0, -1);
	checkInsertIndexPair(markerState & (1 << 4),  0, +1);
	checkInsertIndexPair(markerState & (1 << 5), +1, -1);
	checkInsertIndexPair(markerState & (1 << 6), +1,  0);
	checkInsertIndexPair(markerState & (1 << 7), +1, +1);
}

void LayerEventPlotTriplet::fillEventPlot(const ModuleData& mod_on, const int& col, const int& row, const int& markerState, bool fillMissingPixels)
{
	if(mod_on.det != 0) return;
	int moduleCoordinate = moduleAndColToXCoordinate(mod_on.module, col);
	int ladderCoordinate = ladderAndRowToYCoordinate(mod_on.ladder, row);
	TH2D* plotToFill = nullptr;
	if(mod_on.layer == 1)      plotToFill = &(layer1);
	else if(mod_on.layer == 2) plotToFill = &(layer2);
	else if(mod_on.layer == 3) plotToFill = &(layer3);
	else
	{
		std::cout << c_red << "Error: " << c_def << "layer coordinate of a pixel is invalid: " << mod_on.layer << std::endl;
		std::cout << "Info: Det: " << mod_on.det << ". Ladder:" << mod_on.ladder << ". Module:" << mod_on.module << "." << std::endl;
		return;
	}
	if(markerState == 0) plotToFill -> Fill(moduleCoordinate, ladderCoordinate, 0.5);
	if(markerState != 0) plotToFill -> Fill(moduleCoordinate, ladderCoordinate, markerState);
	if(!fillMissingPixels) return;
	std::vector<int> colModifiers;
	std::vector<int> rowModifiers;
	float xAxisBinWidth = plotToFill -> GetXaxis() -> GetBinWidth(1);
	float yAxisBinWidth = plotToFill -> GetYaxis() -> GetBinWidth(1);
	markerToRowColModifierArrays(markerState, colModifiers, rowModifiers);
	std::cout << "moduleCoordinate: " << moduleCoordinate << std::endl;
	std::cout << "ladderCoordinate: " << ladderCoordinate << std::endl;
	for(unsigned int markedNeighbourIndex = 0; markedNeighbourIndex < colModifiers.size(); ++markedNeighbourIndex)
	{
		std::cout << "modified moduleCoordinate  (col): " << moduleCoordinate + colModifiers[markedNeighbourIndex] * xAxisBinWidth << std::endl;
		std::cout << "modified ladderCoordinate (row): "  << ladderCoordinate + rowModifiers[markedNeighbourIndex] * yAxisBinWidth << std::endl;
		int bin = plotToFill -> GetBin(moduleCoordinate + colModifiers[markedNeighbourIndex] * xAxisBinWidth, ladderCoordinate + rowModifiers[markedNeighbourIndex] * yAxisBinWidth);
		plotToFill -> SetBinContent(bin, 1000);
		// std::cin.get();
	}
}

#endif