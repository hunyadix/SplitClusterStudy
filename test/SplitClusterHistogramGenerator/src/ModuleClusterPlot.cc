#include "../interface/ModuleClusterPlot.h"
#include "../../../interface/CanvasExtras.h"

constexpr std::array<const char*, 4> ModuleClusterPlot::histogramTypePrefixes;
std::vector<ModuleClusterPlot*> ModuleClusterPlot::moduleClusterPlotCollection;

ModuleClusterPlot::ModuleClusterPlot(Type typeArg, const int& layerArg, const int& moduleArg, const int& ladderArg, const int& startEventArg, const int& endEventArg):
	histogram(
		(std::string("ModuleClusterPlot") + "_" + std::to_string(moduleClusterPlotCollection.size()) + "_" + std::to_string(layerArg) + "_" + std::to_string(moduleArg) +  + "_" + std::to_string(ladderArg) + "_" + std::to_string(startEventArg) + "_" + std::to_string(endEventArg)).c_str(), 
		(std::string(histogramTypePrefixes[typeArg]) + " on layer " + std::to_string(layerArg) + ", module " + std::to_string(moduleArg) + ", ladder " + std::to_string(ladderArg) + ";module pix. (col);ladder pix. (row)").c_str(),
		416, 0, 416,
		160, 0, 160),
	type(typeArg),
	layer(layerArg),
	module(moduleArg),
	ladder(ladderArg),
	startEvent(startEventArg),
	endEvent(endEventArg)
{
	moduleClusterPlotCollection.push_back(this);
}

void ModuleClusterPlot::markerToRowColModifierArrays(const int& markerState, std::vector<int>& colModifiers, std::vector<int>& rowModifiers)
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

void ModuleClusterPlot::fillMissingPixels(const int& col, const int& row, const int& markerState, TH2D& histo, const int& weight)
{
	std::vector<int> colModifiers;
	std::vector<int> rowModifiers;
	float xAxisBinWidth = histo.GetXaxis() -> GetBinWidth(1);
	float yAxisBinWidth = histo.GetYaxis() -> GetBinWidth(1);
	markerToRowColModifierArrays(markerState, colModifiers, rowModifiers);
	for(unsigned int markedNeighbourIndex = 0; markedNeighbourIndex < colModifiers.size(); ++markedNeighbourIndex)
	{
		int bin = histo.GetBin(col + colModifiers[markedNeighbourIndex] * xAxisBinWidth, row + rowModifiers[markedNeighbourIndex] * yAxisBinWidth);
		if(histo.GetBinContent(bin) == 0) histo.SetBinContent(bin, weight);
	}
}

int ModuleClusterPlot::isEventNumInRange(const int& eventNum)
{
	if(startEvent <= eventNum && eventNum <= endEvent) return true;
	return false;
}

// TODO: Finish and add this to the next commit

// void ModuleClusterPlot::fillAllPairs(std::vector<Cluster> clusterCollection, const int& eventNum)
// {
// 	auto plotsToFillInTheEvent = filter(moduleClusterPlotCollection, [&eventNum] (ModuleClusterPlot* plotToCheck) { return plotToCheck -> isEventNumInRange(eventNum); });
// 	plotsToFillInTheEvent      = filter(plotsToFillInTheEvent,       [&eventNum] (ModuleClusterPlot* plotToCheck) { return plotToCheck -> type == pairs; });
// 	const ModuleData& mod_on = clusterField.mod_on;
// 	if(mod_on.det    != 0) return;
// 	auto filteredList = filter(plotsToFillInTheEvent, [&mod_on] (ModuleClusterPlot* plotToCheck) { return mod_on.layer  == plotToCheck -> layer;   });
// 	filteredList      = filter(digisInRange, [&mod_on] (ModuleClusterPlot* plotToCheck) { return mod_on.module == plotToCheck -> module; });
// 	filteredList      = filter(digisInRange, [&mod_on] (ModuleClusterPlot* plotToCheck) { return mod_on.ladder == plotToCheck -> ladder; });
// 	for(ModuleClusterPlot* plotDefinitionPtr: filteredList)
// 	{
// 		TH2D& histogramToFill = plotDefinitionPtr -> histogram;
// 		for(const int& digiIndex: range(clusterField.pixelsCol.size()))
// 		{
// 			int col         = clusterField.pixelsCol[digiIndex];
// 			int row         = clusterField.pixelsRow[digiIndex];
// 			int markerState = clusterField.pixelsMarker[digiIndex];
// 			histogramToFill.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
// 			break;
// 		}
// 	}
// }

