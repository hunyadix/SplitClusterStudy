#ifndef MODULE_CLUSTER_PLOT_H
#define MODULE_CLUSTER_PLOT_H

#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"

#include "../interface/CanvasExtras.h"
#include "../interface/HelperFunctionsCommon.h"
#include "../ModuleDataFetcher/interface/ModuleDataProducer.h"

#include <TH2D.h>
#include <TCanvas.h>
#include <TText.h>

#include <string>
#include <vector>
#include <array>
#include <functional>

class ModuleClusterPlot
{
	public:
		enum Type { digi = 0, digiFromMarker, digiFromMarkerWithNeighbours };
	private:
		static constexpr std::array<const char*, 4> histogramTypePrefixes = {{ "Digis", "Marked digis", "Markers with neighbours" }};
		static constexpr float BASE_DIGI_FILL_VALUE    = 0.5f;
		static constexpr float MISSING_NEIGHBOUR_VALUE = 1000.0f;
		static constexpr float PALETTE_MINIMUM    = 0.5f;
		static constexpr float PALETTE_MAXIMUM    = 1500.0f;
		TH2D histogram;
		Type type;
		int layer;
		int module;
		int ladder;
		int startEvent;
		int endEvent;
		static std::vector<ModuleClusterPlot*> moduleClusterPlotCollection;
	private:
		static void markerToRowColModifierArrays(const int& markerState, std::vector<int>& colModifiers, std::vector<int>& rowModifiers);
		static void fillMissingPixels(const int& col, const int& row, const int& markerState, TH2D& histo, const int& weight);
	public:
		ModuleClusterPlot(Type typeArg, const int& layer, const int& module, const int& ladder, const int& startEventArg, const int& endEventArg);
		int         isEventNumInRange(const int& eventNum);
		void        fill(const edm::Handle<edm::DetSetVector<PixelDigi>> digiCollection, const TrackerTopology* const trackerTopology, const int& eventNum);
		static void fillAll(const edm::Handle<edm::DetSetVector<PixelDigi>> digiCollection, const edm::Handle<edm::DetSetVector<PixelDigi>> digiFlagsCollection, const TrackerTopology* const trackerTopology, const int& eventNum);
		static void saveAllFinished(const int& eventNum);
};

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
	static const auto checkInsertIndexPair = [&colModifiers, &rowModifiers] (const int& pixelIsMarked, const int& col, const int& row)
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

void ModuleClusterPlot::fill(const edm::Handle<edm::DetSetVector<PixelDigi>> digiCollection, const TrackerTopology* const trackerTopology, const int& eventNum)
{
	if(!isEventNumInRange(eventNum)) return;
	for(const edm::DetSet<PixelDigi>& markersOnModule: *digiCollection)
	{
		DetId detId(markersOnModule.detId());
		unsigned int subdetId = detId.subdetId();
		if((subdetId != PixelSubdetector::PixelBarrel) && (subdetId != PixelSubdetector::PixelEndcap)) continue;
		ModuleData mod    = ModuleDataProducer::getPhaseZeroOfflineModuleData(detId.rawId(), trackerTopology, std::map<uint32_t, int>());
		ModuleData mod_on = ModuleDataProducer::convertPhaseZeroOfflineOnline(mod);
		if(mod_on.det    != 0)      continue;
		if(mod_on.layer  != layer)  continue;
		if(mod_on.module != module) continue;
		if(mod_on.ladder != ladder) continue;
		for(const PixelDigi& pixel: markersOnModule)
		{
			int col         = pixel.column();
			int row         = pixel.row();
			int markerState = pixel.adc();
			switch(type)
			{
				case digi:
					histogram.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
					break;
				case digiFromMarker:
					if(markerState == 0) histogram.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
					else                 histogram.SetBinContent(col, row, markerState);
					break;
				case digiFromMarkerWithNeighbours:
					if(markerState == 0) histogram.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
					else                 histogram.SetBinContent(col, row, markerState);
					fillMissingPixels(col, row, markerState, histogram, MISSING_NEIGHBOUR_VALUE);
					break;
				default:
					std::cerr << "Error in ModuleClusterPlot::fill(): Error deducing type for histogram type." << std::endl; 
					break;
			}
		}
	}
}

