/*
 * adminPortReader.cpp
 *
 *  Created on: Aug 7, 2017
 *      Author: Debashis
 */

#include "AdminPortReader.h"

#include <zmq.h>
#include <unistd.h>


AdminPortReader::AdminPortReader()
{
	this->_name = "AdminPortReader";
	this->setLogLevel(Log::theLog().level());

	this->adminZmqContext = NULL;
	this->adminZmqRequester = NULL;
}

AdminPortReader::~AdminPortReader()
{ }

void AdminPortReader::run()
{
	bool isStatsInitialized = false;
	char buffer[100], buffer1[100];

	buffer[0] = buffer1[0] = 0;
 	TheLog_nc_v1(Log::Info, name()," Opening Admin Zmq Connection to [%s]...", IPGlobal::ADMIN_PORT.c_str());
	adminZmqContext = zmq_ctx_new ();
	adminZmqRequester = zmq_socket (adminZmqContext, ZMQ_REP);
	int rc = zmq_bind(adminZmqRequester, IPGlobal::ADMIN_PORT.c_str());

	while(IPGlobal::PROBE_STATS_RUNNING_STATUS)
	{
		sleep(5);
		int num = zmq_recv(adminZmqRequester, buffer, sizeof(buffer), 0);

		printf("Command Received:: %s [%d]\n", buffer, num);
		buffer1[0] = 0;
		strncpy(buffer1, buffer, num);
		printf("Command Received:: %s [%d]\n", buffer1, num);

		zmq_send(adminZmqRequester, "SUCCESS", 8, 0);

		if(strstr(buffer, "LOAD") != NULL)
			refreshConfig();
		else if(strstr(buffer, "PAUSE_TRAFFIC") != NULL)
		{
			for(int infCounter = 0; infCounter < IPGlobal::NO_OF_INTERFACES; infCounter++)
				IPGlobal::PUSH_TO_QUEUE[infCounter] = false;
		}
		else if(strstr(buffer, "RESUME_TRAFFIC") != NULL)
		{
			for(uint16_t infCounter = 0; infCounter < IPGlobal::NO_OF_INTERFACES; infCounter++)
			{
				IPGlobal::PUSH_TO_QUEUE[infCounter] = true;
				sleep(30);
			}
		}
		else
			printf("****** Invalid Command ....!!!! \n");

		strcpy(buffer, "NA");
		strcpy(buffer1, "NA");
 	}
	printf("Admin Shutdown Completed.\n");
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}


void AdminPortReader::refreshConfig()
{
	printf("\n Re-loading Configurations...\n");
	openConfigFile("client.config");

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_printStats(Key);							/* PRINT_STATS */
		get_printStatsFrequency(Key);					/* PRINT_STATS_FREQ_SEC */
		get_logStatsFrequency(Key);						/* LOG_STATS_FREQ_SEC */
		get_userIpv4ReadFrequency(Key);					/* USER_IPV4_READ_FREQ_MIN */
		get_ProcessOutOfRange(Key);						/* PROCESS_OUT_OF_RANGE_IP */
		get_ProcessUserAgent(Key);						/* PROCESS_USER_AGENT */
		get_IPV4Range(Key);								/* IPV4 RANGE */
		get_ipv6ProcessFlag(Key);						/* IPV6_PROCESSING */
		get_ipv6Range(Key);								/* IPV6_RANGE */
		get_DnsAnswerCount(Key);						/* DNS_ANSWER */
		get_TcpAckSizeBlock(Key);						/* TCP_ACK_SIZE_BLOCK */
		get_UdpSizeBlock(Key);							/* UDP_SIZE_BLOCK */
		get_ProcessCDN(Key);							/* CDN PROCESSING */
		get_TCPBlockPacket(Key);						/* TCP_BLOCK_PACKT */
		get_ProcessPeering(Key);						/* PROCESS_PEERING */
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

	}
	closeConfigFile();
}

void AdminPortReader::get_TCPBlockPacket(std::string& Key)
{
	Value.clear();

	if(Key.compare("TCP_BLOCK_PACKET") == 0)
	{
		fp >> Value;
		IPGlobal::TCP_BLOCK_PACKET = Value.compare("true") == 0 ? true : false;
		printf("	TCP_BLOCK_PACKET			:: [%d] [%s]\n", IPGlobal::TCP_BLOCK_PACKET, Value.c_str());
	}
}

void AdminPortReader::get_ProcessUserAgent(std::string& Key)
{
	Value.clear();

	if(Key.compare("PROCESS_USER_AGENT") == 0)
	{
		fp >> Value;
		IPGlobal::PROCESS_USER_AGENT = Value.compare("true") == 0 ? true : false;
		printf("	PROCESS_USER_AGENT			:: %s\n", Value.c_str());
	}
}

