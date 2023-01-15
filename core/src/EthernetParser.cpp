/*
 * EthernetProbe.cpp
 *
 *  Created on: 30-Jan-2016
 *      Author: Debashis
 */

#include <sys/time.h>
#include <arpa/inet.h>

#include "EthernetParser.h"

EthernetParser::EthernetParser(uint16_t intfid, uint16_t rId)
{
	this->_name = "EthernetParser";
	this->setLogLevel(Log::theLog().level());

	this->ptr_vlan_t 	= NULL;
	this->vlan_tpid		= 0;
	this->vlan_id		= 0;
	this->packetSize 	= 0;
	this->type 			= 0;
	this->frameLen 		= 0;
	this->intfId 		= intfid;
	this->ethOffset 	= 12;

	this->tcp 			= new TCPParser();
	this->udp 			= new UDPParser();
	this->rParser 		= new radiusParser();
	this->pUt 			= new ProbeUtility();

	this->ip4Header 	= NULL;
	this->ip6Header 	= NULL;
	this->udpHeader 	= NULL;
	this->sIp			= 0;
	this->dIp			= 0;
}

EthernetParser::~EthernetParser()
{
	delete (this->tcp);
	delete (this->udp);
	delete(this->rParser);
	delete(this->pUt);
}

void EthernetParser::hexDump(const void* pv, uint16_t len)
{
  const unsigned char* p = (const unsigned char*) pv;
  uint16_t i;

  for( i = 0; i < len; ++i )
  {
    const char* eos;
    switch( i & 15 ) {
    case 0:
      printf("%08x  ", i);
      eos = "";
      break;
    case 1:
      eos = " ";
      break;
    case 15:
      eos = "\n";
      break;
    default:
      eos = (i & 1) ? " " : "";
      break;
    }
    printf("%02x%s", (unsigned) p[i], eos);
  }
  printf(((len & 15) == 0) ? "\n" : "\n\n");
}

void EthernetParser::parsePacket(const BYTE packet, MPacket *msgObj, uint16_t &frLen)
{
    uint16_t type = packet[ethOffset] * 256 + packet[ethOffset + 1];		/* Ethernet Containing Protocol */

    frameLen = frLen;

    switch(type)
     {
     	case ETH_IP:
     			fn_decodeIPv4(packet + sizeof(struct ether_header), msgObj);
     			break;
     	case ETH_8021Q:
     			fn_decode8021Q(packet + sizeof(struct ether_header), msgObj);
     			break;
     	case ETH_MPLS_UC:
     			fn_decodeMPLS(packet + sizeof(struct ether_header), msgObj);
     			break;
     	case ETH_IPV6:
     			fn_decodeIPv6(packet + sizeof(struct ether_header), msgObj);
     			break;
 		default:
     		break;
     }
}

void EthernetParser::fn_decodeMPLS(const BYTE packet, MPacket *msgObj)
{ fn_decodeIPv4(packet + MPLS_PACKET_SIZE, msgObj); }

void EthernetParser::fn_decode8021Q(const BYTE packet, MPacket *msgObj)
{
	vlan_id = 0;

	ptr_vlan_t = (struct vlan_tag*)packet;
	packetSize = sizeof(struct vlan_tag);
	vlan_tpid = ntohs((unsigned short int)ptr_vlan_t->vlan_tpid);
	vlan_id = (uint16_t)(vlan_tpid & 0x0FFF);

	type = ntohs((unsigned short int)ptr_vlan_t->vlan_tci);

	switch(type)
	{
		case ETH_IP:
					fn_decodeIPv4((const BYTE)(packet + packetSize), msgObj);
					break;
		case ETH_8021Q:
					fn_decode8021Q((const BYTE)(packet + packetSize), msgObj);
					break;
		case ETH_PPP_SES:
					fn_decodePPPoE((const BYTE)packet + packetSize, msgObj);
					break;
     	case ETH_IPV6:
     				fn_decodeIPv6((const BYTE)packet + packetSize, msgObj);
     			break;

		default:
					break;
	}
}

void EthernetParser::fn_decodePPPoE(const BYTE packet, MPacket *msgObj)
{ fn_decodeIPv4((const BYTE)(packet + 8), msgObj); }

