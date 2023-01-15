/*
 * TCPGlobal.h
 *
 *  Created on: Nov 14, 2015
 *      Author: Debashis
 */

#ifndef INC_IPGLOBAL_H_
#define INC_IPGLOBAL_H_

#include <pthread.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <queue>
#include <array>
#include <bitset>

#include "SpectaTypedef.h"
#include "GConfig.h"

using namespace std;

#define UP				1
#define DOWN			2
#define UNKNOWN			3

#define UDP_HDR_LEN		8
#define PPPoE_HDR_LEN	6

#define XDR_ID_BW 		11
#define XDR_ID_CH 		12
#define XDR_ID_UC 		13
#define XDR_ID_PEERING	14

#define ETH_IP        	0x0800          /* Internet Protocol packet     */
#define ETH_8021Q     	0x8100          /* 802.1Q VLAN Extended Header  */
#define ETH_IPV6    	0x86DD          /* IPv6 over bluebook           */
#define ETH_MPLS_UC   	0x8847          /* MPLS Unicast traffic         */
#define ETH_PPP_SES   	0x8864          /* PPPoE session messages       */

#define MAX_PEERING_IP	4500

#define MAX_INTERFACE_SUPPORT				8
#define MAX_ROUTER_PER_INTERFACE_SUPPORT 	8

#define MAX_IP_RANGE								200

#define APN_LEN		 		50
#define URL_LEN		 		50
#define CONTENT_TYPE_LEN	20
#define HTTP_AGENT_LEN		100
#define DESC_LEN			100
#define	XDR_MAX_LEN			32000

#define RADIUS_USER_NAME_LEN	33
#define RADIUS_USER_ID_LEN		50
#define RADIUS_USER_OLT_LEN		100
#define RADIUS_USER_POLICY_LEN	50
#define RADIUS_USER_PLAN_LEN	50

#define MAC_ADDR_LEN		18
#define IPV4_ADDR_LEN		16
#define IPV6_ADDR_LEN 		46

#define PACKET_NO							200000

#define PKT_WRITE_TIME_INDEX(epochsec,ti) ( ((epochsec % ti) + 1) >= ti ? 0 : ((epochsec % ti) + 1) )
#define PKT_READ_TIME_INDEX(epochsec,ti) ( epochsec % ti )
#define PKT_READ_NEXT_TIME_INDEX(idx,ti) ( (idx+1) >= ti ? 0 : (idx+1) )

enum dnsResponse
{
	QUERY 		= 0,
	RESPONSE 	= 1,
	STATUS 		= 2,
	UNASSIGNED 	= 3,
	NOTIFY 		= 4,
	UPDATE 		= 5,
	SUCCESS		= 6
};

typedef enum
{
	PACKET_IPPROTO_IP 		= 0,	/** Dummy protocol for TCP		*/
	PACKET_IPPROTO_HOPOPTS 	= 0,	/** IPv6 Hop-by-Hop options		*/
	PACKET_IPPROTO_ICMP 	= 1,	/** Internet Control Message Protocol */
	PACKET_IPPROTO_IGMP 	= 2,	/** Internet Group management Protocol */
	PACKET_IPPROTO_IPIP 	= 4,	/** IPIP tunnels (older KA9Q tunnels use 94) */
	PACKET_IPPROTO_TCP		= 6,	/** Transmission Control Protocol	*/
	PACLET_IPPROTO_EGP 		= 8,	/** Exterior Gateway Protocol */
	PACKET_IPPROTO_PUP 		= 12,	/** PUP Protocol */
	PACKET_IPPROTO_UDP 		= 17,	/** User Datagram Protocol		*/
	PACKET_IPPROTO_DNS 		= 18,	/** DNS		*/
	PACKET_IPPROTO_ARP 		= 19,	/** ARP		*/
	PACKET_IPPROTO_IDP 		= 22,	/** XNS IDP protocol */
	PACKET_IPPROTO_TP 		= 29,	/** SO Transport Protocol Class 4. */
	PACKET_IPPROTO_DCCP 	= 33,	/** Datagram Congestion Control Protocol. */
	PACKET_IPPROTO_IPV6 	= 41,	/** IPv6 header */
	PACKET_IPPROTO_ROUTING 	= 43,	/** IPv6 Routing header */
	PACKET_IPPROTO_FRAGMENT = 44,	/** IPv6 fragmentation header */
	PACKET_IPPROTO_RSVP 	= 46,	/** Reservation Protocol */
	PACKET_IPPROTO_GRE 		= 47,	/** General Routing Encapsulation */
	PACKET_IPPROTO_GTPU 	= 48,	/** GTPU Protocol		*/
	PACKET_IPPROTO_GTPC 	= 49,	/** GTPC Protocol		*/
	PACKET_IPPROTO_ESP 		= 50,	/** encapsulating security Payload */
	PACKET_IPPROTO_AH 		= 51,	/** Authentication header */
	PACKET_IPPROTO_GX 		= 52,	/** GTPU Protocol		*/
	PACKET_IPPROTO_RADIUS 	= 53,	/** RADIUS Protocol		*/
	PACKET_IPPROTO_ICMPV6 	= 58,	/** ICMPV6 */
	PACKET_IPPROTO_NONE 	= 59,	/** IPv6 no next header */
	PACKET_IPPROTO_DSTOPTS 	= 60,	/** IPv6 destination options */
	PACKET_IPPROTO_MTP 		= 92,	/** Multicast Transport Protocol */
	PACKET_IPPROTO_ENCAP 	= 98,	/** Encapsulation Header */
	PACKET_IPPROTO_PIM 		= 103,	/** Protocol Independent Multicast */
	PACKET_IPPROTO_COMP 	= 108,	/** Compression Header Protocol */
	PACKET_IPPROTO_SCTP 	= 132,	/** SCTP Protocol		*/
	PACKET_IPPROTO_UDPLITE 	= 136,	/** UDP-Lite protocol */
	PACKET_IPPROTO_RAW 		= 255	/** Raw IP Packets */
}IPProtocolTypes;

