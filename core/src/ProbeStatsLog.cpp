/*
 * ProbeStatsLog.cpp
 *
 *  Created on: Jul 21, 2017
 *      Author: Debashis
 */

#include <ctime>
#include "ProbeStatsLog.h"

ProbeStatsLog::ProbeStatsLog()
{
	this->_name = "ProbeStatsLog";
	this->setLogLevel(Log::theLog().level());

	this->nicCounter = 0;
	this->solCounter = 0;
	this->interfaceCounter = 0;
	this->intfName[MAX_INTERFACE_SUPPORT];
}

ProbeStatsLog::~ProbeStatsLog()
{}

void ProbeStatsLog::run()
{
	int		printloopCnt = 0, dd = 0, hh = 0, mm = 0, ss = 0, checkUdpCongestion = 0;
	long 	startTime = 0, runTime = 0, currentHH = 0;

	char buffer[80];
	uint16_t interfaceId = 0, routeCounter = 0, tIdx = 0;

	struct tm *now_tm;

	gettimeofday(&curTime, NULL);
	startTime = curTime.tv_sec;

	for(nicCounter = 0; nicCounter < IPGlobal::NO_OF_NIC_INTERFACE; nicCounter++)
		intfName[nicCounter] = IPGlobal::ETHERNET_INTERFACES[nicCounter];

	interfaceCounter = nicCounter;

	for(solCounter = 0; solCounter < IPGlobal::NO_OF_SOLAR_INTERFACE; solCounter++, interfaceCounter++)
		intfName[interfaceCounter] = IPGlobal::SOLAR_INTERFACES[solCounter];

	while(IPGlobal::PROBE_STATS_RUNNING_STATUS)
	{
		  sleep(1);
		  printloopCnt++;

		  gettimeofday(&curTime, NULL);
		  runTime = curTime.tv_sec - startTime;

		  dd = (int)(runTime / 84600);
		  hh = (int)((runTime - (dd * 84600)) / 3600);
		  mm = (int)((runTime - ((dd * 84600) + (hh * 3600))) / 60);
		  ss = (int)(runTime - ((dd * 84600) + (hh * 3600) + (mm * 60)));
		  sprintf(buffer, "%03d:%03d:%03d:%03d", dd, hh, mm, ss);

		  if(printloopCnt >= IPGlobal::LOG_STATS_FREQ_SEC)
		  {
			  printloopCnt = 0;
			  printInterfaceStats(buffer);
		  }
	}

	printf("  Log Process Stopped...\n");
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void ProbeStatsLog::printInterfaceStats(char *runTime)
{
	char buffer[500];
	uint16_t intf = 0, router = 0, rCount = 0, tIdx = 0;

	TheLog_nc_v1(Log::Info, name(),"", "");

	for(intf = 0; intf < IPGlobal::NO_OF_INTERFACES; intf++)
	{
		TheLog_nc_v5(Log::Info, name(),"   Interface   [%6s] [%s] %08d PPS  %06d Mbps | Packet Rejected %011lu",
				IPGlobal::PNAME[intf].c_str(), runTime, IPGlobal::PKT_RATE_INTF[intf], IPGlobal::BW_MBPS_INTF[intf], IPGlobal::DISCARD_PKT_CNT[intf]);

		buffer[0] = 0;

		for(router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
		{
			rCount = 0;

			for(tIdx = 0; tIdx < 10; tIdx++)
				rCount += PKTStore::pktRepoCnt[intf][router][tIdx];

			if(router == 0)
				sprintf(buffer, "%07d", rCount);
			else
				sprintf(buffer, "%s  %07d",buffer, rCount);
		 }
		TheLog_nc_v2(Log::Info, name(), "   Interface(R)[%6s] %s", IPGlobal::PNAME[intf].c_str(), buffer);
	 }
}

void ProbeStatsLog::packetProcessing(uint16_t interfaceId, bool flag)
{
	switch(flag)
	{
		case true:
				IPGlobal::PUSH_TO_QUEUE[interfaceId] = true;
			break;

		case false:
				IPGlobal::PUSH_TO_QUEUE[interfaceId] = false;
			break;
	}
}
