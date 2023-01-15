/*
 * PacketRouter.cpp
 *
 *  Created on: Nov 22, 2016
 *      Author: Debashis
 */

#include <math.h>
#include "PacketRouter.h"

PacketRouter::PacketRouter(uint16_t intfid, uint16_t rid, uint16_t coreid)
{
	this->_name = "PacketRouter   ";
	this->setLogLevel(Log::theLog().level());

	this->intfId 			= intfid;
	this->routerId 			= rid;
	this->coreId 			= coreid;
	this->repoInitStatus	= false;
	this->curMin			= 0;
	this->prevMin			= 0;
	this->curHour			= 0;
	this->prevHour			= 0;
	this->printCnt 			= 0;

	this->MAX_PKT_LEN 		= IPGlobal::MAX_PKT_LEN_PER_INTERFACE[this->intfId];

	this->bwData  			= new BWData(this->intfId, this->routerId);
	this->cdnData 			= new CDNData(this->intfId, this->routerId);
	this->peeringData		= new PeeringData(this->intfId, this->routerId);
	this->ethParser 		= new EthernetParser(intfId, rid);
	this->msgObj 			= new MPacket();
	this->rawPkt 			= new RawPkt(MAX_PKT_LEN);
}

PacketRouter::~PacketRouter()
{
	delete(this->bwData);
	delete(this->cdnData);
	delete(this->peeringData);
	delete(this->ethParser);
	delete(this->msgObj);
	delete(this->rawPkt);
}

bool PacketRouter::isRepositoryInitialized()
{ return repoInitStatus; }

void PacketRouter::run()
{
	uint16_t lastTIndex, currTIndex;

	IPGlobal::MAX_PACKET_PUSH_SOCKET = (int)(((IPGlobal::PPS_PER_INTERFACE[intfId] / IPGlobal::ROUTER_PER_INTERFACE[intfId]) /100 ) * IPGlobal::PPS_CAP_PERCENTAGE[intfId]);

	TheLog_nc_v3(Log::Info, name(),"Packet Router [%d::%d] Packet to be Pushed -> %u", intfId, routerId, IPGlobal::MAX_PACKET_PUSH_SOCKET);

	curMin = prevMin = IPGlobal::CURRENT_MIN;
	curHour = prevHour = IPGlobal::CURRENT_HOUR;

	lastTIndex = currTIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	repoInitStatus = true;

	while(IPGlobal::PKT_ROUTER_RUNNING_STATUS[intfId][routerId])
	{
		usleep(IPGlobal::THREAD_SLEEP_TIME);

		currTIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC,IPGlobal::TIME_INDEX);

		curMin = IPGlobal::CURRENT_MIN;

		if(prevMin != curMin)		//Min changed
		{
			bwData->setBWData(prevMin);

			if(IPGlobal::PROCESS_CDN)
				cdnData->setCDNData(prevMin);

			if(IPGlobal::PROCESS_PEERING)
				peeringData->setPeeringData(prevMin);

			prevMin = curMin;
		}

		while(lastTIndex != currTIndex)
		{
			processQueue(lastTIndex);
			lastTIndex = PKT_READ_NEXT_TIME_INDEX(lastTIndex, IPGlobal::TIME_INDEX);
		}

		/* Stop Processing Packets for 1 Min during 11:59:00 Hours */

/*
		if(IPGlobal::CURRENT_HOUR == IPGlobal::END_OF_DAY_CLEAN_HOUR && IPGlobal::CURRENT_MIN == IPGlobal::END_OF_DAY_CLEAN_MIN && IPGlobal::CURRENT_SEC == IPGlobal::END_OF_DAY_CLEAN_SEC)
		{
			TheLog_nc_v3(Log::Info, name(),"  Time[%02d:%02d:%02d] Stop Pushing Packet..", IPGlobal::CURRENT_HOUR, IPGlobal::CURRENT_MIN, IPGlobal::CURRENT_SEC);

			if(this->intfId == 0 && this->routerId == 0)
			{
				for(uint16_t infCounter = 0; infCounter < IPGlobal::NO_OF_INTERFACES; infCounter++)
					IPGlobal::PUSH_TO_QUEUE[infCounter] = false;

				sleep(30);
				TheLog_nc_v3(Log::Info, name(),"  Time[%02d:%02d:%02d] Resume Pushing Packet..", IPGlobal::CURRENT_HOUR, IPGlobal::CURRENT_MIN, IPGlobal::CURRENT_SEC);

				for(uint16_t infCounter = 0; infCounter < IPGlobal::NO_OF_INTERFACES; infCounter++)
					IPGlobal::PUSH_TO_QUEUE[infCounter] = true;
			}
		}
*/
	}
	printf("  PacketRouter [%d::%d] Stopped...\n",intfId, routerId);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void PacketRouter::processQueue(uint16_t idx)
{ processQueueDecode(PKTStore::pktRepoBusy[intfId][routerId][idx], PKTStore::pktRepoCnt[intfId][routerId][idx], PKTStore::pktRepository[intfId][routerId][idx]); }