void ModuleClusterPlot::fillAll(const Cluster& clusterField, const int& eventNum)
{
	auto plotsToFillInTheEvent = filter(moduleClusterPlotCollection, [&eventNum] (ModuleClusterPlot* plotToCheck) { return plotToCheck -> isEventNumInRange(eventNum); });
	plotsToFillInTheEvent      = filter(plotsToFillInTheEvent,       [&eventNum] (ModuleClusterPlot* plotToCheck)
	{
		return 
			(plotToCheck -> type == digis) || 
			(plotToCheck -> type == digisFromMarkers) || 
			(plotToCheck -> type == digisFromMarkersWithNeighbours); 
	});
	const ModuleData& mod_on = clusterField.mod_on;
	if(mod_on.det    != 0) return;
	auto filteredList = filter(plotsToFillInTheEvent, [&mod_on] (ModuleClusterPlot* plotToCheck) { return mod_on.layer  == plotToCheck -> layer;  });
	filteredList      = filter(filteredList, [&mod_on] (ModuleClusterPlot* plotToCheck) { return mod_on.module == plotToCheck -> module; });
	filteredList      = filter(filteredList, [&mod_on] (ModuleClusterPlot* plotToCheck) { return mod_on.ladder == plotToCheck -> ladder; });
	for(ModuleClusterPlot* plotDefinitionPtr: filteredList)
	{
		TH2D& histogramToFill = plotDefinitionPtr -> histogram;
		for(const int& digiIndex: range(clusterField.pixelsCol.size()))
		{
			int col         = clusterField.pixelsCol[digiIndex];
			int row         = clusterField.pixelsRow[digiIndex];
			int markerState = clusterField.pixelsMarker[digiIndex];
			switch(plotDefinitionPtr -> type)
			{
				case digis:
					histogramToFill.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
					break;
				case digisFromMarkers:
					if(markerState == 0) histogramToFill.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
					else                 histogramToFill.SetBinContent(col, row, markerState);
					break;
				case digisFromMarkersWithNeighbours:
					// if(markerState == 0) histogramToFill.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
					// else                 histogramToFill.SetBinContent(col, row, markerState);
					histogramToFill.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
					fillMissingPixels(col, row, markerState, histogramToFill, MISSING_NEIGHBOUR_VALUE);
					break;
				default:
					std::cerr << "Error in ModuleClusterPlot::fill(): Error deducing type for histogram type." << std::endl; 
					break;
			}
		}
	}
}

void ModuleClusterPlot::saveAllFinished(const int& eventNum)
{
	auto filteredList = filter(moduleClusterPlotCollection, [&eventNum] (ModuleClusterPlot* plotToCheck) { return plotToCheck -> endEvent == eventNum; });
	if(filteredList.size() == 0) return;
	CanvasExtras::setMulticolorColzPalette();
	std::vector<std::shared_ptr<TCanvas>> canvases;
	for(auto moduleClusterPlotToSave: filteredList)
	{
		// std::cout << "Saving plot!!!" << std::endl;
		TH2D& histogram = moduleClusterPlotToSave -> histogram;
		canvases.emplace_back(std::make_shared<TCanvas>(histogram.GetName(), (histogram.GetTitle() + std::string(" ") + std::to_string(moduleClusterPlotToSave -> startEvent) + " " + std::to_string(moduleClusterPlotToSave -> endEvent)).c_str(), 50, 50, 400, 300));
		canvases.back() -> cd();
		CanvasExtras::redesignCanvas(canvases.back().get(), &histogram);
		histogram.GetZaxis() -> SetRangeUser(PALETTE_MINIMUM, PALETTE_MAXIMUM);
		histogram.GetZaxis() -> SetRangeUser(PALETTE_MINIMUM, PALETTE_MAXIMUM);
		histogram.GetZaxis() -> SetRangeUser(PALETTE_MINIMUM, PALETTE_MAXIMUM);
		canvases.back() -> SetLogz();
		histogram.Draw("COLZ");
		TText* xlabel = new TText();
		xlabel -> SetNDC();
		xlabel -> SetTextFont(1);
		xlabel -> SetTextColor(1);
		xlabel -> SetTextSize(0.04);
		xlabel -> SetTextAlign(22);
		xlabel -> SetTextAngle(0);
		xlabel -> DrawText(0.22, 0.98, ("Events: [" + std::to_string(moduleClusterPlotToSave -> startEvent) + " - " + std::to_string(moduleClusterPlotToSave -> endEvent) + "]").c_str());
		// std::cout << histogram.GetTitle() << " is ready." << std::endl;
	}
	for(const auto& canvas: canvases)
	{
		canvas -> Update();
		std::string filename = canvas -> GetTitle();
		std::transform(filename.begin(), filename.end(), filename.begin(), [] (char ch) { return ch == ' ' ? '_' : ch; });
		filename =  "results/" + filename + ".eps";
		canvas -> SaveAs(filename.c_str());
	}
}