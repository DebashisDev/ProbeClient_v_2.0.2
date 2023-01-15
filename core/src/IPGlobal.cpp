/*
 * TCPGlobal.cpp
 *
 *  Created on: Nov 14, 2015
 *      Author: Debashis
 */

#include "IPGlobal.h"

using namespace std;

namespace ipRange
{
	uint16_t totalIps;
	_ipRange ipRange[100];
}

namespace initSection
{
	std::map<uint32_t, std::string> ipSubNetMap;
	std::multimap<uint32_t, uint16_t> ipPeeringMap;
	std::multimap<uint32_t, uint16_t> ipCacheMap;
}

namespace IPGlobal
{
	/* Received Packet Count */
	uint64_t	TCP_PACKETS_PER_DAY[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT] = {0};
	uint64_t	UDP_PACKETS_PER_DAY[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT] = {0};
	uint64_t	AAA_PACKETS_PER_DAY[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT] = {0};
	uint64_t	DNS_PACKETS_PER_DAY[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT] = {0};

	/* Timer Parameters */
	uint64_t	CURRENT_EPOCH_SEC			= 0;
	uint64_t	CURRENT_EPOCH_MICRO_SEC		= 0;
	uint16_t	CURRENT_SEC					= 0;
	uint16_t	CURRENT_HOUR				= 0;
	uint16_t	CURRENT_MIN					= 0;
	uint16_t	CURRENT_DAY					= 0;
	uint16_t	CURRENT_MONTH				= 0;
	uint16_t	CURRENT_YEAR				= 0;
	uint64_t 	CURRENT_EPOCH_NANO_SEC		= 0;
	uint64_t 	CURRENT_EPOCH_MILI_SEC		= 0;

	/* Clean Hours Parameters */
	uint16_t	END_OF_DAY_CLEAN_HOUR		= 23;
	uint16_t	END_OF_DAY_CLEAN_MIN		= 59;
	uint16_t	END_OF_DAY_CLEAN_SEC		= 30;

	/* Server Address */
	string 		SERVER_TCP_ADDRESS[MAX_INTERFACE_SUPPORT];
	string 		SERVER_UDP_ADDRESS[MAX_INTERFACE_SUPPORT];
	string 		SERVER_AAA_ADDRESS[MAX_INTERFACE_SUPPORT];
	string 		SERVER_DNS_ADDRESS[MAX_INTERFACE_SUPPORT];

