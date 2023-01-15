/*
 * GConfig.cpp
 *
 *  Created on: 26-Jul-2016
 *      Author: Debashis
 */

#include "GConfig.h"

GConfig::GConfig(char *fileName)
{
	initialize(fileName);

	if(IPGlobal::USER_IPV4_DIR_STATUS)
		readUserIpv4Config();
}

GConfig::~GConfig()
{}

void GConfig::initialize(char* fileName)
{
	printf("\n Loading Configurations...\n");
	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_probeId(Key);								/* PROBE_ID */
		get_logLevel(Key);								/* LOG_LEVEL */
		get_printStats(Key);							/* PRINT_STATS */
		get_printStatsFrequency(Key);					/* PRINT_STATS_FREQ_SEC */
		get_logStatsFrequency(Key);						/* LOG_STATS_FREQ_SEC */
		get_userIpv4ReadFrequency(Key);					/* USER_IPV4_READ_FREQ_MIN */

		get_logDir(Key);								/* LOG_DIR */
		get_dataDir(Key);								/* DATA_DIR */
		get_bwDir(Key);									/* BW_DIR */
		get_userIpDirStatus(Key);						/* USER_IPV4_DIR_STATUS */
		get_userIpv4Dir(Key);							/* IPV4_RANGE_DIR */

		get_adminFlag(Key);								/* ADMIN_FLAG */
		get_adminPort(Key);								/* ADMIN_PORT */

		get_timerCore(Key);								/* TIMER_CORE */

		get_ethernetInterface(Key);						/* ETHERNET_INTERFACE */
		get_solarInterface(Key);						/* SOLAR_INTERFACE */

		get_interfaceCPU(Key);							/* PKT_LISTENER_CPU_CORE */
		get_packetLen(Key);								/* MAX_PKT_LEN_PER_INTERFACE */
		get_PPSPerInterface(Key);						/* PPS_PER_INTERFACE */
		get_PPSCap(Key);								/* PPS_CAP_PERCENTAGE */

		get_routerPerInterface(Key);					/* ROUTER_PER_INTERFACE */
		get_routerCPU(Key);								/* PKT_ROUTER_CPU_CORE */

		if(!IPGlobal::USER_IPV4_DIR_STATUS)
			get_IPV4Range(Key);							/* IPV4 RANGE */

		get_ProcessOutOfRange(Key);						/* PROCESS_OUT_OF_RANGE_IP */
		get_ProcessUserAgent(Key);						/* PROCESS_USER_AGENT */
		get_ipv6ProcessFlag(Key);						/* IPV6_PROCESSING */
		get_ProcessCDN(Key);							/* CDN PROCESSING */
		get_ProcessPeering(Key);						/* PROCESS_PEERING */
		get_TCPBlockPacket(Key);						/* TCP_BLOCK_PACKT */

		if(IPGlobal::PROCESS_CDN)
		{
			get_CdnIPRangeV4(Key);						/* CDN IPV4 RANGE */
			get_CdnIPRangeV6(Key);						/* CDN IPV6 RANGE */
		}
		get_DnsAnswerCount(Key);						/* DNS_ANSWER */
		get_dnsDumpTime(Key);							/* DNS_DUMP_TIME */

		get_serverTcpAddress(Key);						/* SERVER_TCP_ADDRESS */
		get_serverUdpAddress(Key);						/* SERVER_UDP_ADDRESS */
		get_serverAaaAddress(Key);						/* SERVER_AAA_ADDRESS */
		get_serverDnsAddress(Key);						/* SERVER_DNS_ADDRESS */

		get_tcpAgentCpuCore(Key);						/* TCP_AGENT_CPU_CORE */
		get_udpAgentCpuCore(Key);						/* UDP_AGENT_CPU_CORE */
		get_aaaAgentCpuCore(Key);						/* AAA_AGENT_CPU_CORE */
		get_dnsAgentCpuCore(Key);						/* DNS_AGENT_CPU_CORE */

		get_TcpAckSizeBlock(Key);						/* TCP_ACK_SIZE_BLOCK */
		get_UdpSizeBlock(Key);							/* UDP_SIZE_BLOCK */

		if(IPGlobal::PROCESS_PEERING)
		{
			get_AKAMAI_Flag(Key);
			get_ALIBABA_Flag(Key);
			get_AMAZON_Flag(Key);
			get_AMS_IX_Flag(Key);
			get_APPLE_Flag(Key);
			get_CLOUDFLARE_Flag(Key);
			get_DE_CIX_Flag(Key);
			get_FASTLY_Flag(Key);
			get_GOOGLE_Flag(Key);
			get_LIMELIGHT_Flag(Key);
			get_FACEBOOK_Flag(Key);
			get_MICROSOFT_Flag(Key);
			get_NIXI_Flag(Key);
			get_ZENLAYER_Flag(Key);

			get_AKAMAI(Key);				/* PEERING_AKAMAI */
			get_ALIBABA(Key);				/* PEERING_ALIBABA */
			get_AMAZON(Key);				/* PEERING_AMAZON */
			get_AMS_IX(Key);				/* PEERING_AMS_IX */
			get_APPLE(Key);					/* PEERING_APPLE */
			get_CLOUDFLARE(Key);			/* PEERING_CLOUDFLARE */
			get_DE_CIX(Key);				/* PEERING_DE_CIX */
			get_FASTLY(Key);				/* PEERING_FASTLY */
			get_GOOGLE(Key);				/* PEERING_GOOGLE */
			get_LIMELIGHT(Key);				/* PEERING_LIMELIGHT */
			get_FACEBOOK(Key);				/* PEERING_FACEBOOK */
			get_MICROSOFT(Key);				/* PEERING_MICROSOFT */
			get_NIXI(Key);					/* PEERING_NIXI */
			get_ZENLAYER(Key);				/* PEERING_ZENLAYER */
		}

		get_solarTimeStamp(Key);						/* SOLARFLARE_HW_TIMESTAMP */
//		if(IPGlobal::IPV6_PROCESSING)
			get_ipv6Range(Key);							/* IPV6_RANGE */

		get_endOfDayCleanTime(Key);						/* END_OF_DAY_PROCESSING_STOP */
	}
	closeConfigFile();
}