void PacketRouter::processQueueDecode(bool &pktRepository_busy, uint32_t &pktRepository_cnt, std::unordered_map<uint32_t, RawPkt*> &pktRepository)
{
	uint32_t recCnt = pktRepository_cnt;
	pktRepository_busy = true;

	if(recCnt > 0)
	{
		for(uint32_t i = 0; i < recCnt; i++)
		{
			decodePacket(pktRepository[i]);
			pktRepository_cnt--;
		}
		pktRepository_cnt = 0;
	}
	pktRepository_busy = false;
}

void PacketRouter::decodePacket(RawPkt* rawPkt)
{
	bool flag = false;

	if(rawPkt->pkt != NULL)
	{
		msgObj->reset();

	    ethParser->parsePacket(rawPkt->pkt, msgObj, rawPkt->len);

	    switch(msgObj->ptype)
	    {
	    	case PACKET_IPPROTO_TCP:
	    	{
	    		IPGlobal::TCP_PACKETS_PER_DAY[this->intfId][this->routerId] ++;
				msgObj->tcp.frTimeEpochSec = rawPkt->tv_sec;
				msgObj->tcp.frTimeEpochNanoSec = rawPkt->tv_nsec;

				switch(msgObj->tcp.direction)
				{
					case 0:
					case UNKNOWN:
						bwData->updateBWData(curMin, msgObj->tcp.frSize, DOWN, msgObj->tcp.frTimeEpochSec);

						if(IPGlobal::PROCESS_CDN && msgObj->tcp.ipVer == IPVersion4)
						{
							if(checkCDNV4(msgObj->tcp.dIp, curMin, msgObj->tcp.frSize, DOWN, msgObj->tcp.frTimeEpochSec))
								flag = true;
							else
								flag = checkCDNV4(msgObj->tcp.sIp, curMin, msgObj->tcp.frSize, DOWN, msgObj->tcp.frTimeEpochSec);
						}

						if(IPGlobal::PROCESS_PEERING && !flag)
						{
							if(checkPeering(msgObj->tcp.dIp))
								peeringData->updatePeeringData(curMin, msgObj->tcp.frSize, DOWN, msgObj->tcp.frTimeEpochSec);
							else if(checkPeering(msgObj->tcp.sIp))
								peeringData->updatePeeringData(curMin, msgObj->tcp.frSize, DOWN, msgObj->tcp.frTimeEpochSec);
						}

						if(msgObj->dropPacket || IPGlobal::TCP_BLOCK_PACKET) return;

						if(IPGlobal::PROCESS_OUT_OF_RANGE_IP[intfId])
							msgObj->processOutOfRange = true;
						else
							msgObj->processOutOfRange = false;

						break;

					case UP:
						bwData->updateBWData(curMin, msgObj->tcp.frSize, msgObj->tcp.direction, msgObj->tcp.frTimeEpochSec);

						if(IPGlobal::PROCESS_CDN && msgObj->tcp.ipVer == IPVersion4)
							flag = checkCDNV4(msgObj->tcp.dIp, curMin, msgObj->tcp.frSize, UP, msgObj->tcp.frTimeEpochSec);

						if(IPGlobal::PROCESS_PEERING && !flag)
							if(checkPeering(msgObj->tcp.dIp))
								peeringData->updatePeeringData(curMin, msgObj->tcp.frSize, UP, msgObj->tcp.frTimeEpochSec);

						if(msgObj->dropPacket || IPGlobal::TCP_BLOCK_PACKET) return;

						break;

					case DOWN:
						bwData->updateBWData(curMin, msgObj->tcp.frSize, msgObj->tcp.direction, msgObj->tcp.frTimeEpochSec);

						if(IPGlobal::PROCESS_CDN && msgObj->tcp.ipVer == IPVersion4)
							flag = checkCDNV4(msgObj->tcp.sIp, curMin, msgObj->tcp.frSize, DOWN, msgObj->tcp.frTimeEpochSec);

						if(IPGlobal::PROCESS_PEERING && !flag)
							if(checkPeering(msgObj->tcp.sIp))
								peeringData->updatePeeringData(curMin, msgObj->tcp.frSize, DOWN, msgObj->tcp.frTimeEpochSec);

						if(msgObj->dropPacket || IPGlobal::TCP_BLOCK_PACKET) return;

						break;
				}

				if(msgObj->tcp.tcpFlags == ACK_RCV && msgObj->tcp.pLoad <= IPGlobal::TCP_ACK_SIZE_BLOCK[this->intfId])
					return;

				if(IPGlobal::PUSH_TO_QUEUE[intfId] && msgObj->processOutOfRange)
					pushTcpToAgentQueue(msgObj);
	    	}
	    	break;

	    	case PACKET_IPPROTO_UDP:
	    	{
	    		IPGlobal::UDP_PACKETS_PER_DAY[this->intfId][this->routerId] ++;;
				msgObj->udp.frTimeEpochSec = rawPkt->tv_sec;
				msgObj->udp.frTimeEpochNanoSec = rawPkt->tv_nsec;

				switch(msgObj->udp.direction)
				{
					case 0:
					case UNKNOWN:
						bwData->updateBWData(curMin, msgObj->udp.frSize, DOWN, msgObj->udp.frTimeEpochSec);

						if(IPGlobal::PROCESS_CDN && msgObj->udp.ipVer == IPVersion4)
						{
							if(checkCDNV4(msgObj->udp.dIp, curMin, msgObj->udp.frSize, DOWN, msgObj->udp.frTimeEpochSec))
								flag = true;
							else
								flag = checkCDNV4(msgObj->udp.sIp, curMin, msgObj->udp.frSize, DOWN, msgObj->udp.frTimeEpochSec);
						}

						if(IPGlobal::PROCESS_PEERING && !flag)
						{
							if(checkPeering(msgObj->udp.dIp))
								peeringData->updatePeeringData(curMin, msgObj->udp.frSize, DOWN, msgObj->udp.frTimeEpochSec);
							else if(checkPeering(msgObj->udp.sIp))
								peeringData->updatePeeringData(curMin, msgObj->udp.frSize, DOWN, msgObj->udp.frTimeEpochSec);
						}

						if(msgObj->dropPacket) return;

						if(IPGlobal::PROCESS_OUT_OF_RANGE_IP[intfId])
							msgObj->processOutOfRange = true;
						else
							msgObj->processOutOfRange = false;

						break;

					case UP:
						bwData->updateBWData(curMin, msgObj->udp.frSize, msgObj->udp.direction, msgObj->udp.frTimeEpochSec);

						if(IPGlobal::PROCESS_CDN && msgObj->udp.ipVer == IPVersion4)
							flag = checkCDNV4(msgObj->udp.dIp, curMin, msgObj->udp.frSize, UP, msgObj->udp.frTimeEpochSec);

						if(IPGlobal::PROCESS_PEERING && !flag)
							if(checkPeering(msgObj->udp.dIp))
								peeringData->updatePeeringData(curMin, msgObj->udp.frSize, UP, msgObj->udp.frTimeEpochSec);

						if(msgObj->dropPacket) return;
						break;

					case DOWN:
						bwData->updateBWData(curMin, msgObj->udp.frSize, msgObj->udp.direction, msgObj->udp.frTimeEpochSec);

						if(IPGlobal::PROCESS_CDN && msgObj->udp.ipVer == IPVersion4)
							flag = checkCDNV4(msgObj->udp.sIp, curMin, msgObj->udp.frSize, DOWN, msgObj->udp.frTimeEpochSec);

						if(IPGlobal::PROCESS_PEERING && !flag)
							if(checkPeering(msgObj->udp.sIp))
								peeringData->updatePeeringData(curMin, msgObj->udp.frSize, DOWN, msgObj->udp.frTimeEpochSec);

						if(msgObj->dropPacket) return;

						break;
				}

				if(msgObj->udp.direction == UP && msgObj->udp.pLoad <= IPGlobal::UDP_SIZE_BLOCK[this->intfId])
					return;

				if(IPGlobal::PUSH_TO_QUEUE[intfId] && msgObj->processOutOfRange)
					pushUdpToAgentQueue(msgObj);
	    	}
	    	break;

	    	case PACKET_IPPROTO_DNS:
	    	{
	    		IPGlobal::DNS_PACKETS_PER_DAY[this->intfId][this->routerId] ++;;
				msgObj->dns.frTimeEpochSec = rawPkt->tv_sec;
				msgObj->dns.frTimeEpochNanoSec = rawPkt->tv_nsec;

				if(IPGlobal::PUSH_TO_QUEUE[intfId] && msgObj->processOutOfRange)
					pushDnsToAgentQueue(msgObj);
	    	}
	    	break;

	    	case PACKET_IPPROTO_RADIUS:
	    	{
	    		IPGlobal::AAA_PACKETS_PER_DAY[this->intfId][this->routerId] ++;
				msgObj->aaa.frTimeEpochSec = rawPkt->tv_sec;
				msgObj->aaa.frTimeEpochMilliSec = (rawPkt->tv_nsec / 1000000);

				bwData->updateBWData(curMin, msgObj->aaa.frSize, msgObj->aaa.direction, msgObj->aaa.frTimeEpochSec);

				if(IPGlobal::PUSH_TO_QUEUE[intfId])
					pushAaaToAgentQueue(msgObj);
	    	}
	    	break;

	    	default:
	    		break;
	    } /* End of Switch */
	}
}

