/*
 * PeeringData.cpp
 *
 *  Created on: 13-Dec-2021
 *      Author: Debashis
 */

#include "PeeringData.h"

PeeringData::PeeringData(uint16_t intfid, uint16_t rid)
{
	this->_name = "PeeringData";
	this->setLogLevel(Log::theLog().level());

	this->intfId 		= intfid;
	this->routerId 		= rid;
	this->volume		= 0;
	this->dir			= 0;
	this->curSec		= 0;

	printf("** PeeringData:: Initialized for Interface [%d]\n", this->intfId);
}

PeeringData::~PeeringData()
{ }

void PeeringData::updatePeeringData(uint16_t curMin, uint16_t frSize, uint8_t peeringDir, uint64_t pckCurSec)
{
	volume = frSize;
	dir = peeringDir;
	curSec = pckCurSec % 100;

	uint16_t t_index = curMin % 2;

	processPeeringData(peering_i_r_t[intfId][routerId][t_index]);
}

void PeeringData::processPeeringData(peeringData (&peering)[PEERING_TIME_INDEX])
{
	peering[curSec].totalVol += volume;
	if(dir == UP)
		peering[curSec].upTotalVol += volume;
	else if(dir == DOWN)
		peering[curSec].dnTotalVol += volume;
}

void PeeringData::setPeeringData(uint16_t curMin)
{
	uint16_t t_index = curMin % 2;
	if(t_index == 0) t_index = 1;
	else if(t_index == 1) t_index = 0;

	peeringData peering;

	peering = calculatePeeringData(peering_i_r_t[intfId][routerId][t_index]);

	IPGlobal::PEERING_MBPS_i_r[intfId][routerId].Bw 			= peering.Bw;
	IPGlobal::PEERING_MBPS_i_r[intfId][routerId].upBw 			= peering.upBw;
	IPGlobal::PEERING_MBPS_i_r[intfId][routerId].dnBw 			= peering.dnBw;
	IPGlobal::PEERING_MBPS_i_r[intfId][routerId].totalVol 		= peering.totalVol;
	IPGlobal::PEERING_MBPS_i_r[intfId][routerId].upTotalVol 	= peering.upTotalVol;
	IPGlobal::PEERING_MBPS_i_r[intfId][routerId].dnTotalVol 	= peering.dnTotalVol;
	IPGlobal::PEERING_MBPS_i_r[intfId][routerId].peakTotalVol 	= peering.peakTotalVol;
	IPGlobal::PEERING_MBPS_i_r[intfId][routerId].peakUpTotalVol = peering.peakUpTotalVol;
	IPGlobal::PEERING_MBPS_i_r[intfId][routerId].peakDnTotalVol = peering.peakDnTotalVol;

	IPGlobal::PEERING_MBPS_i_r[intfId][routerId].avgTotalBw		= peering.avgTotalBw;
	IPGlobal::PEERING_MBPS_i_r[intfId][routerId].avgUpBw 		= peering.avgUpBw;
	IPGlobal::PEERING_MBPS_i_r[intfId][routerId].avgDnBw 		= peering.avgDnBw;
}

peeringData PeeringData::calculatePeeringData(peeringData (&peering)[PEERING_TIME_INDEX])
{
	peeringData peeringdata;
	uint32_t samplesTotal 	= 0;
	uint32_t samplesUp 		= 0;
	uint32_t samplesDn 		= 0;

	peeringdata.Bw 	= 0;
	peeringdata.upBw = 0;
	peeringdata.dnBw = 0;

	peeringdata.totalVol 	= 0;
	peeringdata.upTotalVol 	= 0;
	peeringdata.dnTotalVol 	= 0;
	peeringdata.peakTotalVol = 0;
	peeringdata.peakUpTotalVol = 0;
	peeringdata.peakDnTotalVol = 0;
	peeringdata.avgTotalBw 	= 0;
	peeringdata.avgUpBw 		= 0;
	peeringdata.avgDnBw 		= 0;

	for(uint16_t i = 0; i < PEERING_TIME_INDEX; i++)
	{
		if(peering[i].totalVol > 0)
			samplesTotal++;

		peeringdata.totalVol += peering[i].totalVol;

		if(peering[i].totalVol > peeringdata.peakTotalVol)
			peeringdata.peakTotalVol = peering[i].totalVol;
		peering[i].totalVol = 0;

		if(peering[i].upTotalVol > 0)
			samplesUp++;

		peeringdata.upTotalVol += peering[i].upTotalVol;

		if(peering[i].upTotalVol > peeringdata.peakUpTotalVol)
			peeringdata.peakUpTotalVol = peering[i].upTotalVol;
		peering[i].upTotalVol = 0;

		if(peering[i].dnTotalVol > 0)
			samplesDn++;

		peeringdata.dnTotalVol += peering[i].dnTotalVol;

		if(peering[i].dnTotalVol > peeringdata.peakDnTotalVol)
			peeringdata.peakDnTotalVol = peering[i].dnTotalVol;
		peering[i].dnTotalVol = 0;
	}

	if(samplesTotal > 0)
	{ peeringdata.avgTotalBw 	= (peeringdata.totalVol * 8) / samplesTotal; }

	if(samplesUp > 0)
	{ peeringdata.avgUpBw 		= (peeringdata.upTotalVol * 8) / samplesUp; }
	if(samplesDn > 0)
	{ peeringdata.avgDnBw 		= (peeringdata.dnTotalVol * 8) / samplesDn; }

	return peeringdata;}
