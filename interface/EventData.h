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
		int trig;
		int nclu[4]; // [0: fpix, i: layer i]
		int npix[4]; // [0: fpix, i: layer i]
		unsigned int beamint[2];
		float l1_rate;
		float intlumi;
		float instlumi;
		float instlumi_ext;
		float pileup;
		float weight;
		float vtxndof;
		float vtxchi2;
		float vtxD0;
		float vtxX;
		float vtxY;
		float vtxZ;
		int vtxntrk;
		int good;
		float tmuon;
		float tmuon_err;
		float tecal;
		float tecal_raw;
		float tecal_err;
		float field;
		int wbc;
		int delay;
		int ntracks;
		int ntrackFPix[2]; // tracks crossing the pixels
		int ntrackBPix[3]; // tracks crossing the pixels
		int ntrackFPixvalid[2]; // tracks crossing the pixels with valid hits
		int ntrackBPixvalid[3]; // tracks crossing the pixels with valid hits
		float trackSep;
		int federrs_size;
		// must be the last variable of the object saved to TTree:
		int federrs[16][2]; // [error index] [0:Nerror, 1:errorType]

		std::string list;

		EventData()
		{ 
			this -> init(); 
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
			trig               = NOVAL_I;
			for (size_t i = 0; i < 4; i++) 
			{
				nclu[i] = npix[i] = NOVAL_I;
			}
			beamint[0]         = abs(NOVAL_I);
			beamint[1]         = abs(NOVAL_I);
			l1_rate            = NOVAL_F;
			intlumi            = NOVAL_F;
			instlumi           = NOVAL_F;
			instlumi_ext       = NOVAL_F;
			pileup             = NOVAL_F;
			weight             = NOVAL_F;
			vtxndof            = NOVAL_F;
			vtxD0              = NOVAL_F;
			vtxZ               = NOVAL_F;
			vtxX               = NOVAL_F;
			vtxY               = NOVAL_F;
			vtxchi2            = NOVAL_F;
			vtxntrk            = NOVAL_I;
			good               = NOVAL_I;
			tmuon              = NOVAL_F;
			tmuon_err          = NOVAL_F;
			tecal              = NOVAL_F;
			tecal_raw          = NOVAL_F;
			tecal_err          = NOVAL_F;
			field              = NOVAL_F;
			wbc                = NOVAL_I;
			delay              = NOVAL_I;
			ntracks            = NOVAL_I;
			ntrackFPix[0]      = NOVAL_I;
			ntrackFPix[1]      = NOVAL_I;
			ntrackBPix[0]      = NOVAL_I;
			ntrackBPix[1]      = NOVAL_I;
			ntrackBPix[2]      = NOVAL_I;
			ntrackFPixvalid[0] = NOVAL_I;
			ntrackFPixvalid[1] = NOVAL_I;
			ntrackBPixvalid[0] = NOVAL_I;
			ntrackBPixvalid[1] = NOVAL_I;
			ntrackBPixvalid[2] = NOVAL_I;
			trackSep           = NOVAL_F;
			federrs_size       = 0;
			for(size_t i = 0; i < 16; i++) 
			{
				federrs[i][0]=federrs[i][1]=NOVAL_I;
			}
			list = 	"fill/I:run:ls:orb:bx:evt:nvtx:trig:nclu[4]:npix[4]:beamint[2]/i:"
					"l1_rate/F:intlumi:instlumi:instlumi_ext:pileup:weight:vtxndof:vtxchi2:"
					"vtxD0:vtxX:vtxY:vtxZ:vtxntrk/I:good:tmuon/F:tmuon_err:tecal:tecal_raw:"
					"tecal_err:field:wbc/I:delay:ntracks:ntrackFPix[2]:ntrackBPix[3]:"
					"ntrackFPixvalid[2]:ntrackBPixvalid[3]:trackSep/F:federrs_size/I:"
					"federrs[federrs_size][2]";
		};
};

#endif