void AdminPortReader::get_ProcessCDN(std::string& Key)
{
	Value.clear();

	if(Key.compare("PROCESS_CDN") == 0)
	{
		fp >> Value;
		IPGlobal::PROCESS_CDN = Value.compare("true") == 0 ? true : false;
		printf("	PROCESS_CDN 			:: %s\n", Value.c_str());
	}
}

void AdminPortReader::get_ProcessPeering(std::string& Key)
{
	Value.clear();

	if(Key.compare("PROCESS_PEERING") == 0)
	{
		fp >> Value;
		IPGlobal::PROCESS_PEERING = Value.compare("true") == 0 ? true : false;
		printf("	PROCESS_PEERING 			:: %s\n", Value.c_str());
	}
}

void AdminPortReader::get_AKAMAI_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("AKAMAI") == 0)
	{
		fp >> Value;
		IPGlobal::AKAMAI = Value.compare("true") == 0 ? true : false;
		printf("	AKAMAI				:: [%d] [%s]\n", IPGlobal::AKAMAI, Value.c_str());
	}
}

void AdminPortReader::get_ALIBABA_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("ALIBABA") == 0)
	{
		fp >> Value;
		IPGlobal::ALIBABA = Value.compare("true") == 0 ? true : false;
		printf("	ALIBABA				:: [%d] [%s]\n", IPGlobal::ALIBABA, Value.c_str());
	}
}

void AdminPortReader::get_AMAZON_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("AMAZON") == 0)
	{
		fp >> Value;
		IPGlobal::AMAZON = Value.compare("true") == 0 ? true : false;
		printf("	AMAZON				:: [%d] [%s]\n", IPGlobal::AMAZON, Value.c_str());
	}
}

void AdminPortReader::get_AMS_IX_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("AMS_IX") == 0)
	{
		fp >> Value;
		IPGlobal::AMS_IX = Value.compare("true") == 0 ? true : false;
		printf("	AMS_IX				:: [%d] [%s]\n", IPGlobal::AMS_IX, Value.c_str());
	}
}

void AdminPortReader::get_APPLE_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("APPLE") == 0)
	{
		fp >> Value;
		IPGlobal::APPLE = Value.compare("true") == 0 ? true : false;
		printf("	APPLE				:: [%d] [%s]\n", IPGlobal::APPLE, Value.c_str());
	}
}

void AdminPortReader::get_CLOUDFLARE_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("CLOUDFLARE") == 0)
	{
		fp >> Value;
		IPGlobal::CLOUDFLARE = Value.compare("true") == 0 ? true : false;
		printf("	CLOUDFLARE				:: [%d] [%s]\n", IPGlobal::CLOUDFLARE, Value.c_str());
	}
}

void AdminPortReader::get_DE_CIX_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("DE_CIX") == 0)
	{
		fp >> Value;
		IPGlobal::DE_CIX = Value.compare("true") == 0 ? true : false;
		printf("	DE_CIX				:: [%d] [%s]\n", IPGlobal::DE_CIX, Value.c_str());
	}
}

void AdminPortReader::get_FASTLY_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("FASTLY") == 0)
	{
		fp >> Value;
		IPGlobal::FASTLY = Value.compare("true") == 0 ? true : false;
		printf("	FASTLY				:: [%d] [%s]\n", IPGlobal::FASTLY, Value.c_str());
	}
}

void AdminPortReader::get_GOOGLE_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("GOOGLE") == 0)
	{
		fp >> Value;
		IPGlobal::GOOGLE = Value.compare("true") == 0 ? true : false;
		printf("	GOOGLE				:: [%d] [%s]\n", IPGlobal::GOOGLE, Value.c_str());
	}
}

void AdminPortReader::get_LIMELIGHT_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("LIMELIGHT") == 0)
	{
		fp >> Value;
		IPGlobal::LIMELIGHT = Value.compare("true") == 0 ? true : false;
		printf("	LIMELIGHT				:: [%d] [%s]\n", IPGlobal::LIMELIGHT, Value.c_str());
	}
}

void AdminPortReader::get_FACEBOOK_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("FACEBOOK") == 0)
	{
		fp >> Value;
		IPGlobal::FACEBOOK = Value.compare("true") == 0 ? true : false;
		printf("	FACEBOOK				:: [%d] [%s]\n", IPGlobal::FACEBOOK, Value.c_str());
	}
}

void AdminPortReader::get_MICROSOFT_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("MICROSOFT") == 0)
	{
		fp >> Value;
		IPGlobal::MICROSOFT = Value.compare("true") == 0 ? true : false;
		printf("	MICROSOFT				:: [%d] [%s]\n", IPGlobal::MICROSOFT, Value.c_str());
	}
}

