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
		enum Type { digis = 0, digisFromMarkers, digisFromMarkersWithNeighbours, digisAndMarkers };
	private:
		static constexpr std::array<const char*, 4> histogramTypePrefixes = {{ "Digis", "Marked digis", "Markers with neighbours", "Digis and markers" }};
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
		void        fillDigi(const PixelDigi& pixel);
		void        fillMissingPixels(const int& col, const int& row, const int& markerState, const int& weight);
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
	for(int colModifier = -1; colModifier <= 1; colModifier++)
	{
		for(int rowModifier = -1; rowModifier <= 1; rowModifier++)
		{
			if(colModifier == 0 && rowModifier == 0) continue;
			int bit = colModifier + 1 + (rowModifier + 1) * 3 - int(rowModifier > 0 || (colModifier > 0 && rowModifier == 0));
			if((markerState & (1 << bit)) == 0) continue;
			colModifiers.push_back(colModifier);
			rowModifiers.push_back(rowModifier);
		}
	}
}

void ModuleClusterPlot::fillDigi(const PixelDigi& pixel)
{
	int col         = pixel.column();
	int row         = pixel.row();
	int markerState = pixel.adc();
	switch(type)
	{
		case digis:
			histogram.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
			break;
		case digisFromMarkers:
			if(markerState == 0) histogram.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
			else                 histogram.SetBinContent(col, row, markerState);
			break;
		case digisFromMarkersWithNeighbours:
			// if(markerState == 0) histogram.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
			// else                 histogram.SetBinContent(col, row, markerState);
			if(histogram.GetBinContent(col, row) == BASE_DIGI_FILL_VALUE) break;
			histogram.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
			fillMissingPixels(col, row, markerState, MISSING_NEIGHBOUR_VALUE);
			break;
		case digisAndMarkers:
			if(histogram.GetBinContent(col, row) == BASE_DIGI_FILL_VALUE) break;
			if(markerState != 1) histogram.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
			else                 histogram.SetBinContent(col, row, markerState);
			break;
		default:
			std::cerr << "Error in ModuleClusterPlot::fill(): Error deducing type for histogram type." << std::endl; 
			break;
	}
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
			fillDigi(pixel);
		}
	}
}

void ModuleClusterPlot::fillAll(const edm::Handle<edm::DetSetVector<PixelDigi>> digiCollection, const edm::Handle<edm::DetSetVector<PixelDigi>> digiFlagsCollection, const TrackerTopology* const trackerTopology, const int& eventNum)
{
	auto digiPlotsInRange = filter(moduleClusterPlotCollection, [&eventNum] (ModuleClusterPlot* plotToCheck) { return plotToCheck -> isEventNumInRange(eventNum); });
	digiPlotsInRange      = filter(digiPlotsInRange,            []          (ModuleClusterPlot* plotToCheck)
	{
		return 
			(plotToCheck -> type == digis) ||
			(plotToCheck -> type == digisAndMarkers);
	});
	for(const edm::DetSet<PixelDigi>& digisOnModule: *digiCollection)
	{
		DetId detId(digisOnModule.detId());
		unsigned int subdetId = detId.subdetId();
		if((subdetId != PixelSubdetector::PixelBarrel) && (subdetId != PixelSubdetector::PixelEndcap)) continue;
		ModuleData mod    = ModuleDataProducer::getPhaseZeroOfflineModuleData(detId.rawId(), trackerTopology, std::map<uint32_t, int>());
		ModuleData mod_on = ModuleDataProducer::convertPhaseZeroOfflineOnline(mod);
		auto filteredList = filter(digiPlotsInRange, [&mod_on] (ModuleClusterPlot* plotToCheck)
		{
			return 
				mod_on.layer  == plotToCheck -> layer &&
				mod_on.module == plotToCheck -> module &&
				mod_on.ladder == plotToCheck -> ladder;
		});
		for(ModuleClusterPlot* plotDefinitionPtr: filteredList)
		{
			for(const PixelDigi& pixel: digisOnModule)
			{
				plotDefinitionPtr -> fillDigi(pixel);
			}
		}
	}
	auto markerPlotsInRange = filter(moduleClusterPlotCollection, [&eventNum] (ModuleClusterPlot* plotToCheck) { return plotToCheck -> isEventNumInRange(eventNum); });
	markerPlotsInRange      = filter(markerPlotsInRange,          []          (ModuleClusterPlot* plotToCheck)
	{
		return 
			(plotToCheck -> type == digisFromMarkers)               ||
			(plotToCheck -> type == digisFromMarkersWithNeighbours) ||
			(plotToCheck -> type == digisAndMarkers);
	});
	for(const edm::DetSet<PixelDigi>& markersOnModule: *digiFlagsCollection)
	{
		DetId detId(markersOnModule.detId());
		unsigned int subdetId = detId.subdetId();
		if((subdetId != PixelSubdetector::PixelBarrel) && (subdetId != PixelSubdetector::PixelEndcap)) continue;
		ModuleData mod    = ModuleDataProducer::getPhaseZeroOfflineModuleData(detId.rawId(), trackerTopology, std::map<uint32_t, int>());
		ModuleData mod_on = ModuleDataProducer::convertPhaseZeroOfflineOnline(mod);		
		auto filteredList = filter(markerPlotsInRange, [&mod_on] (ModuleClusterPlot* plotToCheck) 
		{
			return 
				mod_on.layer  == plotToCheck -> layer &&
				mod_on.module == plotToCheck -> module &&
				mod_on.ladder == plotToCheck -> ladder;
		});
		for(ModuleClusterPlot* plotDefinitionPtr: filteredList)
		{
			for(const PixelDigi& pixel: markersOnModule)
			{
				plotDefinitionPtr -> fillDigi(pixel);
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