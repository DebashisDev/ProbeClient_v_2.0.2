/*
 * processBw.h
 *
 *  Created on: 18 Jul 2022
 *      Author: Deabshis
 */

#ifndef SPECTACLIENT_V2_0_1_CORE_SRC_PROCESSBW_H_
#define SPECTACLIENT_V2_0_1_CORE_SRC_PROCESSBW_H_

#include "BaseConfig.h"
#include "IPGlobal.h"
#include "Log.h"

class processBw : public BaseConfig
{
	private:
		bool 		initStatus;
		uint16_t	currentMin, prevMin;

		char 		bwXdr[XDR_MAX_LEN];
		char		cachedXdr[XDR_MAX_LEN];
		char		unCachedXdr[XDR_MAX_LEN];
		char		peeringXdr[XDR_MAX_LEN];

		fstream		BwXdrHandler;
		fstream		CDNXdrHandler;

		void		openCDNCsvXdrFile(uint16_t &currentMin, uint16_t &currentHour, uint16_t &currentDay, uint16_t &currentMonth, uint16_t &currentYear);
		void 		writeCDNXdr(char *bufferBW, char *bufferCDN, char *bufferUNC, char *bufferPeering);
		void		closeCDNCsvXdrFile();

		void		buildBwCSV(uint64_t timems);
		void		openBwCsvXdrFile(uint16_t &currentMin, uint16_t &currentHour, uint16_t &currentDay, uint16_t &currentMonth, uint16_t &currentYear);
		void		writeBwXdr(char *buffer);
		void		closeBwCsvXdrFile();


	public:
		processBw();
		~processBw();

		bool isInitialized();
		void run();
};

#endif /* SPECTACLIENT_V2_0_1_CORE_SRC_PROCESSBW_H_ */
