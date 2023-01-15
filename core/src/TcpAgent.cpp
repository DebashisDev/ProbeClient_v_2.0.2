/*
 * TcpAgent.cpp
 *
 *  Created on: 12 Mar 2020
 *      Author: Debashis
 */

#include "TcpAgent.h"

TcpAgent::TcpAgent(uint16_t intfid): ctx(1), socketBackEnd(ctx, ZMQ_PUSH)
{
	this->intfId 			= intfid;
	this->repoInitStatus 	= false;
}

TcpAgent::~TcpAgent()
{ }

bool TcpAgent::isRepositoryInitialized()
{ return repoInitStatus; }

void TcpAgent::run()
{
	uint16_t curIndex , lastProcessedIndex = 0;

	socketBackEnd.connect(IPGlobal::SERVER_TCP_ADDRESS[this->intfId].c_str());
	printf("          - Interface [%02d] Connecting TCP : %s\n", this->intfId, IPGlobal::SERVER_TCP_ADDRESS[this->intfId].c_str());

	lastProcessedIndex = curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	repoInitStatus = true;

	while(IPGlobal::TCP_AGENT_RUNNING_STATUS[intfId])
	{
		usleep(IPGlobal::THREAD_SLEEP_TIME);

		curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC,IPGlobal::TIME_INDEX);

		while(lastProcessedIndex != curIndex)
		{
			processQueue(lastProcessedIndex);
			lastProcessedIndex = PKT_READ_NEXT_TIME_INDEX(lastProcessedIndex, IPGlobal::TIME_INDEX);
		}
	}
	printf("  TCP Agent Stopped...\n");
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void TcpAgent::processQueue(uint16_t tIdx)
{
	for(uint16_t r = 0; r < IPGlobal::ROUTER_PER_INTERFACE[this->intfId]; r++)
		pushToServer(tcpAgent::cnt[this->intfId][r][tIdx], tcpAgent::store[this->intfId][r][tIdx]);
}

void TcpAgent::pushToServer(uint32_t &ip_cnt, std::unordered_map<uint32_t, MPacket> &ip)
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

void TcpAgent::deliver(MPacket *msgObj)
{
	if(msgObj == NULL)
		return;

	/* Send the Data to Server */
	nwtData.rebuild(sizeof(TcpPacket));
	memcpy(nwtData.data(), (const void *)&msgObj->tcp, sizeof(TcpPacket));
	socketBackEnd.send(nwtData);
}
