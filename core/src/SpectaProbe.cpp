/*
 * SpectaProbe.cpp
 *
 *  Created on: 29-Jan-2016
 *      Author: Debashis
 */

#include <signal.h>
#include <unistd.h>
#include <locale.h>
#include <zmq.h>

#include "SpectaProbe.h"

void *startTimerThread(void *arg)
{
	glbTimer *ft = (glbTimer *)arg;
	ft->run();
	return NULL;
}

void* startTcpAgentThread(void *arg)
{
	TcpAgent *ft = (TcpAgent*)arg;
	ft->run();
	return NULL;
}

void* startUdpAgentThread(void *arg)
{
	UdpAgent *ft = (UdpAgent*)arg;
	ft->run();
	return NULL;
}

void* startDnsAgentThread(void *arg)
{
	DnsAgent *ft = (DnsAgent*)arg;
	ft->run();
	return NULL;
}

void* startAaaAgentThread(void *arg)
{
	AaaAgent *ft = (AaaAgent*)arg;
	ft->run();
	return NULL;
}

void* startPktRouterThread(void* arg)
{
	PacketRouter *ft = (PacketRouter*)arg;
	ft->run();
	return NULL;
}

void* packetListenerThread(void* arg)
{
	PacketListener *ft = (PacketListener*)arg;
	ft->start();
	return NULL;
}

void* ethListenerThread(void* arg)
{
	EthernetSource *ft = (EthernetSource*)arg;
	ft->start();
	return NULL;
}

void* probeStatsThread(void* arg)
{
	ProbeStats *ft = (ProbeStats*)arg;
	ft->run();
	return NULL;
}

void* probeBwThread(void* arg)
{
	processBw *ft = (processBw*)arg;
	ft->run();
	return NULL;
}

void* probeStatsLogThread(void* arg)
{
	ProbeStatsLog *ft = (ProbeStatsLog*)arg;
	ft->run();
	return NULL;
}

void* adminPortListenerThread(void* arg)
{
	AdminPortReader *ft = (AdminPortReader*)arg;
	ft->run();
	return NULL;
}

SpectaProbe::SpectaProbe(char *fileName) {
	this->_name = "SpectaProbe    ";

	pInit 		= new Initialize();
	pGConfig 	= new GConfig(fileName);
	pUtility 	= new ProbeUtility();
	initializeLocks();
	currentMin = prevMin = 0;

	this->setLogLevel(Log::theLog().level());
}

SpectaProbe::~SpectaProbe()
{
	delete(pInit);
	delete(pGConfig);
	delete(pUtility);
}

void SpectaProbe::initializeLocks()
{
	mapDnsLock::count 	= 1;
}

void SpectaProbe::initializeLog()
{
	char logFile[200];

	logFile[0] = 0;
	sprintf(logFile, "%s%s_%d-%02d-%02d-%d.log", IPGlobal::LOG_DIR.c_str(), "client", IPGlobal::PROBE_ID, IPGlobal::CURRENT_DAY, IPGlobal::CURRENT_MONTH, IPGlobal::CURRENT_YEAR);

	Log::theLog().open(logFile);
	Log::theLog().level(IPGlobal::LOG_LEVEL);

	char *probeVer = getenv("PROBE_VER");

	/*
	 * 		2.0.0		11-06-2021		First release
	 * 		2.0.1		17-11-2021		One Protocol Agent / Interface.
	 * 		2.0.2		19-07-2022		BW thread separated from Main thread
	 *
	 */
	printf(" ############################################################\n");
	printf("                                                             \n");
	printf("              Starting SPECTA [%s] Probe Ver : %s            \n", "FixedLine", probeVer);
	printf("                                                             \n");
	printf(" ############################################################\n");


	TheLog_nc_v1(Log::Info, name(),"  ############################################################%s","");
	TheLog_nc_v1(Log::Info, name(),"                                                              %s","");
	TheLog_nc_v2(Log::Info, name(),"                     Starting SPECTA Client [%s] Ver : %s        ", "FixedLine", probeVer);
	TheLog_nc_v1(Log::Info, name(),"                                                              %s","");
	TheLog_nc_v1(Log::Info, name(),"  ############################################################%s","");
	TheLog_nc_v1(Log::Info, name(),"  Log file initialized Level - %d", IPGlobal::LOG_LEVEL);
}