void EthernetParser::fn_decodeIPv4(const BYTE packet, MPacket *msgObj)
{
	bool dirFound = false;
	bool process = false;

	uint16_t headerLength = 0;

	ip4Header = (struct iphdr *)(packet);

	msgObj->ptype 	= ip4Header->protocol; // TCP or UDP

	switch(msgObj->ptype)
	{
		case PACKET_IPPROTO_TCP:
				msgObj->tcp.frSize				= frameLen;
				msgObj->tcp.ipTLen 				= ntohs((uint16_t)ip4Header->tot_len);
				msgObj->tcp.ipHLen 				= ((unsigned int)ip4Header->ihl)*4;
				msgObj->tcp.ipVer 				= ip4Header->version;
				msgObj->tcp.ipIdentification	= VAL_USHORT(packet + 4);
				msgObj->tcp.ipTtl 				= ip4Header->ttl;
				msgObj->tcp.vLanId				= vlan_id;

				abstractIpv4Address(packet, msgObj);

				msgObj->tcp.direction = getDirectionOnIPV4(msgObj->tcp.sIp, msgObj->tcp.dIp);

				tcp->parseTCPPacket(packet + msgObj->tcp.ipHLen, msgObj);

				break;

		case PACKET_IPPROTO_UDP:
				headerLength 				= ((unsigned int)ip4Header->ihl)*4;

				getProtocolType(packet, msgObj, headerLength);
				abstractIpv4Address(packet, msgObj);

				switch(msgObj->ptype)
				{
					case PACKET_IPPROTO_RADIUS:
								msgObj->aaa.frSize				= frameLen;
								msgObj->aaa.ipTLen 				= ntohs((uint16_t)ip4Header->tot_len);
								msgObj->aaa.ipHLen 				= ((unsigned int)ip4Header->ihl)*4;
								msgObj->aaa.ipVer 				= ip4Header->version;
								msgObj->aaa.frByteLen			= IPGlobal::MAX_PKT_LEN_PER_INTERFACE[intfId];
								rParser->parseAAAPacket(packet + msgObj->aaa.ipHLen, msgObj);
								break;

					case PACKET_IPPROTO_UDP:
								msgObj->udp.frSize				= frameLen;
								msgObj->udp.ipTLen 				= ntohs((uint16_t)ip4Header->tot_len);
								msgObj->udp.ipHLen 				= ((unsigned int)ip4Header->ihl)*4;
								msgObj->udp.ipVer 				= ip4Header->version;
								msgObj->udp.vLanId				= vlan_id;
								msgObj->udp.direction = getDirectionOnIPV4(msgObj->udp.sIp, msgObj->udp.dIp);

								udp->parseUDPPacket(packet + msgObj->udp.ipHLen, msgObj);
								break;
				}
				break;

		default:
				break;
	}
}

bool EthernetParser::checkPeeringIp(uint32_t *ip)
{
	auto it = initSection::ipPeeringMap.equal_range(*ip);

	for (auto itr = it.first; itr != it.second; ++itr)
		if(itr->second == 24)
			return true;

	return false;
}

void EthernetParser::abstractIpv4Address(const BYTE packet, MPacket *msgObj)
{
	uint16_t offset = 12;
	sIp = 0, dIp = 0;

	switch(msgObj->ptype)
	{
		case PACKET_IPPROTO_TCP:
		{
					msgObj->tcp.sIp=(msgObj->tcp.sIp << 8) + (0xff & packet[offset]);
					msgObj->tcp.sIp=(msgObj->tcp.sIp << 8) + (0xff & packet[offset + 1]);
					msgObj->tcp.sIp=(msgObj->tcp.sIp << 8) + (0xff & packet[offset + 2]);
					msgObj->tcp.sIp=(msgObj->tcp.sIp << 8) + (0xff & packet[offset + 3]);
//					sIp = (sIp << 8) + (0xff & 0);

//					msgObj->peeringFlag = checkPeeringIp(&sIp);

					offset += 4;
					msgObj->tcp.dIp=(msgObj->tcp.dIp << 8) + (0xff & packet[offset]);
					msgObj->tcp.dIp=(msgObj->tcp.dIp << 8) + (0xff & packet[offset + 1]);
					msgObj->tcp.dIp=(msgObj->tcp.dIp << 8) + (0xff & packet[offset + 2]);
					msgObj->tcp.dIp=(msgObj->tcp.dIp << 8) + (0xff & packet[offset + 3]);
//					dIp = (dIp << 8) + (0xff & 0);

//					if(!msgObj->peeringFlag)
//						msgObj->peeringFlag = checkPeeringIp(&dIp);
		}
		break;

		case PACKET_IPPROTO_UDP:
		{
					msgObj->udp.sIp=(msgObj->udp.sIp << 8) + (0xff & packet[offset]);
					msgObj->udp.sIp=(msgObj->udp.sIp << 8) + (0xff & packet[offset + 1]);
					msgObj->udp.sIp=(msgObj->udp.sIp << 8) + (0xff & packet[offset + 2]);
					msgObj->udp.sIp=(msgObj->udp.sIp << 8) + (0xff & packet[offset + 3]);
//					sIp = (sIp << 8) + (0xff & 0);

					offset += 4;
					msgObj->udp.dIp=(msgObj->udp.dIp << 8) + (0xff & packet[offset]);
					msgObj->udp.dIp=(msgObj->udp.dIp << 8) + (0xff & packet[offset + 1]);
					msgObj->udp.dIp=(msgObj->udp.dIp << 8) + (0xff & packet[offset + 2]);
					msgObj->udp.dIp=(msgObj->udp.dIp << 8) + (0xff & packet[offset + 3]);
//					dIp = (dIp << 8) + (0xff & 0);

//					if(!msgObj->peeringFlag)
//						msgObj->peeringFlag = checkPeeringIp(&dIp);
		}
		break;

		case PACKET_IPPROTO_RADIUS:
					msgObj->aaa.sIp=(msgObj->aaa.sIp << 8) + (0xff & packet[offset]);
					msgObj->aaa.sIp=(msgObj->aaa.sIp << 8) + (0xff & packet[offset + 1]);
					msgObj->aaa.sIp=(msgObj->aaa.sIp << 8) + (0xff & packet[offset + 2]);
					msgObj->aaa.sIp=(msgObj->aaa.sIp << 8) + (0xff & packet[offset + 3]);

					offset += 4;
					msgObj->aaa.dIp=(msgObj->aaa.dIp << 8) + (0xff & packet[offset]);
					msgObj->aaa.dIp=(msgObj->aaa.dIp << 8) + (0xff & packet[offset + 1]);
					msgObj->aaa.dIp=(msgObj->aaa.dIp << 8) + (0xff & packet[offset + 2]);
					msgObj->aaa.dIp=(msgObj->aaa.dIp << 8) + (0xff & packet[offset + 3]);

					break;
	}
}

