/*
 * TcpAgent.h
 *
 *  Created on: 12 Mar 2020
 *      Author: Debashis
 */

#ifndef CORE_SRC_TCPAGENT_H_
#define CORE_SRC_TCPAGENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <iterator>
#include <list>

#include "SpectaTypedef.h"
#include "IPGlobal.h"
#include "TCPUDPGlobal.h"
#include "BaseConfig.h"
#include "zhelpers.hpp"
#include "zmq.hpp"

class TcpAgent : public BaseConfig
{
	private:

		bool		repoInitStatus;
		uint16_t 	intfId;

	    zmq::context_t ctx;
	    zmq::socket_t socketBackEnd;
	    zmq::message_t nwtData;

		void	processQueue(uint16_t lastProcessedIndex);

		void 	pushToServer(uint32_t &ip_cnt, std::unordered_map<uint32_t, MPacket> &ip);
		void 	deliver(MPacket* msgObj);

	public:
		TcpAgent(uint16_t intfid);
		~TcpAgent();

		bool isRepositoryInitialized();
		void run();
};

#endif /* CORE_SRC_TCPAGENT_H_ */
