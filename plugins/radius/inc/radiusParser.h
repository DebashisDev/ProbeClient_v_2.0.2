/*
 * radiusTataSkyParser.h
 *
 *  Created on: Nov 10, 2017
 *      Author: Debashis
 */

#ifndef PLUGINS_RADIUS_SRC_RADIUSPARSER_H_
#define PLUGINS_RADIUS_SRC_RADIUSPARSER_H_

#include "IPGlobal.h"
#include "SpectaTypedef.h"
#include "ProbeUtility.h"
#include "Log.h"
#include "BaseConfig.h"

#define ACCESS_REQUEST			1
#define ACCESS_ACCEPT			2
#define ACCESS_REJECT			3

#define ACCOUNTING_REQUEST		4
#define ACCOUNTING_RESPONSE		5

#define	ACCOUNTING_START		1
#define	ACCOUNTING_STOP			2
#define ACCOUNTING_UPDATE		3

#define FLUSH_REQ_RSP			30
#define FLUSH_RSP_REQ			31
#define FLUSH_DUPLICATE			32
#define FLUSH_CLEANUP			33

#define AUTHENTICATOR_LEN		16

/* Definition of Attributes (RFC 2865 & 2866) */

#define USER_NAME				1
#define USER_PASSWORD			2
#define CHAP_PASSWORD			3
#define NAS_IP_ADDRESS			4
#define NAS_PORT				5
#define SERVICE_TYPE			6
#define FRAMED_PROTOCOL			7
#define FRAMED_IP_ADDRESS		8
#define FRAMED_IP_NETMASK		9
#define FRAMED_ROUTING			10
#define FILTER_ID				11
#define FRAMED_MTU				12
#define FRAMED_COMPRESSION		13
#define LOGIN_IP_HOST			14
#define LOGIN_SERVICE			15
#define LOGIN_TCP_PORT			16
//(UNASSIGNED)					17
#define REPLY_MESSAGE			18
#define CALLBACK_NUMBER			19
#define CALLBACK_ID				20
//(UNASSIGNED)					21
#define FRAMED_ROUTE			22
#define FRAMED_IPX_NETWORK		23
#define STATE					24
#define CLASS					25
#define VENDOR_SPECIFIC			26
#define SESSION_TIMEOUT			27
#define IDLE_TIMEOUT			28
#define TERMINATION_ACTION		29
#define CALLED_STATION_ID		30
#define CALLING_STATION_ID		31
#define NAS_IDENTIFIER			32
#define PROXY_STATE				33
#define LOGIN_LAT_SERVICE		34
#define LOGIN_LAT_NODE			35
#define LOGIN_LAT_GROUP			36
#define FRAMED_APPLETALK_LINK	37
#define FRAMED_APPLETALK_NETWORK 38
#define FRAMED_APPLETALK_ZONE	39
#define ACCT_STATUS_TYPE		40
#define ACCT_DELAY_TIME			41
#define ACCT_INPUT_OCTETS		42
#define ACCT_OUTPUT_OCTETS		43
#define ACCT_SESSION_ID			44
#define ACCT_AUTHENTIC			45
#define ACCT_SESSION_TIME		46
#define ACCT_INPUT_PACKETS		47
#define ACCT_OUTPUT_PACKETS		48
#define ACCT_TERMINATE_CAUSE	49
#define ACCT_MULTI_SESSION_ID	50
#define ACCT_LINK_COUNT			51
#define ACCT_INPUT_GIGAWORDS	52
#define ACCT_OUTPUT_GIGAWORDS	53
#define CHAP_CHALLENGE			60
#define NAS_PORT_TYPE			61
#define PORT_LIMIT				62
#define LOGIN_LAT_PORT			63
#define DELEGATED_IPV6_PREFIX	123

/* VSA (Vendor Specific Attributes) Type */
#define USER_AGENT_CIRCUIT_ID	1	/* OLT 	*/
#define SUBSC_ID				11
#define SUBSC_PROF				12	/* Subscriber Profile 	*/
#define SLA_PROF				13	/* Subscriber Plan 		*/
#define CLIENT_HW_ADDR			27	/* Subscriber MAC Address */

//#define CALCULATE_SUBNETMASK(subnetMask) ((subnetMask / 16) * 2)
#define CALCULATE_SUBNETMASK(subnetMask) ((subnetMask / 15) * 4)

#define	OCTATE 					1
#define INVALID					6

class radiusParser : public BaseConfig, public ProbeUtility
{
	private:
		BYTE 	packet;
		void    hexDump(const void* pv, uint16_t len);
		bool	oltFlag;
		bool	process;

	public:
		radiusParser();
		~radiusParser();

		uint16_t parseAVPType(const BYTE packet, size_t offset, uint16_t remLen, MPacket *msgObj);

		string getAVPValue(uint16_t len, size_t bodyOffset, const BYTE packet);
		void parseAAAPacket(const BYTE packet, MPacket *msgObj);

};

#endif /* PLUGINS_RADIUS_SRC_RADIUSPARSER_H_ */