void EthernetParser::abstractIpv6Address(const BYTE packet, MPacket *msgObj)
{
#if 0
	uint16_t offset = 4;

	switch(msgObj->ptype)
	{
		case PACKET_IPPROTO_TCP:
					offset += 4;
					pUt->ExtractIP6Address(packet, msgObj->tcp.sIpv6, offset);

					offset += 16;
					pUt->ExtractIP6Address(packet, msgObj->tcp.dIpv6, offset);

					if(strlen(msgObj->tcp.sIpv6) > 40 || strlen(msgObj->tcp.dIpv6) > 40)
					{
						msgObj->ptype 		= 0;
						return;
					}
					break;

		case PACKET_IPPROTO_UDP:
					offset += 4;
					pUt->ExtractIP6Address(packet, msgObj->udp.sIpv6, offset);

					offset += 16;
					pUt->ExtractIP6Address(packet, msgObj->udp.dIpv6, offset);

					if(strlen(msgObj->udp.sIpv6) > 40 || strlen(msgObj->udp.dIpv6) > 40)
					{
						msgObj->ptype 		= 0;
						return;
					}
					break;
	}
#endif
}

void EthernetParser::fn_decodeIPv6(const BYTE packet, MPacket *msgObj)
{
#if 0
	uint16_t offset = 4;
	char buffer[16];

	ip6Header = (struct ip6_hdr *)packet;

	if((ip6Header->ip6_vfc >> 4) != IPVersion6)
	{
		msgObj->ptype = 0;
		return;
	}

	msgObj->ptype 			= ip6Header->ip6_nxt;

	switch(msgObj->ptype)
	{
		case PACKET_IPPROTO_TCP:
				msgObj->tcp.frSize				= frameLen;
				msgObj->tcp.ipHLen 				= IPV6_HEADER_LEN;
				msgObj->tcp.ipTLen 				= msgObj->tcp.ipHLen + (packet[offset] << 8) + packet[offset+1];
				msgObj->tcp.ipVer 				= ip6Header->ip6_vfc >> 4;

				abstractIpv6Address(packet, msgObj);
				msgObj->tcp.direction = getDirectionOnIPV6(msgObj->tcp.sIpv6, msgObj->tcp.dIpv6);

				tcp->parseTCPPacket(packet + msgObj->tcp.ipHLen, msgObj);

							break;

		case PACKET_IPPROTO_UDP:
				msgObj->udp.frSize				= frameLen;
				msgObj->udp.ipHLen 				= IPV6_HEADER_LEN;
				msgObj->udp.ipTLen 				= msgObj->tcp.ipHLen + (packet[offset] << 8) + packet[offset+1];
				msgObj->udp.ipVer 				= ip6Header->ip6_vfc >> 4;

				abstractIpv6Address(packet, msgObj);
				msgObj->udp.direction = getDirectionOnIPV6(msgObj->udp.sIpv6, msgObj->udp.dIpv6);

				getProtocolType(packet, msgObj, IPV6_HEADER_LEN);

				/* As of Now IPV6 Not Supported for Radius Packets */
				if(msgObj->ptype == PACKET_IPPROTO_RADIUS) return;

				udp->parseUDPPacket(packet + msgObj->udp.ipHLen, msgObj);
					break;
		default:
				return;
				break;
	}
#endif
}