void GConfig::readUserIpv4Config()
{
	printf("\n Loading User IPV4 Configurations...\n");
	openUserIPv4ConfigFile();

	char startIp[16], endIp[16];
	uint32_t x = 0;

	Value.clear();

	fpIpv4 >> Value;
	int cnt = 0;

	char *pch = strtok((char *)Value.c_str(),",");

	while (pch != NULL)
	{
		converSubNetToRange(pch, startIp, endIp);

		IPGlobal::IPV4_RANGE[cnt][0] = ipToLong(startIp, &x);
		IPGlobal::IPV4_RANGE[cnt][1] = ipToLong(endIp, &x);

		pch = strtok (NULL, ",");
		cnt++;
	}
	IPGlobal::IPV4_NO_OF_RANGE = cnt;
	printf("\tIPV4_NO_OF_RANGE			:: %02d\n", IPGlobal::IPV4_NO_OF_RANGE);

	for(uint16_t i = 0; i < IPGlobal::IPV4_NO_OF_RANGE; i++)
		printf("\tIPV4_RANGE        			:: [%u]| [%u]\n", IPGlobal::IPV4_RANGE[i][0], IPGlobal::IPV4_RANGE[i][1]);

	closeUserIPv4ConfigFile();
}

//void GConfig::get_VERISIGN(std::string& Key)
//{
//	startIp[0] = endIp[0] = 0;
//	x = 0;
//	Value.clear();
//
//	if(Key.compare("PEERING_VERISIGN") == 0)
//	{
//		fp >> Value;
//		int cnt = 0;
//
//		char *pch = strtok((char *)Value.c_str(),",");
//
//		while (pch != NULL)
//		{
//			converSubNetToRange(pch, startIp, endIp);
//
//			IPGlobal::PEERING_VERISIGN[cnt][0] = ipToLong(startIp, &x);
//			IPGlobal::PEERING_VERISIGN[cnt][1] = ipToLong(endIp, &x);
//
//			pch = strtok (NULL, ",");
//			cnt++;
//		}
//		IPGlobal::PEERING_VERISIGN_COUNT = cnt;
//		printf("PEERING_VERISIGN_COUNT = %d\n", IPGlobal::PEERING_VERISIGN_COUNT);
//	}
//}
//
//
//void GConfig::get_EDGECAST(std::string& Key)
//{
//	startIp[0] = endIp[0] = 0;
//	x = 0;
//	Value.clear();
//
//	if(Key.compare("PEERING_EDGECAST") == 0)
//	{
//		fp >> Value;
//		int cnt = 0;
//
//		char *pch = strtok((char *)Value.c_str(),",");
//
//		while (pch != NULL)
//		{
//			converSubNetToRange(pch, startIp, endIp);
//
//			IPGlobal::PEERING_EDGECAST[cnt][0] = ipToLong(startIp, &x);
//			IPGlobal::PEERING_EDGECAST[cnt][1] = ipToLong(endIp, &x);
//
//			pch = strtok (NULL, ",");
//			cnt++;
//		}
//		IPGlobal::PEERING_EDGECAST_COUNT = cnt;
//		printf("PEERING_EDGECAST_COUNT = %d\n", IPGlobal::PEERING_EDGECAST_COUNT);
//	}
//}
//void GConfig::get_PKT_CLEARING_HOUSE(std::string& Key)
//{
//	startIp[0] = endIp[0] = 0;
//	x = 0;
//	Value.clear();
//
//	if(Key.compare("PEERING_PKT_CLEARING_HOUSE") == 0)
//	{
//		fp >> Value;
//		int cnt = 0;
//
//		char *pch = strtok((char *)Value.c_str(),",");
//
//		while (pch != NULL)
//		{
//			converSubNetToRange(pch, startIp, endIp);
//
//			IPGlobal::PEERING_PKT_CLEARING_HOUSE[cnt][0] = ipToLong(startIp, &x);
//			IPGlobal::PEERING_PKT_CLEARING_HOUSE[cnt][1] = ipToLong(endIp, &x);
//
//			pch = strtok (NULL, ",");
//			cnt++;
//		}
//		IPGlobal::PEERING_PKT_CLEARING_HOUSE_COUNT = cnt;
//		printf("PEERING_PKT_CLEARING_HOUSE_COUNT = %d\n", IPGlobal::PEERING_PKT_CLEARING_HOUSE_COUNT);
//	}
//}
//
//void GConfig::get_HUAWEI_CLOUD(std::string& Key)
//{
//	startIp[0] = endIp[0] = 0;
//	x = 0;
//	Value.clear();
//
//	if(Key.compare("PEERING_HUAWEI_CLOUD") == 0)
//	{
//		fp >> Value;
//		int cnt = 0;
//
//		char *pch = strtok((char *)Value.c_str(),",");
//
//		while (pch != NULL)
//		{
//			converSubNetToRange(pch, startIp, endIp);
//
//			IPGlobal::PEERING_HUAWEI_CLOUD[cnt][0] = ipToLong(startIp, &x);
//			IPGlobal::PEERING_HUAWEI_CLOUD[cnt][1] = ipToLong(endIp, &x);
//
//			pch = strtok (NULL, ",");
//			cnt++;
//		}
//		IPGlobal::PEERING_HUAWEI_CLOUD_COUNT = cnt;
//		printf("PEERING_HUAWEI_CLOUD_COUNT = %d\n", IPGlobal::PEERING_HUAWEI_CLOUD_COUNT);
//	}
//}

