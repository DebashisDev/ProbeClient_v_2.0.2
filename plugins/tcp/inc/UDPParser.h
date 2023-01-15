/*
 * PUDP.h
 *
 *  Created on: Nov 29, 2015
 *      Author: Debashis
 */

#ifndef INC_UDPPROBE_H_
#define INC_UDPPROBE_H_

#include "IPGlobal.h"
#include "ProbeUtility.h"
#include "TCPUDPGlobal.h"
#include "DnsData.h"
#include "Log.h"

using namespace std;

enum
{
  A 	= 1,				// IPv4 Address
  NS 	= 2,
  MD 	= 3,
  MF 	= 4,
  CNAME = 5,
  SOA	= 6,
  MB	= 7,
  MG	= 8,
  MR	= 9,
  null	= 10,
  WKS	= 11,
  PTR	= 12,
  HINFO	= 13,
  MINFO	= 14,
  MX	= 15,
  TXT	= 16,
  RP	= 17,
  AFSDB	= 18,
  X25	= 19,
  ISDN	= 20,
  RT	= 21,
  NSAP	= 22,
  NSAPPTR	= 23,
  SIG	= 24,
  KEY	= 25,
  PX	= 26,
  GPOS	= 27,
  AAAA 	= 28,				// IPv6 Address
  LOC	= 29,
  NXT	= 30,
  EID	= 31
}IPTYPE;

enum
{
	IPV4_DNS_RR_HEADER = 16,
	STUN_PKT_HEADER	   = 20,
	IPV6_DNS_RR_HEADER = 28
};


class UDPParser : public DnsData
{
	private:
		ProbeUtility 	*pUt;
		uint16_t 		qdcount, ancount;
		uint16_t 		dnsTLen;

	public:

		UDPParser();
		~UDPParser();

		void	parseUDPPacket(const BYTE packet, MPacket *tPacket);
		void	parsePacketDNS(const BYTE packet, MPacket *, uint16_t protoTLen);
		bool	parsePacketDNSQueries(uint32_t pos, uint32_t id_pos, MPacket *msgObj, const BYTE packet, uint32_t *retPos);
		string 	read_rr_name(const BYTE packet, uint32_t *packet_p, uint32_t id_pos, uint16_t len);
		void	parsePacketDNSAnswers(uint32_t pos, MPacket *msgObj, const BYTE packet);
		void	lockDnsMap();
		void	unLockDnsMap();
};

#endif	/* INC_UDPPROBE_H_ */
