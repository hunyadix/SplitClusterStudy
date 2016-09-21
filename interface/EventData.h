#ifndef EVENT_DATA_H
#define EVENT_DATA_H

#ifndef NOVAL_I
#define NOVAL_I -9999
#endif
#ifndef NOVAL_F
#define NOVAL_F -9999.0
#endif

#include <iostream>
#include <string>

class EventData 
{
	public:
		int fill;
		int run;
		int ls;
		int orb;
		int bx;
		int evt;
		int nvtx;
		std::string list;
		EventData()
		{ 
			init(); 
		};
		void init() 
		{
			fill               = NOVAL_I;
			run                = NOVAL_I;
			ls                 = NOVAL_I;
			orb                = NOVAL_I;
			bx                 = NOVAL_I;
			evt                = NOVAL_I;
			nvtx               = NOVAL_I;
			list = 	"fill/I:run:ls:orb:bx:evt:nvtx";
		};
};

#endif