void GConfig::get_AKAMAI_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("AKAMAI") == 0)
	{
		fp >> Value;
		IPGlobal::AKAMAI = Value.compare("true") == 0 ? true : false;
		printf("	AKAMAI				:: [%d] [%s]\n", IPGlobal::AKAMAI, Value.c_str());
	}
}

void GConfig::get_ALIBABA_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("ALIBABA") == 0)
	{
		fp >> Value;
		IPGlobal::ALIBABA = Value.compare("true") == 0 ? true : false;
		printf("	ALIBABA				:: [%d] [%s]\n", IPGlobal::ALIBABA, Value.c_str());
	}
}

void GConfig::get_AMAZON_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("AMAZON") == 0)
	{
		fp >> Value;
		IPGlobal::AMAZON = Value.compare("true") == 0 ? true : false;
		printf("	AMAZON				:: [%d] [%s]\n", IPGlobal::AMAZON, Value.c_str());
	}
}

void GConfig::get_AMS_IX_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("AMS_IX") == 0)
	{
		fp >> Value;
		IPGlobal::AMS_IX = Value.compare("true") == 0 ? true : false;
		printf("	AMS_IX				:: [%d] [%s]\n", IPGlobal::AMS_IX, Value.c_str());
	}
}

void GConfig::get_APPLE_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("APPLE") == 0)
	{
		fp >> Value;
		IPGlobal::APPLE = Value.compare("true") == 0 ? true : false;
		printf("	APPLE				:: [%d] [%s]\n", IPGlobal::APPLE, Value.c_str());
	}
}

void GConfig::get_CLOUDFLARE_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("CLOUDFLARE") == 0)
	{
		fp >> Value;
		IPGlobal::CLOUDFLARE = Value.compare("true") == 0 ? true : false;
		printf("	CLOUDFLARE				:: [%d] [%s]\n", IPGlobal::CLOUDFLARE, Value.c_str());
	}
}

void GConfig::get_DE_CIX_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("DE_CIX") == 0)
	{
		fp >> Value;
		IPGlobal::DE_CIX = Value.compare("true") == 0 ? true : false;
		printf("	DE_CIX				:: [%d] [%s]\n", IPGlobal::DE_CIX, Value.c_str());
	}
}

void GConfig::get_FASTLY_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("FASTLY") == 0)
	{
		fp >> Value;
		IPGlobal::FASTLY = Value.compare("true") == 0 ? true : false;
		printf("	FASTLY				:: [%d] [%s]\n", IPGlobal::FASTLY, Value.c_str());
	}
}

void GConfig::get_GOOGLE_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("GOOGLE") == 0)
	{
		fp >> Value;
		IPGlobal::GOOGLE = Value.compare("true") == 0 ? true : false;
		printf("	GOOGLE				:: [%d] [%s]\n", IPGlobal::GOOGLE, Value.c_str());
	}
}

void GConfig::get_LIMELIGHT_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("LIMELIGHT") == 0)
	{
		fp >> Value;
		IPGlobal::LIMELIGHT = Value.compare("true") == 0 ? true : false;
		printf("	LIMELIGHT				:: [%d] [%s]\n", IPGlobal::LIMELIGHT, Value.c_str());
	}
}

void GConfig::get_FACEBOOK_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("FACEBOOK") == 0)
	{
		fp >> Value;
		IPGlobal::FACEBOOK = Value.compare("true") == 0 ? true : false;
		printf("	FACEBOOK				:: [%d] [%s]\n", IPGlobal::FACEBOOK, Value.c_str());
	}
}

void GConfig::get_MICROSOFT_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("MICROSOFT") == 0)
	{
		fp >> Value;
		IPGlobal::MICROSOFT = Value.compare("true") == 0 ? true : false;
		printf("	MICROSOFT				:: [%d] [%s]\n", IPGlobal::MICROSOFT, Value.c_str());
	}
}

void GConfig::get_NIXI_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("NIXI") == 0)
	{
		fp >> Value;
		IPGlobal::NIXI = Value.compare("true") == 0 ? true : false;
		printf("	NIXI				:: [%d] [%s]\n", IPGlobal::NIXI, Value.c_str());
	}
}

void GConfig::get_ZENLAYER_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("ZENLAYER") == 0)
	{
		fp >> Value;
		IPGlobal::ZENLAYER = Value.compare("true") == 0 ? true : false;
		printf("	ZENLAYER				:: [%d] [%s]\n", IPGlobal::ZENLAYER, Value.c_str());
	}
}


void GConfig::get_AKAMAI(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_AKAMAI") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_AKAMAI[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_AKAMAI[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_AKAMAI_COUNT = cnt;
		printf("PEERING_AKAMAI_COUNT = %d\n", IPGlobal::PEERING_AKAMAI_COUNT);
	}
}