typedef enum
{
	HTTP 		= 2,
	BW 			= 3,
}protocolId;

typedef enum
{
	DNS_PORT 		= 53,
	HTTP_PORT 		= 80,
	SYSLOG_PORT		= 514,
	HTTPS_PORT 		= 443,
	GTPU_PORT 		= 2152,
	GTPC_PORT 		= 2123,
	GTPC_PORT1 		= 3386,
	HTTP_PORT1 		= 8080,
	GX_PORT			= 3868,
	RADIUS_AUTH		= 1812,
	RADIUS_AUTH1	= 2812,
	RADIUS_AUTH2	= 64418,
	RADIUS_AUTH3 	= 64433,
	RADIUS_ACCO 	= 1813,
	RADIUS_ACCO1 	= 2813
};

typedef struct
{
	uint32_t startIp;
	uint32_t mask;
}_ipRange;

namespace initSection
{
	extern std::map<uint32_t, std::string> ipSubNetMap;
	extern std::multimap<uint32_t, uint16_t> ipPeeringMap;
	extern std::multimap<uint32_t, uint16_t> ipCacheMap;
}

namespace ipRange
{
	extern uint16_t totalIps;
	extern _ipRange ipRange[100];
}

typedef struct _bwData
{
	uint64_t Bw;
	uint64_t upBw;
	uint64_t dnBw;

	uint64_t totalVol;
	uint64_t upTotalVol;
	uint64_t dnTotalVol;
	uint64_t avgTotalBw;
	uint64_t avgUpBw;
	uint64_t avgDnBw;
	uint64_t peakTotalVol;
	uint64_t peakUpTotalVol;
	uint64_t peakDnTotalVol;

	_bwData()
	{
		Bw 				= 0;
		upBw 			= 0;
		dnBw 			= 0;
		totalVol 		= 0;
		upTotalVol 		= 0;
		dnTotalVol 		= 0;
		avgTotalBw 		= 0;
		avgUpBw 		= 0;
		avgDnBw 		= 0;
		peakTotalVol 	= 0;
		peakUpTotalVol 	= 0;
		peakDnTotalVol 	= 0;
	}
}bwData;

typedef struct _cdnData
{
	uint64_t Bw;
	uint64_t upBw;
	uint64_t dnBw;

	uint64_t totalVol;
	uint64_t upTotalVol;
	uint64_t dnTotalVol;
	uint64_t avgTotalBw;
	uint64_t avgUpBw;
	uint64_t avgDnBw;
	uint64_t peakTotalVol;
	uint64_t peakUpTotalVol;
	uint64_t peakDnTotalVol;

	_cdnData()
	{
		Bw 				= 0;
		upBw 			= 0;
		dnBw 			= 0;
		totalVol 		= 0;
		upTotalVol 		= 0;
		dnTotalVol 		= 0;
		avgTotalBw 		= 0;
		avgUpBw 		= 0;
		avgDnBw 		= 0;
		peakTotalVol 	= 0;
		peakUpTotalVol 	= 0;
		peakDnTotalVol 	= 0;
	}
}cdnData;

typedef struct _peeringData
{
	uint64_t Bw;
	uint64_t upBw;
	uint64_t dnBw;

	uint64_t totalVol;
	uint64_t upTotalVol;
	uint64_t dnTotalVol;
	uint64_t avgTotalBw;
	uint64_t avgUpBw;
	uint64_t avgDnBw;
	uint64_t peakTotalVol;
	uint64_t peakUpTotalVol;
	uint64_t peakDnTotalVol;

	_peeringData()
	{
		Bw 				= 0;
		upBw 			= 0;
		dnBw 			= 0;
		totalVol 		= 0;
		upTotalVol 		= 0;
		dnTotalVol 		= 0;
		avgTotalBw 		= 0;
		avgUpBw 		= 0;
		avgDnBw 		= 0;
		peakTotalVol 	= 0;
		peakUpTotalVol 	= 0;
		peakDnTotalVol 	= 0;
	}
}peeringData;


