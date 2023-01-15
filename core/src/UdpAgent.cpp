/*
 * UdpAgent.cpp
 *
 *  Created on: 16 Mar 2021
 *      Author: debas
 */

#include "UdpAgent.h"

UdpAgent::UdpAgent(uint16_t intfid): ctx(1), socketBackEnd(ctx, ZMQ_PUSH)
{
	this->intfId 	 	 = intfid;
	this->repoInitStatus = false;
}

UdpAgent::~UdpAgent()
{ }

bool UdpAgent::isRepositoryInitialized()
{ return repoInitStatus; }

void UdpAgent::run()
{
	uint16_t curIndex, lastProcessedIndex = 0;

	socketBackEnd.connect(IPGlobal::SERVER_UDP_ADDRESS[this->intfId].c_str());
	printf("          - Interface [%02d] Connected To UDP Socket : %s\n", this->intfId, IPGlobal::SERVER_UDP_ADDRESS[this->intfId].c_str());

	lastProcessedIndex = curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	repoInitStatus = true;

	while(IPGlobal::UDP_AGENT_RUNNING_STATUS[intfId])
	{
		usleep(IPGlobal::THREAD_SLEEP_TIME);

		curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC,IPGlobal::TIME_INDEX);

		while(lastProcessedIndex != curIndex)
		{
			processQueue(lastProcessedIndex);
			lastProcessedIndex = PKT_READ_NEXT_TIME_INDEX(lastProcessedIndex, IPGlobal::TIME_INDEX);
		}
	}
	printf("  UDP Agent Stopped...\n");
	pthread_detach(pthread_self());
	pthread_exit(NULL);

}

void UdpAgent::processQueue(uint16_t tIdx)
{
	for(uint16_t r = 0; r < IPGlobal::ROUTER_PER_INTERFACE[this->intfId]; r++)
		pushToServer(udpAgent::cnt[this->intfId][r][tIdx], udpAgent::store[this->intfId][r][tIdx]);
}

void UdpAgent::pushToServer(uint32_t &ip_cnt, std::unordered_map<uint32_t, MPacket> &ip)
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

void UdpAgent::deliver(MPacket *msgObj)
{
	if(msgObj == NULL)
		return;

	/* Send the Data to Server */
	nwtData.rebuild(sizeof(UdpPacket));
	memcpy(nwtData.data(), (const void *)&msgObj->udp, sizeof(UdpPacket));
	socketBackEnd.send(nwtData);
}
