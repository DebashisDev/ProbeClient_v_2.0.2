/*
 * InterfaceMonitor.cpp
 *
 *  Created on: Feb 1, 2017
 *      Author: Deb
 */

#include "InterfaceMonitor.h"

InterfaceMonitor::InterfaceMonitor(uint16_t intfid, interfaceThread *t)
{
	this->intfId = intfid;
	this->thread = t;
	this->_name = "InterfaceMonitor";
	this->setLogLevel(Log::theLog().level());
}

InterfaceMonitor::~InterfaceMonitor()
{ }

void InterfaceMonitor::run()
{
	uint64_t now_bytes, prev_bytes;
	struct timeval start, end;
	struct tm *now_tm;
	uint64_t prev_pkts, now_pkts;
	int ms, pkt_rate, mbps;
	long ppsMax = 0;

	prev_pkts = thread->n_rx_pkts;
	prev_bytes = thread->n_rx_bytes;
	gettimeofday(&start, NULL);

	now_tm = localtime(&start.tv_sec);
	int nowMin = 0, prevMin = 0;

	nowMin = prevMin = now_tm->tm_min;

	for(int i=0; i<60; i++)
		ppsArray[i] = 0;

	while(IPGlobal::PKT_LISTENER_INTF_MON_RUNNING_STATUS[intfId])
	{
		sleep(1);
		gettimeofday(&end, NULL);

		now_tm = localtime(&end.tv_sec);
		nowMin = now_tm->tm_min;

		now_pkts = thread->n_rx_pkts;
		now_bytes = thread->n_rx_bytes;

		if(now_pkts < prev_pkts)
			prev_pkts = now_pkts;
		if(now_bytes < prev_bytes)
			prev_bytes = now_bytes;

		ms = (end.tv_sec - start.tv_sec) * 1000;
		ms += (end.tv_usec - start.tv_usec) / 1000;
		pkt_rate = (int) ((int64_t) (now_pkts - prev_pkts) * 1000 / ms);
		mbps = (int) ((now_bytes - prev_bytes) * 8 / 1000 / ms);

		ppsArray[now_tm->tm_sec] = pkt_rate;
		if(nowMin != prevMin)
		{
			ppsMax = 0;
			prevMin = nowMin;
			for(int i=0; i<60; i++)
			{
				if(ppsArray[i] > ppsMax)
					ppsMax = ppsArray[i];
				ppsArray[i] = 0;
			}
		}

		IPGlobal::PKTS_TOTAL_INTF[intfId] = now_pkts;
		IPGlobal::PKT_RATE_INTF[intfId] = pkt_rate;
		IPGlobal::BW_MBPS_INTF[intfId] = mbps;

		prev_pkts = now_pkts;
		prev_bytes = now_bytes;
		start = end;
	}
	printf("InterfaceMonitor [%02d] shutdown complete.\n\n", intfId);
}
