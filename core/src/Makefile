#########################################################################
#																		#
# SCRIPT NAME	: Makefile												#
# DESCRIPTION	: To build all .cpp files								#
# DATE 			: 19-02-2016										    #
# AUTHOR		: Debashis.											    #
#																		#
# Copyright (c) 2016, Pinnacle Digital (P) Ltd. New-Delhi				# 
#########################################################################

include ${PROBE_ROOT}/probe.mk
OBJS= \
	AaaAgent.o			\
	AdminPortReader.o 	\
	BWData.o			\
	CDNData.o			\
	DnsAgent.o			\
	EthernetParser.o 	\
	GConfig.o 			\
	glbTimer.o			\
	Initialize.o		\
	IPGlobal.o 			\
	Main.o 				\
	PacketRouter.o 		\
	PeeringData.o		\
	ProbeStats.o 		\
	ProbeStatsLog.o		\
	ProbeUtility.o 		\
	processBw.o			\
	SpectaProbe.o		\
	TcpAgent.o			\
	UdpAgent.o			\

all:${OBJS}

.cpp.o:

	${GCC} ${CFLAGS} -I/usr/include ${PROBE_INCLUDE} -c $< 

clean:
	${RM} ${OBJS}
