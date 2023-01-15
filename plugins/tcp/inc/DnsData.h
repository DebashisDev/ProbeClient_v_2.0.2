/*
 * DnsData.h
 *
 *  Created on: 11-Jun-2016
 *      Author: deb
 */

#ifndef INC_DNSDATA_H_
#define INC_DNSDATA_H_

#include <string.h>    //strlen
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

#include "IPGlobal.h"
#include "Log.h"

#include "GConfig.h"
#include "BaseConfig.h"
#include "TCPUDPGlobal.h"

class DnsData : public BaseConfig
{
	private:
		void	lockDnsMap();
		void	unLockDnsMap();

	public:
		DnsData();
		~DnsData();

		static void 	updateDns(uint32_t resolvedip, std::string url);
//		static void 	updateDnsV6(string resolvedip, string url);
};

#endif /* INC_DNSDATA_H_ */