	/* Agents Parameters */
	bool		TCP_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT] = {false};
	bool		UDP_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT] = {false};
	bool		AAA_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT] = {false};
	bool		DNS_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT] = {false};

	uint16_t	TCP_AGENT_CPU_CORE[MAX_INTERFACE_SUPPORT] = {0};
	uint16_t	UDP_AGENT_CPU_CORE[MAX_INTERFACE_SUPPORT] = {0};
	uint16_t	AAA_AGENT_CPU_CORE[MAX_INTERFACE_SUPPORT] = {0};
	uint16_t	DNS_AGENT_CPU_CORE[MAX_INTERFACE_SUPPORT] = {0};

	/* Running Status */
	bool		PKT_LISTENER_RUNNING_STATUS[MAX_INTERFACE_SUPPORT] = {false};
	bool		PKT_LISTENER_INTF_MON_RUNNING_STATUS[MAX_INTERFACE_SUPPORT] = {false};
	bool		PKT_ROUTER_RUNNING_STATUS[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT] = {false};
	bool		PKT_LISTENER_DAYCHANGE_INDICATION[MAX_INTERFACE_SUPPORT] = {false};
	bool		PROBE_RUNNING_STATUS = true;
	bool		PROBE_STATS_RUNNING_STATUS = true;

	/* Logs Status */
	uint16_t	TIMER_CORE				= 0;
	uint16_t	PROBE_ID				= 0;
	bool 		PRINT_STATS				= false;
	uint16_t 	PRINT_STATS_FREQ_SEC	= 0;
	uint16_t	LOG_STATS_FREQ_SEC		= 0;
	uint16_t	USER_IPV4_READ_FREQ_MIN = 15;

	uint16_t 		LOG_LEVEL;

	std::string 	LOG_DIR;
	std::string 	BW_DIR;
	std::string 	DATA_DIR;
	std::string 	USER_IPV4_DIR;

	/* Interface Parameters */
	uint16_t	NO_OF_NIC_INTERFACE		= 0;
	uint16_t	NO_OF_SOLAR_INTERFACE	= 0;
	uint16_t 	NO_OF_INTERFACES		= 0;
	uint16_t	PKT_LISTENER_CPU_CORE[MAX_INTERFACE_SUPPORT] 	= {0};
	uint32_t 	PPS_PER_INTERFACE[MAX_INTERFACE_SUPPORT] 		= {0};
	uint16_t	PPS_CAP_PERCENTAGE[MAX_INTERFACE_SUPPORT] 		= {0};
	uint16_t	SOLARFLARE_HW_TIMESTAMP = 0;
	uint16_t	MAX_PKT_LEN_PER_INTERFACE[MAX_INTERFACE_SUPPORT];

	string		PNAME[MAX_INTERFACE_SUPPORT];
	string 		SOLAR_INTERFACES[MAX_INTERFACE_SUPPORT];
	string 		ETHERNET_INTERFACES[MAX_INTERFACE_SUPPORT];

	/* Routers Parameters */
	uint16_t	NO_OF_ROUTERS = 0;
	uint16_t 	ROUTER_PER_INTERFACE[MAX_INTERFACE_SUPPORT] = {0};
	uint16_t 	PKT_ROUTER_CPU_CORE[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT] = {0};

	/* Bandwidth Parameters */
	uint16_t	MAX_BW_INTERFACE[MAX_INTERFACE_SUPPORT] = {0};
	uint32_t 	PKT_RATE_INTF[MAX_INTERFACE_SUPPORT] 	= {0};
	uint64_t 	PKTS_TOTAL_INTF[MAX_INTERFACE_SUPPORT]	= {0};
	uint64_t 	BW_MBPS_INTF[MAX_INTERFACE_SUPPORT]		= {0};
	bwData 		BW_MBPS_i_r[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];
	cdnData 	CDN_MBPS_i_r[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];
	peeringData PEERING_MBPS_i_r[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];

	/* IP Ranges */
	bool		PROCESS_OUT_OF_RANGE_IP[MAX_INTERFACE_SUPPORT] 	= {false, false, false, false, false, false, false, false};

	uint16_t	IPV4_NO_OF_RANGE			= 0;
	uint32_t	IPV4_RANGE[MAX_IP_RANGE][2] = {0};
	vector<string> IPV6Range;

	uint16_t	NO_OF_IPV4_CDN				= 0;
	uint32_t	CDN_IPV4_RANGE[MAX_IP_RANGE][2] = {0};
	vector<string> CDN_IPV6_RANGE;

	/* Packet Processing Parameters */
	uint32_t	MAX_PACKET_PUSH_SOCKET		 = {0};
	bool		PACKET_PROCESSING[MAX_INTERFACE_SUPPORT]	 = {false};
	bool		PUSH_TO_QUEUE[MAX_INTERFACE_SUPPORT]		 = {false};

	/* Flags & Counters */
	bool		TCP_BLOCK_PACKET	= false;
	bool 		PROCESS_PEERING		= false;
	bool 		PROCESS_CDN			= false;
	bool		PROCESS_USER_AGENT	= false;
	bool		IPV6_PROCESSING		= false;
	bool		TIMER_PROCESSING	= false;
	bool		ADMIN_FLAG			= false;
	bool		USER_IPV4_DIR_STATUS = false;

	bool		AKAMAI		= false;
	bool		ALIBABA 	= false;
	bool		AMAZON 		= false;
	bool		AMS_IX 		= false;
	bool		APPLE 		= false;
	bool		CLOUDFLARE 	= false;
	bool		DE_CIX 		= false;
	bool		FASTLY 		= false;
	bool		GOOGLE 		= false;
	bool		LIMELIGHT 	= false;
	bool		FACEBOOK 	= false;
	bool		MICROSOFT 	= false;
	bool		NIXI 		= false;
	bool		ZENLAYER 	= false;

	uint16_t 	MAX_TCP_SIZE		= 3000;
	uint16_t	TCP_ACK_SIZE_BLOCK[MAX_INTERFACE_SUPPORT]	= {100, 100, 100, 100, 100, 100, 100, 100};
	uint16_t	UDP_SIZE_BLOCK[MAX_INTERFACE_SUPPORT]	= {100, 100, 100, 100, 100, 100, 100, 100};
	uint16_t	THREAD_SLEEP_TIME	= 25000;
	uint16_t	DNS_DUMP_TIME		= 2;
	uint16_t	TIME_INDEX			= 10;
	uint16_t 	DNS_ANSWER			= 3;
    uint64_t 	DISCARD_PKT_CNT[MAX_INTERFACE_SUPPORT] = {0};
	string		ADMIN_PORT;

	uint32_t	PEERING_I3DNET[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_I3DNET_COUNT = 0;

	uint32_t	PEERING_VERISIGN[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_VERISIGN_COUNT = 0;

	uint32_t	PEERING_SUBSPACE[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_SUBSPACE_COUNT = 0;

	uint32_t	PEERING_EDGECAST[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_EDGECAST_COUNT = 0;

	uint32_t	PEERING_FASTLY[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_FASTLY_COUNT = 0;

	uint32_t	PEERING_PKT_CLEARING_HOUSE[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_PKT_CLEARING_HOUSE_COUNT = 0;

	uint32_t	PEERING_HUAWEI_CLOUD[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_HUAWEI_CLOUD_COUNT = 0;

	uint32_t	PEERING_TENCENT_GLOBAL[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_TENCENT_GLOBAL_COUNT = 0;

	uint32_t	PEERING_ALIBABA[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_ALIBABA_COUNT = 0;

	uint32_t	PEERING_APPLE[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_APPLE_COUNT = 0;

	uint32_t	PEERING_MICROSOFT[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_MICROSOFT_COUNT = 0;

	uint32_t	PEERING_LIMELIGHT[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_LIMELIGHT_COUNT = 0;

	uint32_t	PEERING_ZENLAYER[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_ZENLAYER_COUNT = 0;

	uint32_t	PEERING_CLOUDFLARE[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_CLOUDFLARE_COUNT = 0;

	uint32_t	PEERING_DE_CIX[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_DE_CIX_COUNT = 0;

	uint32_t	PEERING_AKAMAI[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_AKAMAI_COUNT = 0;

	uint32_t	PEERING_GOOGLE[MAX_PEERING_IP][2] = {0};
	uint16_t 	PEERING_GOOGLE_COUNT = 0;

	uint32_t	PEERING_FACEBOOK[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_FACEBOOK_COUNT = 0;

	uint32_t	PEERING_AMAZON[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_AMAZON_COUNT = 0;

	uint32_t	PEERING_NIXI[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_NIXI_COUNT = 0;

	uint32_t	PEERING_AMS_IX[MAX_PEERING_IP][2] = {0};
	uint16_t	PEERING_AMS_IX_COUNT = 0;

}


namespace PKTStore
{
	std::unordered_map<uint32_t, RawPkt*> pktRepository[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	uint32_t pktRepoCnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	bool pktRepoBusy[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace tcpAgent
{
	std::unordered_map<uint32_t, MPacket> store[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	uint32_t cnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace udpAgent
{
	std::unordered_map<uint32_t, MPacket> store[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	uint32_t cnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace aaaAgent
{
	std::unordered_map<uint32_t, MPacket> store[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	uint32_t cnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace dnsAgent
{
	std::unordered_map<uint32_t, MPacket> store[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	uint32_t cnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace mapDnsLock {
	pthread_mutex_t lockCount = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t nonzero = PTHREAD_COND_INITIALIZER;
	unsigned count;
}
