/*
 * PUDP.cpp
 *
 *  Created on: Nov 14, 2015
 *      Author: Debashis
 */


#include <netinet/udp.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <algorithm>

#include "UDPParser.h"

using namespace std;

UDPParser::UDPParser()
{
	pUt = new ProbeUtility();
	dnsTLen = 0;
	qdcount = 0;
	ancount = 0;

	this->_name = "UDP";
	this->setLogLevel(Log::theLog().level());
}

UDPParser::~UDPParser()
{ delete (pUt); }

void UDPParser::lockDnsMap()
{
	    pthread_mutex_lock(&mapDnsLock::lockCount);
	    while (mapDnsLock::count == 0)
	        pthread_cond_wait(&mapDnsLock::nonzero, &mapDnsLock::lockCount);
	    mapDnsLock::count = mapDnsLock::count - 1;
	    pthread_mutex_unlock(&mapDnsLock::lockCount);
}

void UDPParser::unLockDnsMap()
{
    pthread_mutex_lock(&mapDnsLock::lockCount);
    if (mapDnsLock::count == 0)
        pthread_cond_signal(&mapDnsLock::nonzero);
    mapDnsLock::count = mapDnsLock::count + 1;
    pthread_mutex_unlock(&mapDnsLock::lockCount);
}

void UDPParser::parseUDPPacket(const BYTE packet, MPacket *msgObj)
{ 
	std::string 	URL, appProtocol;
	uint16_t 		udpTLen = 0, ret = 0;

	uint16_t		sPort, dPort;

	struct udphdr *udpHeader = (struct udphdr *)(packet);

 	udpTLen = ntohs((unsigned short int)udpHeader->len);
	msgObj->udp.sPort = ntohs((unsigned short int)udpHeader->source);
	msgObj->udp.dPort = ntohs((unsigned short int)udpHeader->dest);


	uint16_t checkByte = VAL_USHORT(packet + UDP_HDR_LEN);

	switch(checkByte)
	{
		case   1:	/* STUN Request */
		case 257:	/* STUN Response */
				msgObj->dropPacket = true;
				return;
				break;
	}

	// Block
	// 161 - SMTP, 514 - SysLog, 1194 - OpenVPN, 2268 - Automatic Multicast Tunneling, 4500 - Net keepalive,
	// 5246 - 8801 - Control & provisioning of wireless Access point, 40975 - GQUIC, 7708/7707 - RX Protocol

	switch(msgObj->udp.sPort)
	{
		case 514:
		case 161:
		case 1194:
		case 2268:
		case 4500:
		case 5246:
		case 7707:
		case 7708:
		case 8801:
		case 40975:
			msgObj->dropPacket = true;
			break;
	}

	switch(msgObj->udp.dPort)
	{
		case 161:
		case 514:
		case 1194:
		case 2268:
		case 4500:
		case 5246:
		case 7707:
		case 7708:
		case 8801:
		case 40975:
			msgObj->dropPacket = true;
			break;
	}

	switch(msgObj->udp.ipVer)
	{
		case IPVersion4:
			msgObj->udp.ipv4FlowId = pUt->getIpv4SessionKey(msgObj->ptype, msgObj->udp.direction, msgObj->udp.sIp, msgObj->udp.dIp, msgObj->udp.sPort, msgObj->udp.dPort);
			break;

		case IPVersion6:
			/* Not process DNS for IpV6 Traffic */
			if((msgObj->udp.sPort == DNS_PORT) || (msgObj->udp.dPort == DNS_PORT))
			{
				msgObj->ptype = 0;
				msgObj->dropPacket = true;
				return;
			}
			break;
		default:
			return;
			break;
	}

	if((msgObj->udp.sPort == DNS_PORT) || (msgObj->udp.dPort == DNS_PORT))
	{
    	if((IPGlobal::DNS_ANSWER == 0) || (msgObj->udp.direction == 0) || (msgObj->udp.direction == UNKNOWN))
    	{
    		msgObj->dropPacket = true;
    		return;
    	}

    	lockDnsMap();

    	msgObj->dns.ipv4FlowId 	= msgObj->udp.ipv4FlowId;
    	msgObj->dns.ipVer 		= msgObj->udp.ipVer;
    	msgObj->dns.sIp 		= msgObj->udp.sIp;
    	msgObj->dns.dIp 		= msgObj->udp.dIp;
    	msgObj->dns.sPort 		= msgObj->udp.sPort;
    	msgObj->dns.dPort 		= msgObj->udp.dPort;

    	parsePacketDNS(packet + UDP_HDR_LEN, msgObj, udpTLen); // Total Length of UDP message (8)

   		if(msgObj->dns.qrFlag == QUERY || msgObj->dns.qrFlag == RESPONSE)
   		{ msgObj->ptype = PACKET_IPPROTO_DNS; }

    	unLockDnsMap();
	}
	else
	{

		msgObj->udp.pLoad = msgObj->udp.ipTLen - (msgObj->udp.ipHLen + UDP_HDR_LEN);

		if(msgObj->udp.pLoad > 0 && msgObj->udp.pLoad >= IPGlobal::MAX_TCP_SIZE)
			msgObj->udp.pLoad = IPGlobal::MAX_TCP_SIZE;
	}
}


