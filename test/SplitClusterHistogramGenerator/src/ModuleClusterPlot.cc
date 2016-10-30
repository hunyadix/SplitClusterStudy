#include "../interface/ModuleClusterPlot.h"

constexpr std::array<const char*, 8> ModuleClusterPlot::histogramTypePrefixes;
std::vector<ModuleClusterPlot*> ModuleClusterPlot::moduleClusterPlotCollection;

ModuleClusterPlot::ModuleClusterPlot(Type typeArg, const int& layerArg, const int& moduleArg, const int& ladderArg, const int& startEventArg, const int& endEventArg):
	histogram(
		(std::string("ModuleClusterPlot") + "_" + std::to_string(moduleClusterPlotCollection.size()) + "_" + std::to_string(layerArg) + "_" + std::to_string(moduleArg) +  + "_" + std::to_string(ladderArg) + "_" + std::to_string(startEventArg) + "_" + std::to_string(endEventArg)).c_str(), 
		(std::string(histogramTypePrefixes[typeArg]) + " on layer " + std::to_string(layerArg) + ", module " + std::to_string(moduleArg) + ", ladder " + std::to_string(ladderArg) + ";module pix. (col);ladder pix. (row)").c_str(),
		416, 0, 416,
		160, 0, 160),
	canvas(histogram.GetName(), (histogram.GetTitle() + std::string(" ") + std::to_string(startEventArg) + " " + std::to_string(endEventArg)).c_str(), 50, 50, 400, 300),
	type(typeArg),
	layer(layerArg),
	module(moduleArg),
	ladder(ladderArg),
	startEvent(startEventArg),
	endEvent(endEventArg)
{
	CanvasExtras::redesignCanvas(&canvas, &histogram);
	moduleClusterPlotCollection.push_back(this);
}

void ModuleClusterPlot::markerToRowColModifierArrays(const int& markerState, std::vector<int>& colModifiers, std::vector<int>& rowModifiers)
{
	static const auto checkInsertIndexPair = [&colModifiers, &rowModifiers] (const int& pixelIsMarked, const int& col, const int& row)
	{
		if(pixelIsMarked)
		{
			colModifiers.push_back(col);
			rowModifiers.push_back(row);
		}
	};
	colModifiers.clear();
	rowModifiers.clear();
	checkInsertIndexPair(markerState & (1 << 0), -1, -1);
	checkInsertIndexPair(markerState & (1 << 1), -1,  0);
	checkInsertIndexPair(markerState & (1 << 2), -1, +1);
	checkInsertIndexPair(markerState & (1 << 3),  0, -1);
	checkInsertIndexPair(markerState & (1 << 4),  0, +1);
	checkInsertIndexPair(markerState & (1 << 5), +1, -1);
	checkInsertIndexPair(markerState & (1 << 6), +1,  0);
	checkInsertIndexPair(markerState & (1 << 7), +1, +1);
}

void ModuleClusterPlot::fillMissingPixels(const int& col, const int& row, const int& markerState, const int& weight)
{
	std::vector<int> colModifiers;
	std::vector<int> rowModifiers;
	float xAxisBinWidth = histogram.GetXaxis() -> GetBinWidth(1);
	float yAxisBinWidth = histogram.GetYaxis() -> GetBinWidth(1);
	markerToRowColModifierArrays(markerState, colModifiers, rowModifiers);
	for(unsigned int markedNeighbourIndex = 0; markedNeighbourIndex < colModifiers.size(); ++markedNeighbourIndex)
	{
		int bin = histogram.GetBin(col + colModifiers[markedNeighbourIndex] * xAxisBinWidth, row + rowModifiers[markedNeighbourIndex] * yAxisBinWidth);
		if(histogram.GetBinContent(bin) == 0) histogram.SetBinContent(bin, weight);
	}
}

