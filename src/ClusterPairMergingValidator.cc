#include "../interface/ClusterPairMergingValidator.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <iostream>

// #include <iostream>

ClusterPairMergingValidator::ClusterForValidator::ClusterForValidator(const SiPixelCluster& clusterArg)
: cluster(&clusterArg)
{
	float maxLength = 0;
	int maxPixelPairCharge = 0;
	const auto& pixels = cluster -> pixels();
	typedef std::vector<SiPixelCluster::Pixel>::const_iterator PixelConstIt_t;
	if(pixels.size() == 0)
	{
		length = 0;
		dir = -1;
		return;
	}
	if(pixels.size() == 1)
	{
		startPixel = &(*pixels.begin());
		endPixel   = &(*pixels.begin());
		length     = 0;
		dir        = -1;
		return;
	}
	// std::cerr << "---------- Begin node check ----------" << std::endl;
	// Find longest distance
	for(PixelConstIt_t startPixelCandidate = pixels.begin(); startPixelCandidate != pixels.end(); ++startPixelCandidate)
	{
		for(PixelConstIt_t endPixelCandidate = pixels.end() - 1; endPixelCandidate != startPixelCandidate; --endPixelCandidate)
		{
			// std::cerr << "Node pair: " << startPixelCandidate - pixels.begin() << ", " << endPixelCandidate - pixels.begin() << std::endl;
			bool saveAsNewMax = false;
			float length = ClusterGeometry::pixelAbsDistance(*startPixelCandidate, *endPixelCandidate);
			// Save the furthermost pixelpair
			if(maxLength < length) saveAsNewMax = true;
			// Check charge for pixels with the same length
			if(length == maxLength)
			{
				int pixelPairCharge = startPixelCandidate -> adc + endPixelCandidate -> adc;
				if(maxPixelPairCharge < pixelPairCharge) saveAsNewMax = true;
			}
			if(saveAsNewMax == true)
			{
				startPixel = &(*startPixelCandidate);
				endPixel   = &(*endPixelCandidate);
				maxLength  = length;
				maxPixelPairCharge = startPixel -> adc + endPixel -> adc;
			}
		}
	}
	// std::cerr << "----------  End node check  ----------" << std::endl;
	// Check start-end order
	if(!startPixel || !endPixel)
	{
		length = 0;
		dir = -1;
		return;
	}
	if(endPixel -> x < startPixel -> x) std::swap(endPixel, startPixel);
	// Save cluster data
	length = maxLength;
	dir = std::atan2(static_cast<float>(endPixel -> y - startPixel -> y), static_cast<float>(endPixel -> x - startPixel -> x));
}

void ClusterPairMergingValidator::ClusterForValidator::swap(ClusterForValidator&& other)
{
	std::swap(cluster, other.cluster);
	std::swap(startPixel, other.startPixel);
	std::swap(endPixel, other.endPixel);
	std::swap(length, other.length);
	std::swap(dir, other.dir);
}

ClusterPairMergingValidator::ClusterPairMergingValidator(const SiPixelCluster& first, const SiPixelCluster& second)
: firstCluster(first), secondCluster(second)
{
	if(second.x() < first.x())
	{
		firstCluster.swap(std::move(secondCluster));
	}
}

// Check if the distance between the two columns corresponds to a double pixel
bool ClusterPairMergingValidator::isMissingPartDoubleColumn()
{
	if(firstCluster.endPixel -> x % 2 == 1 && secondCluster.startPixel -> x == firstCluster.endPixel -> x + 3)
	{
		if(std::abs(firstCluster.endPixel -> y - secondCluster.startPixel -> y) < 4)
		{
			return true;
		}
		return false;
	}
	return false;
}

// Check if the merged cluster would be too long
bool ClusterPairMergingValidator::isPairNotTooLong()
{
	if(12 < firstCluster.length + secondCluster.length)
	{
		return false;
	}
	return true;
}

// Check if the direction of the "longest distance" in the clusters are similar
bool ClusterPairMergingValidator::isDirSimilar()
{
	float dirDiff = std::abs(secondCluster.dir - firstCluster.dir);
	if(dirDiff < M_PI / 4) // 45 degrees
	{
		return true;
	}
	return false;
}

float ClusterPairMergingValidator::getClusterAngle(const SiPixelCluster& cluster)
{
	ClusterForValidator clusterWithInfo(cluster);
	return clusterWithInfo.dir;
}

// Simple breadth first search
std::vector<std::pair<int, int>> ClusterPairMergingValidator::getShortestPathBetweenClusters(const SiPixelCluster& lhs, const SiPixelCluster& rhs)
{
	auto getNeighbourPositionVector = [] (int row, int col)
	{
		std::vector<std::pair<int, int>> neighbourIndexCollection;
		auto checkInsertIndexPair = [&neighbourIndexCollection] (int row, int col)
		{
		  if(row < 0 || row > 159 || col < 0 || col > 415)
		  {
		    return;
		  }
		  neighbourIndexCollection.emplace_back(row, col);
		};
		checkInsertIndexPair(row - 1, col - 1);
		checkInsertIndexPair(row - 1, col    );
		checkInsertIndexPair(row - 1, col + 1);
		checkInsertIndexPair(row,     col - 1);
		checkInsertIndexPair(row,     col + 1);
		checkInsertIndexPair(row + 1, col - 1);
		checkInsertIndexPair(row + 1, col    );
		checkInsertIndexPair(row + 1, col + 1);
		return neighbourIndexCollection;
	};
	struct SearchData
	{
		uint16_t x;
		uint16_t y;
		uint16_t distance;
		SearchData* parent = nullptr;
	};
	std::vector<std::pair<uint16_t, uint16_t>> goalPositions;
	std::list<SearchData> processedPoints;
	std::queue<SearchData>  unprocessedPoints;
	for(const auto& pixel: rhs.pixels())
	{
		goalPositions.emplace_back(pixel.x, pixel.y);
	}
	for(const auto& pixel: lhs.pixels())
	{
		SearchData pixelData;
		pixelData.x = pixel.x;
		pixelData.y = pixel.y;
		pixelData.distance = 0;
		pixelData.parent = nullptr;
		unprocessedPoints.push(std::move(pixelData));
	}
	while(!(unprocessedPoints.empty()))
	{
		processedPoints.push_back(unprocessedPoints.front());
		unprocessedPoints.pop();
		SearchData& pixelData = processedPoints.back();
		if(std::find(goalPositions.begin(), goalPositions.end(), std::make_pair(pixelData.x, pixelData.y)) != goalPositions.end()) break;
		for(const auto& neighbourPosition: getNeighbourPositionVector(pixelData.x, pixelData.y))
		{
			SearchData newPixelData;
			newPixelData.x = neighbourPosition.first;
			newPixelData.y = neighbourPosition.second;
			newPixelData.distance = pixelData.distance + 1;
			newPixelData.parent = &pixelData;
			unprocessedPoints.push(newPixelData);
		}
	}
	std::vector<std::pair<int, int>> result;
	SearchData* tip = &(processedPoints.back());
	while(tip != nullptr)
	{
		int x = tip -> x; // Implicit conversion
		int y = tip -> y; // Implicit conversion
		result.emplace_back(x, y);
		tip = tip -> parent;
	}
	return result;
}