void GConfig::get_GOOGLE(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_GOOGLE") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_GOOGLE[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_GOOGLE[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_GOOGLE_COUNT = cnt;
		printf("PEERING_GOOGLE_COUNT = %d\n", IPGlobal::PEERING_GOOGLE_COUNT);
	}
}

void GConfig::get_FACEBOOK(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_FACEBOOK") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_FACEBOOK[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_FACEBOOK[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_FACEBOOK_COUNT = cnt;
		printf("PEERING_FACEBOOK_COUNT = %d\n", IPGlobal::PEERING_FACEBOOK_COUNT);
	}
}

void GConfig::get_AMAZON(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_AMAZON") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_AMAZON[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_AMAZON[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_AMAZON_COUNT = cnt;
		printf("PEERING_AMAZON_COUNT = %d\n", IPGlobal::PEERING_AMAZON_COUNT);
	}
}

void GConfig::get_AMS_IX(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_AMS_IX") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_AMS_IX[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_AMS_IX[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_AMS_IX_COUNT = cnt;
		printf("PEERING_AMS_IX_COUNT = %d\n", IPGlobal::PEERING_AMS_IX_COUNT);
	}
}

void GConfig::get_NIXI(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_NIXI") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_NIXI[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_NIXI[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_NIXI_COUNT = cnt;
		printf("PEERING_NIXI_COUNT = %d\n", IPGlobal::PEERING_NIXI_COUNT);
	}
}

void GConfig::get_FASTLY(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_FASTLY") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_FASTLY[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_FASTLY[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_FASTLY_COUNT = cnt;
		printf("PEERING_FASTLY_COUNT = %d\n", IPGlobal::PEERING_FASTLY_COUNT);
	}
}

void GConfig::get_ALIBABA(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_ALIBABA") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_ALIBABA[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_ALIBABA[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_ALIBABA_COUNT = cnt;
		printf("PEERING_ALIBABA_COUNT = %d\n", IPGlobal::PEERING_ALIBABA_COUNT);
	}
}

void GConfig::get_APPLE(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_APPLE") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_APPLE[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_APPLE[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_APPLE_COUNT = cnt;
		printf("PEERING_APPLE_COUNT = %d\n", IPGlobal::PEERING_APPLE_COUNT);
	}
}

void GConfig::get_MICROSOFT(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_MICROSOFT") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_MICROSOFT[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_MICROSOFT[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_MICROSOFT_COUNT = cnt;
		printf("PEERING_MICROSOFT_COUNT = %d\n", IPGlobal::PEERING_MICROSOFT_COUNT);
	}
}

void GConfig::get_LIMELIGHT(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_LIMELIGHT") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_LIMELIGHT[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_LIMELIGHT[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_LIMELIGHT_COUNT = cnt;
		printf("PEERING_LIMELIGHT_COUNT = %d\n", IPGlobal::PEERING_LIMELIGHT_COUNT);
	}
}

void GConfig::get_ZENLAYER(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_ZENLAYER") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_ZENLAYER[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_ZENLAYER[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_ZENLAYER_COUNT = cnt;
		printf("PEERING_ZENLAYER_COUNT = %d\n", IPGlobal::PEERING_ZENLAYER_COUNT);
	}
}

void GConfig::get_CLOUDFLARE(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_CLOUDFLARE") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_CLOUDFLARE[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_CLOUDFLARE[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_CLOUDFLARE_COUNT = cnt;
		printf("PEERING_CLOUDFLARE_COUNT = %d\n", IPGlobal::PEERING_CLOUDFLARE_COUNT);
	}
}