void ModuleClusterPlot::fillDigisFromCluster(const Cluster& cluster, const float& fillWeight)
{
	for(const int& digiIndex: range(cluster.pixelsCol.size()))
	{
		int col         = cluster.pixelsCol[digiIndex];
		int row         = cluster.pixelsRow[digiIndex];
		int markerState = cluster.pixelsMarker[digiIndex];
		switch(type)
		{
			case digis:
			case pairs:
			case pairsWithAngleLabels:
				histogram.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
				break;
			case digisFromMarkers:
			case pairsWithMarkers:
				if(markerState == 0) histogram.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
				else                 histogram.SetBinContent(col, row, markerState);
				break;
			case digisFromMarkersWithNeighbours:
			case pairsWithNeighbours:
				// if(markerState == 0) histogram.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
				// else                 histogram.SetBinContent(col, row, markerState);
				histogram.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
				fillMissingPixels(col, row, markerState, MISSING_NEIGHBOUR_VALUE);
				break;
			case pairsWithAngleColorCodes:
				// std::cout << "fillWeight: " << fillWeight << std::endl;
				histogram.SetBinContent(col, row, fillWeight);
				break;
			default:
				std::cerr << "Error in ModuleClusterPlot::fill(): Error deducing type for histogram type." << std::endl; 
				break;
		}
	}
}

int ModuleClusterPlot::isEventNumInRange(const int& eventNum)
{
	if(startEvent <= eventNum && eventNum <= endEvent) return true;
	return false;
}

void ModuleClusterPlot::fillDigisMarkers(const Cluster& cluster, const int& eventNum)
{
	auto filteredList = filter(moduleClusterPlotCollection, [&eventNum] (ModuleClusterPlot* plotToCheck) { return plotToCheck -> isEventNumInRange(eventNum); });
	filteredList      = filter(filteredList,                []          (ModuleClusterPlot* plotToCheck)
	{
		return 
			(plotToCheck -> type == digis) || 
			(plotToCheck -> type == digisFromMarkers) || 
			(plotToCheck -> type == digisFromMarkersWithNeighbours); 
	});
	if(filteredList.empty()) return;
	const ModuleData& mod_on = cluster.mod_on;
	if(mod_on.det != 0) return;
	filteredList = filter(filteredList, [&mod_on] (ModuleClusterPlot* plotToCheck)
	{
		return 
			mod_on.layer  == plotToCheck -> layer &&
			mod_on.module == plotToCheck -> module &&
			mod_on.ladder == plotToCheck -> ladder;
	});
	for(ModuleClusterPlot* plotDefinitionPtr: filteredList)
	{
		plotDefinitionPtr -> fillDigisFromCluster(cluster);
	}
}
void ModuleClusterPlot::fillDigisMarkers(const std::vector<Cluster>& clusterCollection, const int& eventNum)
{
	for(const auto& cluster: clusterCollection)
	{
		fillDigisMarkers(cluster, eventNum);
	}
}

