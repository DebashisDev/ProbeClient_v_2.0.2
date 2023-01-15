/*
 * adminPortReader.h
 *
 *  Created on: Aug 7, 2017
 *      Author: Debashis
 */

#ifndef CORE_SRC_ADMINPORTREADER_H_
#define CORE_SRC_ADMINPORTREADER_H_

#include "IPGlobal.h"
#include "SpectaTypedef.h"
#include "Log.h"
#include "BaseConfig.h"

#define		MAX_BUFFER_SIZE			500

class AdminPortReader:public BaseConfig
{
	private:
		ifstream 	fp;
		string 		Key, Value;

		void*		adminZmqContext;
		void*		adminZmqRequester;
		void 		refreshConfig();
		void 		updatePPS(string &buffer);
		void 		openConfigFile(char *fileName);
		void 		closeConfigFile();

		void		get_printStats(std::string& Key);
		void		get_printStatsFrequency(std::string& Key);
		void		get_logStatsFrequency(std::string& Key);
		void		get_userIpv4ReadFrequency(std::string& Key);
		void		get_ProcessOutOfRange(std::string& Key);
		void		get_ProcessUserAgent(std::string& Key);
		void		get_TcpAckSizeBlock(std::string& Key);
		void		get_UdpSizeBlock(std::string& Key);
		void		get_TCPBlockPacket(std::string& Key);

		void		get_IPV4Range(std::string& Key);
		void		get_ipv6ProcessFlag(std::string& Key);
		void 		get_ipv6Range(std::string& Key);
		void		get_DnsAnswerCount(std::string& Key);
		uint32_t 	ipToLong(char *ip, uint32_t *plong);
		void		get_ProcessCDN(std::string& Key);
		void		get_ProcessPeering(std::string& Key);

		void		get_AKAMAI_Flag(std::string& Key);
		void		get_ALIBABA_Flag(std::string& Key);
		void		get_AMAZON_Flag(std::string& Key);
		void		get_AMS_IX_Flag(std::string& Key);
		void		get_APPLE_Flag(std::string& Key);
		void		get_CLOUDFLARE_Flag(std::string& Key);
		void		get_DE_CIX_Flag(std::string& Key);
		void		get_FASTLY_Flag(std::string& Key);
		void		get_GOOGLE_Flag(std::string& Key);
		void		get_LIMELIGHT_Flag(std::string& Key);
		void		get_FACEBOOK_Flag(std::string& Key);
		void		get_MICROSOFT_Flag(std::string& Key);
		void		get_NIXI_Flag(std::string& Key);
		void		get_ZENLAYER_Flag(std::string& Key);

		void 	converSubNetToRange(char *ipr, char *Start, char *End);
		unsigned int IPToUInt(const std::string ip);
		vector<int> bina(vector<string> str);
		char 		cls(vector<string> str);
		vector<int> deci(vector<int> bi);

	public:
		AdminPortReader();
		~AdminPortReader();
		void run();
};

#endif /* CORE_SRC_ADMINPORTREADER_H_ */
