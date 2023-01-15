/*
 * CDNData.cpp
 *
 *  Created on: 03-Sep-2019
 *      Author: singh
 */

#include "CDNData.h"

CDNData::CDNData(uint16_t intfid, uint16_t rid)
{
	this->_name = "CDNData";
	this->setLogLevel(Log::theLog().level());

	this->intfId 		= intfid;
	this->routerId 		= rid;
	this->volume		= 0;
	this->dir			= 0;
	this->curSec		= 0;

	printf("** CDNData:: Initialized for Interface [%d]\n", this->intfId);
}

CDNData::~CDNData() {
}

void CDNData::updateCDNData(uint16_t curMin, uint16_t frSize, uint8_t cdnDir, uint64_t pckCurSec)
{
	volume = frSize;
	dir = cdnDir;
	curSec = pckCurSec % 100;

	uint16_t t_index = curMin % 2;

	processCdnData(cdn_i_r_t[intfId][routerId][t_index]);
}

void CDNData::processCdnData(cdnData (&cdn)[CDN_TIME_INDEX])
{
	cdn[curSec].totalVol += volume;
	if(dir == UP)
		cdn[curSec].upTotalVol += volume;
	else if(dir == DOWN)
		cdn[curSec].dnTotalVol += volume;
}

void CDNData::setCDNData(uint16_t curMin)
{
	uint16_t t_index = curMin % 2;
	if(t_index == 0) t_index = 1;
	else if(t_index == 1) t_index = 0;

	cdnData cdn;

	cdn = calculateCdnData(cdn_i_r_t[intfId][routerId][t_index]);

	IPGlobal::CDN_MBPS_i_r[intfId][routerId].Bw = cdn.Bw;
	IPGlobal::CDN_MBPS_i_r[intfId][routerId].upBw = cdn.upBw;
	IPGlobal::CDN_MBPS_i_r[intfId][routerId].dnBw = cdn.dnBw;
	IPGlobal::CDN_MBPS_i_r[intfId][routerId].totalVol = cdn.totalVol;
	IPGlobal::CDN_MBPS_i_r[intfId][routerId].upTotalVol = cdn.upTotalVol;
	IPGlobal::CDN_MBPS_i_r[intfId][routerId].dnTotalVol = cdn.dnTotalVol;
	IPGlobal::CDN_MBPS_i_r[intfId][routerId].peakTotalVol 	= cdn.peakTotalVol;
	IPGlobal::CDN_MBPS_i_r[intfId][routerId].peakUpTotalVol = cdn.peakUpTotalVol;
	IPGlobal::CDN_MBPS_i_r[intfId][routerId].peakDnTotalVol = cdn.peakDnTotalVol;

	IPGlobal::CDN_MBPS_i_r[intfId][routerId].avgTotalBw= cdn.avgTotalBw;
	IPGlobal::CDN_MBPS_i_r[intfId][routerId].avgUpBw 	= cdn.avgUpBw;
	IPGlobal::CDN_MBPS_i_r[intfId][routerId].avgDnBw 	= cdn.avgDnBw;

}

cdnData CDNData::calculateCdnData(cdnData (&cdn)[CDN_TIME_INDEX])
{
	cdnData cdndata;
	cdndata.Bw 	= 0;
	cdndata.upBw = 0;
	cdndata.dnBw = 0;

	cdndata.totalVol 	= 0;
	cdndata.upTotalVol 	= 0;
	cdndata.dnTotalVol 	= 0;
	cdndata.peakTotalVol = 0;
	cdndata.peakUpTotalVol = 0;
	cdndata.peakDnTotalVol = 0;
	cdndata.avgTotalBw 	= 0;
	cdndata.avgUpBw 		= 0;
	cdndata.avgDnBw 		= 0;

	uint32_t samplesTotal 	= 0;
	uint32_t samplesUp 		= 0;
	uint32_t samplesDn 		= 0;

	for(uint16_t i = 0; i < CDN_TIME_INDEX; i++)
	{
		if(cdn[i].totalVol > 0)
			samplesTotal++;

		cdndata.totalVol += cdn[i].totalVol;

		if(cdn[i].totalVol > cdndata.peakTotalVol)
			cdndata.peakTotalVol = cdn[i].totalVol;
		cdn[i].totalVol = 0;

		if(cdn[i].upTotalVol > 0)
			samplesUp++;

		cdndata.upTotalVol += cdn[i].upTotalVol;

		if(cdn[i].upTotalVol > cdndata.peakUpTotalVol)
			cdndata.peakUpTotalVol = cdn[i].upTotalVol;
		cdn[i].upTotalVol = 0;

		if(cdn[i].dnTotalVol > 0)
			samplesDn++;

		cdndata.dnTotalVol += cdn[i].dnTotalVol;

		if(cdn[i].dnTotalVol > cdndata.peakDnTotalVol)
			cdndata.peakDnTotalVol = cdn[i].dnTotalVol;
		cdn[i].dnTotalVol = 0;
	}

	if(samplesTotal > 0)
	{ cdndata.avgTotalBw 	= (cdndata.totalVol * 8) / samplesTotal; }

	if(samplesUp > 0)
	{ cdndata.avgUpBw 		= (cdndata.upTotalVol * 8) / samplesUp; }
	if(samplesDn > 0)
	{ cdndata.avgDnBw 		= (cdndata.dnTotalVol * 8) / samplesDn; }

	return cdndata;
}
