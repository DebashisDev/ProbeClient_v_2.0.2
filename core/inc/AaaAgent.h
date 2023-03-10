/*
 * AaaAgent.h
 *
 *  Created on: 16 Mar 2021
 *      Author: debas
 */

#ifndef CORE_SRC_AAAAGENT_H_
#define CORE_SRC_AAAAGENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iterator>
#include <list>

#include "SpectaTypedef.h"
#include "IPGlobal.h"
#include "Log.h"
#include "TCPUDPGlobal.h"
#include "BaseConfig.h"
#include "zhelpers.hpp"
#include "zmq.hpp"

class AaaAgent : public BaseConfig
{
	private:
		bool			repoInitStatus;
		uint16_t 		intfId;

		zmq::context_t 	ctx;
		zmq::socket_t 	socketBackEnd;
		zmq::message_t 	nwtData;

		void	processQueue(uint16_t lastProcessedIndex);

		void 	pushToServer(uint32_t &ip_cnt, std::unordered_map<uint32_t, MPacket> &ip);
		void 	deliver(MPacket* msgObj);

	public:
		AaaAgent(uint16_t intfid);
		~AaaAgent();

		bool 	isInitialized();
		void 	run();
};

#endif /* CORE_SRC_AAAAGENT_H_ */
