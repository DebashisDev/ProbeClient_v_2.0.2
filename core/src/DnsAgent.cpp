/*
 * DnsAgent.cpp
 *
 *  Created on: 16 Mar 2021
 *      Author: debas
 */

#include "DnsAgent.h"

DnsAgent::DnsAgent(uint16_t intfid): ctx(1), socketBackEnd(ctx, ZMQ_PUSH)
{
	this->intfId = intfid;
	this->repoInitStatus = false;
}

DnsAgent::~DnsAgent()
{ }

bool DnsAgent::isRepositoryInitialized()
{ return repoInitStatus; }

void DnsAgent::run()
{
	uint16_t curIndex, lastProcessedIndex = 0;

	socketBackEnd.connect(IPGlobal::SERVER_DNS_ADDRESS[this->intfId].c_str());
	printf("          - Interface [%02d] Connected To DNS Socket : %s\n", this->intfId, IPGlobal::SERVER_DNS_ADDRESS[this->intfId].c_str());

	lastProcessedIndex = curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	repoInitStatus = true;

	while(IPGlobal::DNS_AGENT_RUNNING_STATUS[intfId])
	{
		usleep(IPGlobal::THREAD_SLEEP_TIME);

		curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC,IPGlobal::TIME_INDEX);

		while(lastProcessedIndex != curIndex)
		{
			processQueue(lastProcessedIndex);
			lastProcessedIndex = PKT_READ_NEXT_TIME_INDEX(lastProcessedIndex, IPGlobal::TIME_INDEX);
		}
	}
	printf("  DNS Agent Stopped...\n");
	pthread_detach(pthread_self());
	pthread_exit(NULL);

}

void DnsAgent::processQueue(uint16_t tIdx)
{
	for(uint16_t r = 0; r < IPGlobal::ROUTER_PER_INTERFACE[this->intfId]; r++)
		pushToServer(dnsAgent::cnt[this->intfId][r][tIdx], dnsAgent::store[this->intfId][r][tIdx]);
}

//void DnsAgent::pushToServer(bool &ip_busy, uint32_t &ip_cnt, std::unordered_map<uint32_t, MPacket> &ip)
//{
//	UdpPacket fLocal;
//	uint32_t recCnt = ip_cnt;
//	ip_busy = true;
//
//	if(recCnt > 0)
//	{
//		for (auto it = ip.cbegin(), next_it = it; it != ip.cend(); it = next_it)
//		{
////			fLocal = it->second.udp;
//			deliver(it->second.udp);
//
//			++next_it;
////			ip.erase(it);
//			ip_cnt --;
//		}
//		ip.clear();
//		ip_cnt = 0;
//		ip_busy = false;
//	}
//}

void DnsAgent::pushToServer(uint32_t &ip_cnt, std::unordered_map<uint32_t, MPacket> &ip)
{
	uint32_t recCnt = ip_cnt;

	if(recCnt > 0)
	{
		for(uint32_t i = 0; i < recCnt; i++)
		{
			deliver(&ip[i]);
			ip_cnt--;
		}
		ip_cnt = 0;
	}
}

void DnsAgent::deliver(MPacket *msgObj)
{
	if(msgObj == NULL)
		return;

	/* Send the Data to Server */
	nwtData.rebuild(sizeof(DnsPacket));
	memcpy(nwtData.data(), (const void *)&msgObj->dns, sizeof(DnsPacket));
	socketBackEnd.send(nwtData);
}