void ModuleClusterPlot::fillAllPairs(const std::vector<Cluster>& clusterCollection, const int& eventNum)
{
	auto plotsToFillInTheEvent = filter(moduleClusterPlotCollection, [&eventNum] (ModuleClusterPlot* plotToCheck) { return plotToCheck -> isEventNumInRange(eventNum); });
	plotsToFillInTheEvent      = filter(plotsToFillInTheEvent,       []          (ModuleClusterPlot* plotToCheck)
	{ 
		return 
			(plotToCheck -> type == pairs                    ) || 
			(plotToCheck -> type == pairsWithMarkers         ) || 
			(plotToCheck -> type == pairsWithNeighbours      ) || 
			(plotToCheck -> type == pairsWithAngleLabels     ) ||
			(plotToCheck -> type == pairsWithAngleColorCodes);
	});
	if(plotsToFillInTheEvent.empty()) return;
	for(auto firstClusterIt = clusterCollection.begin(); firstClusterIt != clusterCollection.end(); ++firstClusterIt)
	{
		const ModuleData& mod1 = firstClusterIt -> mod_on;
		if(mod1.det != 0) continue;
		auto filteredList = filter(plotsToFillInTheEvent, [&mod1] (ModuleClusterPlot* plotToCheck) 
		{
			return 
				mod1.layer  == plotToCheck -> layer  &&
				mod1.module == plotToCheck -> module &&
				mod1.ladder == plotToCheck -> ladder;
		});
		if(filteredList.size() == 0) continue;
		for(auto secondClusterIt = firstClusterIt + 1; secondClusterIt != clusterCollection.end(); ++secondClusterIt)
		{
			const ModuleData& mod2 = secondClusterIt -> mod_on;
			if(!(mod1 == mod2)) continue;
			if(!ClusterPairFunctions::areClustersPair(*firstClusterIt, *secondClusterIt)) continue;
			for(ModuleClusterPlot* plotDefinitionPtr: filteredList)
			{
				if(plotDefinitionPtr -> type == pairsWithAngleColorCodes)
				{
					float angle = ClusterPairFunctions::getClusterPairAngle(std::make_pair(
						std::make_shared<Cluster>(*firstClusterIt),
						std::make_shared<Cluster>(*secondClusterIt)));
					plotDefinitionPtr -> fillDigisFromCluster(*firstClusterIt,  angle);
					plotDefinitionPtr -> fillDigisFromCluster(*secondClusterIt, angle);
				}
				plotDefinitionPtr -> fillDigisFromCluster(*firstClusterIt);
				plotDefinitionPtr -> fillDigisFromCluster(*secondClusterIt);
				if(plotDefinitionPtr -> type == pairsWithAngleLabels)
				{
					plotDefinitionPtr -> canvas.cd();
					TText* eventlabel = new TText();
					CanvasExtras::setLabelStyleNote(*eventlabel);		
					eventlabel -> DrawText(0.32, 0.48, std::string("FIXME").c_str());
				}
			}
		}
	}
}

void ModuleClusterPlot::saveAllFinished(const int& eventNum)
{
	auto filteredList = filter(moduleClusterPlotCollection, [&eventNum] (ModuleClusterPlot* plotToCheck) { return plotToCheck -> endEvent == eventNum; });
	if(filteredList.size() == 0) return;
	// Quick and dirty palette
	CanvasExtras::setMulticolorColzPalette();
	for(auto moduleClusterPlotToSave: filteredList)
	{
		TH2D& histogram = moduleClusterPlotToSave -> histogram;
		TCanvas& canvas = moduleClusterPlotToSave -> canvas;
		canvas.cd();
		// Angle identifier colors
		if(moduleClusterPlotToSave -> type == pairsWithAngleColorCodes)
		{
			histogram.GetZaxis() -> SetRangeUser(ANGLE_PALETTE_MINIMUM, ANGLE_PALETTE_MAXIMUM);
			histogram.GetZaxis() -> SetRangeUser(ANGLE_PALETTE_MINIMUM, ANGLE_PALETTE_MAXIMUM);
			histogram.GetZaxis() -> SetRangeUser(ANGLE_PALETTE_MINIMUM, ANGLE_PALETTE_MAXIMUM);			
		}
		// Marker identifier colors
		else
		{
			histogram.GetZaxis() -> SetRangeUser(NORMAL_PALETTE_MINIMUM, NORMAL_PALETTE_MAXIMUM);
			histogram.GetZaxis() -> SetRangeUser(NORMAL_PALETTE_MINIMUM, NORMAL_PALETTE_MAXIMUM);
			histogram.GetZaxis() -> SetRangeUser(NORMAL_PALETTE_MINIMUM, NORMAL_PALETTE_MAXIMUM);
			canvas.SetLogz();
		}
		// Slow, but accurate palette
		// CanvasExtras::setMulticolorColzPalette();
		histogram.Draw("COLZ");
		TText* eventlabel = new TText();
		CanvasExtras::setLabelStyleNote(*eventlabel);		
		eventlabel -> DrawText(0.22, 0.98, (
			"Events: [" + 
			std::to_string(moduleClusterPlotToSave -> startEvent) + " - " +
			std::to_string(moduleClusterPlotToSave -> endEvent  ) + "]").c_str());
		canvas.Update();
		std::string filename = canvas.GetTitle();
		std::transform(filename.begin(), filename.end(), filename.begin(), [] (char ch) { return ch == ' ' ? '_' : ch; });
		filename =  "results/" + filename + ".eps";
		canvas.SaveAs(filename.c_str());
	}
}