void ModuleClusterPlot::fillAll(const edm::Handle<edm::DetSetVector<PixelDigi>> digiCollection, const edm::Handle<edm::DetSetVector<PixelDigi>> digiFlagsCollection, const TrackerTopology* const trackerTopology, const int& eventNum)
{
	auto digiPlotsInRange = filter(moduleClusterPlotCollection, [&eventNum] (ModuleClusterPlot* plotToCheck) { return plotToCheck -> isEventNumInRange(eventNum); });
	digiPlotsInRange      = filter(digiPlotsInRange,            []          (ModuleClusterPlot* plotToCheck) { return plotToCheck -> type == digi; });
	for(const edm::DetSet<PixelDigi>& digisOnModule: *digiCollection)
	{
		DetId detId(digisOnModule.detId());
		unsigned int subdetId = detId.subdetId();
		if((subdetId != PixelSubdetector::PixelBarrel) && (subdetId != PixelSubdetector::PixelEndcap)) continue;
		ModuleData mod    = ModuleDataProducer::getPhaseZeroOfflineModuleData(detId.rawId(), trackerTopology, std::map<uint32_t, int>());
		ModuleData mod_on = ModuleDataProducer::convertPhaseZeroOfflineOnline(mod);
		auto filteredList = filter(digiPlotsInRange, [&mod_on] (ModuleClusterPlot* plotToCheck) { return mod_on.layer  == plotToCheck -> layer;   });
		filteredList      = filter(filteredList, [&mod_on] (ModuleClusterPlot* plotToCheck)     { return mod_on.module == plotToCheck -> module; });
		filteredList      = filter(filteredList, [&mod_on] (ModuleClusterPlot* plotToCheck)     { return mod_on.ladder == plotToCheck -> ladder; });
		for(ModuleClusterPlot* plotDefinitionPtr: filteredList)
		{
			TH2D& histogramToFill = plotDefinitionPtr -> histogram;
			for(const PixelDigi& pixel: digisOnModule)
			{
				int col         = pixel.column();
				int row         = pixel.row();
				histogramToFill.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
			}
		}
	}
	auto markerPlotsInRange = filter(moduleClusterPlotCollection, [&eventNum] (ModuleClusterPlot* plotToCheck) { return plotToCheck -> isEventNumInRange(eventNum); });
	markerPlotsInRange      = filter(markerPlotsInRange,          []          (ModuleClusterPlot* plotToCheck) { return plotToCheck -> type != digi; });
	for(const edm::DetSet<PixelDigi>& markersOnModule: *digiFlagsCollection)
	{
		DetId detId(markersOnModule.detId());
		unsigned int subdetId = detId.subdetId();
		if((subdetId != PixelSubdetector::PixelBarrel) && (subdetId != PixelSubdetector::PixelEndcap)) continue;
		ModuleData mod    = ModuleDataProducer::getPhaseZeroOfflineModuleData(detId.rawId(), trackerTopology, std::map<uint32_t, int>());
		ModuleData mod_on = ModuleDataProducer::convertPhaseZeroOfflineOnline(mod);		auto filteredList = filter(markerPlotsInRange, [&mod_on] (ModuleClusterPlot* plotToCheck) { return mod_on.layer  == plotToCheck -> layer;   });
		filteredList      = filter(markerPlotsInRange, [&mod_on] (ModuleClusterPlot* plotToCheck) { return mod_on.module == plotToCheck -> module; });
		filteredList      = filter(markerPlotsInRange, [&mod_on] (ModuleClusterPlot* plotToCheck) { return mod_on.ladder == plotToCheck -> ladder; });
		for(ModuleClusterPlot* plotDefinitionPtr: filteredList)
		{
			TH2D& histogramToFill = plotDefinitionPtr -> histogram;
			for(const PixelDigi& pixel: markersOnModule)
			{
				int col         = pixel.column();
				int row         = pixel.row();
				int markerState = pixel.adc();
				switch(plotDefinitionPtr -> type)
				{
					case digiFromMarker:
						if(markerState == 0) histogramToFill.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
						else                 histogramToFill.SetBinContent(col, row, markerState);
						break;
					case digiFromMarkerWithNeighbours:
						if(markerState == 0) histogramToFill.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
						else                 histogramToFill.SetBinContent(col, row, markerState);
						fillMissingPixels(col, row, markerState, histogramToFill, MISSING_NEIGHBOUR_VALUE);
						break;
					default:
						std::cerr << "Error in ModuleClusterPlot::fill(): Error deducing type for histogram type." << std::endl; 
						break;
				}
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

#endif