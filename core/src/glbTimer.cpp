/*
 * glbTimer.cpp
 *
 *  Created on: Feb 6, 2019
 *      Author: Debashis
 */

#include "glbTimer.h"

glbTimer::glbTimer()
{ this->timerReadyState = false; }

glbTimer::~glbTimer()
{}

bool glbTimer::isGlbTimerInitialized()
{ return timerReadyState; }

void glbTimer::run()
{
	struct tm *now_tm;
	timerReadyState = true;

	while(IPGlobal::TIMER_PROCESSING)
	{
		gettimeofday(&curTime, NULL);
		now_tm = localtime(&curTime.tv_sec);

		IPGlobal::CURRENT_EPOCH_SEC 	= curTime.tv_sec ;
		IPGlobal::CURRENT_EPOCH_MICRO_SEC 	= curTime.tv_usec ;
		IPGlobal::CURRENT_EPOCH_MILI_SEC= ((curTime.tv_sec) * 1000 + (curTime.tv_usec)/1000);
		IPGlobal::CURRENT_EPOCH_NANO_SEC= ((curTime.tv_sec * 1000000 + curTime.tv_usec) * 1000);
		IPGlobal::CURRENT_DAY = now_tm->tm_mday;
		IPGlobal::CURRENT_HOUR = now_tm->tm_hour;
		IPGlobal::CURRENT_MIN = now_tm->tm_min;
		IPGlobal::CURRENT_SEC = now_tm->tm_sec;
		IPGlobal::CURRENT_MONTH 	= 1 + now_tm->tm_mon;
		IPGlobal::CURRENT_YEAR 		= 1900 + now_tm->tm_year;
	}

	printf("  Timer Stopped...\n");
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}