void GConfig::get_DE_CIX(std::string& Key)
{
	startIp[0] = endIp[0] = 0;
	x = 0;
	Value.clear();

	if(Key.compare("PEERING_DE_CIX") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::PEERING_DE_CIX[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::PEERING_DE_CIX[cnt][1] = ipToLong(endIp, &x);;

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::PEERING_DE_CIX_COUNT = cnt;
		printf("PEERING_DE_CIX_COUNT = %d\n", IPGlobal::PEERING_DE_CIX_COUNT);
	}
}


void GConfig::openConfigFile(char* fileName)
{
	char probeConfigBaseDir[100];
	char* probeConfigDir;
	char* probeRootEnv;

	probeConfigDir = getenv("PROBE_CONF");
	probeRootEnv = getenv("PROBE_ROOT");

	if(probeConfigDir == NULL || probeRootEnv == NULL)
	{
		printf("\n\n\n  !!! ******* SpectaProbe Environment NOT Set ******* !!! \n\n\n");
		exit(1);
	}
	sprintf(probeConfigBaseDir, "%s/%s", probeConfigDir, fileName);
	fp.open(probeConfigBaseDir);


	if(fp.fail())
	{
		printf("  Error in Opening Configuration File : %s\n", probeConfigBaseDir);
		exit(1);
	}
}

void GConfig::closeConfigFile()
{ fp.close(); }

void GConfig::openUserIPv4ConfigFile()
{
	char ipv4ConfigBaseDir[100];

	sprintf(ipv4ConfigBaseDir, "%s/%s", IPGlobal::USER_IPV4_DIR.c_str(), "userip.config");
	fpIpv4.open(ipv4ConfigBaseDir);


	if(fpIpv4.fail())
	{
		printf("  Error in Opening Configuration File : %s\n", ipv4ConfigBaseDir);
		exit(1);
	}
}

void GConfig::closeUserIPv4ConfigFile()
{ fpIpv4.close(); }

void GConfig::get_probeId(std::string& Key)
{
	Value.clear();

	if(Key.compare("PROBE_ID") == 0)
	{
		fp >> Value;
		IPGlobal::PROBE_ID = atol(Value.c_str());
		printf("\tPROBE_ID               			:: %d\n", IPGlobal::PROBE_ID);
	}
}

void GConfig::get_logLevel(std::string& Key)
{
	Value.clear();

	if(Key.compare("LOG_LEVEL") == 0)
	{
		fp >> Value;
		IPGlobal::LOG_LEVEL = atoi(Value.c_str());
		printf("\tLOG_LEVEL               		:: %d\n", IPGlobal::LOG_LEVEL);

	}
}

void GConfig::get_printStats(std::string& Key)
{
	Value.clear();

	if(Key.compare("PRINT_STATS") == 0)
	{
		fp >> Value;
		IPGlobal::PRINT_STATS = Value.compare("true") == 0 ? true : false;
		printf("	PRINT_STATS				:: [%d] [%s]\n", IPGlobal::PRINT_STATS, Value.c_str());
	}
}

void GConfig::get_printStatsFrequency(std::string& Key)
{
	Value.clear();

	if(Key.compare("PRINT_STATS_FREQ_SEC") == 0)
	{
		fp >> Value;
		IPGlobal::PRINT_STATS_FREQ_SEC = atoi(Value.c_str());
		printf("	PRINT_STATS_FREQ_SEC			:: %d\n", IPGlobal::PRINT_STATS_FREQ_SEC);
	}
}

void GConfig::get_logStatsFrequency(std::string& Key)
{
	Value.clear();

	if(Key.compare("LOG_STATS_FREQ_SEC") == 0)
	{
		fp >> Value;
		IPGlobal::LOG_STATS_FREQ_SEC = atoi(Value.c_str());
		printf("	LOG_STATS_FREQ_SEC			:: %d\n", IPGlobal::LOG_STATS_FREQ_SEC);
	}
}

void GConfig::get_userIpv4ReadFrequency(std::string& Key)
{
	Value.clear();

	if(Key.compare("USER_IPV4_READ_FREQ_MIN") == 0)
	{
		fp >> Value;
		IPGlobal::USER_IPV4_READ_FREQ_MIN = atoi(Value.c_str());
		printf("	USER_IPV4_READ_FREQ_MIN			:: %d\n", IPGlobal::USER_IPV4_READ_FREQ_MIN);
	}
}

void GConfig::get_dnsDumpTime(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_DUMP_TIME") == 0)
	{
		fp >> Value;
		IPGlobal::DNS_DUMP_TIME = atoi(Value.c_str());
		printf("	DNS_DUMP_TIME			:: %d\n", IPGlobal::DNS_DUMP_TIME);
	}
}

void GConfig::get_logDir(std::string& Key)
{
	Value.clear();

	if(Key.compare("LOG_DIR") == 0)
	{
		fp >> Value;
		IPGlobal::LOG_DIR = Value;
		printf("\tLOG_DIR               			:: %s\n", IPGlobal::LOG_DIR.c_str());
	}
}

void GConfig::get_bwDir(std::string& Key)
{
	Value.clear();

	if(Key.compare("BW_DIR") == 0)
	{
		fp >> Value;
		IPGlobal::BW_DIR = Value;
		printf("\tBW_DIR               			:: %s\n", IPGlobal::BW_DIR.c_str());
	}
}

void GConfig::get_userIpDirStatus(std::string& Key)
{
	Value.clear();

	if(Key.compare("USER_IPV4_DIR_STATUS") == 0)
	{
		fp >> Value;
		IPGlobal::USER_IPV4_DIR_STATUS = Value.compare("true") == 0 ? true : false;
		printf("	USER_IPV4_DIR_STATUS 			:: %s\n", Value.c_str());
	}
}

void GConfig::get_dataDir(std::string& Key)
{
	Value.clear();

	if(Key.compare("DATA_DIR") == 0)
	{
		fp >> Value;
		IPGlobal::DATA_DIR = Value;
		printf("\tDATA_DIR               			:: %s\n", IPGlobal::DATA_DIR.c_str());
	}
}

void GConfig::get_userIpv4Dir(std::string& Key)
{
	Value.clear();

	if(Key.compare("USER_IPV4_DIR") == 0)
	{
		fp >> Value;
		IPGlobal::USER_IPV4_DIR = Value;
		printf("\tUSER_IPV4_DIR               			:: %s\n", IPGlobal::USER_IPV4_DIR.c_str());
	}
}

void GConfig::get_adminFlag(std::string& Key)
{
	Value.clear();

	if(Key.compare("ADMIN_FLAG") == 0)
	{
		fp >> Value;
		IPGlobal::ADMIN_FLAG = Value.compare("true") == 0 ? 1 : 0;
		printf("\tADMIN_FLAG\t\t\t\t:: %s\n", Value.c_str());
	}
}

void GConfig::get_adminPort(std::string& Key)
{
	Value.clear();

	if(Key.compare("ADMIN_PORT") == 0)
	{
			fp >> Value;
			IPGlobal::ADMIN_PORT = Value;
			printf("\tADMIN_PORT\t\t\t\t:: %s\n", IPGlobal::ADMIN_PORT.c_str());
	}
}

void GConfig::get_timerCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("TIMER_CORE") == 0)
	{
		fp >> Value;
		IPGlobal::TIMER_CORE = atoi(Value.c_str());
		printf("\tTIMER_CORE               		:: %d\n", IPGlobal::TIMER_CORE);

	}
}