void AdminPortReader::get_NIXI_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("NIXI") == 0)
	{
		fp >> Value;
		IPGlobal::NIXI = Value.compare("true") == 0 ? true : false;
		printf("	NIXI				:: [%d] [%s]\n", IPGlobal::NIXI, Value.c_str());
	}
}

void AdminPortReader::get_ZENLAYER_Flag(std::string& Key)
{
	Value.clear();

	if(Key.compare("ZENLAYER") == 0)
	{
		fp >> Value;
		IPGlobal::ZENLAYER = Value.compare("true") == 0 ? true : false;
		printf("	ZENLAYER				:: [%d] [%s]\n", IPGlobal::ZENLAYER, Value.c_str());
	}
}

void AdminPortReader::get_DnsAnswerCount(std::string& Key)
{
	Value.clear();

	if(Key.compare("DNS_ANSWER") == 0)
	{
		fp >> Value;
		IPGlobal::DNS_ANSWER = atoi(Value.c_str());
		printf("\tDNS_ANSWER               		:: %d\n", IPGlobal::DNS_ANSWER);
	}
}

void AdminPortReader::get_printStats(std::string& Key)
{
	Value.clear();

	if(Key.compare("PRINT_STATS") == 0)
	{
		fp >> Value;
		IPGlobal::PRINT_STATS = Value.compare("true") == 0 ? true : false;
		printf("	PRINT_STATS				:: [%d] [%s]\n", IPGlobal::PRINT_STATS, Value.c_str());
	}
}

void AdminPortReader::get_TcpAckSizeBlock(std::string& Key)
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

void AdminPortReader::get_UdpSizeBlock(std::string& Key)
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

void AdminPortReader::get_printStatsFrequency(std::string& Key)
{
	Value.clear();

	if(Key.compare("PRINT_STATS_FREQ_SEC") == 0)
	{
		fp >> Value;
		IPGlobal::PRINT_STATS_FREQ_SEC = atoi(Value.c_str());
		printf("	PRINT_STATS_FREQ_SEC			:: %d\n", IPGlobal::PRINT_STATS_FREQ_SEC);
	}
}

void AdminPortReader::get_logStatsFrequency(std::string& Key)
{
	Value.clear();

	if(Key.compare("LOG_STATS_FREQ_SEC") == 0)
	{
		fp >> Value;
		IPGlobal::LOG_STATS_FREQ_SEC = atoi(Value.c_str());
		printf("	LOG_STATS_FREQ_SEC			:: %d\n", IPGlobal::LOG_STATS_FREQ_SEC);
	}
}

void AdminPortReader::get_userIpv4ReadFrequency(std::string& Key)
{
	Value.clear();

	if(Key.compare("USER_IPV4_READ_FREQ_MIN") == 0)
	{
		fp >> Value;
		IPGlobal::USER_IPV4_READ_FREQ_MIN = atoi(Value.c_str());
		printf("	USER_IPV4_READ_FREQ_MIN			:: %d\n", IPGlobal::USER_IPV4_READ_FREQ_MIN);
	}
}

void AdminPortReader::get_ProcessOutOfRange(std::string& Key)
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

//void AdminPortReader::get_IPV4Range(std::string& Key)
//{
//	Value.clear();
//
//	if(Key.compare("IPV4_RANGE") == 0)
//	{
//		fp >> Value;
//		char *pchHash, *pchComma;
//		uint16_t cnt, cnt1;
//		uint32_t x = 0;
//		cnt = cnt1 = 0;
//		size_t pos = 0;
//		std::string token;
//
//		while ((pos = Value.find(",")) != std::string::npos)
//		{
//			token = Value.substr(0, pos);
//			pchHash = strtok((char *)token.c_str(),"-");
//			while (pchHash != NULL)
//			{
//				IPGlobal::IPV4_RANGE[cnt1][cnt] = ipToLong(pchHash, &x);
//				printf("\tIPV4_RANGE[%d][%d]			:: %lu\n", cnt1, cnt, IPGlobal::IPV4_RANGE[cnt1][cnt]);
//				pchHash = strtok (NULL, "-");
//				cnt++;
//				x = 0;
//			}
//			cnt1++;
//			cnt = 0;
//			Value.erase(0, pos + 1);
//		}
//		cnt = 0;
//		x = 0;
//		pchComma = strtok((char *)Value.c_str(),"-");
//		while (pchComma != NULL)
//		{
//			IPGlobal::IPV4_RANGE[cnt1][cnt] = ipToLong(pchComma, &x);
//			printf("\tIPV4_RANGE[%d][%d]			:: %lu\n", cnt1, cnt, IPGlobal::IPV4_RANGE[cnt1][cnt]);
//			pchComma = strtok (NULL, "-");
//			cnt++;
//			x = 0;
//		}
//		IPGlobal::IPV4_NO_OF_RANGE = cnt1;
//	}
//}