typedef struct _RawPkt
{
	uint16_t	len;
	uint32_t 	tv_sec;
	uint64_t 	tv_nsec;
	BYTE		pkt;

	_RawPkt(int rawPckSize)
	{
		reset();
		pkt = (BYTE) malloc(rawPckSize);
	}

	_RawPkt(const _RawPkt& rpkt)
	{
		len 	= rpkt.len;
		tv_sec 	= rpkt.tv_sec;
		tv_nsec = rpkt.tv_nsec;
		pkt 	= rpkt.pkt;
	}

	void copy(const _RawPkt* rpkt)
	{
		len 	= rpkt->len;
		tv_sec 	= rpkt->tv_sec;
		tv_nsec = rpkt->tv_nsec;
		pkt 	= rpkt->pkt;
	}

	void operator=(const _RawPkt& rpkt)
	{
		len 	= rpkt.len;
		tv_sec 	= rpkt.tv_sec;
		tv_nsec = rpkt.tv_nsec;
		pkt 	= rpkt.pkt;
	}

	void reset()
	{
		len = 0;
		tv_sec = 0;
		tv_nsec = 0;
	}

}RawPkt;

typedef struct _TcpPacket
{
	uint8_t 	ipVer;
	uint8_t 	ipTtl;
	uint8_t		direction;
	uint16_t	vLanId;
	uint16_t	frSize;
	uint16_t 	ipTLen;
	uint16_t 	ipHLen;
	uint16_t	ipIdentification;
	uint16_t 	sPort;
	uint16_t 	dPort;
	uint16_t	pLoad;
	uint16_t	tcpFlags;
	uint32_t 	sIp;
	uint32_t	dIp;
	uint32_t 	tcpSeqNo;
	uint64_t	frTimeEpochSec;				//Check size to optimize
	uint64_t	ipv4FlowId;
	uint64_t	frTimeEpochNanoSec;
	char		httpAgent[HTTP_AGENT_LEN];

	_TcpPacket()
	{ reset(); }

	_TcpPacket(const _TcpPacket& tcppkt)
	{
		this->ipVer 				= tcppkt.ipVer;
		this->ipTtl 				= tcppkt.ipTtl;
		this->direction 			= tcppkt.direction;
		this->vLanId				= tcppkt.vLanId;
		this->frSize 				= tcppkt.frSize;
		this->ipTLen 				= tcppkt.ipTLen;
		this->ipHLen 				= tcppkt.ipHLen;
		this->ipIdentification		= tcppkt.ipIdentification;
		this->sPort 				= tcppkt.sPort;
		this->dPort 				= tcppkt.dPort;
		this->pLoad 				= tcppkt.pLoad;
		this->tcpFlags 				= tcppkt.tcpFlags;
		this->sIp 					= tcppkt.sIp;
		this->dIp 					= tcppkt.dIp;
		this->tcpSeqNo 				= tcppkt.tcpSeqNo;
		this->frTimeEpochSec 		= tcppkt.frTimeEpochSec;
		this->ipv4FlowId			= tcppkt.ipv4FlowId;
		this->frTimeEpochNanoSec 	= tcppkt.frTimeEpochNanoSec;
		strcpy(this->httpAgent, tcppkt.httpAgent);
	}

	void copy(const _TcpPacket* tcppkt)
	{
		this->ipVer 				= tcppkt->ipVer;
		this->ipTtl 				= tcppkt->ipTtl;
		this->direction 			= tcppkt->direction;
		this->vLanId				= tcppkt->vLanId;
		this->frSize 				= tcppkt->frSize;
		this->ipTLen 				= tcppkt->ipTLen;
		this->ipHLen 				= tcppkt->ipHLen;
		this->ipIdentification		= tcppkt->ipIdentification;
		this->sPort 				= tcppkt->sPort;
		this->dPort 				= tcppkt->dPort;
		this->pLoad 				= tcppkt->pLoad;
		this->tcpFlags 				= tcppkt->tcpFlags;
		this->sIp 					= tcppkt->sIp;
		this->dIp 					= tcppkt->dIp;
		this->tcpSeqNo 				= tcppkt->tcpSeqNo;
		this->frTimeEpochSec 		= tcppkt->frTimeEpochSec;
		this->ipv4FlowId			= tcppkt->ipv4FlowId;
		this->frTimeEpochNanoSec 	= tcppkt->frTimeEpochNanoSec;
		strcpy(this->httpAgent, tcppkt->httpAgent);
	}

	void reset()
	{
		this->ipVer 			= 0;
		this->ipTtl 			= 0;
		this->direction			= 0;
		this->vLanId			= 0;
		this->frSize 			= 0;
		this->ipTLen 			= 0;
		this->ipHLen 			= 0;
		this->ipIdentification 	= 0;
		this->sPort				= 0;
		this->dPort 			= 0;
		this->pLoad 			= 0;
		this->tcpFlags 			= 0;
		this->sIp 				= 0;
		this->dIp 				= 0;
		this->tcpSeqNo 			= 0;
		this->frTimeEpochSec	= 0;
		this->ipv4FlowId	 	= 0;
		this->frTimeEpochNanoSec= 0;
		this->httpAgent[0]		= 0;
	}
}TcpPacket;

