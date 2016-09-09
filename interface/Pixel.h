#ifndef PIXEL_H
#define PIXEL_H

#include "PixelData.h"
#include "../ModuleDataFetcher/interface/ModuleData.h"

#ifndef NOVAL_I
#define NOVAL_I -9999
#endif
#ifndef NOVAL_F
#define NOVAL_F -9999.0
#endif

class Pixel : public PixelData
{
	public:
		ModuleData mod; // offline module number
		ModuleData mod_on; // online module number
		Pixel() {mod.init(); mod_on.init();};
		void init() 
		{
			PixelData::init();
			mod.init();
			mod_on.init();
		}
};

#endif