uint8_t EthernetParser::getDirectionOnIPV4(uint32_t &sourceIP, uint32_t &destIP)
{
	bool dirSet = false;
    uint16_t counter = 0;
    uint8_t direction = UNKNOWN;

	for(counter = 0; counter < IPGlobal::IPV4_NO_OF_RANGE; counter++)
	{
		if(IsIPInRange(sourceIP, IPGlobal::IPV4_RANGE[counter][0], IPGlobal::IPV4_RANGE[counter][1]))
		{
			direction = UP;
			break;
		}
		else if(IsIPInRange(destIP, IPGlobal::IPV4_RANGE[counter][0], IPGlobal::IPV4_RANGE[counter][1]))
		{
			direction = DOWN;
			break;
		}
	}
	return direction;
}

uint8_t EthernetParser::getDirectionOnIPV6(char *sourceIP, char *destIP)
{
    uint8_t direction = 0;
    uint16_t rangeLen = 0, counter = 0;
    std::string ipInList;

	for (counter = 0; counter < IPGlobal::IPV6Range.size(); ++counter)
	{
		ipInList = IPGlobal::IPV6Range.at(counter);
		rangeLen = ipInList.length();

		if(std::string(sourceIP).compare(0, rangeLen, ipInList) == 0)
		{
			direction = UP;
			break;

		}
		else if(std::string(destIP).compare(0, rangeLen, ipInList) == 0)
		{
			direction = DOWN;
			break;
		}
	}

	if(IPGlobal::PROCESS_OUT_OF_RANGE_IP)
	{
		if(direction == 0)
			direction = UNKNOWN;
	}

	return direction;
}

bool EthernetParser::IsIPInRange(uint32_t ip, uint32_t network, uint32_t mask)
{
    uint32_t net_lower = (network & mask);
    uint32_t net_upper = (net_lower | (~mask));

    if(ip >= net_lower && ip <= net_upper)
        return true;
    return false;
}

uint8_t	getDirectionOnIPV6(char* sourceIP, char* destIP)
{
	bool dirSet 		= false;
    uint8_t direction 	= 0;
	uint16_t counter 	= 0, rangeLen 	= 0;
    std::string ipInList;

	for (counter = 0; counter < IPGlobal::IPV6Range.size(); ++counter)
	{
		ipInList = IPGlobal::IPV6Range.at(counter);
		rangeLen = ipInList.length();

		if(std::string(sourceIP).compare(0, rangeLen, ipInList) == 0)
		{
			direction = UP;
			dirSet = true;
			break;

		}
		else if(std::string(destIP).compare(0, rangeLen, ipInList) == 0)
		{
			direction = DOWN;
			dirSet = true;
			break;
		}
	}

	if(!dirSet)
	{
		if(IPGlobal::PROCESS_OUT_OF_RANGE_IP)
		{
			if(direction == 0)
				direction = UNKNOWN;
		}
	}
	return direction;
}

void EthernetParser::getProtocolType(const BYTE packet, MPacket *msgObj, uint16_t headerLength)
{
	uint16_t	sPort = 0, dPort = 0;

	udpHeader = (struct udphdr *)(packet + headerLength);

	sPort = ntohs((unsigned short int)udpHeader->source);
	dPort = ntohs((unsigned short int)udpHeader->dest);

	if((sPort == RADIUS_AUTH) || (sPort == RADIUS_ACCO) || (sPort == RADIUS_AUTH1) || (sPort == RADIUS_ACCO1) || (sPort == RADIUS_AUTH2) || (sPort == RADIUS_AUTH3)) {
		msgObj->ptype = PACKET_IPPROTO_RADIUS;
		msgObj->aaa.sPort = sPort;
		msgObj->aaa.dPort = dPort;
		msgObj->aaa.direction = DOWN;
	}
	else if((dPort == RADIUS_AUTH) || (dPort == RADIUS_ACCO) || (dPort == RADIUS_AUTH1) || (dPort == RADIUS_ACCO1) || (dPort == RADIUS_AUTH2) || (dPort == RADIUS_AUTH3)) {
		msgObj->ptype = PACKET_IPPROTO_RADIUS;
		msgObj->aaa.sPort = sPort;
		msgObj->aaa.dPort = dPort;
		msgObj->aaa.direction = DOWN;
	}
}