typedef struct _UdpPacket
{
	uint8_t 	ipVer;
	uint8_t		direction;
	uint16_t	vLanId;
	uint16_t	frSize;
	uint16_t 	ipTLen;
	uint16_t 	ipHLen;
	uint16_t 	sPort;
	uint16_t 	dPort;
	uint16_t	pLoad;
	uint32_t 	sIp;
	uint32_t	dIp;
	uint64_t	frTimeEpochSec;				//Check size to optimize
	uint64_t	ipv4FlowId;
	uint64_t	frTimeEpochNanoSec;

	_UdpPacket()
	{ reset(); }

	_UdpPacket(const _UdpPacket& udpPkt)
	{
		this->ipVer 				= udpPkt.ipVer;
		this->direction 			= udpPkt.direction;
		this->vLanId				= udpPkt.vLanId;
		this->frSize 				= udpPkt.frSize;
		this->ipTLen 				= udpPkt.ipTLen;
		this->ipHLen 				= udpPkt.ipHLen;
		this->sPort 				= udpPkt.sPort;
		this->dPort 				= udpPkt.dPort;
		this->pLoad 				= udpPkt.pLoad;
		this->sIp 					= udpPkt.sIp;
		this->dIp 					= udpPkt.dIp;
		this->frTimeEpochSec 		= udpPkt.frTimeEpochSec;
		this->ipv4FlowId			= udpPkt.ipv4FlowId;
		this->frTimeEpochNanoSec 	= udpPkt.frTimeEpochNanoSec;
	}

	void copy(const _UdpPacket* udpPkt)
	{
		this->ipVer 				= udpPkt->ipVer;
		this->direction 			= udpPkt->direction;
		this->vLanId				= udpPkt->vLanId;
		this->frSize 				= udpPkt->frSize;
		this->ipTLen 				= udpPkt->ipTLen;
		this->ipHLen 				= udpPkt->ipHLen;
		this->sPort 				= udpPkt->sPort;
		this->dPort 				= udpPkt->dPort;
		this->pLoad 				= udpPkt->pLoad;
		this->sIp 					= udpPkt->sIp;
		this->dIp 					= udpPkt->dIp;
		this->frTimeEpochSec 		= udpPkt->frTimeEpochSec;
		this->ipv4FlowId			= udpPkt->ipv4FlowId;
		this->frTimeEpochNanoSec 	= udpPkt->frTimeEpochNanoSec;
	}

	void reset()
	{
		this->ipVer 			= 0;
		this->direction			= 0;
		this->vLanId			= 0;
		this->frSize 			= 0;
		this->ipTLen 			= 0;
		this->ipHLen 			= 0;
		this->sPort				= 0;
		this->dPort 			= 0;
		this->pLoad 			= 0;
		this->sIp 				= 0;
		this->dIp 				= 0;
		this->frTimeEpochSec 	= 0;
		this->ipv4FlowId		= 0;
		this->frTimeEpochNanoSec= 0;
	}
}UdpPacket;