void SpectaProbe::start()
{
	uint16_t today = 0, lastday = 0, dnsDumpLoop = 0;
	uint16_t logPrintLoop = 0, userIpReadLoop = 0;

	uint16_t 	intfId, routerCnt;
	uint64_t 	tcpCount, udpCount, aaaCount;
	char 		logFile[200];

	IPGlobal::NO_OF_INTERFACES = IPGlobal::NO_OF_SOLAR_INTERFACE + IPGlobal::NO_OF_NIC_INTERFACE;

	initializePktRepo();
	initializeAgentMaps();

	createTimer(1); 					/* Start Timer Thread */

	sleep(2);
	initializeLog();                    /* Initialize Log File */

	printf("  *** [%02d] Packet Processing Paused. \n", 2);
	TheLog_nc_v1(Log::Info, name(),"  *** [%02d] Packet Processing Paused. ", 2);
	packetProcessing(false);			/* Pause the incoming Traffic */

	createRoutersPerInterface(3);		/* Start Router / Interface Threads ---- */

	initializeNICs(4);					/* Start NIC Listener Threads ---- */

	/* [04] ---- Start Admin Threads ---- */
	createAdmin(5);

	/* [05] ---- Start Probe Log Threads ---- */
	printStats(6);

	/* [06] ---- Start Probe Statistic Threads ---- */
	writeStats(7);

	sleep(10); /* Start Processing the data after 10 seconds */

	createBwThread(8);

	printf("SpectaProbe Started Successfully.\n");
	TheLog_nc_v1(Log::Info, name(),"  SpectaProbe Started Successfully. %s","");

	pushToQueue(true);	/* Push To Queue */

	packetProcessing(true);			/* Resume the incoming Traffic */

	intfId = routerCnt = tcpCount = udpCount = aaaCount = 0;

	currentMin = prevMin = IPGlobal::CURRENT_MIN;
	lastday = today 	 =  IPGlobal::CURRENT_DAY;

	while(IPGlobal::PROBE_RUNNING_STATUS)
	{
		sleep(1);
		currentMin = IPGlobal::CURRENT_MIN;

		if(currentMin != prevMin)
		{
//			/* Bandwidth Processing */
//
//			buildBwCSV(IPGlobal::CURRENT_EPOCH_MILI_SEC);
//
//			if(IPGlobal::PROCESS_CDN)
//			{
//				openCDNCsvXdrFile(IPGlobal::CURRENT_MIN, IPGlobal::CURRENT_HOUR, IPGlobal::CURRENT_DAY, IPGlobal::CURRENT_MONTH, IPGlobal::CURRENT_YEAR);
//				writeCDNXdr(bwXdr, cachedXdr, unCachedXdr, peeringXdr);
//				closeCDNCsvXdrFile();
//			}
//			else
//			{
//				openBwCsvXdrFile(IPGlobal::CURRENT_MIN, IPGlobal::CURRENT_HOUR, IPGlobal::CURRENT_DAY, IPGlobal::CURRENT_MONTH, IPGlobal::CURRENT_YEAR);
//				writeBwXdr(bwXdr);
//				closeBwCsvXdrFile();
//			}

			/*
			 * Dumping DNS Resolved IP in Minutes
			 */
			dnsDumpLoop ++;

			if(dnsDumpLoop >= IPGlobal::DNS_DUMP_TIME)
			{
				dnsDumpLoop = 0;
				pUtility->dnsDumpIpv4Data(IPGlobal::DATA_DIR);
				pUtility->dnsDumpIpv6Data(IPGlobal::DATA_DIR);
			}

			userIpReadLoop ++;  //To read Ipv4 range config file

			if(userIpReadLoop >= IPGlobal::USER_IPV4_READ_FREQ_MIN && IPGlobal::USER_IPV4_DIR_STATUS)
			{
				userIpReadLoop = 0;
				pGConfig->readUserIpv4Config();
			}
			prevMin = currentMin;
		}

		/*
		 * Accumulating no. of Packets processed
		 */

		logPrintLoop ++;

		if(logPrintLoop >= IPGlobal::LOG_STATS_FREQ_SEC)
		{
			logPrintLoop = 0;

			for(intfId = 0; intfId < IPGlobal::NO_OF_INTERFACES; intfId++)
			{
				for(routerCnt = 0; routerCnt < IPGlobal::ROUTER_PER_INTERFACE[intfId]; routerCnt++)
				{
					tcpCount += IPGlobal::TCP_PACKETS_PER_DAY[intfId][routerCnt];
					udpCount += IPGlobal::UDP_PACKETS_PER_DAY[intfId][routerCnt];
					aaaCount += IPGlobal::AAA_PACKETS_PER_DAY[intfId][routerCnt];
				}
			}

			printf("  TCP|%llu UDP|%llu AAA|%llu\n", tcpCount, udpCount, aaaCount);
			TheLog_nc_v3(Log::Info, name(),"  Packet Count TCP|%llu UDP|%llu AAA|%llu", tcpCount, udpCount, aaaCount);
			tcpCount = udpCount = aaaCount = 0;
		}

		today = IPGlobal::CURRENT_DAY;

		if(lastday != today)
		{
		//	TheLog_nc_v2(Log::Info, name(),"  Last Day = %d| Today = %d", lastday, today);
			lastday = today;
			TheLog_nc_v1(Log::Info, name(),"  Day Changed .... !!! Initializing Counters....%s", "");

			tcpCount = udpCount = aaaCount = 0;

			/*
			 * Initializing counters of Packet Processing
			 */
			TheLog_nc_v1(Log::Info, name(),"  Initializing Packet Processing Counters.%s", "");

			for(intfId = 0; intfId < IPGlobal::NO_OF_INTERFACES; intfId ++)
			{
				for(routerCnt = 0; routerCnt < IPGlobal::ROUTER_PER_INTERFACE[intfId]; routerCnt++)
				{
					IPGlobal::TCP_PACKETS_PER_DAY[intfId][routerCnt] = 0;
					IPGlobal::UDP_PACKETS_PER_DAY[intfId][routerCnt] = 0;
					IPGlobal::AAA_PACKETS_PER_DAY[intfId][routerCnt] = 0;
				}
			}

			TheLog_nc_v1(Log::Info, name(),"  Closing the Log file and creating new log file.%s", "");

			Log::theLog().close();

			logFile[0] = 0;
			sprintf(logFile, "%s%s_%d-%02d-%02d-%d.log", IPGlobal::LOG_DIR.c_str(), "client", IPGlobal::PROBE_ID, IPGlobal::CURRENT_DAY, IPGlobal::CURRENT_MONTH, IPGlobal::CURRENT_YEAR);

			Log::theLog().open(logFile);
			Log::theLog().level(IPGlobal::LOG_LEVEL);

			for(int infCounter = 0; infCounter < IPGlobal::NO_OF_INTERFACES; infCounter++)
				IPGlobal::PUSH_TO_QUEUE[infCounter] = false;

			TheLog_nc_v3(Log::Info, name(),"  Time[%02d:%02d:%02d] Stop Pushing Packet..", IPGlobal::CURRENT_HOUR, IPGlobal::CURRENT_MIN, IPGlobal::CURRENT_SEC);

			sleep(120);

			TheLog_nc_v3(Log::Info, name(),"  Time[%02d:%02d:%02d] Resume Pushing Packet..", IPGlobal::CURRENT_HOUR, IPGlobal::CURRENT_MIN, IPGlobal::CURRENT_SEC);

			for(uint16_t infCounter = 0; infCounter < IPGlobal::NO_OF_INTERFACES; infCounter++)
			{
				IPGlobal::PUSH_TO_QUEUE[infCounter] = true;
				sleep(30);
			}
		}
	}
	printf("\n  SpectaProbe Shutdown Complete...\n");
	exit(0);
}

