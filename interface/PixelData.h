#ifndef PIXELDATA_H
#define PIXELDATA_H

#ifndef NOVAL_I
#define NOVAL_I -9999
#endif
#ifndef NOVAL_F
#define NOVAL_F -9999.0
#endif

#include <string>

class PixelData
{
	public:
		// Position of the digi
		int col;
		int row;
		// Charge deposit in the digi
		int adc;
		int isMarked;

		std::string list;

		PixelData() { init(); }
		void init()
		{
			col = NOVAL_I;
			row = NOVAL_I;
			adc = NOVAL_I;
			isMarked = NOVAL_I;
			list = "col/I:row:adc:isMarked";
		};
};

#endif