void GConfig::get_ethernetInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("ETHERNET_INTERFACE") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;

		char* pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			IPGlobal::ETHERNET_INTERFACES[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			printf("\tETHERNET_INTERFACES[%d]     		:: %s\n", cnt, IPGlobal::ETHERNET_INTERFACES[cnt].c_str());
			cnt++;
		}
		IPGlobal::NO_OF_NIC_INTERFACE = cnt;
		printf("\tETHERNET_INTERFACE          		:: %d\n", IPGlobal::NO_OF_NIC_INTERFACE);
	}
}

void GConfig::get_solarInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("SOLAR_INTERFACE") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;
		char* pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			IPGlobal::SOLAR_INTERFACES[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			printf("\tSOLAR_INTERFACES[%d] 			:: %s\n", cnt, IPGlobal::SOLAR_INTERFACES[cnt].c_str());
			cnt++;
		}
		IPGlobal::NO_OF_SOLAR_INTERFACE = cnt;
		printf("\tSOLAR_INTERFACES          		:: %d\n", IPGlobal::NO_OF_SOLAR_INTERFACE);
	}
}

void GConfig::get_solarTimeStamp(std::string& Key)
{
	Value.clear();

	if(Key.compare("SOLARFLARE_HW_TIMESTAMP") == 0)
	{
		fp >> Value;
		IPGlobal::SOLARFLARE_HW_TIMESTAMP = Value.compare("true") == 0 ? 1 : 0;
		printf("\tSOLARFLARE_HW_TIMESTAMP			:: %s\n", Value.c_str());
	}
}