void SpectaProbe::createTimer(uint16_t no)
{
	pGlbTimer = new glbTimer;

	IPGlobal::TIMER_PROCESSING = true;

	pthread_create(&glbTimerThrId, NULL, startTimerThread, pGlbTimer);
	pinThread(glbTimerThrId, IPGlobal::TIMER_CORE);

	while(!pGlbTimer->isGlbTimerInitialized())
		sleep(1);

	printf("  *** [%02d] Timer Thread Started Successfully. Pinned to CPU Core [%02d]\n", no, IPGlobal::TIMER_CORE);
	TheLog_nc_v2(Log::Info, name(),"  *** [%02d] Timer Thread Started Successfully. Pinned to CPU Core [%02d]", no, IPGlobal::TIMER_CORE);
}

void SpectaProbe::packetProcessing(bool flag)
{
	switch(flag)
	{
		case true:
			for(uint16_t infCounter = 0; infCounter < IPGlobal::NO_OF_INTERFACES; infCounter++)
			{
				IPGlobal::PACKET_PROCESSING[infCounter] = true;
				sleep(60);
			}
			break;

		case false:
			for(uint16_t infCounter = 0; infCounter < IPGlobal::NO_OF_INTERFACES; infCounter++)
				IPGlobal::PACKET_PROCESSING[infCounter] = false;

			break;
	}
}

