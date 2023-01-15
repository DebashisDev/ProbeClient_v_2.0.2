/*
 * AaaAgent.cpp
 *
 *  Created on: 16 Mar 2021
 *      Author: Debashis
 */

#include "AaaAgent.h"

AaaAgent::AaaAgent(uint16_t intfid): ctx(1), socketBackEnd(ctx, ZMQ_PUSH)
{
	this->intfId 			= intfid;
	this->repoInitStatus 	= false;
	this->_name 			= "AaaAgent";
	this->setLogLevel(Log::theLog().level());
}

AaaAgent::~AaaAgent()
{ }

bool AaaAgent::isInitialized()
{ return repoInitStatus; }

void AaaAgent::run()
{
	uint16_t curIndex, lastProcessedIndex = 0;

	socketBackEnd.connect(IPGlobal::SERVER_AAA_ADDRESS[this->intfId].c_str());
	printf("          - Interface [%02d] Connected To AAA Socket : %s\n", this->intfId, IPGlobal::SERVER_AAA_ADDRESS[this->intfId].c_str());

	lastProcessedIndex = curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	repoInitStatus = true;

	while(IPGlobal::AAA_AGENT_RUNNING_STATUS[intfId])
	{
		usleep(IPGlobal::THREAD_SLEEP_TIME);

		curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC,IPGlobal::TIME_INDEX);

		while(lastProcessedIndex != curIndex)
		{
			processQueue(lastProcessedIndex);
			lastProcessedIndex = PKT_READ_NEXT_TIME_INDEX(lastProcessedIndex, IPGlobal::TIME_INDEX);
		}
	}
	printf("  AAA Agent Stopped...\n");
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

void AaaAgent::processQueue(uint16_t tIdx)
{
	for(uint16_t r = 0; r < IPGlobal::ROUTER_PER_INTERFACE[this->intfId]; r++)
		pushToServer(aaaAgent::cnt[this->intfId][r][tIdx], aaaAgent::store[this->intfId][r][tIdx]);
}

void AaaAgent::pushToServer(uint32_t &ip_cnt, std::unordered_map<uint32_t, MPacket> &ip)
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

void AaaAgent::deliver(MPacket *msgObj)
{
	if(msgObj == NULL)
		return;

	/* Send the Data to Server */
	nwtData.rebuild(sizeof(AAAPacket));
	memcpy(nwtData.data(), (const void *)&msgObj->aaa, sizeof(AAAPacket));
	socketBackEnd.send(nwtData);
}

//void AaaAgent::pushToServer(bool &ip_busy, uint32_t &ip_cnt, std::unordered_map<uint32_t, MPacket> &ip)
//{
//	AAAPacket fLocal;
//
//	uint32_t recCnt = ip_cnt;
//	ip_busy = true;
//
//	if(recCnt > 0)
//	{
//		for (auto it = ip.cbegin(), next_it = it; it != ip.cend(); it = next_it)
//		{
////			fLocal = it->second.aaa;
//			deliver(it->second.aaa);
//
//			++next_it;
//			ip.erase(it);
//			ip_cnt --;
//		}
//		ip.clear();
//		ip_cnt = 0;
//		ip_busy = false;
//	}
//}
//
//void AaaAgent::pushToServer(bool &ip_busy, uint32_t &ip_cnt, std::unordered_map<uint32_t, MPacket> &ip)
//{
//	AAAPacket fLocal;
//	uint32_t recCnt = ip_cnt;
//	ip_busy = true;
//
//	if(recCnt > 0)
//	{
//		std::list<std::unordered_map<uint32_t, MPacket>::const_iterator> iterators;
//
//		for (auto it = ip.cbegin(); it != ip.cend(); it++)
//		{
//			fLocal = it->second.aaa;
//			deliver(&fLocal);
//			ip.erase(it->first);
////			iterators.push_back(it);
//			ip_cnt --;
//		}
//		ip_cnt = 0;
//		ip_busy = false;
//
//		for (auto it: iterators)
//			ip.erase(it);
//	}
//}
