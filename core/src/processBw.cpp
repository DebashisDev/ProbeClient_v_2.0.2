/*
 * processBw.cpp
 *
 *  Created on: 18 Jul 2022
 *      Author: Debashis
 */

#include "processBw.h"

processBw::processBw()
{
	this->_name = "processBW   ";
	this->setLogLevel(Log::theLog().level());

	this->initStatus 	= false;
	this->prevMin		= 0;
	this->currentMin	= 0;
}

processBw::~processBw()
{

}

bool processBw::isInitialized()
{ return initStatus; }

void processBw::run()
{
	while(IPGlobal::PROBE_RUNNING_STATUS)
	{
		sleep(1);
		currentMin = IPGlobal::CURRENT_MIN;

		if(currentMin != prevMin)
		{
			/* Bandwidth Processing */

			buildBwCSV(IPGlobal::CURRENT_EPOCH_MILI_SEC);

			if(IPGlobal::PROCESS_CDN)
			{
				openCDNCsvXdrFile(IPGlobal::CURRENT_MIN, IPGlobal::CURRENT_HOUR, IPGlobal::CURRENT_DAY, IPGlobal::CURRENT_MONTH, IPGlobal::CURRENT_YEAR);
				writeCDNXdr(bwXdr, cachedXdr, unCachedXdr, peeringXdr);
				closeCDNCsvXdrFile();
			}
			else
			{
				openBwCsvXdrFile(IPGlobal::CURRENT_MIN, IPGlobal::CURRENT_HOUR, IPGlobal::CURRENT_DAY, IPGlobal::CURRENT_MONTH, IPGlobal::CURRENT_YEAR);
				writeBwXdr(bwXdr);
				closeBwCsvXdrFile();
			}
			prevMin = currentMin;
		}
	}
}

void processBw::openCDNCsvXdrFile(uint16_t &currentMin, uint16_t &currentHour, uint16_t &currentDay, uint16_t &currentMonth, uint16_t &currentYear)
{
	char filePath[300];
	filePath[0] = 0;

	sprintf(filePath, "%s%s/%s_%d-%02d-%02d-%02d-%02d.csv",
					IPGlobal::BW_DIR.c_str(),
					"cdn",
					"cdn",
					currentYear,
					currentMonth,
					currentDay,
					currentHour,
					currentMin);
	CDNXdrHandler.open((char *)filePath, ios :: out | ios :: app);

	filePath[0] = 0;
}

void processBw::writeCDNXdr(char *bufferBw, char *bufferCdn, char *bufferUnc, char *bufferPeering)
{
	TheLog_nc_v1(Log::Info, name(),"    Writing          BW  [%s]", bufferBw);
	TheLog_nc_v1(Log::Info, name(),"    Writing Cached   BW  [%s]", bufferCdn);
	TheLog_nc_v1(Log::Info, name(),"    Writing unCached BW  [%s]", bufferUnc);
	TheLog_nc_v1(Log::Info, name(),"    Writing Peering  BW  [%s]", bufferPeering);

	CDNXdrHandler << bufferBw << std::endl;
	CDNXdrHandler << bufferCdn << std::endl;
	CDNXdrHandler << bufferUnc << std::endl;
	CDNXdrHandler << bufferPeering << std::endl;
}

void processBw::closeCDNCsvXdrFile()
{ CDNXdrHandler.close(); }

