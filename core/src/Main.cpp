#include <signal.h>

#include "SpectaProbe.h"

void stopPacketCapturing()
{
	uint16_t cnt = 0;

	printf("\n  SpectaProbe Shutdown Initiated...\n");

	for(cnt = 0; cnt < IPGlobal::NO_OF_INTERFACES; cnt++)
		IPGlobal::PACKET_PROCESSING[cnt] = false;

	printf("  Traffic Stopped...\n");
	sleep(5);

	IPGlobal::PROBE_STATS_RUNNING_STATUS = false;
	sleep(5);

	for(cnt = 0; cnt < IPGlobal::NO_OF_INTERFACES; cnt++)
		IPGlobal::PKT_LISTENER_RUNNING_STATUS[cnt] = false;

	sleep(5);

	for(cnt = 0; cnt < IPGlobal::NO_OF_INTERFACES; cnt++)
	{
		for(uint32_t router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[cnt]; router++)
		{
			IPGlobal::PKT_ROUTER_RUNNING_STATUS[cnt][router] = false;
		}
	}
	sleep(5);

	IPGlobal::TIMER_PROCESSING = false;

	for(cnt = 0; cnt < IPGlobal::NO_OF_INTERFACES; cnt++)
	{
		IPGlobal::TCP_AGENT_RUNNING_STATUS[cnt] = false;
		IPGlobal::UDP_AGENT_RUNNING_STATUS[cnt] = false;

		IPGlobal::DNS_AGENT_RUNNING_STATUS[cnt] = false;
		IPGlobal::AAA_AGENT_RUNNING_STATUS[cnt] = false;
	}

	IPGlobal::PROBE_RUNNING_STATUS = false;
}

void sig_handler(int signo)
{
	if (signo == SIGTERM || signo == SIGINT)
	{
		stopPacketCapturing();
		IPGlobal::PROBE_RUNNING_STATUS = false;
	}
}

int main(int argc, char *argv[])
{
	sleep(5);

	mapDnsLock::count 		= 1;

	if (signal(SIGTERM, sig_handler) == SIG_ERR || signal(SIGINT, sig_handler) == SIG_ERR)
		printf("SpectaProbe Can't Received Signal...\n");

	timeval curTime;
	struct tm *now_tm;

	IPGlobal::PROBE_RUNNING_STATUS = true;
	SpectaProbe *spectaProbe = new SpectaProbe("client.config");
	spectaProbe->start();

	printf("  **** SpectaProbe Exiting...Please wait... ***** \n");
}