void PacketRouter::pushTcpToAgentQueue(MPacket *msgObj)
{
	if(msgObj == NULL)
		return;

	uint16_t idx = PKT_WRITE_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	if(tcpAgent::cnt[this->intfId][this->routerId][idx] > IPGlobal::MAX_PACKET_PUSH_SOCKET) return;

	copyMsgObj(tcpAgent::cnt[this->intfId][this->routerId][idx], tcpAgent::store[this->intfId][this->routerId][idx], msgObj);

}

void PacketRouter::pushUdpToAgentQueue(MPacket *msgObj)
{
	if(msgObj == NULL)
		return;

	uint16_t idx = PKT_WRITE_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	if(udpAgent::cnt[this->intfId][this->routerId][idx] > IPGlobal::MAX_PACKET_PUSH_SOCKET) return;

	copyMsgObj(udpAgent::cnt[this->intfId][this->routerId][idx], udpAgent::store[this->intfId][this->routerId][idx], msgObj);
}

void PacketRouter::pushAaaToAgentQueue(MPacket *msgObj)
{
	if(msgObj == NULL)
		return;

	uint16_t idx = PKT_WRITE_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	copyMsgObj(aaaAgent::cnt[this->intfId][this->routerId][idx], aaaAgent::store[this->intfId][this->routerId][idx], msgObj);
}