void SpectaProbe::pushToQueue(bool flag)
{
	switch(flag)
	{
		case true:
			for(uint16_t infCounter = 0; infCounter < IPGlobal::NO_OF_INTERFACES; infCounter++)
				IPGlobal::PUSH_TO_QUEUE[infCounter] = true;
			break;
	}
}

void SpectaProbe::startTcpAgent(uint16_t interfaceId, uint16_t no)
{
	IPGlobal::TCP_AGENT_RUNNING_STATUS[interfaceId] = true;
	pTcpAgent[interfaceId] = new TcpAgent(interfaceId);
	pthread_create(&thTcpAgent[interfaceId], NULL, startTcpAgentThread, pTcpAgent[interfaceId]);
	pinThread(thTcpAgent[interfaceId], IPGlobal::TCP_AGENT_CPU_CORE[interfaceId]);

	printf("       *** [%02d] startTcpAgent for Interface [%02d] Pinned to CPU Core [%02d]\n", no, interfaceId, IPGlobal::TCP_AGENT_CPU_CORE[interfaceId]);

	TheLog_nc_v3(Log::Info, name(),"       *** [%02d] startTcpAgent for Interface [%02d] Pinned to CPU Core [%02d]", no, interfaceId, IPGlobal::TCP_AGENT_CPU_CORE[interfaceId]);

	while(!pTcpAgent[interfaceId]->isRepositoryInitialized())
	sleep(1);
}

void SpectaProbe::startUdpAgent(uint16_t interfaceId, uint16_t no)
{
	IPGlobal::UDP_AGENT_RUNNING_STATUS[interfaceId] = true;
	pUdpAgent[interfaceId] = new UdpAgent(interfaceId);
	pthread_create(&thUdpAgent[interfaceId], NULL, startUdpAgentThread, pUdpAgent[interfaceId]);

	pinThread(thUdpAgent[interfaceId], IPGlobal::UDP_AGENT_CPU_CORE[interfaceId]);

	printf("       *** [%02d] startUdpAgent for Interface [%02d] Pinned to CPU Core [%02d]\n", no, interfaceId, IPGlobal::UDP_AGENT_CPU_CORE[interfaceId]);

	TheLog_nc_v3(Log::Info, name(),"       *** [%02d] startUdpAgent for Interface [%02d] Pinned to CPU Core [%02d]", no, interfaceId, IPGlobal::UDP_AGENT_CPU_CORE[interfaceId]);

	while(!pUdpAgent[interfaceId]->isRepositoryInitialized())
		sleep(1);
}

void SpectaProbe::startAaaAgent(uint16_t interfaceId, uint16_t no)
{
	IPGlobal::AAA_AGENT_RUNNING_STATUS[interfaceId] = true;

	pAaaAgent[interfaceId] = new AaaAgent(interfaceId);
	pthread_create(&thAaaAgent[interfaceId], NULL, startAaaAgentThread, pAaaAgent[interfaceId]);

	pinThread(thAaaAgent[interfaceId], IPGlobal::AAA_AGENT_CPU_CORE[interfaceId]);

	printf("       *** [%02d] startAaaAgent for Interface [%02d] Pinned to CPU Core [%02d]\n", no, interfaceId, IPGlobal::AAA_AGENT_CPU_CORE[interfaceId]);

	TheLog_nc_v3(Log::Info, name(),"        *** [%02d] startAaaAgent for Interface [%02d] Pinned to CPU Core [%02d]", no, interfaceId, IPGlobal::AAA_AGENT_CPU_CORE[interfaceId]);

	while(!pAaaAgent[interfaceId]->isInitialized())
		sleep(1);
}

void SpectaProbe::startDnsAgent(uint16_t interfaceId, uint16_t no)
{
	IPGlobal::DNS_AGENT_RUNNING_STATUS[interfaceId] = true;

	pDnsAgent[interfaceId] = new DnsAgent(interfaceId);
	pthread_create(&thDnsAgent[interfaceId], NULL, startDnsAgentThread, pDnsAgent[interfaceId]);

	pinThread(thDnsAgent[interfaceId], IPGlobal::DNS_AGENT_CPU_CORE[interfaceId]);

	printf("       *** [%02d] startDnsAgent for Interface [%02d] Pinned to CPU Core [%02d]\n", no, interfaceId, IPGlobal::DNS_AGENT_CPU_CORE[interfaceId]);

	TheLog_nc_v3(Log::Info, name(),"        *** [%02d] startDnsAgent for Interface [%02d] Pinned to CPU Core [%02d]", no, interfaceId, IPGlobal::DNS_AGENT_CPU_CORE[interfaceId]);

	while(!pDnsAgent[interfaceId]->isRepositoryInitialized())
		sleep(1);
}