void AdminPortReader::get_IPV4Range(std::string& Key)
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


void AdminPortReader::get_ipv6ProcessFlag(std::string& Key)
{
	Value.clear();

	if(Key.compare("IPV6_PROCESSING") == 0)
	{
		fp >> Value;
		IPGlobal::IPV6_PROCESSING = Value.compare("true") == 0 ? 1 : 0;
		printf("\tIPV6_PROCESSING\t\t\t\t:: %s\n", Value.c_str());
	}
}

void AdminPortReader::get_ipv6Range(std::string& Key)
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

void AdminPortReader::openConfigFile(char *fileName)
{
	char  probeConfigBaseDir[100];
	char* probeConfigDir;
	char* probeRootEnv;

	probeConfigDir 	= getenv("PROBE_CONF");
	probeRootEnv 	= getenv("PROBE_ROOT");

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
void AdminPortReader::closeConfigFile()
{ fp.close(); }

uint32_t AdminPortReader::ipToLong(char *ip, uint32_t *plong)
{
	char *next = NULL;
	uint16_t i, err = 0;
	const char *curr = ip;
	unsigned long tmp;

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

void AdminPortReader::converSubNetToRange(char *ipr, char *Start, char *End)
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

unsigned int AdminPortReader::IPToUInt(const std::string ip)
{
	uint32_t i = 0;

    int a, b, c, d;
    unsigned int addr = 0;

    if (sscanf(ip.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d) != 4)
        return 0;

    addr = a << 24;
    addr |= b << 16;
    addr |= c << 8;
    addr |= d;
    return addr;
}

// Converts IP address to the binary form
vector<int> AdminPortReader::bina(vector<string> str)
{
	vector<int> re(32,0);
	int a, b, c, d, i, rem;
	a = b = c = d = 1;
	stack<int> st;
	// Separate each number of the IP address

	a = atoi((const char *)str[0].c_str());
	b = atoi((const char *)str[1].c_str());
	c = atoi((const char *)str[2].c_str());
	d = atoi((const char *)str[3].c_str());

	// convert first number to binary
	for (i = 0; i <= 7; i++)
	{
		rem = a % 2;
		st.push(rem);
		a = a / 2;
	}

	// Obtain First octet
	for (i = 0; i <= 7; i++)
	{
		re[i] = st.top();
		st.pop();
	}

	// convert second number to binary
	for (i = 8; i <= 15; i++)
	{
		rem = b % 2;
		st.push(rem);
		b = b / 2;
	}

	// Obtain Second octet
	for (i = 8; i <= 15; i++)
	{
		re[i] = st.top();
		st.pop();
	}

	// convert Third number to binary
	for (i = 16; i <= 23; i++)
	{
		rem = c % 2;
		st.push(rem);
		c = c / 2;
	}

	// Obtain Third octet
	for (i = 16; i <= 23; i++)
	{
		re[i] = st.top();
		st.pop();
	}

	// convert fourth number to binary
	for (i = 24; i <= 31; i++)
	{
		rem = d % 2;
		st.push(rem);
		d = d / 2;
	}

	// Obtain Fourth octet
	for (i = 24; i <= 31; i++)
	{
		re[i] = st.top();
		st.pop();
	}

	return (re);
}

// cls returns class of given IP address
char AdminPortReader::cls(vector<string> str)
{
	int a = atoi((const char *)str[0].c_str());
	if (a >= 0 && a <= 127)
		return ('A');
	else if (a >= 128 && a <= 191)
		return ('B');
	else if (a >= 192 && a <= 223)
		return ('C');
	else if (a >= 224 && a <= 239)
		return ('D');
	else
		return ('E');
}

// Converts IP address
// from binary to decimal form
vector<int> AdminPortReader::deci(vector<int> bi)
{
	vector<int> arr(4,0);
	int a, b, c, d, i, j;
	a = b = c = d = 0;
	j = 7;

	for (i = 0; i < 8; i++)
	{
		a = a + (int)(pow(2, j)) * bi[i];
		j--;
	}

	j = 7;

	for (i = 8; i < 16; i++)
	{
		b = b + bi[i] * (int)(pow(2, j));
		j--;
	}

	j = 7;
	for (i = 16; i < 24; i++)
	{
		c = c + bi[i] * (int)(pow(2, j));
		j--;
	}

	j = 7;
	for (i = 24; i < 32; i++)
	{
		d = d + bi[i] * (int)(pow(2, j));
		j--;
	}

	arr[0] = a;
	arr[1] = b;
	arr[2] = c;
	arr[3] = d;
	return arr;
}


