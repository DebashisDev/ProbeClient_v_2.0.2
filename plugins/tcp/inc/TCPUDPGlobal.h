/*
 * TCPUDPGlobal.h
 *
 *  Created on: 15-Jul-2016
 *      Author: Debashis
 */

#ifndef PLUGINS_TCP_INC_TCPUDPGLOBAL_H_
#define PLUGINS_TCP_INC_TCPUDPGLOBAL_H_


#include <map>

#include "SpectaTypedef.h"

using namespace std;

#define DNS_HDR_LEN		12
#define IP4LEN			4
#define IP6LEN			16
#define	STUN_PORT		3478
#define UDP_NO_ERROR	0

#define DIAMETER_SEQ_ID	263

typedef enum
{
	SYN_RCV = 1,
	SYN_ACK_RCV,
	ACK_RCV,
	CONNECTED,
	DATA_RCV,
	FIN_RCV,
}IPState;

namespace DNSGlobal
{ extern std::map<uint32_t, std::string> dnsLookUpMap[10]; }

#endif /* PLUGINS_TCP_INC_TCPUDPGLOBAL_H_ */
