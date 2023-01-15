/*
 * TCPProbe.cpp
 *
 *  Created on: Nov 14, 2015
 *      Author: debashis
 */

#include <pthread.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <ctype.h>
#include <algorithm>
#include <string>

#include "TCPParser.h"

using namespace std;

TCPParser::TCPParser()
{
	psh = rst = syn = fin = window = ack = ackNo = 0;
	protoHLen = 0;
	this->pUt 			= new ProbeUtility();
}

TCPParser::~TCPParser()
{ delete(this->pUt); }

void TCPParser::parseTCPPacket(const BYTE packet, MPacket *msgObj)
{ 
	tcphdr 		*tcpHeader;
	msgObj->tcp.pLoad = 0;

	tcpHeader = (struct tcphdr *)(packet);

	protoHLen = ((tcpHeader->doff) << 2);
	msgObj->tcp.sPort = ntohs((unsigned short int)tcpHeader->source);
	msgObj->tcp.dPort = ntohs((unsigned short int)tcpHeader->dest);
	msgObj->tcp.pLoad = msgObj->tcp.ipTLen - (msgObj->tcp.ipHLen + protoHLen);

//	5000 	-  RSL
//	56680 	- MPTCP
//	6881 	- BitTorrent
//	17500 	- DB-LSP
//	3306	- MySQL

//	switch(msgObj->tcp.sPort)
//	{
//		case 5000:
//		case 56680:
//		case 6881:
//		case 17500:
//		case 3360:
//		case 514:
//				msgObj->processFlag = true;
//				break;
//	}
//
//	switch(msgObj->tcp.dPort)
//	{
//		case 5000:
//		case 56680:
//		case 6881:
//		case 17500:
//		case 3360:
//		case 514:
//				msgObj->processFlag = true;
//				break;
//	}

	switch(msgObj->tcp.ipVer)
	{
		case IPVersion4:
			msgObj->tcp.ipv4FlowId = pUt->getIpv4SessionKey(msgObj->ptype, msgObj->tcp.direction, msgObj->tcp.sIp, msgObj->tcp.dIp, msgObj->tcp.sPort, msgObj->tcp.dPort);
			break;

		case IPVersion6:
			/* IPv6 FlowId is generated in Session Manager */
			break;
	}


	if(msgObj->tcp.pLoad > 0 && msgObj->tcp.pLoad >= IPGlobal::MAX_TCP_SIZE)
		msgObj->tcp.pLoad = IPGlobal::MAX_TCP_SIZE;

	if((msgObj->tcp.sPort == DNS_PORT) || (msgObj->tcp.dPort == DNS_PORT))
	{
		msgObj->dropPacket = true;
		return;
	}

	msgObj->tcp.tcpSeqNo = VAL_ULONG(packet + 4);

	ack = tcpHeader->ack;
	psh = tcpHeader->psh;
	rst = tcpHeader->rst;
	syn = tcpHeader->syn;
	fin = tcpHeader->fin;

	/* ** Connection Request ** */
	if((syn) && (!ack) && (!psh) && (!fin))
	{ msgObj->tcp.tcpFlags = SYN_RCV; msgObj->tcp.pLoad = 0; }

	/* ** Connection Request with Response ** */
	else if((syn) && (ack) && (!psh) && (!fin))
	{ msgObj->tcp.tcpFlags = SYN_ACK_RCV; msgObj->tcp.pLoad = 0; }

	/* ** Connection Complete ** */
   	else if((!syn) && (ack) && (!rst) && (!fin) && (!psh))
	{
   		msgObj->tcp.tcpFlags = ACK_RCV;
	}

	/* ** Data Complete ** */
   	else if(psh)
	{ msgObj->tcp.tcpFlags = DATA_RCV; }

	/* ** Disconnect Request ** */
	else if(fin || rst)
	{ msgObj->tcp.tcpFlags = FIN_RCV; msgObj->tcp.pLoad = 0; }

	/* This should never happen, but in case */
	else
	{
		msgObj->ptype		= 0;
		msgObj->dropPacket = true;
		return;
	}

	if((msgObj->tcp.tcpFlags == DATA_RCV) && (msgObj->tcp.pLoad > 0))
	{
		if(msgObj->tcp.dPort == 80 && IPGlobal::PROCESS_USER_AGENT == true)
			checkAgentType(packet + protoHLen, msgObj);
	}

	/* ---------------- End of Session Management --------------------- */

	tcpHeader = NULL;
}

void TCPParser::checkAgentType(BYTE packet, MPacket *msgObj)
{
	int i, posIndex;
	const u_char *ch;
	std::string buffer, httpRspHdr;
	int length = 3;
	bool doFlag = false;

	msgObj->tcp.httpAgent[0] = 0;

	string::iterator it;

	buffer.clear();
	httpRspHdr.clear();

	posIndex = 0;

	ch = packet;

	for(i = 0; i < length; i++)
	{
		httpRspHdr.append(1, *ch);
		ch++;
	}

	std::size_t pos = httpRspHdr.find("GET");

	if(pos != std::string::npos)
		doFlag = true;
	else
		return;

	int len = msgObj->tcp.pLoad - length;

	if(doFlag)
	{
		for(i = 0; i < len; i++)
		{
			if(*ch != CR)
			{
				if(*ch == COMMA)
					buffer.append(1, ';');
				else
					buffer.append(1, *ch);
				posIndex ++;
				ch++;
			}
			else
			{
				std::size_t pos = buffer.find("User-Agent:");

				if(pos != std::string::npos)
					strncpy(msgObj->tcp.httpAgent, buffer.c_str(), (HTTP_AGENT_LEN - 1));

				ch += 2;
				buffer.clear();
			} // Else
		}	// For Loop
	}	// End of If Condition
}