void GConfig::get_packetLen(std::string& Key)
{
	Value.clear();

	if(Key.compare("MAX_PKT_LEN_PER_INTERFACE") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;
		char* pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::MAX_PKT_LEN_PER_INTERFACE[cnt] = atoi(pch1);
			printf("\tMAX_PKT_LEN_PER_INTERFACE[%d]     	:: %d\n", cnt, IPGlobal::MAX_PKT_LEN_PER_INTERFACE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_PPSPerInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("PPS_PER_INTERFACE") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;
		char* pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::PPS_PER_INTERFACE[cnt] = atoi(pch1);
			printf("\tPPS_PER_INTERFACE[%d]     		:: %d\n", cnt, IPGlobal::PPS_PER_INTERFACE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_PPSCap(std::string& Key)
{
	Value.clear();

	if(Key.compare("PPS_CAP_PERCENTAGE") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;
		char* pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::PPS_CAP_PERCENTAGE[cnt] = atoi(pch1);
			printf("\tPPS_CAP_PERCENTAGE[%d]     		:: %d\n", cnt, IPGlobal::PPS_CAP_PERCENTAGE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_routerPerInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("ROUTER_PER_INTERFACE") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;
		char* pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::ROUTER_PER_INTERFACE[cnt] = atoi(pch1);
			printf("\tROUTER_PER_INTERFACE[%d]			:: %d\n", cnt, IPGlobal::ROUTER_PER_INTERFACE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_routerCPU(std::string& Key)
{
	Value.clear();

	if(Key.compare("PKT_ROUTER_CPU_CORE") == 0)
	{
		fp >> Value;
		char *pchHash, *pchComma;
		uint16_t cnt, cnt1;

		cnt = cnt1 = 0;
		size_t pos = 0;
		std::string token;

		while ((pos = Value.find("-")) != std::string::npos)
		{
		    token = Value.substr(0, pos);
		    pchHash = strtok((char *)token.c_str(),",");

		    while (pchHash != NULL)
			{
				IPGlobal::PKT_ROUTER_CPU_CORE[cnt1][cnt] = atoi(pchHash);
				printf("\tPKT_ROUTER_CPU_CORE[%d][%d]		:: %d\n", cnt1, cnt, IPGlobal::PKT_ROUTER_CPU_CORE[cnt1][cnt]);
				pchHash = strtok (NULL, ",");
				cnt++;
			}
			cnt1++;
			cnt = 0;
		    Value.erase(0, pos + 1);
		}
		cnt = 0;
		pchComma = strtok((char *)Value.c_str(),",");

		while (pchComma != NULL)
		{
			IPGlobal::PKT_ROUTER_CPU_CORE[cnt1][cnt] = atoi(pchComma);
			printf("\tPKT_ROUTER_CPU_CORE[%d][%d]		:: %d\n", cnt1, cnt, IPGlobal::PKT_ROUTER_CPU_CORE[cnt1][cnt]);
			pchComma = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_IPV4Range(std::string& Key)
{
	char startIp[16], endIp[16];
	uint32_t x = 0;

	Value.clear();

	if(Key.compare("IPV4_RANGE") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::IPV4_RANGE[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::IPV4_RANGE[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::IPV4_NO_OF_RANGE = cnt;
		printf("\tIPV4_NO_OF_RANGE			:: %02d\n", IPGlobal::IPV4_NO_OF_RANGE);

		for(uint16_t i = 0; i < IPGlobal::IPV4_NO_OF_RANGE; i++)
		printf("\tIPV4_RANGE        			:: [%u]| [%u]\n", IPGlobal::IPV4_RANGE[i][0], IPGlobal::IPV4_RANGE[i][1]);
	}
}

void GConfig::get_ipv6ProcessFlag(std::string& Key)
{
	Value.clear();

	if(Key.compare("IPV6_PROCESSING") == 0)
	{
		fp >> Value;
		IPGlobal::IPV6_PROCESSING = Value.compare("true") == 0 ? 1 : 0;
		printf("\tIPV6_PROCESSING\t\t\t\t:: %s\n", Value.c_str());
	}
}

void GConfig::get_ipv6Range(std::string& Key)
{
	Value.clear();

	if(Key.compare("IPV6_RANGE") == 0)
	{
		fp >> Value;
		char* pch;
		pch = strtok((char *)Value.c_str(),",");
		while (pch != NULL)
		{
			IPGlobal::IPV6Range.push_back(pch);
			printf("%50s\t%50s\n", "IPV6_RANGE",  pch);
			pch = strtok (NULL, ",");
		}
	}
}

void GConfig::get_serverTcpAddress(std::string& Key)
{
	Value.clear();

	if(Key.compare("SERVER_TCP_ADDRESS") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;

		char* pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			IPGlobal::SERVER_TCP_ADDRESS[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			printf("\tSERVER_TCP_ADDRESS[%d]     		:: %s\n", cnt, IPGlobal::SERVER_TCP_ADDRESS[cnt].c_str());
			cnt++;
		}
	}
}

void GConfig::get_serverUdpAddress(std::string& Key)
{
	Value.clear();

	if(Key.compare("SERVER_UDP_ADDRESS") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;

		char* pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			IPGlobal::SERVER_UDP_ADDRESS[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			printf("\tSERVER_UDP_ADDRESS[%d]     		:: %s\n", cnt, IPGlobal::SERVER_UDP_ADDRESS[cnt].c_str());
			cnt++;
		}
	}
}

void GConfig::get_serverAaaAddress(std::string& Key)
{
	Value.clear();

	if(Key.compare("SERVER_AAA_ADDRESS") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;

		char* pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			IPGlobal::SERVER_AAA_ADDRESS[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			printf("\tSERVER_AAA_ADDRESS[%d]     		:: %s\n", cnt, IPGlobal::SERVER_AAA_ADDRESS[cnt].c_str());
			cnt++;
		}
	}
}

void GConfig::get_serverDnsAddress(std::string& Key)
{
	Value.clear();

	if(Key.compare("SERVER_DNS_ADDRESS") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;

		char* pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			IPGlobal::SERVER_DNS_ADDRESS[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			printf("\tSERVER_DNS_ADDRESS[%d]     		:: %s\n", cnt, IPGlobal::SERVER_DNS_ADDRESS[cnt].c_str());
			cnt++;
		}
	}
}


void GConfig::get_interfaceCPU(std::string& Key)
{
	Value.clear();

	if(Key.compare("PKT_LISTENER_CPU_CORE") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;
		char* pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::PKT_LISTENER_CPU_CORE[cnt] = atoi(pch1);
			printf("\tPKT_LISTENER_CPU_CORE[%d]		:: %d\n", cnt, IPGlobal::PKT_LISTENER_CPU_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_tcpAgentCpuCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("TCP_AGENT_CPU_CORE") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;
		char* pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::TCP_AGENT_CPU_CORE[cnt] = atoi(pch1);
			printf("\tTCP_AGENT_CPU_CORE[%d]		:: %d\n", cnt, IPGlobal::TCP_AGENT_CPU_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_udpAgentCpuCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("UDP_AGENT_CPU_CORE") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;
		char* pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::UDP_AGENT_CPU_CORE[cnt] = atoi(pch1);
			printf("\tUDP_AGENT_CPU_CORE[%d]		:: %d\n", cnt, IPGlobal::UDP_AGENT_CPU_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_aaaAgentCpuCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("AAA_AGENT_CPU_CORE") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;
		char* pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::AAA_AGENT_CPU_CORE[cnt] = atoi(pch1);
			printf("\tAAA_AGENT_CPU_CORE[%d]		:: %d\n", cnt, IPGlobal::AAA_AGENT_CPU_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_dnsAgentCpuCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_AGENT_CPU_CORE") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;
		char* pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::DNS_AGENT_CPU_CORE[cnt] = atoi(pch1);
			printf("\tDNS_AGENT_CPU_CORE[%d]		:: %d\n", cnt, IPGlobal::DNS_AGENT_CPU_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_TCPBlockPacket(std::string& Key)
{
	Value.clear();

	if(Key.compare("TCP_BLOCK_PACKET") == 0)
	{
		fp >> Value;
		IPGlobal::TCP_BLOCK_PACKET = Value.compare("true") == 0 ? true : false;
		printf("	TCP_BLOCK_PACKET			:: [%d] [%s]\n", IPGlobal::TCP_BLOCK_PACKET, Value.c_str());
	}
}


void GConfig::get_ProcessOutOfRange(std::string& Key)
{
	Value.clear();

	if(Key.compare("PROCESS_OUT_OF_RANGE_IP") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;

		char* pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			IPGlobal::PROCESS_OUT_OF_RANGE_IP[cnt] = strstr(pch,"true") == NULL ? false : true;
			printf("\tPROCESS_OUT_OF_RANGE_IP[%d]     		:: %s\n", cnt, pch);
			pch = strtok (NULL, ",");
			cnt++;
		}
	}
}


void GConfig::get_ProcessUserAgent(std::string& Key)
{
	Value.clear();

	if(Key.compare("PROCESS_USER_AGENT") == 0)
	{
		fp >> Value;
		IPGlobal::PROCESS_USER_AGENT = Value.compare("true") == 0 ? true : false;
		printf("	PROCESS_USER_AGENT			:: %s\n", Value.c_str());
	}
}

void GConfig::get_DnsAnswerCount(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_ANSWER") == 0)
	{
		fp >> Value;
		IPGlobal::DNS_ANSWER = atoi(Value.c_str());
		printf("\tDNS_ANSWER               		:: %d\n", IPGlobal::DNS_ANSWER);
	}
}

void GConfig::get_TcpAckSizeBlock(std::string& Key)
{
	Value.clear();

	if(Key.compare("TCP_ACK_SIZE_BLOCK") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;
		char* pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::TCP_ACK_SIZE_BLOCK[cnt] = atoi(pch1);
			printf("\tTCP_ACK_SIZE_BLOCK[%d]     	:: %d\n", cnt, IPGlobal::TCP_ACK_SIZE_BLOCK[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_UdpSizeBlock(std::string& Key)
{
	Value.clear();

	if(Key.compare("UDP_SIZE_BLOCK") == 0)
	{
		fp >> Value;
		uint16_t cnt = 0;
		char* pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::UDP_SIZE_BLOCK[cnt] = atoi(pch1);
			printf("\tUDP_SIZE_BLOCK[%d]     	:: %d\n", cnt, IPGlobal::UDP_SIZE_BLOCK[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

void GConfig::get_ProcessCDN(std::string& Key)
{
	Value.clear();

	if(Key.compare("PROCESS_CDN") == 0)
	{
		fp >> Value;
		IPGlobal::PROCESS_CDN = Value.compare("true") == 0 ? true : false;
		printf("	PROCESS_CDN 			:: %s\n", Value.c_str());
	}
}

void GConfig::get_ProcessPeering(std::string& Key)
{
	Value.clear();

	if(Key.compare("PROCESS_PEERING") == 0)
	{
		fp >> Value;
		IPGlobal::PROCESS_PEERING = Value.compare("true") == 0 ? true : false;
		printf("	PROCESS_PEERING 			:: %s\n", Value.c_str());
	}
}

void GConfig::get_CdnIPRangeV4(std::string& Key)
{
	char startIp[16], endIp[16];
	uint32_t x = 0;

	Value.clear();

	if(Key.compare("CDN_IPV4_RANGE") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			converSubNetToRange(pch, startIp, endIp);

			IPGlobal::CDN_IPV4_RANGE[cnt][0] = ipToLong(startIp, &x);
			IPGlobal::CDN_IPV4_RANGE[cnt][1] = ipToLong(endIp, &x);

			pch = strtok (NULL, ",");
			cnt++;
		}
		IPGlobal::NO_OF_IPV4_CDN = cnt;
		printf("\tNO_OF_IPV4_CDN			:: %02d\n", IPGlobal::NO_OF_IPV4_CDN);

		for(uint16_t i = 0; i < IPGlobal::NO_OF_IPV4_CDN; i++)
		printf("\tNO_OF_IPV4_CDN        			:: [%u]| [%u]\n", IPGlobal::CDN_IPV4_RANGE[i][0], IPGlobal::CDN_IPV4_RANGE[i][1]);
	}
}

void GConfig::get_CdnIPRangeV6(std::string& Key)
{
	Value.clear();

	if(Key.compare("CDN_IPV6_RANGE") == 0)
	{
		fp >> Value;
		char* pch;
		pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			IPGlobal::CDN_IPV6_RANGE.push_back(pch);
			printf("\tCDN_IPV6_RANGE     				:: %s\n", pch);
			pch = strtok (NULL, ",");
		}
	}
}

void GConfig::get_endOfDayCleanTime(std::string& Key)
{
	Value.clear();

	if(Key.compare("END_OF_DAY_CLEAN_HOUR") == 0)
	{
		fp >> Value;
		IPGlobal::END_OF_DAY_CLEAN_HOUR = atoi(Value.c_str());
		printf("\tEND_OF_DAY_CLEAN_HOUR			:: %s\n", Value.c_str());
	}

	Value.clear();

	if(Key.compare("END_OF_DAY_CLEAN_MIN") == 0)
	{
		fp >> Value;
		IPGlobal::END_OF_DAY_CLEAN_MIN = atoi(Value.c_str());
		printf("\tEND_OF_DAY_CLEAN_MIN			:: %s\n", Value.c_str());
	}

	Value.clear();

	if(Key.compare("END_OF_DAY_CLEAN_SEC") == 0)
	{
		fp >> Value;
		IPGlobal::END_OF_DAY_CLEAN_SEC = atoi(Value.c_str());
		printf("\tEND_OF_DAY_CLEAN_SEC			:: %s\n", Value.c_str());
	}

}

uint32_t GConfig::ipToLong(char *ip, uint32_t *plong)
{
	char *next = NULL;
	const char *curr = ip;
	uint32_t tmp;
	int i, err = 0;

	*plong = 0;
	for (i = 0; i < NUM_OCTETTS; i++)
	{
		tmp = strtoul(curr, &next, 10);
		if (tmp >= 256 || (tmp == 0 && next == curr))
		{
			err++;
			break;
		}
		*plong = (*plong << 8) + tmp;
		curr = next + 1;
	}

	if (err)
		return 1;
	else
		return *plong;
}

void GConfig::converSubNetToRange(char *ipr, char *Start, char *End)
{
	string str1 = "";
	string str2 = "";

	int idx = 0;
	int len = strlen(ipr) - 3;

	while(len--)
	{
		str1 += ipr[idx];
		idx++;
	}

	strcpy(Start, str1.c_str());

	idx++;
	str2 += ipr[idx];
	idx++;
	str2 += ipr[idx];

	strcpy(End, initSection::ipSubNetMap[atoi(str2.c_str())].c_str());
}
