#ifndef SRC_PACKETLISTENER_H_
#define SRC_PACKETLISTENER_H_

#include <string>

#include "SolarGlobal.h"
#include "Log.h"
#include "SpectaTypedef.h"
#include "InterfaceMonitor.h"
#include "BaseConfig.h"

#define	ETHERNET_HDR_LEN	14
#define	IPV4_HDR_LEN		20
#define BYTE_TO_COPY		48

using namespace std;

class PacketListener : public BaseConfig
{
	public:
		PacketListener(uint16_t perListenerRouters, uint16_t index, uint16_t intfId);
		~PacketListener();

		void 	start();
		bool  	isRepositoryInitialized();

	private:
		bool 		repoInitStatus;
		BYTE 		pkt;
		uint16_t 	ROUTER_TO_PROCESS;
		uint16_t 	intfId;
		uint16_t 	END_ROUTER_ID;
		uint16_t 	cfg_verbose;
		uint16_t 	cfg_timestamping;
		uint16_t 	len;
		uint16_t 	tIdx;
		uint16_t 	maxPktLen;
		uint16_t 	copy_len;
		uint16_t 	pTidx;
		uint32_t 	tv_sec;
		uint32_t 	MAX_PKT_ALLOWED_PER_TIME_INDEX;
		uint32_t 	noOfPackets;
		uint64_t 	tv_nsec;
		int 		cfg_max_fill;

		string 	intfName;

		void hexdump(const void* pv, int len);
		void consume_packet(ef_packed_stream_packet* ps_pkt);
		void handle_rx_ps(interfaceThread* t, const ef_event* pev);
		void receivePackets(interfaceThread* t);
		void countDiscardedPkt();
};

#endif /* SRC_PACKETLISTENER_H_ */