void SpectaProbe::createRoutersPerInterface(uint16_t no)
{
	for(uint16_t interfaceId = 0; interfaceId < IPGlobal::NO_OF_INTERFACES; interfaceId++)
	{
		for(uint16_t routeCounter = 0; routeCounter < IPGlobal::ROUTER_PER_INTERFACE[interfaceId]; routeCounter++)
		{
			IPGlobal::PKT_ROUTER_RUNNING_STATUS[interfaceId][routeCounter] = true;

			pRouter[interfaceId][routeCounter] = new PacketRouter(interfaceId, routeCounter, IPGlobal::PKT_ROUTER_CPU_CORE[interfaceId][routeCounter]);
			pthread_create(&thPktRouter[interfaceId][routeCounter], NULL, startPktRouterThread, pRouter[interfaceId][routeCounter]);

			pinThread(thPktRouter[interfaceId][routeCounter], IPGlobal::PKT_ROUTER_CPU_CORE[interfaceId][routeCounter]);

			printf("  *** [%02d] PacketRouter [Interface]::[Router] [%02d]::[%02d] Allocated Core [%02d]\n", no, interfaceId, routeCounter, IPGlobal::PKT_ROUTER_CPU_CORE[interfaceId][routeCounter]);
			TheLog_nc_v4(Log::Info, name(),"  *** [%02d] PacketRouter Instance [%d][%d] Allocated Core [%d]", no, interfaceId, routeCounter, IPGlobal::PKT_ROUTER_CPU_CORE[interfaceId][routeCounter]);

			while(!pRouter[interfaceId][routeCounter]->isRepositoryInitialized())
				sleep(1);
		}

		/* Start Agent Threads / Interface */
		startTcpAgent(interfaceId, no);
		startUdpAgent(interfaceId, no);
		startAaaAgent(interfaceId, no);
		startDnsAgent(interfaceId, no);
	}
	printf("\n");
}