void UDPParser::parsePacketDNS(const BYTE packet, MPacket *msgObj, uint16_t protoTLen)
{
    uint32_t pos = 0, id_pos = 0, retPos = 0;

	if (msgObj->udp.frSize - protoTLen < 12)
	{ return; }

    dnsTLen 					= protoTLen - UDP_HDR_LEN;
    msgObj->dns.transactionId 	= (packet[pos] << 8) + packet[pos+1];		// Transaction ID
    msgObj->dns.qrFlag 			= packet[pos+2] >> 7;					    // Query Response -> Question = 0 and Answer = 1

//    authoritative = (packet[pos+2] & 0x04) >> 2;
//    truncatedFlag = (packet[pos+2] & 0x02) >> 1;							// Truncated Message Flag

    /*
     * RCODE = 0 - No Error, 1- Format Error, 2- Server Error, 3- Name Error, 4- Not Implemented, 5- Refused.
     */

    switch(msgObj->dns.qrFlag)
    {
		case QUERY:
				qdcount = (packet[pos+4] << 8) + packet[pos+5];			// Query Count

				if(qdcount == 1)
				{
					if(!parsePacketDNSQueries(pos + DNS_HDR_LEN, id_pos, msgObj, packet, &retPos))
						return;
				}
				break;

		case RESPONSE:
				qdcount = (packet[pos+4] << 8) + packet[pos+5];			// Query Count
				ancount = (packet[pos+6] << 8) + packet[pos+7];			// Answer Count

				msgObj->dns.responseCode = packet[pos + 3] & 0x0f;		// rcode will be there in case of Response (Answer = 1)

				if (msgObj->dns.responseCode != 0) // Earlier 26
					return;

				if(ancount >= IPGlobal::DNS_ANSWER)
					ancount = IPGlobal::DNS_ANSWER;

				if(qdcount == 1 && (ancount > 0 && ancount <= IPGlobal::DNS_ANSWER))
				{
					if(parsePacketDNSQueries((pos + DNS_HDR_LEN), id_pos, msgObj, packet, &retPos))
					{	if(msgObj->dns.responseCode == 0)
							parsePacketDNSAnswers(retPos, msgObj, packet);
					}
					else
						return;
				}
				break;

		default:
				msgObj->ptype = 0;
				msgObj->dropPacket = false;
				return;
				break;
//				msgObj->udp.qrFlag 			= 3;		// Query Response -> Question=0 and Answer=1
//				qdcount = ancount 			= 0;
//				msgObj->udp.transactionId 	= 3;		// Transaction ID
//				break;
    }
}

bool UDPParser::parsePacketDNSQueries(uint32_t pos, uint32_t id_pos, MPacket *msgObj, const BYTE packet, uint32_t *retPos)
{
    uint16_t type = 0;
    std::string url;

    url = read_rr_name(packet, &pos, id_pos, dnsTLen);
    std::replace(url.begin(), url.end(), ',', '.');

    if (url.compare("NULL") == 0)
    { return false; }

    if(url.length() >= URL_LEN)
    {
    	url = url.substr(url.length() - (URL_LEN - 1));
    	strcpy(msgObj->dns.url, url.c_str());
    }
    else
    { strcpy(msgObj->dns.url, url.c_str()); }

    url.clear();

    type = VAL_USHORT(packet + pos);

    if(type == 255) return false;	// 255 is for Any Ip Address

    *retPos = pos + 4;
    return true;
}

