/*
 * InterfaceMonitor.h
 *
 *  Created on: Feb 1, 2017
 *      Author: Deb
 */

#ifndef PACKETSOURCE_SOLARFLARE_SRC_INTERFACEMONITOR_H_
#define PACKETSOURCE_SOLARFLARE_SRC_INTERFACEMONITOR_H_

#include "SolarGlobal.h"
#include <time.h>
#include <sys/time.h>
#include "IPGlobal.h"
#include "Log.h"
#include "BaseConfig.h"

class InterfaceMonitor : public BaseConfig {
	public:
		InterfaceMonitor(uint16_t intfid, interfaceThread *t);
		~InterfaceMonitor();
		void run();

	private:
		uint16_t intfId;
		interfaceThread *thread;
		long ppsArray[60];
};

#endif /* PACKETSOURCE_SOLARFLARE_SRC_INTERFACEMONITOR_H_ */