void SpectaProbe::initializeNICs(uint16_t no)
{
	nicCounter = solCounter = interfaceCounter = 0;
	caseNo = -1;

	if(IPGlobal::NO_OF_NIC_INTERFACE > 0 && IPGlobal::NO_OF_SOLAR_INTERFACE > 0)
		caseNo = 0; /* Both NIC and Solarflare */
	else if(IPGlobal::NO_OF_NIC_INTERFACE > 0 && IPGlobal::NO_OF_SOLAR_INTERFACE == 0)
		caseNo = 1; /* Only NIC */
	else if(IPGlobal::NO_OF_NIC_INTERFACE == 0 && IPGlobal::NO_OF_SOLAR_INTERFACE > 0)
		caseNo = 2; /* Only Solarflare */

	switch(caseNo)
	{
		case 0:		/* Both NIC and Solarflare */
		{
			for(nicCounter = 0; nicCounter < IPGlobal::NO_OF_NIC_INTERFACE; nicCounter++)
			{
				printf("  *** [%02d] Ethernet Interface [%02d]->[%s] with No of Routers [%02d] Allocated Core [%02d] \n",
						no, nicCounter, IPGlobal::ETHERNET_INTERFACES[nicCounter].c_str(), IPGlobal::ROUTER_PER_INTERFACE[nicCounter], IPGlobal::PKT_LISTENER_CPU_CORE[nicCounter]);

			IPGlobal::PNAME[nicCounter] = IPGlobal::ETHERNET_INTERFACES[nicCounter];

			IPGlobal::PKT_LISTENER_RUNNING_STATUS[nicCounter] = true;
			IPGlobal::PKT_LISTENER_DAYCHANGE_INDICATION[nicCounter] = false;

			ethReader[nicCounter] = new EthernetSource(IPGlobal::ROUTER_PER_INTERFACE[nicCounter], nicCounter);
			pthread_create(&pktLisThread[nicCounter], NULL, ethListenerThread, ethReader[nicCounter]);
			pinThread(pktLisThread[nicCounter], IPGlobal::PKT_LISTENER_CPU_CORE[nicCounter]);

			TheLog_nc_v5(Log::Info, name(),"  *** [%02d] Ethernet Interface [%02d]->[%s] with No of Routers [%02d] Allocated Core [%02d]",
							no, nicCounter, IPGlobal::ETHERNET_INTERFACES[nicCounter].c_str(), IPGlobal::ROUTER_PER_INTERFACE[nicCounter], IPGlobal::PKT_LISTENER_CPU_CORE[nicCounter]);

			while(!ethReader[nicCounter]->isRepositoryInitialized())
				sleep(1);
			}

			interfaceCounter = nicCounter;
			for(uint16_t solCounter = 0; solCounter < IPGlobal::NO_OF_SOLAR_INTERFACE; solCounter++, interfaceCounter++)
			{
				printf("  *** [%02d] Solarflare Interface [%02d]->[%s] with No of Routers [%02d] Allocated Core [%02d] \n",
						no, interfaceCounter, IPGlobal::SOLAR_INTERFACES[solCounter].c_str(), IPGlobal::ROUTER_PER_INTERFACE[interfaceCounter], IPGlobal::PKT_LISTENER_CPU_CORE[interfaceCounter]);

				IPGlobal::PNAME[interfaceCounter] = IPGlobal::SOLAR_INTERFACES[solCounter];

				IPGlobal::PKT_LISTENER_RUNNING_STATUS[interfaceCounter] = true;
				IPGlobal::PKT_LISTENER_DAYCHANGE_INDICATION[interfaceCounter] = false;

				sfReader[interfaceCounter] = new PacketListener(IPGlobal::ROUTER_PER_INTERFACE[interfaceCounter], solCounter, interfaceCounter);
				pthread_create(&pktLisThread[interfaceCounter], NULL, packetListenerThread, sfReader[interfaceCounter]);
				pinThread(pktLisThread[interfaceCounter], IPGlobal::PKT_LISTENER_CPU_CORE[interfaceCounter]);

				TheLog_nc_v5(Log::Info, name(),"  *** [%02d] Solarflare Interface [%02d]->[%s] with No of Routers [%02d] Allocated Core [%02d]",
						no, interfaceCounter, IPGlobal::SOLAR_INTERFACES[solCounter].c_str(), IPGlobal::ROUTER_PER_INTERFACE[interfaceCounter], IPGlobal::PKT_LISTENER_CPU_CORE[interfaceCounter]);

				while(!sfReader[interfaceCounter]->isRepositoryInitialized())
					sleep(1);
			}
		}
		break;

		case 1:		/* Only NIC */
		{
			for(nicCounter = 0; nicCounter < IPGlobal::NO_OF_INTERFACES; nicCounter++)
			{
				printf("  *** [%02d] Ethernet Interface [%02d]->[%s] with No of Routers [%02d] Allocated Core [%02d] \n",
						no, nicCounter, IPGlobal::ETHERNET_INTERFACES[nicCounter].c_str(), IPGlobal::ROUTER_PER_INTERFACE[nicCounter], IPGlobal::PKT_LISTENER_CPU_CORE[nicCounter]);

				IPGlobal::PNAME[nicCounter] = IPGlobal::ETHERNET_INTERFACES[nicCounter];

				IPGlobal::PKT_LISTENER_RUNNING_STATUS[nicCounter] = true;
				IPGlobal::PKT_LISTENER_DAYCHANGE_INDICATION[nicCounter] = false;
				ethReader[nicCounter] = new EthernetSource(IPGlobal::ROUTER_PER_INTERFACE[nicCounter], nicCounter);
				pthread_create(&pktLisThread[nicCounter], NULL, ethListenerThread, ethReader[nicCounter]);
				pinThread(pktLisThread[nicCounter], IPGlobal::PKT_LISTENER_CPU_CORE[nicCounter]);

				TheLog_nc_v5(Log::Info, name(),"  *** [%02d] Ethernet Interface [%02d]->[%s] with No of Routers [%02d] Allocated Core [%02d]",
								no, nicCounter, IPGlobal::ETHERNET_INTERFACES[nicCounter].c_str(), IPGlobal::ROUTER_PER_INTERFACE[nicCounter], IPGlobal::PKT_LISTENER_CPU_CORE[nicCounter]);

				while(!ethReader[nicCounter]->isRepositoryInitialized())
					sleep(1);
			}
		}
		break;

		case 2:		/* Only Solarflare */
		{
			for(nicCounter = 0; nicCounter < IPGlobal::NO_OF_INTERFACES; nicCounter++)
			{
				printf("  *** [%02d] Solarflare Interface [%02d]->[%s] with No of Routers [%02d] Allocated Core [%02d] \n",
						no, nicCounter, IPGlobal::SOLAR_INTERFACES[nicCounter].c_str(), IPGlobal::ROUTER_PER_INTERFACE[nicCounter], IPGlobal::PKT_LISTENER_CPU_CORE[nicCounter]);

				IPGlobal::PNAME[nicCounter] = IPGlobal::SOLAR_INTERFACES[nicCounter];

				IPGlobal::PKT_LISTENER_RUNNING_STATUS[nicCounter] = true;
				IPGlobal::PKT_LISTENER_DAYCHANGE_INDICATION[nicCounter] = false;
				sfReader[nicCounter] = new PacketListener(IPGlobal::ROUTER_PER_INTERFACE[nicCounter], nicCounter, nicCounter);
				pthread_create(&pktLisThread[nicCounter], NULL, packetListenerThread, sfReader[nicCounter]);
				pinThread(pktLisThread[nicCounter], IPGlobal::PKT_LISTENER_CPU_CORE[nicCounter]);

				TheLog_nc_v5(Log::Info, name(),"  *** [%02d] Solarflare Interface [%02d]->[%s] with No of Routers [%02d] Allocated Core [%02d]",
								no, nicCounter, IPGlobal::SOLAR_INTERFACES[nicCounter].c_str(), IPGlobal::ROUTER_PER_INTERFACE[nicCounter], IPGlobal::PKT_LISTENER_CPU_CORE[nicCounter]);

				while(!sfReader[nicCounter]->isRepositoryInitialized())
					sleep(1);
			}
		}
		break;
	}
}

