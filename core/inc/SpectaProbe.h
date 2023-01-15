/*
 * SpectaProbe.h
 *
 *  Created on: 29-Jan-2016
 *      Author: Debashis
 */

#ifndef SRC_SPECTAPROBE_H_
#define SRC_SPECTAPROBE_H_

#include <signal.h>
#include <string.h>
#include <string>
#include <time.h>
#include <sys/time.h>

#include "PacketListener.h"
#include "EthernetSource.h"
#include "SpectaTypedef.h"
#include "BaseConfig.h"
#include "ProbeStats.h"
#include "ProbeStatsLog.h"
#include "ProbeUtility.h"
#include "Initialize.h"

#include "AdminPortReader.h"
#include "glbTimer.h"
#include "PacketRouter.h"
#include "Log.h"
#include "processBw.h"

#include "TcpAgent.h"
#include "UdpAgent.h"
#include "AaaAgent.h"
#include "DnsAgent.h"

#include "IPGlobal.h"

class SpectaProbe : public BaseConfig
{
	private:
		uint16_t caseNo, nicCounter, solCounter, interfaceCounter, currentMin, prevMin;

		GConfig			*pGConfig;
		Initialize		*pInit;
		ProbeUtility 	*pUtility;

		glbTimer		*pGlbTimer;
		pthread_t		glbTimerThrId;

		TcpAgent 		*pTcpAgent[MAX_INTERFACE_SUPPORT];
		pthread_t 		thTcpAgent[MAX_INTERFACE_SUPPORT];

		UdpAgent 		*pUdpAgent[MAX_INTERFACE_SUPPORT];
		pthread_t 		thUdpAgent[MAX_INTERFACE_SUPPORT];

		AaaAgent 		*pAaaAgent[MAX_INTERFACE_SUPPORT];
		pthread_t 		thAaaAgent[MAX_INTERFACE_SUPPORT];

		DnsAgent 		*pDnsAgent[MAX_INTERFACE_SUPPORT];
		pthread_t 		thDnsAgent[MAX_INTERFACE_SUPPORT];

		EthernetSource 	*ethReader[MAX_INTERFACE_SUPPORT];
		PacketListener 	*sfReader[MAX_INTERFACE_SUPPORT];
		pthread_t 		pktLisThread[MAX_INTERFACE_SUPPORT];

		ProbeStatsLog	*psLog;
		pthread_t 		psLogThread;

		processBw		*pBw;
		pthread_t 		bwThread;

		ProbeStats 		*ps;
		pthread_t 		psThread ;

		AdminPortReader	*adminPort;
		pthread_t 		adminPortThread;

		PacketRouter 	*pRouter[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];
		pthread_t 		thPktRouter[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];

		fstream			BwXdrHandler;
		fstream			CDNXdrHandler;

		char 			bwXdr[XDR_MAX_LEN];
		char			cachedXdr[XDR_MAX_LEN];
		char			unCachedXdr[XDR_MAX_LEN];
		char			peeringXdr[XDR_MAX_LEN];

		void			buildBwCSV(uint64_t timems);
		void			openBwCsvXdrFile(uint16_t &currentMin, uint16_t &currentHour, uint16_t &currentDay, uint16_t &currentMonth, uint16_t &currentYear);
		void			writeBwXdr(char *buffer);
		void			closeBwCsvXdrFile();

		void			openCDNCsvXdrFile(uint16_t &currentMin, uint16_t &currentHour, uint16_t &currentDay, uint16_t &currentMonth, uint16_t &currentYear);
		void 			writeCDNXdr(char *bufferBW, char *bufferCDN, char *bufferUNC, char *bufferPeering);
		void			closeCDNCsvXdrFile();

		void 			pinThread(pthread_t th, uint16_t i);

		void 			initializePktRepo();
		void			initializeAgentMaps();

		void 			initializeLog();
		void			initializeLocks();

		void			createBwThread(uint16_t no);
		void 			createTimer(uint16_t no);
		void 			packetProcessing(bool flag);
		void			pushToQueue(bool flag);
		void 			createRoutersPerInterface(uint16_t no);
		void			startTcpAgent(uint16_t interfaceId, uint16_t no);
		void			startUdpAgent(uint16_t interfaceId, uint16_t no);
		void			startAaaAgent(uint16_t interfaceId, uint16_t no);
		void			startDnsAgent(uint16_t interfaceId, uint16_t no);
		void 			initializeNICs(uint16_t no);
		void 			createAdmin(uint16_t no);
		void 			printStats(uint16_t no);
		void 			writeStats(uint16_t no);

	public:
		SpectaProbe(char *fileName);
		~SpectaProbe();

		void 	start();
};

#endif /* SRC_SPECTAPROBE_H_ */
