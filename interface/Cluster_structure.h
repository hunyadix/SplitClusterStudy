#ifndef CLUSTER_STRUCTURE_H
#define CLUSTER_STRUCTURE_H

#ifndef NOVAL_I
#define NOVAL_I -9999
#endif
#ifndef NOVAL_F
#define NOVAL_F -9999.0
#endif

#include <string>

class ClustData
{
	public:
		float x;
		float y;
		int sizeX;
		int sizeY;
		int i;
		int size;
		float charge;
		int pixelsCol[250];
		int pixelsRow[250];
		int pixelsAdc[250];
		int pixelsMarker[250];
		std::string list;
		ClustData() { init(); }
		void init()
		{
			x      = NOVAL_F;
			y      = NOVAL_F;
			sizeX  = NOVAL_I;
			sizeY  = NOVAL_I;
			i      = NOVAL_I;
			size   = 0;
			charge = NOVAL_F;
			for(size_t j = 0; j < 250; j++)
			{
				pixelsCol[j] = NOVAL_I;
				pixelsRow[j] = NOVAL_I;
				pixelsAdc[j] = NOVAL_I;
				pixelsMarker[j] = NOVAL_I;
			}
			list = "x/F:y/F:sizeX/I:sizeY/I:i/I:size/I:charge/F:pixelsCol[250]/I:pixelsRow[250]/I:pixelsAdc[250]/I:pixelsMarker[250]/I";
		};
};

#endif