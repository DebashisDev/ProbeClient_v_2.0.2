/*
 * GConfig.h
 *
 *  Created on: 26-Jul-2016
 *      Author: Debashis
 */

#ifndef CORE_SRC_GCONFIG_H_
#define CORE_SRC_GCONFIG_H_

#include <string.h>
#include <fstream>
#include <stack>
#include <vector>
#include <math.h>

#include "SpectaTypedef.h"
#include "IPGlobal.h"

#define NUM_OCTETTS 4

using namespace std;

class GConfig
{
	private:
		ifstream fp;
		string Key, Value;
		char 		startIp[16], endIp[16];
		uint32_t 	x = 0;

		void 	initialize(char *fileName);
		void 	openConfigFile(char *fileName);
		void 	closeConfigFile();

		ifstream fpIpv4;
		void 	openUserIPv4ConfigFile();
		void 	closeUserIPv4ConfigFile();

		uint32_t ipToLong(char *ip, uint32_t *plong);

		void 	get_probeId(std::string& Key);
		void	get_logLevel(std::string& Key);
		void	get_printStats(std::string& Key);
		void	get_printStatsFrequency(std::string& Key);
		void	get_logStatsFrequency(std::string& Key);
		void	get_userIpv4ReadFrequency(std::string& Key);
		void	get_dnsDumpTime(std::string& Key);
		void	get_logDir(std::string& Key);
		void	get_userIpDirStatus(std::string& Key);
		void	get_bwDir(std::string& Key);
		void	get_dataDir(std::string& Key);
		void	get_userIpv4Dir(std::string& Key);
		void	get_adminFlag(std::string& Key);
		void	get_adminPort(std::string& Key);
		void	get_timerCore(std::string& Key);
		void	get_ethernetInterface(std::string& Key);
		void	get_solarInterface(std::string& Key);
		void	get_interfaceCPU(std::string& Key);

		void	get_tcpAgentCpuCore(std::string& Key);
		void	get_udpAgentCpuCore(std::string& Key);
		void	get_aaaAgentCpuCore(std::string& Key);
		void	get_dnsAgentCpuCore(std::string& Key);

		void	get_solarTimeStamp(std::string& Key);
		void	get_packetLen(std::string& Key);
		void	get_PPSPerInterface(std::string& Key);
		void	get_PPSCap(std::string& Key);
		void	get_routerPerInterface(std::string& Key);
		void	get_routerCPU(std::string& Key);
		void	get_IPV4Range(std::string& Key);

		void	get_ipv6ProcessFlag(std::string& Key);
		void 	get_ipv6Range(std::string& Key);
		void	get_CdnIPRangeV4(std::string& Key);
		void	get_CdnIPRangeV6(std::string& Key);

		void	get_serverTcpAddress(std::string& Key);
		void	get_serverUdpAddress(std::string& Key);
		void	get_serverAaaAddress(std::string& Key);
		void	get_serverDnsAddress(std::string& Key);

		void	get_TCPBlockPacket(std::string& Key);
		void	get_ProcessOutOfRange(std::string& Key);
		void	get_ProcessUserAgent(std::string& Key);
		void	get_DnsAnswerCount(std::string& Key);
		void	get_TcpAckSizeBlock(std::string& Key);
		void	get_UdpSizeBlock(std::string& Key);
		void	get_ProcessCDN(std::string& Key);
		void	get_ProcessPeering(std::string& Key);
		void	get_endOfDayCleanTime(std::string& Key);

		void 	converSubNetToRange(char *ipr, char *Start, char *End);

		void	get_AKAMAI(std::string& Key);				/* PEERING_AKAMAI */
		void	get_ALIBABA(std::string& Key);				/* PEERING_ALIBABA */
		void	get_AMAZON(std::string& Key);				/* PEERING_AMAZON */
		void	get_AMS_IX(std::string& Key);				/* PEERING_AMS_IX */
		void	get_APPLE(std::string& Key);				/* PEERING_APPLE */
		void	get_CLOUDFLARE(std::string& Key);			/* PEERING_CLOUDFLARE */
		void	get_DE_CIX(std::string& Key);				/* PEERING_NETFLIX */
		void	get_FASTLY(std::string& Key);				/* PEERING_FASTLY */
		void	get_GOOGLE(std::string& Key);				/* PEERING_GOOGLE */
		void	get_LIMELIGHT(std::string& Key);			/* PEERING_LIMELIGHT */
		void	get_FACEBOOK(std::string& Key);				/* PEERING_FACEBOOK */
		void	get_MICROSOFT(std::string& Key);			/* PEERING_MICROSOFT */
		void	get_NIXI(std::string& Key);					/* PEERING_NIXI */
		void	get_ZENLAYER(std::string& Key);				/* PEERING_ZENLAYER */


//		void	get_I3DNET(std::string& Key);				/* PEERING_I3DNET */
//		void	get_VERISIGN(std::string& Key);				/* PEERING_VERISIGN */
//		void	get_SUBSPACE(std::string& Key);				/* PEERING_SUBSPACE */
//		void	get_EDGECAST(std::string& Key);				/* PEERING_EDGECAST */
//		void	get_PKT_CLEARING_HOUSE(std::string& Key);	/* PEERING_PKT_CLEARING_HOUSE */
//		void	get_HUAWEI_CLOUD(std::string& Key);			/* PEERING_HUAWEI_CLOUD */
//		void	get_PEERING_TENCENT_GLOBAL(std::string& Key);/* PEERING_TENCENT_GLOBAL  */
//		void	get_META(std::string& Key);					/* PEERING_META */

		void	get_AKAMAI_Flag(std::string& Key);
		void	get_ALIBABA_Flag(std::string& Key);
		void	get_AMAZON_Flag(std::string& Key);
		void	get_AMS_IX_Flag(std::string& Key);
		void	get_APPLE_Flag(std::string& Key);
		void	get_CLOUDFLARE_Flag(std::string& Key);
		void	get_DE_CIX_Flag(std::string& Key);
		void	get_FASTLY_Flag(std::string& Key);
		void	get_GOOGLE_Flag(std::string& Key);
		void	get_LIMELIGHT_Flag(std::string& Key);
		void	get_FACEBOOK_Flag(std::string& Key);
		void	get_MICROSOFT_Flag(std::string& Key);
		void	get_NIXI_Flag(std::string& Key);
		void	get_ZENLAYER_Flag(std::string& Key);

	public:
		GConfig(char *fileName);
		~GConfig();

		void	readUserIpv4Config();

};
#endif /* CORE_SRC_GCONFIG_H_ */
