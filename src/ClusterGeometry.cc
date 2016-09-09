#include "../interface/ClusterGeometry.h"

namespace ClusterGeometry
{
	// Pixel distances
	int pixelSignedXDistance(const SiPixelCluster::Pixel& lhs, const SiPixelCluster::Pixel& rhs)
	{
		return rhs.x - lhs.x;
	}
	int pixelSignedYDistance(const SiPixelCluster::Pixel& lhs, const SiPixelCluster::Pixel& rhs)
	{
		return rhs.y - lhs.y;
	}
	int pixelAbsXDistance(const SiPixelCluster::Pixel& lhs, const SiPixelCluster::Pixel& rhs)
	{
		return std::abs(lhs.x - rhs.x);
	}
	int pixelAbsYDistance(const SiPixelCluster::Pixel& lhs, const SiPixelCluster::Pixel& rhs)
	{
		return std::abs(lhs.y - rhs.y);
	}
	int pixelDistanceSquared(const SiPixelCluster::Pixel& lhs, const SiPixelCluster::Pixel& rhs)
	{
		int xDist = pixelSignedXDistance(lhs, rhs);
		int yDist = pixelSignedYDistance(lhs, rhs);
		return xDist * xDist + yDist * yDist;
	}
	float pixelAbsDistance(const SiPixelCluster::Pixel& lhs, const SiPixelCluster::Pixel& rhs)
	{
		return sqrt(pixelDistanceSquared(lhs, rhs));
	}
	// Cluster distances
	float signedDistanceX(const SiPixelCluster& lhs, const SiPixelCluster& rhs)
	{
		return rhs.x() - lhs.x();
	}
	float signedDistanceY(const SiPixelCluster& lhs, const SiPixelCluster& rhs)
	{
		return rhs.y() - lhs.y();
	}
	float absDistanceX(const SiPixelCluster& lhs, const SiPixelCluster& rhs)
	{
		return std::abs(lhs.x() - rhs.x());
	}
	float absDistanceY(const SiPixelCluster& lhs, const SiPixelCluster& rhs)
	{
		return std::abs(lhs.y() - rhs.y());
	}
	float distanceSquared(const SiPixelCluster& lhs, const SiPixelCluster& rhs)
	{
		int xDist = signedDistanceX(lhs, rhs);
		int yDist = signedDistanceY(lhs, rhs);
		return xDist * xDist + yDist * yDist;
	}
	bool isSameCluster(const SiPixelCluster& lhs, const SiPixelCluster& rhs)
	{
		if(lhs.x() == rhs.x() && lhs.y() == rhs.y())
		{
			return true;
		}
		return false;
	}
	bool isCloserThan(float dist, const SiPixelCluster& lhs, const SiPixelCluster& rhs)
	{
		float distSquared = distanceSquared(lhs, rhs);
		if(dist * dist <= distSquared)
		{
			return false;
		}
		return true;
	}

};
