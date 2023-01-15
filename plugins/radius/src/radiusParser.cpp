/*
 * radiusLink3Parser.cpp
 *
 *  Created on: Nov 10, 2017
 *      Author: Debashis
 */

#include "radiusParser.h"

#include <algorithm>

using namespace std;

radiusParser::radiusParser()
{
	packet = NULL;
	oltFlag = false;
	process = false;
}

radiusParser::~radiusParser()
{ }

void radiusParser::hexDump(const void* pv, uint16_t len)
{
  const BYTE p = (const BYTE) pv;
  uint16_t i;

  for(i = 0; i < len; ++i )
  {
    const char* eos;

    switch( i & 15 )
    {
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

void radiusParser::parseAAAPacket(const BYTE packet, MPacket *msgObj)
{
	size_t offset = 0;
	uint16_t avpLen = 0, radiusRemainingLen = 0;

	process = false;

	this->packet = packet + UDP_HDR_LEN; // 8 bytes -- UDP Header Size

	msgObj->aaa.code = VAL_BYTE(this->packet + offset);
	offset += 1;

	if(msgObj->aaa.code < ACCESS_REQUEST && msgObj->aaa.code > ACCOUNTING_RESPONSE)
	{
		msgObj->processOutOfRange = 0;
		return;
	}

	msgObj->aaa.identifier = VAL_BYTE(this->packet + offset);

	offset += 1;	// Packet Identifier 1 Byte

	uint16_t rediusLen = VAL_USHORT(this->packet + offset);

	offset += 2;	// Radius Length 2 Byte

	/* implemented in case of YouBB */
	uint16_t authenticator = VAL_BYTE(this->packet + offset);

	if(authenticator == 0)
	{
		msgObj->processOutOfRange = 0;
		return;
	}
	offset += AUTHENTICATOR_LEN;

	if(rediusLen < msgObj->aaa.frByteLen)
		radiusRemainingLen = rediusLen - offset; //bodyOffset;
	else
		radiusRemainingLen = msgObj->aaa.frByteLen - offset;

	while(radiusRemainingLen > 0 && avpLen >= 0)
	{
		avpLen = parseAVPType(this->packet, offset, radiusRemainingLen, msgObj); //Attribute-Value Pairs (AVP)

		/* Some Error Indecording or Wrong Radius Packet */
		if(avpLen == 0)
			{
				if(process == false && msgObj->aaa.code == 4) msgObj->processOutOfRange = 0;
				return;
			}

		radiusRemainingLen -= avpLen;

		offset += avpLen;
	}
}

uint16_t radiusParser::parseAVPType(const BYTE packet, size_t bodyOffset, uint16_t remLen, MPacket *msgObj)
{
	std::string value;
	uint16_t iVal;
	bool dumpFlag = true;

	uint8_t addr[16];
	char userId[RADIUS_USER_NAME_LEN];
	uint16_t userIdLen = 0;

	addr[0] = userId[0] = 0;

	uint16_t type = VAL_BYTE(this->packet + bodyOffset);
	bodyOffset += 1;		/* Type Length */
	uint16_t length = VAL_BYTE(this->packet + bodyOffset);

	if(remLen < length) return -1;

	bodyOffset += 1;	/* Length */

	switch(type)
	{
#if 0
		case USER_NAME: /* userNameLen ( >= 3 and <= 32 ) */
				if(length > 3 && length <= 32)
				{
                    uint16_t check = VAL_BYTE(this->packet + bodyOffset);
					if(check >= 32)		// Avoid non-printable charactors
					{
						std::string name = getAVPValue(length - 2, bodyOffset, packet);
						std::replace(name.begin(), name.end(), ',', '.');

						strcpy(msgObj->aaa.userName, name.c_str());
					}
				}
				else
				{ return 0; }

				break;
#endif

		case USER_NAME: /* userNameLen ( >= 3 and <= 32 ) */
				if(length > 3 && length <= 32)
				{
                    uint16_t check = VAL_BYTE(this->packet + bodyOffset);
					if(check >= 32)		// Avoid non-printable charactors
					{
						std::string name = getAVPValue(length - 2, bodyOffset, packet);
						std::replace(name.begin(), name.end(), ',', '.');
						strcpy(userId, name.c_str());

						for(userIdLen = 0; userIdLen < strlen(userId); userIdLen++)
						{
							if(userId[userIdLen] < 32 || userId[userIdLen] > 126)
							{
								 dumpFlag = false;
								 break;
							}
						}
						if(dumpFlag)
							strcpy(msgObj->aaa.userName, userId);
						else
							return 0;
					}
				}
				else
				{ return 0; }

				break;

		case SERVICE_TYPE:
					if(length != 6)
						return 0;

					msgObj->aaa.serviceType = VAL_ULONG(packet + bodyOffset);
					break;

		case FRAMED_PROTOCOL:
					if(length != 6)
						return 0;

					msgObj->aaa.protocol = VAL_ULONG(packet + bodyOffset);
					break;

		case FRAMED_IP_ADDRESS:
					if(length != 6)
						return 0;

					msgObj->aaa.framedIp =(msgObj->aaa.framedIp << 8) + (0xff & packet[bodyOffset]);
					msgObj->aaa.framedIp=(msgObj->aaa.framedIp << 8) + (0xff & packet[bodyOffset + 1]);
					msgObj->aaa.framedIp=(msgObj->aaa.framedIp << 8) + (0xff & packet[bodyOffset + 2]);
					msgObj->aaa.framedIp=(msgObj->aaa.framedIp << 8) + (0xff & packet[bodyOffset + 3]);

					process = true;
					break;

		case REPLY_MESSAGE:
					value.clear();

					if(length > 3 && length <= 50)
					{
						for(uint16_t count = 0; count < (length - 2); count++)
						{
							if(isprint(packet[bodyOffset + count]) != 0)
								value.append(1, packet[bodyOffset + count]);
						}
						strcpy(msgObj->aaa.replyMsg, value.c_str());
					}
					else
					{ return 0; }

					break;

		case ACCT_TERMINATE_CAUSE:
					if(length != 6)
						return 0;

					msgObj->aaa.terminationCause = VAL_ULONG(packet + bodyOffset);
					break;

		case ACCT_STATUS_TYPE:
					if(length != 6)
						return 0;

					msgObj->aaa.accStatusType = VAL_ULONG(packet + bodyOffset);
					break;

		case ACCT_AUTHENTIC:
					if(length != 6)
						return 0;

					msgObj->aaa.accAuth = VAL_ULONG(packet + bodyOffset);
					break;

		case ACCT_INPUT_OCTETS:
					{
						if(length != 6)
							return 0;

						msgObj->aaa.inputOctets = VAL_ULONG(packet + bodyOffset);
					}
					break;

		case ACCT_OUTPUT_OCTETS:
					{
						if(length != 6)
							return 0;

						msgObj->aaa.outputOctets = VAL_ULONG(packet + bodyOffset);
					}
					break;

		case ACCT_INPUT_PACKETS:
					{
						if(length != 6)
							return 0;

						msgObj->aaa.inputPackets = VAL_ULONG(packet + bodyOffset);
					}
					break;

		case ACCT_OUTPUT_PACKETS:
					{
						if(length != 6)
							return 0;

						msgObj->aaa.outputPackets = VAL_ULONG(packet + bodyOffset);
					}
					break;

		case ACCT_INPUT_GIGAWORDS:
					{
						if(length != 6)
							return 0;

						msgObj->aaa.inputGigaWords = VAL_ULONG(packet + bodyOffset);
					}
					break;

		case ACCT_OUTPUT_GIGAWORDS:
					{
						if(length != 6)
							return 0;

						msgObj->aaa.outputGigaWords = VAL_ULONG(packet + bodyOffset);
					}
					break;

		default:
				break;
	}
	return length;
}


string radiusParser::getAVPValue(uint16_t len, size_t bodyOffset, const BYTE packet)
{
	std::string val;
	val.clear();

	for(uint16_t count = 0; count < len; count++)
	{ val.append(1, packet[bodyOffset + count]); }

	return val;
}
