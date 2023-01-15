/*
 * Initialize.h
 *
 *  Created on: 30 Nov 2021
 *      Author: Debashis
 */

#ifndef CORE_SRC_INITIALIZE_H_
#define CORE_SRC_INITIALIZE_H_

#include "IPGlobal.h"

using namespace std;

class Initialize
{
	public:
		Initialize();
		~Initialize();


	private:
		void ipSubNet();
		void ipPeering();
		void ipCache();
};

#endif /* CORE_SRC_INITIALIZE_H_ */