void UDPParser::parsePacketDNSAnswers(uint32_t pos, MPacket *msgObj, const BYTE packet)
{
	uint16_t type, dataLen, ttl;
	char ipv6ResolvedIp[INET6_ADDRSTRLEN];

	dataLen = ttl = 0;

	try
	{
		for(uint16_t ansCounter = 0; ansCounter < ancount; ansCounter++)
		{
			while(packet[pos] != 192) { // Reference Question Name Start with '0xc0' locate it
				pos += 1;
			}

			pos = pos + 2;												// Reference Question Name (2 Bytes)
			type = (packet[pos] << 8) + packet[pos + 1];

			pos = pos + 2;												// Type
			pos = pos + 2;												// Class

			pos = pos + 4;												// TTL

			dataLen = (packet[pos] << 8) + packet[pos + 1];
			pos = pos + 2;												// Data Length

			uint32_t longResolvedIp = 0;

			switch(type)
			{
				case A:	/* IP4 Address */
					msgObj->dns.responseCode = 0;

					longResolvedIp=(longResolvedIp << 8) + (0xff & packet[pos]);
					longResolvedIp=(longResolvedIp << 8) + (0xff & packet[pos + 1]);
					longResolvedIp=(longResolvedIp << 8) + (0xff & packet[pos + 2]);
					longResolvedIp=(longResolvedIp << 8) + (0xff & packet[pos + 3]);

					/* If Resolved Ip is 1.0.0.0 ~ 255.255.255.255 */
					if((longResolvedIp >= 16777216 && longResolvedIp <= 4294967295) && strlen(msgObj->dns.url) > 0)
					{ updateDns(longResolvedIp, std::string(msgObj->dns.url)); }

					pos = pos + dataLen;

					break;

				case AAAA: /* IP6 Address */
//					msgObj->udp.responseCode = 0;
//					pUt->ExtractIP6Address(packet, ipv6ResolvedIp, pos);
//
//					updateDnsV6(std::string(ipv6ResolvedIp), std::string(msgObj->udp.url));
//
//					pos = pos + dataLen;
//					ipv6ResolvedIp[0] = 0;
					break;

				default:
					pos = pos + dataLen;
					break;
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cout << " a standard exception was caught, with message '" << e.what() << "'\n";
	}
}

string UDPParser::read_rr_name(const BYTE packet, uint32_t * packet_p, uint32_t id_pos, uint16_t len)
{
    uint32_t i, next, pos=*packet_p;
    uint32_t end_pos = 0;
    uint32_t name_len=0;
    uint32_t steps = 0;

    next = pos;

    while (pos < len && !(next == pos && packet[pos] == 0) && steps < len*2)
    {
        char c = packet[pos];
        steps++;
        if (next == pos)
        {
            if ((c & 0xc0) == 0xc0)
            {
                if (pos + 1 >= len)
                { return "NULL"; }

                if (end_pos == 0)
                	end_pos = pos + 1;

                pos = id_pos + ((c & 0x3f) << 8) + packet[pos+1];
                next = pos;
            }
            else
            {
                name_len++;
                pos++;
                next = next + c + 1;
            }
        }
        else
        {
            if (c >= '!' && c <= 'z' && c != '\\')
            	name_len++;
            else
            	name_len += 4;

            pos++;
        }
    }
    if (end_pos == 0)
    	end_pos = pos;

    if (steps >= 2*len || pos >= len)
    	return "NULL";

    name_len++;

    if(name_len > len *2)
    	return "NULL";

    string name;
    pos = *packet_p;

    next = pos;
    i = 0;

    while (next != pos || packet[pos] != 0)
    {
        if (pos == next)
        {
            if ((packet[pos] & 0xc0) == 0xc0)
            {
                pos = id_pos + ((packet[pos] & 0x3f) << 8) + packet[pos+1];
                next = pos;
            }
            else
            {
                if (i != 0) name.append(1,'.');i++;
                next = pos + packet[pos] + 1;
                pos++;
            }
        }
        else
        {
            char c = packet[pos];

            if (c >= '!' && c <= '~' && c != '\\')
            {
                name.append(1, (char) c);
                i++; pos++;
            }
            else
            { return "NULL"; }
        }
    }
    *packet_p = end_pos + 1;
    return name;
}