void SpectaProbe::createAdmin(uint16_t no)
{
	if(IPGlobal::ADMIN_FLAG)
	{
		adminPort = new AdminPortReader();
		pthread_create(&adminPortThread, NULL, adminPortListenerThread, adminPort);
		sleep(1);
	}
	printf("  *** [%02d] Admin Thread Started Successfully. \n", no);
	TheLog_nc_v1(Log::Info, name(),"  *** [%02d] Admin Thread Started Successfully. ", no);
}

void SpectaProbe::printStats(uint16_t no)
{
	psLog = new ProbeStatsLog();
	pthread_create(&psLogThread, NULL, probeStatsLogThread, psLog);

	printf("  *** [%02d] Statistic Thread Started Successfully. \n", no);
	TheLog_nc_v1(Log::Info, name(),"  *** [%02d] Statistic Thread Started Successfully. ", no);
}

void SpectaProbe::writeStats(uint16_t no)
{
	if(IPGlobal::PRINT_STATS)
	{
		ps = new ProbeStats();
		pthread_create(&psThread, NULL, probeStatsThread, ps);
	}
	printf("  *** [%02d] Log Write Thread Started Successfully. \n", no);
	TheLog_nc_v1(Log::Info, name(),"  *** [%02d] Log Write Thread Started Successfully. ", no);
}

void SpectaProbe::createBwThread(uint16_t no)
{
	pBw = new processBw();
	pthread_create(&bwThread, NULL, probeBwThread, pBw);

	printf("  *** [%02d] BW Thread Started Successfully. \n", no);
	TheLog_nc_v1(Log::Info, name(),"  *** [%02d] BW Thread Started Successfully. ", no);
}


void SpectaProbe::pinThread(pthread_t th, uint16_t i)
{
   /* Set affinity mask to include CPUs 0 to 7 */
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(i,&cpuset);

	int s = pthread_setaffinity_np(th, sizeof(cpu_set_t), &cpuset);
	if (s != 0)
		handle_error_en(s, "ERROR!!! pthread_setaffinity_np");

	/* Check the actual affinity mask assigned to the thread */
	s = pthread_getaffinity_np(th, sizeof(cpu_set_t), &cpuset);
	if (s != 0)
		handle_error_en(s, "ERROR!!! pthread_getaffinity_np");

	if (!CPU_ISSET(i, &cpuset)){
		printf("CPU pinning failed at core :: %d\n", i);
		TheLog_nc_v1(Log::Info, name(),"  CPU pinning failed at core :: %d",i);
	}
}

