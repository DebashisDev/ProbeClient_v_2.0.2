/*
 * ProbeStatsLog.h
 *
 *  Created on: Jul 21, 2017
 *      Author: Debashis
 */

#ifndef CORE_SRC_PROBESTATSLOG_H_
#define CORE_SRC_PROBESTATSLOG_H_

#include <unistd.h>
#include "SpectaTypedef.h"
#include "Log.h"
#include <locale.h>
#include <time.h>
#include <sys/time.h>

#include "BaseConfig.h"
#include "IPGlobal.h"

class ProbeStatsLog : public BaseConfig
{
	public:
		ProbeStatsLog();
		~ProbeStatsLog();
		void run();

	private:
		uint16_t nicCounter, solCounter, interfaceCounter;
		timeval curTime;
		string 	intfName[MAX_INTERFACE_SUPPORT];

		void printInterfaceStats(char *runTime);
		void packetProcessing(uint16_t interfaceId, bool flag);

		void printPktStoreStats_i_0();
		void printPktStoreStats_i_1();
		void printPktStoreStats_i_2();
		void printPktStoreStats_i_3();
};

#endif /* CORE_SRC_PROBESTATSLOG_H_ */
