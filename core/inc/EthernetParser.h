/*
 * EthernetProbe.h
 *
 *  Created on: 30-Jan-2016
 *      Author: Debashis
 */

#ifndef CORE_SRC_ETHERNETPARSER_H_
#define CORE_SRC_ETHERNETPARSER_H_

#include <string.h>
#include <algorithm>
#include <stdlib.h>

#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <pcap/vlan.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/ip.h>

#include "radiusParser.h"
#include "Log.h"
#include "BaseConfig.h"
#include "IPGlobal.h"
#include "TCPParser.h"
#include "UDPParser.h"
#include "ProbeUtility.h"

#define ETH_P_MPLS 			34887
#define MPLS_PACKET_SIZE 	4
#define IPV6_HEADER_LEN		40
#define IPV6_STRSIZE 		46
#define BASE 				16

class EthernetParser : public BaseConfig
{
	private:

		TCPParser*			tcp;
		UDPParser*			udp;
		ProbeUtility*		pUt;
		radiusParser*		rParser;
		struct iphdr*		ip4Header;
		struct ip6_hdr*  	ip6Header;
		struct udphdr*		udpHeader;
		struct vlan_tag*	ptr_vlan_t;
		uint16_t 			vlan_tpid;
		uint16_t 			vlan_id;
		uint16_t 			packetSize;
		uint16_t 			type;
		uint16_t 			ethOffset;
		uint16_t 			frameLen;
		uint16_t 			intfId;
		uint32_t 			sIp, dIp;

		void 	fn_decode8021Q(const BYTE packet, MPacket *msgObj);
		void	fn_decodeIPv4(const BYTE packet, MPacket *msgObj);
		void	fn_decodeIPv6(const BYTE packet, MPacket *msgObj);
		void 	fn_decodeMPLS(const BYTE packet, MPacket *msgObj);
		void 	fn_decodePPPoE(const BYTE packet, MPacket *msgObj);

		void 	getProtocolType(const BYTE packet, MPacket *msgObj, uint16_t headerLength);
		void 	getGxProtocolType(const BYTE packet, MPacket *msgObj);
		void 	generateKey(MPacket *msgObj);
		void    hexDump(const void* pv, uint16_t len);

		uint8_t	getDirectionOnIPV4(uint32_t &sourceIP, uint32_t &destIP);
		bool	IsIPInRange(uint32_t ip, uint32_t network, uint32_t mask);
		uint8_t	getDirectionOnIPV6(char* sourceIP, char* destIP);

		void	abstractIpv4Address(const BYTE packet, MPacket *msgObj);
		void	abstractIpv6Address(const BYTE packet, MPacket *msgObj);

		  bool	checkPeeringIp(uint32_t *ipAddress);
//        bool 	check_I3DNET(uint32_t &ip);
//        bool    check_VERISIGN(uint32_t &ip);
//        bool    check_SUBSPACE(uint32_t &ip);
//        bool    check_EDGECAST(uint32_t &ip);
//        bool    check_FASTLY(uint32_t &ip);
//        bool    check_PKT_CLEARING_HOUSE(uint32_t &ip);
//        bool    check_HUAWEI_CLOUD(uint32_t &ip);
//        bool    check_PEERING_TENCENT_GLOBAL(uint32_t &ip);
//        bool    check_ALIBABA(uint32_t &ip);
//        bool    check_QUANTIL(uint32_t &ip);
//        bool    check_MICROSOFT(uint32_t &ip);
//        bool    check_LIMELIGHT(uint32_t &ip);
//        bool    check_ZENLAYER(uint32_t &ip);
//        bool    check_CLOUDFLARE(uint32_t &ip);
//        bool    check_NETFLIX(uint32_t &ip);
//        bool    check_AKAMAI(uint32_t &ip);
//        bool    check_GOOGLE(uint32_t &ip);
//        bool    check_FACEBOOK(uint32_t &ip);
//        bool    check_AMAZON(uint32_t &ip);

	public:
		EthernetParser(uint16_t intfid, uint16_t rId);
		~EthernetParser();

		void 	parsePacket(const BYTE packet, MPacket *msgObj, uint16_t &frLen);
		void	initProtocol();
};




#endif /* CORE_SRC_ETHERNETPARSER_H_ */
