#ifndef MODULE_CLUSTER_PLOT_H
#define MODULE_CLUSTER_PLOT_H

#include "../../../interface/Cluster.h"
#include "../../../interface/HelperFunctionsCommon.h"

#include <TH2D.h>
#include <TCanvas.h>
#include <TText.h>

#include <string>
#include <memory>
#include <vector>
#include <array>
#include <functional>

class ModuleClusterPlot
{
	public:
		enum Type {digis = 0, digisFromMarkers, digisFromMarkersWithNeighbours, pairs};
	private:
		static constexpr std::array<const char*, 4> histogramTypePrefixes = {{ "Digis", "Marked digis", "Ntuple markers with neighbours", "Marked digi pairs" }};
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
		static void fillAll(const Cluster& Cluster, const int& eventNum);
		static void saveAllFinished(const int& eventNum);
};

#endif