void SpectaProbe::initializePktRepo()
{
	uint32_t maxLen = 0;

	for(uint16_t intf = 0; intf < IPGlobal::NO_OF_INTERFACES; intf++)
	{
		maxLen = IPGlobal::PPS_PER_INTERFACE[intf] / IPGlobal::ROUTER_PER_INTERFACE[intf];

		printf("PKTStore Repository for Interface [%d] Initializing [%'d] per Router x 10 x %d Router RawPkt... ", intf, maxLen, IPGlobal::ROUTER_PER_INTERFACE[intf]);
		TheLog_nc_v3(Log::Info, name(),"  PKTStore Repository for Interface [%d] Initializing [%'d] per Router x 10 x %d Router RawPkt...", intf, maxLen, IPGlobal::ROUTER_PER_INTERFACE[intf]);

		for(uint16_t router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
			for(uint16_t ti = 0; ti < 10; ti++)
			{
				PKTStore::pktRepoCnt[intf][router][ti] = 0;
				PKTStore::pktRepoBusy[intf][router][ti] = false;

				for(uint32_t ml = 0; ml < maxLen; ml++)
					PKTStore::pktRepository[intf][router][ti][ml] = new RawPkt(IPGlobal::MAX_PKT_LEN_PER_INTERFACE[intf]);
			}
		printf("Completed for Interface [%d] Initializing [%'d] per Router x 10 x %d Router\n", intf, maxLen, IPGlobal::ROUTER_PER_INTERFACE[intf]);
		TheLog_nc_v3(Log::Info, name(),"  Completed for Interface [%d] Initializing [%'d] per Router x 10 x %d Router RawPkt...Completed", intf, maxLen, IPGlobal::ROUTER_PER_INTERFACE[intf]);
	}
}

void SpectaProbe::initializeAgentMaps()
{
	for(uint16_t intf = 0; intf < IPGlobal::NO_OF_INTERFACES; intf++)
	{
		printf("TCP Agent Repository for Interface [%d] Initializing for [%d] Router MPacket... \n", intf, IPGlobal::ROUTER_PER_INTERFACE[intf]);
		TheLog_nc_v2(Log::Info, name(),"TCP Agent Repository for Interface [%d] Initializing for [%d] Router MPacket... ", intf, IPGlobal::ROUTER_PER_INTERFACE[intf]);

		for(uint16_t router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
		{
			for(uint16_t ti = 0; ti < 10; ti++)
			{
				tcpAgent::cnt[intf][router][ti] = 0;
			}
		}

		printf("UDP Agent Repository for Interface [%d] Initializing for [%d] Router MPacket... \n", intf, IPGlobal::ROUTER_PER_INTERFACE[intf]);
		TheLog_nc_v2(Log::Info, name(),"UDP Agent Repository for Interface [%d] Initializing for [%d] Router MPacket... ", intf, IPGlobal::ROUTER_PER_INTERFACE[intf]);

		for(uint16_t router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
		{
			for(uint16_t ti = 0; ti < 10; ti++)
			{
				udpAgent::cnt[intf][router][ti] = 0;
			}
		}

		printf("DNS Agent Repository for Interface [%d] Initializing for [%d] Router MPacket... \n", intf, IPGlobal::ROUTER_PER_INTERFACE[intf]);
		TheLog_nc_v2(Log::Info, name(),"DNS Agent Repository for Interface [%d] Initializing for [%d] Router MPacket... ", intf, IPGlobal::ROUTER_PER_INTERFACE[intf]);

		for(uint16_t router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
		{
			for(uint16_t ti = 0; ti < 10; ti++)
			{
				dnsAgent::cnt[intf][router][ti] = 0;
			}
		}

		printf("AAA Agent Repository for Interface [%d] Initializing for [%d] Router MPacket... \n", intf, IPGlobal::ROUTER_PER_INTERFACE[intf]);
		TheLog_nc_v2(Log::Info, name(),"AAA Agent Repository for Interface [%d] Initializing for [%d] Router MPacket... ", intf, IPGlobal::ROUTER_PER_INTERFACE[intf]);

		for(uint16_t router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
		{
			for(uint16_t ti = 0; ti < 10; ti++)
			{
				aaaAgent::cnt[intf][router][ti] = 0;
//				aaaAgent::busyFlag[intf][router][ti] = false;
			}
		}
	}
}

void SpectaProbe::buildBwCSV(uint64_t timems)
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

void SpectaProbe::openBwCsvXdrFile(uint16_t &currentMin, uint16_t &currentHour, uint16_t &currentDay, uint16_t &currentMonth, uint16_t &currentYear)
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

void SpectaProbe::writeBwXdr(char *buffer)
{
	TheLog_nc_v1(Log::Info, name(),"    Writing BW           [%s]", buffer);
	BwXdrHandler << buffer << std::endl;
}

void SpectaProbe::closeBwCsvXdrFile()
{ BwXdrHandler.close(); }

void SpectaProbe::openCDNCsvXdrFile(uint16_t &currentMin, uint16_t &currentHour, uint16_t &currentDay, uint16_t &currentMonth, uint16_t &currentYear)
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

void SpectaProbe::writeCDNXdr(char *bufferBw, char *bufferCdn, char *bufferUnc, char *bufferPeering)
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

void SpectaProbe::closeCDNCsvXdrFile()
{ CDNXdrHandler.close(); }