typedef struct _DnsPacket
{
	uint8_t 	ipVer;
	uint8_t		qrFlag;
	uint8_t		responseCode;
	char 		url[URL_LEN];
	uint16_t 	sPort;
	uint16_t 	dPort;
	uint32_t 	sIp;
	uint32_t	dIp;
	uint32_t	transactionId;
	uint64_t	frTimeEpochSec;				//Check size to optimize
	uint64_t	ipv4FlowId;
	uint64_t	frTimeEpochNanoSec;

	_DnsPacket()
	{ reset(); }

	_DnsPacket(const _DnsPacket& dnsPkt)
	{
		this->ipVer 				= dnsPkt.ipVer;
		this->qrFlag				= dnsPkt.qrFlag;
		this->responseCode 			= dnsPkt.responseCode;
		strcpy(this->url, dnsPkt.url);
		this->sPort 				= dnsPkt.sPort;
		this->dPort 				= dnsPkt.dPort;
		this->sIp 					= dnsPkt.sIp;
		this->dIp 					= dnsPkt.dIp;
		this->transactionId			= dnsPkt.transactionId;
		this->frTimeEpochSec 		= dnsPkt.frTimeEpochSec;
		this->ipv4FlowId			= dnsPkt.ipv4FlowId;
		this->frTimeEpochNanoSec 	= dnsPkt.frTimeEpochNanoSec;
	}

	void copy(const _DnsPacket* dnsPkt)
	{
		this->ipVer 				= dnsPkt->ipVer;
		this->qrFlag				= dnsPkt->qrFlag;
		this->responseCode 			= dnsPkt->responseCode;
		strcpy(this->url, dnsPkt->url);
		this->sPort 				= dnsPkt->sPort;
		this->dPort 				= dnsPkt->dPort;
		this->sIp 					= dnsPkt->sIp;
		this->dIp 					= dnsPkt->dIp;
		this->transactionId			= dnsPkt->transactionId;
		this->frTimeEpochSec 		= dnsPkt->frTimeEpochSec;
		this->ipv4FlowId			= dnsPkt->ipv4FlowId;
		this->frTimeEpochNanoSec 	= dnsPkt->frTimeEpochNanoSec;
	}

	void reset()
	{
		this->ipVer 			= 0;
		this->qrFlag			= 3;	/* Default is 3, 0 - Request, 1 - Response */
		this->responseCode 		= 3;
		this->url[0]			= 0;
		this->sPort				= 0;
		this->dPort 			= 0;
		this->sIp 				= 0;
		this->dIp 				= 0;
		this->transactionId		= 0;
		this->frTimeEpochSec 	= 0;
		this->ipv4FlowId		= 0;
		this->frTimeEpochNanoSec= 0;
	}
}DnsPacket;

typedef struct _AAAPacket
{
	uint8_t 	ipVer;
	uint8_t		direction;
	char		userName[RADIUS_USER_NAME_LEN];
	char		replyMsg[35];
	uint16_t	frSize;
	uint16_t 	ipTLen; /* TODO */
	uint16_t 	ipHLen; /* TODO */
	uint16_t	frByteLen; /* TODO */
	uint16_t 	sPort;
	uint16_t 	dPort;
	uint16_t 	code;
	uint16_t	identifier;
	uint32_t	protocol;
	uint32_t	serviceType;
	uint32_t	accStatusType;
	uint32_t	accAuth;
	uint32_t	terminationCause;
	uint32_t 	sIp;
	uint32_t	dIp;
	uint32_t	framedIp;
	uint32_t	inputOctets;
	uint32_t	outputOctets;
	uint32_t	inputGigaWords;
	uint32_t	outputGigaWords;
	uint32_t	inputPackets;
	uint32_t	outputPackets;
	uint64_t	frTimeEpochSec;				//Check size to optimize
	uint64_t    frTimeEpochMilliSec;

	_AAAPacket()
	{ reset(); }

	_AAAPacket(const _AAAPacket& aaaPkt)
	{
		ipVer 					= aaaPkt.ipVer;
		direction 				= aaaPkt.direction;
		strcpy(userName, aaaPkt.userName);
		strcpy(replyMsg, aaaPkt.replyMsg);
		frSize 					= aaaPkt.frSize;
		ipTLen 					= aaaPkt.ipTLen;
		ipHLen 					= aaaPkt.ipHLen;
		frByteLen				= aaaPkt.frByteLen;
		sPort 					= aaaPkt.sPort;
		dPort 					= aaaPkt.dPort;
		code 					= aaaPkt.code;
		identifier 				= aaaPkt.identifier;
		protocol 				= aaaPkt.protocol;
		serviceType 			= aaaPkt.serviceType;
		accStatusType 			= aaaPkt.accStatusType;
		accAuth 				= aaaPkt.accAuth;
		terminationCause 		= aaaPkt.terminationCause;
		sIp 					= aaaPkt.sIp;
		dIp 					= aaaPkt.dIp;
		framedIp 				= aaaPkt.framedIp;
		inputOctets 			= aaaPkt.inputOctets;
		outputOctets 			= aaaPkt.outputOctets;
		inputGigaWords 			= aaaPkt.inputGigaWords;
		outputGigaWords 		= aaaPkt.outputGigaWords;
		inputPackets 			= aaaPkt.inputPackets;
		outputPackets 			= aaaPkt.outputPackets;
		frTimeEpochSec 			= aaaPkt.frTimeEpochSec;
		frTimeEpochMilliSec 	= aaaPkt.frTimeEpochMilliSec;
	}

	void copy(const _AAAPacket* aaaPkt)
	{
		ipVer 					= aaaPkt->ipVer;
		direction 				= aaaPkt->direction;
		strcpy(userName, aaaPkt->userName);
		strcpy(replyMsg, aaaPkt->replyMsg);
		frSize 					= aaaPkt->frSize;
		ipTLen 					= aaaPkt->ipTLen;
		ipHLen 					= aaaPkt->ipHLen;
		frByteLen				= aaaPkt->frByteLen;
		sPort 					= aaaPkt->sPort;
		dPort 					= aaaPkt->dPort;
		code 					= aaaPkt->code;
		identifier 				= aaaPkt->identifier;
		protocol 				= aaaPkt->protocol;
		serviceType 			= aaaPkt->serviceType;
		accStatusType 			= aaaPkt->accStatusType;
		accAuth 				= aaaPkt->accAuth;
		terminationCause 		= aaaPkt->terminationCause;
		sIp 					= aaaPkt->sIp;
		dIp 					= aaaPkt->dIp;
		framedIp 				= aaaPkt->framedIp;
		inputOctets 			= aaaPkt->inputOctets;
		outputOctets 			= aaaPkt->outputOctets;
		inputGigaWords 			= aaaPkt->inputGigaWords;
		outputGigaWords 		= aaaPkt->outputGigaWords;
		inputPackets 			= aaaPkt->inputPackets;
		outputPackets 			= aaaPkt->outputPackets;
		frTimeEpochSec 			= aaaPkt->frTimeEpochSec;
		frTimeEpochMilliSec 	= aaaPkt->frTimeEpochMilliSec;
	}

	void reset()
	{
		ipVer 					= 0;
		direction 				= 0;
		strcpy(userName, "NA");
		strcpy(replyMsg, "NA");
		frSize 					= 0;
		ipTLen 					= 0;
		ipHLen 					= 0;
		frByteLen				= 0;
		sPort 					= 0;
		dPort 					= 0;
		code 					= 0;
		identifier 				= 0;
		protocol 				= 0;
		serviceType 			= 0;
		accStatusType 			= 0;
		accAuth 				= 0;
		terminationCause 		= 0;
		sIp 					= 0;
		dIp 					= 0;
		framedIp 				= 0;
		inputOctets 			= 0;
		outputOctets 			= 0;
		inputGigaWords 			= 0;
		outputGigaWords 		= 0;
		inputPackets 			= 0;
		outputPackets 			= 0;
		frTimeEpochSec 			= 0;
		frTimeEpochMilliSec 	= 0;
	}
}AAAPacket;

