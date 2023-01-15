/*
 * ProbeStats.cpp
 *
 *  Created on: Feb 1, 2017
 *      Author: Debashis
 */

#include <ctime>
#include "ProbeStats.h"

ProbeStats::ProbeStats()
{ }

ProbeStats::~ProbeStats()
{ }

void ProbeStats::run()
{
	char buffer[80];
	uint16_t printloopCnt 	= 0;

	long startTime 		= 0;
	long runTime 		= 0;

	int dd = 0, hh = 0, mm = 0, ss = 0;

	gettimeofday(&curTime, NULL);
	startTime = curTime.tv_sec;

	bool isStatsInitialized = false;

	while(IPGlobal::PROBE_STATS_RUNNING_STATUS)
	{
		sleep(1);

		if(IPGlobal::PRINT_STATS_FREQ_SEC > 0 && IPGlobal::PRINT_STATS)
		{
			printloopCnt++;

			gettimeofday(&curTime, NULL);
			now_tm = localtime(&curTime.tv_sec);
			runTime = curTime.tv_sec - startTime;

			dd = (int)(runTime / 84600);
			hh = (int)((runTime - (dd * 84600)) / 3600);
			mm = (int)((runTime - ((dd * 84600) + (hh * 3600))) / 60);
			ss = (int)(runTime - ((dd * 84600) + (hh * 3600) + (mm * 60)));

			sprintf(buffer, "%03d:%02d:%02d",dd,hh,mm);

			if(printloopCnt >= IPGlobal::PRINT_STATS_FREQ_SEC)
			{
				printloopCnt = 0;
				printInterfaceStats(buffer);
				printf("\n\n");
			}
		}
	}
	printf("  ProbeStats Stopped...\n");
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}


void ProbeStats::printInterfaceStats(char *runTime)
{
	uint16_t tIdx = 0, intf = 0, router = 0;

	printf("\n   %s   [%02d:%02d]         PPS       BW                        T0       T1       T2       T3       T4       T5       T6       T7       T8       T9\n", runTime, now_tm->tm_hour,now_tm->tm_min);

	  for(intf = 0; intf < IPGlobal::NO_OF_INTERFACES; intf ++)
	  {
		  printf("         Interface [%6s]   %08d  %06d             ", IPGlobal::PNAME[intf].c_str(), IPGlobal::PKT_RATE_INTF[intf], IPGlobal::BW_MBPS_INTF[intf]);
		  printf("   ");

		  for(router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
		  {
			for(tIdx = 0; tIdx < 10; tIdx++)
				printf("  %07d", PKTStore::pktRepoCnt[intf][router][tIdx]);

			printf("\n");
			printf("                                                              ");
		  }
		  printf("\n");
		  printAgentStats(intf);
	  }

}

void ProbeStats::printAgentStats(uint16_t intf)
{
	uint16_t router = 0, tIdx = 0;
	uint32_t t_cnt[10] = {0};

//	switch(IPGlobal::MAX_TCP_AGENT)
//	{
//		case 1:
//			printf("                                          TcpAgent [%6s] ->", IPGlobal::PNAME[intf].c_str());
//
//			for (tIdx = 0; tIdx < 10; tIdx++)
//			{
//				for(router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
//					t_cnt[tIdx] += tcpAgent::cnt[intf][router][0][tIdx];
//
//				printf("  %07u",	t_cnt[tIdx]);
//
//				t_cnt[tIdx] = 0;
//			}
//			printf("\n");
//
//			t_cnt[10] = {0};
//
//			printf("                                          UdpAgent [%6s] ->", IPGlobal::PNAME[intf].c_str());
//
//			for (tIdx = 0; tIdx < 10; tIdx++)
//			{
//				for(router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
//					t_cnt[tIdx] += udpAgent::cnt[intf][router][0][tIdx];
//
//				printf("  %07u",	t_cnt[tIdx]);
//
//				t_cnt[tIdx] = 0;
//			}
//			printf("\n");
//
//			t_cnt[10] = {0};
//
//			printf("                                          DnsAgent [%6s] ->", IPGlobal::PNAME[intf].c_str());
//
//			for (tIdx = 0; tIdx < 10; tIdx++)
//			{
//				for(router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
//					t_cnt[tIdx] += dnsAgent::cnt[intf][router][tIdx];
//
//				printf("  %07u",	t_cnt[tIdx]);
//
//				t_cnt[tIdx] = 0;
//			}
//			printf("\n");
//
//			t_cnt[10] = {0};
//
//			printf("                                          AAAAgent [%6s] ->", IPGlobal::PNAME[intf].c_str());
//
//			for (tIdx = 0; tIdx < 10; tIdx++)
//			{
//				for(router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
//					t_cnt[tIdx] += aaaAgent::cnt[intf][router][tIdx];
//
//				printf("  %07u",	t_cnt[tIdx]);
//
//				t_cnt[tIdx] = 0;
//			}
//			break;
//
//		case 2:
			printf("                                          TcpAgent [%6s] ->", IPGlobal::PNAME[intf].c_str());

			for (tIdx = 0; tIdx < 10; tIdx++)
			{
				for(router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
					t_cnt[tIdx] += tcpAgent::cnt[intf][router][tIdx];

				printf("  %07u",	t_cnt[tIdx]);

				t_cnt[tIdx] = 0;
			}
			printf("\n");

			t_cnt[10] = {0};

			printf("                                          UdpAgent [%6s] ->", IPGlobal::PNAME[intf].c_str());

			for (tIdx = 0; tIdx < 10; tIdx++)
			{
				for(router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
					t_cnt[tIdx] += udpAgent::cnt[intf][router][tIdx];

				printf("  %07u",	t_cnt[tIdx]);

				t_cnt[tIdx] = 0;
			}
			printf("\n");

			t_cnt[10] = {0};

			printf("                                          DnsAgent [%6s] ->", IPGlobal::PNAME[intf].c_str());

			for (tIdx = 0; tIdx < 10; tIdx++)
			{
				for(router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
					t_cnt[tIdx] += dnsAgent::cnt[intf][router][tIdx];

				printf("  %07u",	t_cnt[tIdx]);

				t_cnt[tIdx] = 0;
			}
			printf("\n");

			t_cnt[10] = {0};

			printf("                                          AAAAgent [%6s] ->", IPGlobal::PNAME[intf].c_str());

			for (tIdx = 0; tIdx < 10; tIdx++)
			{
				for(router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
					t_cnt[tIdx] += aaaAgent::cnt[intf][router][tIdx];

				printf("  %07u",	t_cnt[tIdx]);

				t_cnt[tIdx] = 0;
			}
//			break;
//	}
	printf("\n\n");
}
