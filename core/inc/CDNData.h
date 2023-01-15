/*
 * CDNData.h
 *
 *  Created on: 03-Sep-2019
 *      Author: singh
 */

#ifndef CORE_INC_CDNDATA_H_
#define CORE_INC_CDNDATA_H_

#include <stdlib.h>    //malloc
#include <string.h>    //strlen
#include <ctime>
#include "Log.h"
#include "BaseConfig.h"
#include "IPGlobal.h"

#define CDN_TIME_INDEX	100

class CDNData : public BaseConfig
{
	private:
		uint16_t 	intfId, routerId;
		uint16_t 	volume;
		uint8_t  	dir;
		uint16_t	curSec;

		void processCdnData(cdnData (&cdn)[CDN_TIME_INDEX]);

		cdnData cdn_i_r_t[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][2][CDN_TIME_INDEX];

		cdnData calculateCdnData(cdnData (&cdn)[CDN_TIME_INDEX]);

	public:
		CDNData(uint16_t intfid, uint16_t rid);
		~CDNData();

		void updateCDNData(uint16_t curMin, uint16_t frSize, uint8_t cdnDirection, uint64_t pktCurSec);
		void setCDNData(uint16_t curMin);
};

#endif /* CORE_INC_CDNDATA_H_ */