#define TCPPACKET	TcpPacket 		tcp
#define UDPPACKET	UdpPacket 		udp
#define AAAPACKET	AAAPacket 		aaa
#define DNSPACKET	DnsPacket 		dns

typedef struct _MPacket
{
	uint8_t ptype;			/* Layer 4 Protocol Type */
	bool	processOutOfRange;
	bool	dropPacket;
	bool	peeringFlag;

	union
	{
		TCPPACKET;
		UDPPACKET;
		AAAPACKET;
		DNSPACKET;
	};

	_MPacket()
	{ reset(); }

	void reset()
	{
		this->ptype = 0;
		this->processOutOfRange = true;
		this->dropPacket = false;
		this->peeringFlag = false;
		this->tcp.reset();
		this->udp.reset();
		this->aaa.reset();
		this->dns.reset();
	}

	void copy(const _MPacket* mpkt)
	{
		this->ptype	= mpkt->ptype;
		this->processOutOfRange = mpkt->processOutOfRange;
		this->dropPacket = mpkt->dropPacket;
		this->peeringFlag = mpkt->peeringFlag;
		this->tcp 	= mpkt->tcp;
		this->udp	= mpkt->udp;
		this->aaa 	= mpkt->aaa;
		this->dns 	= mpkt->dns;
	}
}MPacket;

namespace IPGlobal
{
	/* Received Packet Count */
	extern uint64_t	TCP_PACKETS_PER_DAY[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];
	extern uint64_t	UDP_PACKETS_PER_DAY[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];
	extern uint64_t	AAA_PACKETS_PER_DAY[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];
	extern uint64_t	DNS_PACKETS_PER_DAY[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];

	/* Timer Parameters */
	extern uint64_t	CURRENT_EPOCH_SEC;
	extern uint64_t	CURRENT_EPOCH_MICRO_SEC;
	extern uint16_t	CURRENT_SEC;
	extern uint16_t	CURRENT_HOUR;
	extern uint16_t	CURRENT_MIN;
	extern uint16_t	CURRENT_DAY;
	extern uint16_t	CURRENT_MONTH;
	extern uint16_t	CURRENT_YEAR;
	extern uint64_t CURRENT_EPOCH_NANO_SEC;
	extern uint64_t CURRENT_EPOCH_MILI_SEC;

	/* Clean Hours Parameters */
	extern uint16_t	END_OF_DAY_CLEAN_HOUR;
	extern uint16_t	END_OF_DAY_CLEAN_MIN;
	extern uint16_t	END_OF_DAY_CLEAN_SEC;

