/*
 * PeeringData.h
 *
 *  Created on: 13-Dec-2021
 *      Author: Debashis
 */

#ifndef CORE_INC_PeeringData_H_
#define CORE_INC_PeeringData_H_

#include <stdlib.h>    //malloc
#include <string.h>    //strlen
#include <ctime>
#include "Log.h"
#include "BaseConfig.h"
#include "IPGlobal.h"

#define PEERING_TIME_INDEX	100

class PeeringData : public BaseConfig
{
	private:
		uint16_t 	intfId, routerId;
		uint16_t 	volume;
		uint8_t  	dir;
		uint16_t	curSec;

		void processPeeringData(peeringData (&peering)[PEERING_TIME_INDEX]);

		peeringData peering_i_r_t[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][2][PEERING_TIME_INDEX];

		peeringData calculatePeeringData(peeringData (&peering)[PEERING_TIME_INDEX]);

	public:
		PeeringData(uint16_t intfid, uint16_t rid);
		~PeeringData();

		void updatePeeringData(uint16_t curMin, uint16_t frSize, uint8_t cdnDirection, uint64_t pktCurSec);
		void setPeeringData(uint16_t curMin);
};

#endif /* CORE_INC_PeeringData_H_ */