void processBw::buildBwCSV(uint64_t timems)
{
	bwXdr[0] = 0;

	bwData bw_i[MAX_INTERFACE_SUPPORT];
	peeringData peering_i[MAX_INTERFACE_SUPPORT];

	for(uint16_t intf = 0; intf < IPGlobal::NO_OF_INTERFACES; intf++)
		for(uint16_t router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
		{
			bw_i[intf].peakTotalVol += IPGlobal::BW_MBPS_i_r[intf][router].peakTotalVol;
			bw_i[intf].peakUpTotalVol += IPGlobal::BW_MBPS_i_r[intf][router].peakUpTotalVol;
			bw_i[intf].peakDnTotalVol += IPGlobal::BW_MBPS_i_r[intf][router].peakDnTotalVol;
			bw_i[intf].totalVol += IPGlobal::BW_MBPS_i_r[intf][router].totalVol;
			bw_i[intf].upTotalVol += IPGlobal::BW_MBPS_i_r[intf][router].upTotalVol;
			bw_i[intf].dnTotalVol += IPGlobal::BW_MBPS_i_r[intf][router].dnTotalVol;
			bw_i[intf].avgTotalBw += IPGlobal::BW_MBPS_i_r[intf][router].avgTotalBw;
			bw_i[intf].avgUpBw += IPGlobal::BW_MBPS_i_r[intf][router].avgUpBw;
			bw_i[intf].avgDnBw += IPGlobal::BW_MBPS_i_r[intf][router].avgDnBw;
		}


	sprintf(bwXdr, "%d,%d,"
					"%lu,"
					"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
					"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
					"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
					"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
					"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
					"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
					"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
					"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu",
			IPGlobal::PROBE_ID, XDR_ID_BW,
			timems,
			bw_i[0].peakTotalVol*8, bw_i[0].peakUpTotalVol*8, bw_i[0].peakDnTotalVol*8, bw_i[0].totalVol, bw_i[0].upTotalVol, bw_i[0].dnTotalVol, bw_i[0].avgTotalBw, bw_i[0].avgUpBw, bw_i[0].avgDnBw,
			bw_i[1].peakTotalVol*8, bw_i[1].peakUpTotalVol*8, bw_i[1].peakDnTotalVol*8, bw_i[1].totalVol, bw_i[1].upTotalVol, bw_i[1].dnTotalVol, bw_i[1].avgTotalBw, bw_i[1].avgUpBw, bw_i[1].avgDnBw,
			bw_i[2].peakTotalVol*8, bw_i[2].peakUpTotalVol*8, bw_i[2].peakDnTotalVol*8,bw_i[2].totalVol, bw_i[2].upTotalVol, bw_i[2].dnTotalVol, bw_i[2].avgTotalBw, bw_i[2].avgUpBw, bw_i[2].avgDnBw,
			bw_i[3].peakTotalVol*8, bw_i[3].peakUpTotalVol*8, bw_i[3].peakDnTotalVol*8,bw_i[3].totalVol, bw_i[3].upTotalVol, bw_i[3].dnTotalVol, bw_i[3].avgTotalBw, bw_i[3].avgUpBw, bw_i[3].avgDnBw,
			bw_i[4].peakTotalVol*8, bw_i[4].peakUpTotalVol*8, bw_i[4].peakDnTotalVol*8,bw_i[4].totalVol, bw_i[4].upTotalVol, bw_i[4].dnTotalVol, bw_i[4].avgTotalBw, bw_i[4].avgUpBw, bw_i[4].avgDnBw,
			bw_i[5].peakTotalVol*8, bw_i[5].peakUpTotalVol*8, bw_i[5].peakDnTotalVol*8,bw_i[5].totalVol, bw_i[5].upTotalVol, bw_i[5].dnTotalVol, bw_i[5].avgTotalBw, bw_i[5].avgUpBw, bw_i[5].avgDnBw,
			bw_i[6].peakTotalVol*8, bw_i[6].peakUpTotalVol*8, bw_i[6].peakDnTotalVol*8,bw_i[6].totalVol, bw_i[6].upTotalVol, bw_i[6].dnTotalVol, bw_i[6].avgTotalBw, bw_i[6].avgUpBw, bw_i[6].avgDnBw,
			bw_i[7].peakTotalVol*8, bw_i[7].peakUpTotalVol*8, bw_i[7].peakDnTotalVol*8,bw_i[7].totalVol, bw_i[7].upTotalVol, bw_i[7].dnTotalVol, bw_i[7].avgTotalBw, bw_i[7].avgUpBw, bw_i[7].avgDnBw);


	if(IPGlobal::PROCESS_PEERING)
	{
		peeringXdr[0] = 0;

		for(uint16_t intf = 0; intf < IPGlobal::NO_OF_INTERFACES; intf++)
			for(uint16_t router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
			{
				peering_i[intf].peakTotalVol += IPGlobal::PEERING_MBPS_i_r[intf][router].peakTotalVol;
				peering_i[intf].peakUpTotalVol += IPGlobal::PEERING_MBPS_i_r[intf][router].peakUpTotalVol;
				peering_i[intf].peakDnTotalVol += IPGlobal::PEERING_MBPS_i_r[intf][router].peakDnTotalVol;
				peering_i[intf].totalVol += IPGlobal::PEERING_MBPS_i_r[intf][router].totalVol;
				peering_i[intf].upTotalVol += IPGlobal::PEERING_MBPS_i_r[intf][router].upTotalVol;
				peering_i[intf].dnTotalVol += IPGlobal::PEERING_MBPS_i_r[intf][router].dnTotalVol;
				peering_i[intf].avgTotalBw += IPGlobal::PEERING_MBPS_i_r[intf][router].avgTotalBw;
				peering_i[intf].avgUpBw += IPGlobal::PEERING_MBPS_i_r[intf][router].avgUpBw;
				peering_i[intf].avgDnBw += IPGlobal::PEERING_MBPS_i_r[intf][router].avgDnBw;
			}

		sprintf(peeringXdr, "%d,%d,"
						"%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu",
				IPGlobal::PROBE_ID, XDR_ID_PEERING,
				timems,
				peering_i[0].peakTotalVol*8, peering_i[0].peakUpTotalVol*8, peering_i[0].peakDnTotalVol*8, peering_i[0].totalVol, peering_i[0].upTotalVol, peering_i[0].dnTotalVol, peering_i[0].avgTotalBw, peering_i[0].avgUpBw, peering_i[0].avgDnBw,
				peering_i[1].peakTotalVol*8, peering_i[1].peakUpTotalVol*8, peering_i[1].peakDnTotalVol*8, peering_i[1].totalVol, peering_i[1].upTotalVol, peering_i[1].dnTotalVol, peering_i[1].avgTotalBw, peering_i[1].avgUpBw, peering_i[1].avgDnBw,
				peering_i[2].peakTotalVol*8, peering_i[2].peakUpTotalVol*8, peering_i[2].peakDnTotalVol*8,peering_i[2].totalVol, peering_i[2].upTotalVol, peering_i[2].dnTotalVol, peering_i[2].avgTotalBw, peering_i[2].avgUpBw, peering_i[2].avgDnBw,
				peering_i[3].peakTotalVol*8, peering_i[3].peakUpTotalVol*8, peering_i[3].peakDnTotalVol*8,peering_i[3].totalVol, peering_i[3].upTotalVol, peering_i[3].dnTotalVol, peering_i[3].avgTotalBw, peering_i[3].avgUpBw, peering_i[3].avgDnBw,
				peering_i[4].peakTotalVol*8, peering_i[4].peakUpTotalVol*8, peering_i[4].peakDnTotalVol*8,peering_i[4].totalVol, peering_i[4].upTotalVol, peering_i[4].dnTotalVol, peering_i[4].avgTotalBw, peering_i[4].avgUpBw, peering_i[4].avgDnBw,
				peering_i[5].peakTotalVol*8, peering_i[5].peakUpTotalVol*8, peering_i[5].peakDnTotalVol*8,peering_i[5].totalVol, peering_i[5].upTotalVol, peering_i[5].dnTotalVol, peering_i[5].avgTotalBw, peering_i[5].avgUpBw, peering_i[5].avgDnBw,
				peering_i[6].peakTotalVol*8, peering_i[6].peakUpTotalVol*8, peering_i[6].peakDnTotalVol*8,peering_i[6].totalVol, peering_i[6].upTotalVol, peering_i[6].dnTotalVol, peering_i[6].avgTotalBw, peering_i[6].avgUpBw, peering_i[6].avgDnBw,
				peering_i[7].peakTotalVol*8, peering_i[7].peakUpTotalVol*8, peering_i[7].peakDnTotalVol*8,peering_i[7].totalVol, peering_i[7].upTotalVol, peering_i[7].dnTotalVol, peering_i[7].avgTotalBw, peering_i[7].avgUpBw, peering_i[7].avgDnBw);
	}

	if(IPGlobal::PROCESS_CDN)
	{
		cachedXdr[0] = 0;
		unCachedXdr[0] = 0;

		cdnData cdn_i[MAX_INTERFACE_SUPPORT];

		for(uint16_t intf = 0; intf < IPGlobal::NO_OF_INTERFACES; intf++)
			for(uint16_t router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
			{
				cdn_i[intf].peakTotalVol += IPGlobal::CDN_MBPS_i_r[intf][router].peakTotalVol;
				cdn_i[intf].peakUpTotalVol += IPGlobal::CDN_MBPS_i_r[intf][router].peakUpTotalVol;
				cdn_i[intf].peakDnTotalVol += IPGlobal::CDN_MBPS_i_r[intf][router].peakDnTotalVol;
				cdn_i[intf].totalVol += IPGlobal::CDN_MBPS_i_r[intf][router].totalVol;
				cdn_i[intf].upTotalVol += IPGlobal::CDN_MBPS_i_r[intf][router].upTotalVol;
				cdn_i[intf].dnTotalVol += IPGlobal::CDN_MBPS_i_r[intf][router].dnTotalVol;
				cdn_i[intf].avgTotalBw += IPGlobal::CDN_MBPS_i_r[intf][router].avgTotalBw;
				cdn_i[intf].avgUpBw += IPGlobal::CDN_MBPS_i_r[intf][router].avgUpBw;
				cdn_i[intf].avgDnBw += IPGlobal::CDN_MBPS_i_r[intf][router].avgDnBw;
			}

		sprintf(cachedXdr, "%d,%d,"
						"%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu",
				IPGlobal::PROBE_ID, XDR_ID_CH,
				timems,
				cdn_i[0].peakTotalVol*8, cdn_i[0].peakUpTotalVol*8, cdn_i[0].peakDnTotalVol*8, cdn_i[0].totalVol, cdn_i[0].upTotalVol, cdn_i[0].dnTotalVol, cdn_i[0].avgTotalBw, cdn_i[0].avgUpBw, cdn_i[0].avgDnBw,
				cdn_i[1].peakTotalVol*8, cdn_i[1].peakUpTotalVol*8, cdn_i[1].peakDnTotalVol*8, cdn_i[1].totalVol, cdn_i[1].upTotalVol, cdn_i[1].dnTotalVol, cdn_i[1].avgTotalBw, cdn_i[1].avgUpBw, cdn_i[1].avgDnBw,
				cdn_i[2].peakTotalVol*8, cdn_i[2].peakUpTotalVol*8, cdn_i[2].peakDnTotalVol*8,cdn_i[2].totalVol, cdn_i[2].upTotalVol, cdn_i[2].dnTotalVol, cdn_i[2].avgTotalBw, cdn_i[2].avgUpBw, cdn_i[2].avgDnBw,
				cdn_i[3].peakTotalVol*8, cdn_i[3].peakUpTotalVol*8, cdn_i[3].peakDnTotalVol*8,cdn_i[3].totalVol, cdn_i[3].upTotalVol, cdn_i[3].dnTotalVol, cdn_i[3].avgTotalBw, cdn_i[3].avgUpBw, cdn_i[3].avgDnBw,
				cdn_i[4].peakTotalVol*8, cdn_i[4].peakUpTotalVol*8, cdn_i[4].peakDnTotalVol*8,cdn_i[4].totalVol, cdn_i[4].upTotalVol, cdn_i[4].dnTotalVol, cdn_i[4].avgTotalBw, cdn_i[4].avgUpBw, cdn_i[4].avgDnBw,
				cdn_i[5].peakTotalVol*8, cdn_i[5].peakUpTotalVol*8, cdn_i[5].peakDnTotalVol*8,cdn_i[5].totalVol, cdn_i[5].upTotalVol, cdn_i[5].dnTotalVol, cdn_i[5].avgTotalBw, cdn_i[5].avgUpBw, cdn_i[5].avgDnBw,
				cdn_i[6].peakTotalVol*8, cdn_i[6].peakUpTotalVol*8, cdn_i[6].peakDnTotalVol*8,cdn_i[6].totalVol, cdn_i[6].upTotalVol, cdn_i[6].dnTotalVol, cdn_i[6].avgTotalBw, cdn_i[6].avgUpBw, cdn_i[6].avgDnBw,
				cdn_i[7].peakTotalVol*8, cdn_i[7].peakUpTotalVol*8, cdn_i[7].peakDnTotalVol*8,cdn_i[7].totalVol, cdn_i[7].upTotalVol, cdn_i[7].dnTotalVol, cdn_i[7].avgTotalBw, cdn_i[7].avgUpBw, cdn_i[7].avgDnBw);

		sprintf(unCachedXdr, "%d,%d,"
						"%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,"
						"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu",
				IPGlobal::PROBE_ID, XDR_ID_UC,
				timems,
				(bw_i[0].peakTotalVol - (cdn_i[0].peakTotalVol + peering_i[0].peakTotalVol))*8, (bw_i[0].peakUpTotalVol - (cdn_i[0].peakUpTotalVol + peering_i[0].peakUpTotalVol))*8, (bw_i[0].peakDnTotalVol - (cdn_i[0].peakDnTotalVol + peering_i[0].peakDnTotalVol))*8, (bw_i[0].totalVol - (cdn_i[0].totalVol + peering_i[0].totalVol)), (bw_i[0].upTotalVol - (cdn_i[0].upTotalVol + peering_i[0].upTotalVol)), (bw_i[0].dnTotalVol - (cdn_i[0].dnTotalVol + peering_i[0].dnTotalVol)), (bw_i[0].avgTotalBw - (cdn_i[0].avgTotalBw + peering_i[0].avgTotalBw)), (bw_i[0].avgUpBw - (cdn_i[0].avgUpBw + peering_i[0].avgUpBw)), (bw_i[0].avgDnBw - (cdn_i[0].avgDnBw + peering_i[0].avgDnBw)),
				(bw_i[1].peakTotalVol - (cdn_i[1].peakTotalVol + peering_i[1].peakTotalVol))*8, (bw_i[1].peakUpTotalVol - (cdn_i[1].peakUpTotalVol + peering_i[1].peakUpTotalVol))*8, (bw_i[1].peakDnTotalVol - (cdn_i[1].peakDnTotalVol + peering_i[1].peakDnTotalVol))*8, (bw_i[1].totalVol - (cdn_i[1].totalVol + peering_i[1].totalVol)), (bw_i[1].upTotalVol - (cdn_i[1].upTotalVol + peering_i[1].upTotalVol)), (bw_i[1].dnTotalVol - (cdn_i[1].dnTotalVol + peering_i[1].dnTotalVol)), (bw_i[1].avgTotalBw - (cdn_i[1].avgTotalBw + peering_i[1].avgTotalBw)), (bw_i[1].avgUpBw - (cdn_i[1].avgUpBw + peering_i[1].avgUpBw)), (bw_i[1].avgDnBw - (cdn_i[1].avgDnBw + peering_i[1].avgDnBw)),
				(bw_i[2].peakTotalVol - (cdn_i[2].peakTotalVol + peering_i[2].peakTotalVol))*8, (bw_i[2].peakUpTotalVol - (cdn_i[2].peakUpTotalVol + peering_i[2].peakUpTotalVol))*8, (bw_i[2].peakDnTotalVol - (cdn_i[2].peakDnTotalVol + peering_i[2].peakDnTotalVol))*8, (bw_i[2].totalVol - (cdn_i[2].totalVol + peering_i[2].totalVol)), (bw_i[2].upTotalVol - (cdn_i[2].upTotalVol + peering_i[2].upTotalVol)), (bw_i[2].dnTotalVol - (cdn_i[2].dnTotalVol + peering_i[2].dnTotalVol)), (bw_i[2].avgTotalBw - (cdn_i[2].avgTotalBw + peering_i[2].avgTotalBw)), (bw_i[2].avgUpBw - (cdn_i[2].avgUpBw + peering_i[2].avgUpBw)), (bw_i[2].avgDnBw - (cdn_i[2].avgDnBw + peering_i[2].avgDnBw)),
				(bw_i[3].peakTotalVol - (cdn_i[3].peakTotalVol + peering_i[3].peakTotalVol))*8, (bw_i[3].peakUpTotalVol - (cdn_i[3].peakUpTotalVol + peering_i[3].peakUpTotalVol))*8, (bw_i[3].peakDnTotalVol - (cdn_i[3].peakDnTotalVol + peering_i[3].peakDnTotalVol))*8, (bw_i[3].totalVol - (cdn_i[3].totalVol + peering_i[3].totalVol)), (bw_i[3].upTotalVol - (cdn_i[3].upTotalVol + peering_i[3].upTotalVol)), (bw_i[3].dnTotalVol - (cdn_i[3].dnTotalVol + peering_i[3].dnTotalVol)), (bw_i[3].avgTotalBw - (cdn_i[3].avgTotalBw + peering_i[3].avgTotalBw)), (bw_i[3].avgUpBw - (cdn_i[3].avgUpBw + peering_i[3].avgUpBw)), (bw_i[3].avgDnBw - (cdn_i[3].avgDnBw + peering_i[3].avgDnBw)),
				(bw_i[4].peakTotalVol - (cdn_i[4].peakTotalVol + peering_i[4].peakTotalVol))*8, (bw_i[4].peakUpTotalVol - (cdn_i[4].peakUpTotalVol + peering_i[4].peakUpTotalVol))*8, (bw_i[4].peakDnTotalVol - (cdn_i[4].peakDnTotalVol + peering_i[4].peakDnTotalVol))*8, (bw_i[4].totalVol - (cdn_i[4].totalVol + peering_i[4].totalVol)), (bw_i[4].upTotalVol - (cdn_i[4].upTotalVol + peering_i[4].upTotalVol)), (bw_i[4].dnTotalVol - (cdn_i[4].dnTotalVol + peering_i[4].dnTotalVol)), (bw_i[4].avgTotalBw - (cdn_i[4].avgTotalBw + peering_i[4].avgTotalBw)), (bw_i[4].avgUpBw - (cdn_i[4].avgUpBw + peering_i[4].avgUpBw)), (bw_i[4].avgDnBw - (cdn_i[4].avgDnBw + peering_i[4].avgDnBw)),
				(bw_i[5].peakTotalVol - (cdn_i[5].peakTotalVol + peering_i[5].peakTotalVol))*8, (bw_i[5].peakUpTotalVol - (cdn_i[5].peakUpTotalVol + peering_i[5].peakUpTotalVol))*8, (bw_i[5].peakDnTotalVol - (cdn_i[5].peakDnTotalVol + peering_i[5].peakDnTotalVol))*8, (bw_i[5].totalVol - (cdn_i[5].totalVol + peering_i[5].totalVol)), (bw_i[5].upTotalVol - (cdn_i[5].upTotalVol + peering_i[5].upTotalVol)), (bw_i[5].dnTotalVol - (cdn_i[5].dnTotalVol + peering_i[5].dnTotalVol)), (bw_i[5].avgTotalBw - (cdn_i[5].avgTotalBw + peering_i[5].avgTotalBw)), (bw_i[5].avgUpBw - (cdn_i[5].avgUpBw + peering_i[5].avgUpBw)), (bw_i[5].avgDnBw - (cdn_i[5].avgDnBw + peering_i[5].avgDnBw)),
				(bw_i[6].peakTotalVol - (cdn_i[6].peakTotalVol + peering_i[6].peakTotalVol))*8, (bw_i[6].peakUpTotalVol - (cdn_i[6].peakUpTotalVol + peering_i[6].peakUpTotalVol))*8, (bw_i[6].peakDnTotalVol - (cdn_i[6].peakDnTotalVol + peering_i[6].peakDnTotalVol))*8, (bw_i[6].totalVol - (cdn_i[6].totalVol + peering_i[6].totalVol)), (bw_i[6].upTotalVol - (cdn_i[6].upTotalVol + peering_i[6].upTotalVol)), (bw_i[6].dnTotalVol - (cdn_i[6].dnTotalVol + peering_i[6].dnTotalVol)), (bw_i[6].avgTotalBw - (cdn_i[6].avgTotalBw + peering_i[6].avgTotalBw)), (bw_i[6].avgUpBw - (cdn_i[6].avgUpBw + peering_i[6].avgUpBw)), (bw_i[6].avgDnBw - (cdn_i[6].avgDnBw + peering_i[6].avgDnBw)),
				(bw_i[7].peakTotalVol - (cdn_i[7].peakTotalVol + peering_i[7].peakTotalVol))*8, (bw_i[7].peakUpTotalVol - (cdn_i[7].peakUpTotalVol + peering_i[7].peakUpTotalVol))*8, (bw_i[7].peakDnTotalVol - (cdn_i[7].peakDnTotalVol + peering_i[7].peakDnTotalVol))*8, (bw_i[7].totalVol - (cdn_i[7].totalVol + peering_i[7].totalVol)), (bw_i[7].upTotalVol - (cdn_i[7].upTotalVol + peering_i[7].upTotalVol)), (bw_i[7].dnTotalVol - (cdn_i[7].dnTotalVol + peering_i[7].dnTotalVol)), (bw_i[7].avgTotalBw - (cdn_i[7].avgTotalBw + peering_i[7].avgTotalBw)), (bw_i[7].avgUpBw - (cdn_i[7].avgUpBw + peering_i[7].avgUpBw)), (bw_i[7].avgDnBw - (cdn_i[7].avgDnBw + peering_i[7].avgDnBw)));
	}

}

void processBw::openBwCsvXdrFile(uint16_t &currentMin, uint16_t &currentHour, uint16_t &currentDay, uint16_t &currentMonth, uint16_t &currentYear)
{
	char filePath[300];
	filePath[0] = 0;

	sprintf(filePath, "%s%s/%s_%d-%02d-%02d-%02d-%02d.csv",
					IPGlobal::BW_DIR.c_str(),
					"bw",
					"bw",
					currentYear,
					currentMonth,
					currentDay,
					currentHour,
					currentMin);
	BwXdrHandler.open((char *)filePath, ios :: out | ios :: app);

	filePath[0] = 0;
}

void processBw::writeBwXdr(char *buffer)
{
	TheLog_nc_v1(Log::Info, name(),"    Writing BW           [%s]", buffer);
	BwXdrHandler << buffer << std::endl;
}

void processBw::closeBwCsvXdrFile()
{ BwXdrHandler.close(); }