void PacketRouter::pushDnsToAgentQueue(MPacket *msgObj)
{
	if(msgObj == NULL)
		return;

	uint16_t idx = PKT_WRITE_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	copyMsgObj(dnsAgent::cnt[this->intfId][this->routerId][idx], dnsAgent::store[this->intfId][this->routerId][idx], msgObj);
}


void PacketRouter::copyMsgObj(uint32_t &ip_msg_cnt, std::unordered_map<uint32_t, MPacket> &ip_msg, MPacket *msgObj)
{
	if(msgObj == NULL)
		return;

	ip_msg[ip_msg_cnt].copy(msgObj);
	ip_msg_cnt++;
}

bool PacketRouter::checkCDNV4(uint32_t ip, uint16_t curMin, uint16_t frSize, uint8_t dir, uint64_t curSec)
{
	for(uint16_t counter = 0; counter < IPGlobal::NO_OF_IPV4_CDN; counter++)
	{
		if(IsIPInRange(ip, IPGlobal::CDN_IPV4_RANGE[counter][0], IPGlobal::CDN_IPV4_RANGE[counter][1]))
		{
			cdnData->updateCDNData(curMin, frSize, dir, curSec);
			return true;
			break;
		}
	}
	return false;
}

bool PacketRouter::checkPeering(uint32_t ip)
{
	bool flag = false;
	uint16_t counter = 0;

	if(IPGlobal::GOOGLE)
	{
		for(counter = 0; counter < IPGlobal::PEERING_GOOGLE_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_GOOGLE[counter][0], IPGlobal::PEERING_GOOGLE[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	if(IPGlobal::AMAZON)
	{
		for(counter = 0; counter < IPGlobal::PEERING_AMAZON_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_AMAZON[counter][0], IPGlobal::PEERING_AMAZON[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	if(IPGlobal::NIXI)
	{
		for(counter = 0; counter < IPGlobal::PEERING_NIXI_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_NIXI[counter][0], IPGlobal::PEERING_NIXI[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	if(IPGlobal::FACEBOOK)
	{
		for(counter = 0; counter < IPGlobal::PEERING_FACEBOOK_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_FACEBOOK[counter][0], IPGlobal::PEERING_FACEBOOK[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	if(IPGlobal::LIMELIGHT)
	{
		for(counter = 0; counter < IPGlobal::PEERING_LIMELIGHT_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_LIMELIGHT[counter][0], IPGlobal::PEERING_LIMELIGHT[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	if(IPGlobal::DE_CIX)
	{
		for(counter = 0; counter < IPGlobal::PEERING_DE_CIX_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_DE_CIX[counter][0], IPGlobal::PEERING_DE_CIX[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	if(IPGlobal::MICROSOFT)
	{
		for(counter = 0; counter < IPGlobal::PEERING_MICROSOFT_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_MICROSOFT[counter][0], IPGlobal::PEERING_MICROSOFT[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	if(IPGlobal::CLOUDFLARE)
	{
		for(counter = 0; counter < IPGlobal::PEERING_CLOUDFLARE_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_CLOUDFLARE[counter][0], IPGlobal::PEERING_CLOUDFLARE[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	if(IPGlobal::ZENLAYER)
	{
		for(counter = 0; counter < IPGlobal::PEERING_ZENLAYER_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_ZENLAYER[counter][0], IPGlobal::PEERING_ZENLAYER[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	if(IPGlobal::FASTLY)
	{
		for(counter = 0; counter < IPGlobal::PEERING_FASTLY_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_FASTLY[counter][0], IPGlobal::PEERING_FASTLY[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	if(IPGlobal::AKAMAI)
	{
		for(counter = 0; counter < IPGlobal::PEERING_AKAMAI_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_AKAMAI[counter][0], IPGlobal::PEERING_AKAMAI[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	if(IPGlobal::ALIBABA)
	{
		for(counter = 0; counter < IPGlobal::PEERING_ALIBABA_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_ALIBABA[counter][0], IPGlobal::PEERING_ALIBABA[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	if(IPGlobal::AMS_IX)
	{
		for(counter = 0; counter < IPGlobal::PEERING_AMS_IX_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_AMS_IX[counter][0], IPGlobal::PEERING_AMS_IX[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	if(IPGlobal::APPLE)
	{
		for(counter = 0; counter < IPGlobal::PEERING_APPLE_COUNT; counter++)
		{
			if(IsIPInRange(ip, IPGlobal::PEERING_APPLE[counter][0], IPGlobal::PEERING_APPLE[counter][1]))
			{
				return true;
				break;
			}
		}
	}
	return flag;
}

void PacketRouter::checkCDNV6(char *sourceIpAddrChar, char *destIpAddrChar, uint16_t curMin, uint16_t frSize, uint8_t dir, uint64_t curSec)
{
	std::string ipInList;
	uint16_t rangeLen = 0;

    for (uint16_t counter = 0; counter < IPGlobal::CDN_IPV6_RANGE.size(); ++counter)
    {
    	ipInList = IPGlobal::CDN_IPV6_RANGE.at(counter);
    	rangeLen = ipInList.length();
    	if(std::string(sourceIpAddrChar).compare(0, rangeLen, ipInList) == 0)
    	{
    		cdnData->updateCDNData(curMin, frSize, dir, curSec);
    		break;
    	}
    	else if(std::string(destIpAddrChar).compare(0, rangeLen, ipInList) == 0)
    	{
    		cdnData->updateCDNData(curMin, frSize, dir, curSec);
    		break;
    	}
    }
}

bool PacketRouter::IsIPInRange(uint32_t ip, uint32_t network, uint32_t mask)
{
    uint32_t net_lower = (network & mask);
    uint32_t net_upper = (net_lower | (~mask));

    if(ip >= net_lower && ip <= net_upper)
        return true;
    return false;
}