	/* Server Address */
	extern string 	SERVER_TCP_ADDRESS[MAX_INTERFACE_SUPPORT];
	extern string 	SERVER_UDP_ADDRESS[MAX_INTERFACE_SUPPORT];
	extern string 	SERVER_AAA_ADDRESS[MAX_INTERFACE_SUPPORT];
	extern string 	SERVER_DNS_ADDRESS[MAX_INTERFACE_SUPPORT];

	/* Agents Parameters */
	extern bool		TCP_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT];
	extern bool		UDP_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT];
	extern bool		AAA_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT];
	extern bool		DNS_AGENT_RUNNING_STATUS[MAX_INTERFACE_SUPPORT];

	extern uint16_t	TCP_AGENT_CPU_CORE[MAX_INTERFACE_SUPPORT];
	extern uint16_t	UDP_AGENT_CPU_CORE[MAX_INTERFACE_SUPPORT];
	extern uint16_t	AAA_AGENT_CPU_CORE[MAX_INTERFACE_SUPPORT];
	extern uint16_t	DNS_AGENT_CPU_CORE[MAX_INTERFACE_SUPPORT];

	/* Running Status */
	extern bool		PKT_LISTENER_RUNNING_STATUS[MAX_INTERFACE_SUPPORT];
	extern bool		PKT_LISTENER_INTF_MON_RUNNING_STATUS[MAX_INTERFACE_SUPPORT];
	extern bool		PKT_ROUTER_RUNNING_STATUS[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];
	extern bool		PKT_LISTENER_DAYCHANGE_INDICATION[MAX_INTERFACE_SUPPORT];
	extern bool		PROBE_RUNNING_STATUS;
	extern bool		PROBE_STATS_RUNNING_STATUS;

	/* Logs Status */
	extern uint16_t TIMER_CORE;
	extern uint16_t	PROBE_ID;
	extern bool 	PRINT_STATS;
	extern uint16_t PRINT_STATS_FREQ_SEC;
	extern uint16_t	LOG_STATS_FREQ_SEC;
	extern uint16_t	USER_IPV4_READ_FREQ_MIN;

	extern uint16_t LOG_LEVEL;

	extern std::string 	LOG_DIR;
	extern std::string 	BW_DIR;
	extern std::string 	DATA_DIR;
	extern std::string 	USER_IPV4_DIR;


	/* Interface Parameters */
	extern uint16_t	NO_OF_NIC_INTERFACE;
	extern uint16_t	NO_OF_SOLAR_INTERFACE;
	extern uint16_t NO_OF_INTERFACES;
	extern uint16_t	PKT_LISTENER_CPU_CORE[MAX_INTERFACE_SUPPORT];
	extern uint32_t PPS_PER_INTERFACE[MAX_INTERFACE_SUPPORT];
	extern uint16_t	PPS_CAP_PERCENTAGE[MAX_INTERFACE_SUPPORT];
	extern uint16_t	SOLARFLARE_HW_TIMESTAMP;
	extern uint16_t	MAX_PKT_LEN_PER_INTERFACE[MAX_INTERFACE_SUPPORT];

	extern	string	PNAME[MAX_INTERFACE_SUPPORT];
	extern	string 	SOLAR_INTERFACES[MAX_INTERFACE_SUPPORT];
	extern	string 	ETHERNET_INTERFACES[MAX_INTERFACE_SUPPORT];

	/* Routers Parameters */
	extern uint16_t	NO_OF_ROUTERS;
	extern uint16_t ROUTER_PER_INTERFACE[MAX_INTERFACE_SUPPORT];
	extern uint16_t PKT_ROUTER_CPU_CORE[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];

	/* Bandwidth Parameters */
	extern uint16_t	MAX_BW_INTERFACE[MAX_INTERFACE_SUPPORT];
	extern uint32_t PKT_RATE_INTF[MAX_INTERFACE_SUPPORT];
	extern uint64_t PKTS_TOTAL_INTF[MAX_INTERFACE_SUPPORT];
	extern uint64_t BW_MBPS_INTF[MAX_INTERFACE_SUPPORT];
	extern bwData 	BW_MBPS_i_r[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];
	extern cdnData 	CDN_MBPS_i_r[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];
	extern peeringData PEERING_MBPS_i_r[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];

	/* IP Ranges */
	extern bool		PROCESS_OUT_OF_RANGE_IP[MAX_INTERFACE_SUPPORT];

	extern uint16_t	IPV4_NO_OF_RANGE;
	extern uint32_t	IPV4_RANGE[MAX_IP_RANGE][2];
	extern  vector<string> IPV6Range;

	extern uint16_t	NO_OF_IPV4_CDN;
	extern uint32_t	CDN_IPV4_RANGE[MAX_IP_RANGE][2];
	extern vector<string> CDN_IPV6_RANGE;

	/* Packet Processing Parameters */
	extern uint32_t	MAX_PACKET_PUSH_SOCKET;
	extern bool		PACKET_PROCESSING[MAX_INTERFACE_SUPPORT];
	extern bool		PUSH_TO_QUEUE[MAX_INTERFACE_SUPPORT];

	/* Flags & Counters */
	extern bool		TCP_BLOCK_PACKET;
	extern bool 	PROCESS_PEERING;
	extern bool 	PROCESS_CDN;
	extern bool		PROCESS_USER_AGENT;
	extern bool		IPV6_PROCESSING;
	extern bool		TIMER_PROCESSING;
	extern bool		ADMIN_FLAG;
	extern bool		USER_IPV4_DIR_STATUS;

	extern bool		AKAMAI;
	extern bool		ALIBABA;
	extern bool		AMAZON;
	extern bool		AMS_IX;
	extern bool		APPLE;
	extern bool		CLOUDFLARE;
	extern bool		DE_CIX;
	extern bool		FASTLY;
	extern bool		GOOGLE;
	extern bool		LIMELIGHT;
	extern bool		FACEBOOK;
	extern bool		MICROSOFT;
	extern bool		NIXI;
	extern bool		ZENLAYER;

	extern uint16_t MAX_TCP_SIZE;
	extern uint16_t	TCP_ACK_SIZE_BLOCK[MAX_INTERFACE_SUPPORT];
	extern uint16_t	UDP_SIZE_BLOCK[MAX_INTERFACE_SUPPORT];
	extern uint16_t	THREAD_SLEEP_TIME;
	extern uint16_t	DNS_DUMP_TIME;
	extern uint16_t	TIME_INDEX;
	extern uint16_t DNS_ANSWER;
    extern uint64_t DISCARD_PKT_CNT[MAX_INTERFACE_SUPPORT];
	extern string	ADMIN_PORT;

	extern	uint32_t	PEERING_I3DNET[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_I3DNET_COUNT;

	extern	uint32_t	PEERING_VERISIGN[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_VERISIGN_COUNT;

	extern	uint32_t	PEERING_SUBSPACE[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_SUBSPACE_COUNT;

	extern	uint32_t	PEERING_EDGECAST[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_EDGECAST_COUNT;

	extern	uint32_t	PEERING_FASTLY[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_FASTLY_COUNT;

	extern	uint32_t	PEERING_PKT_CLEARING_HOUSE[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_PKT_CLEARING_HOUSE_COUNT;

	extern	uint32_t	PEERING_HUAWEI_CLOUD[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_HUAWEI_CLOUD_COUNT;

	extern	uint32_t	PEERING_TENCENT_GLOBAL[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_TENCENT_GLOBAL_COUNT;

	extern	uint32_t	PEERING_ALIBABA[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_ALIBABA_COUNT;

	extern	uint32_t	PEERING_APPLE[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_APPLE_COUNT;

	extern	uint32_t	PEERING_MICROSOFT[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_MICROSOFT_COUNT;

	extern	uint32_t	PEERING_LIMELIGHT[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_LIMELIGHT_COUNT;

	extern	uint32_t	PEERING_ZENLAYER[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_ZENLAYER_COUNT;

	extern	uint32_t	PEERING_CLOUDFLARE[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_CLOUDFLARE_COUNT;

	extern	uint32_t	PEERING_DE_CIX[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_DE_CIX_COUNT;

	extern	uint32_t	PEERING_AKAMAI[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_AKAMAI_COUNT;

	extern	uint32_t	PEERING_GOOGLE[MAX_PEERING_IP][2];
	extern	uint16_t 	PEERING_GOOGLE_COUNT;

	extern	uint32_t	PEERING_FACEBOOK[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_FACEBOOK_COUNT;

	extern	uint32_t	PEERING_AMAZON[MAX_PEERING_IP][2];
	extern	uint16_t	PEERING_AMAZON_COUNT;

	extern uint32_t	PEERING_NIXI[MAX_PEERING_IP][2];
	extern uint16_t	PEERING_NIXI_COUNT;

	extern uint32_t	PEERING_AMS_IX[MAX_PEERING_IP][2];
	extern uint16_t	PEERING_AMS_IX_COUNT;

}

namespace PKTStore
{
	extern std::unordered_map<uint32_t, RawPkt*> pktRepository[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	extern uint32_t pktRepoCnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	extern bool pktRepoBusy[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace tcpAgent
{
	extern std::unordered_map<uint32_t, MPacket> store[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	extern uint32_t cnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace udpAgent
{
	extern std::unordered_map<uint32_t, MPacket> store[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	extern uint32_t cnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace aaaAgent
{
	extern std::unordered_map<uint32_t, MPacket> store[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	extern uint32_t cnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace dnsAgent
{
	extern std::unordered_map<uint32_t, MPacket> store[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	extern uint32_t cnt[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace mapDnsLock
{
	extern pthread_mutex_t lockCount;
	extern pthread_cond_t nonzero;
	extern unsigned count;
}

#endif /* INC_IPGLOBAL_H_ */
