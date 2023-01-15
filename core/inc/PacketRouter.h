/*
 * PacketRouter.h
 *
 *  Created on: Nov 22, 2016
 *      Author: Debashis
 */

#ifndef CORE_SRC_PACKETROUTER_H_
#define CORE_SRC_PACKETROUTER_H_

#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "SpectaTypedef.h"
#include "BWData.h"
#include "CDNData.h"
#include "PeeringData.h"

#include "EthernetParser.h"
#include "BaseConfig.h"
#include "IPGlobal.h"
#include "Log.h"

struct pcapPkthdr
{
	uint32_t tv_sec;
	uint32_t tv_usec;
	uint32_t caplen;
	uint32_t len;
};

class PacketRouter : public BaseConfig
{
	public:

		PacketRouter(uint16_t intfid, uint16_t rid, uint16_t coreid);
		~PacketRouter();

		bool isRepositoryInitialized();
		void run();

	private:

		uint16_t 	intfId, routerId, coreId;
		uint16_t	curMin, prevMin, curHour, prevHour, printCnt;
		uint16_t 	MAX_PKT_LEN;
		bool 		repoInitStatus;

		MPacket*	msgObj;
		RawPkt*		rawPkt;

		BWData*		bwData;
		CDNData*	cdnData;
		PeeringData* peeringData;

		EthernetParser*	ethParser;


		void processQueue(uint16_t t_index);

		void processQueueDecode(bool &pktRepository_busy, uint32_t &pktRepository_cnt, std::unordered_map<uint32_t, RawPkt*> &pktRepository);

		void decodePacket(RawPkt* rawPkt);
		bool	IsIPInRange(uint32_t ip, uint32_t network, uint32_t mask);
		bool 	checkCDNV4(uint32_t ip, uint16_t curMin, uint16_t frSize, uint8_t direction, uint64_t curSec);
		void 	checkCDNV6(char *sourceIpAddrChar, char *destIpAddrChar, uint16_t curMin, uint16_t frSize, uint8_t direction, uint64_t curSec);
		bool	checkPeering(uint32_t ip);
		void 	pushTcpToAgentQueue(MPacket *msgObj);
		void 	pushUdpToAgentQueue(MPacket *msgObj);
		void 	pushAaaToAgentQueue(MPacket *msgObj);
		void 	pushDnsToAgentQueue(MPacket *msgObj);

		void copyMsgObj(uint32_t &ip_msg_cnt, std::unordered_map<uint32_t, MPacket> &ip_msg, MPacket *msgObj);
};

#endif /* CORE_SRC_PACKETROUTER_H_ */
