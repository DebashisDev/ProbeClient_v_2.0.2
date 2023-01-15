/*
 * PTCP.h
 *
 *  Created on: Nov 29, 2015
 *      Author: debashis
 */

#ifndef INC_TCPPROBE_H_
#define INC_TCPPROBE_H_

#include "SpectaTypedef.h"
#include "IPGlobal.h"
#include "TCPUDPGlobal.h"
#include "ProbeUtility.h"

#include <vector>
#include <string>
#include <sstream>

using namespace std;

#define TCPHDR 20
#define MAX_TCP_PAYLOAD 20

#define LF              10
#define CR				13
#define COMMA			44
#define SEMICOLON		59

typedef enum {
    SSL_V_1         = 769,	//0x03, 0x01
    SSL_V_1_2       = 771	//0x03, 0x03
} sslVersion;

class TCPParser
{
	private:
		uint16_t 		psh, rst, syn, fin, window, ack, ackNo;
		uint16_t 		protoHLen;
		ProbeUtility	*pUt;

	public:
		TCPParser();
		~TCPParser();

		void parseTCPPacket(const BYTE packet, MPacket *msgObj);
		void checkAgentType(BYTE packet, MPacket *msgObj);
};

#endif  /* INC_TCPPROBE_H_ */
