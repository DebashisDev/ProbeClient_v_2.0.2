/*
 * SolarGlobal.h
 *
 *  Created on: Feb 1, 2017
 *      Author: Deb
 */

#ifndef PACKETSOURCE_SOLARFLARE_SRC_SOLARGLOBAL_H_
#define PACKETSOURCE_SOLARFLARE_SRC_SOLARGLOBAL_H_

#include "vi.h"
#include "pd.h"
#include "memreg.h"
#include "packedstream.h"
#include "utils.h"

struct buf {
  ef_addr     ef_address;
  struct buf* next;
};

typedef struct _interfaceThread {
  ef_driver_handle         dh;
  struct ef_pd             pd;
  struct ef_vi             vi;
  struct ef_memreg         memreg;
  int                      psp_start_offset;
  struct buf*              current_buf;
  struct buf*              posted_bufs;
  struct buf**             posted_bufs_tail;
  ef_packed_stream_packet* ps_pkt_iter;
  uint64_t                 n_rx_pkts;
  uint64_t                 n_rx_bytes;
}interfaceThread;

#endif /* PACKETSOURCE_SOLARFLARE_SRC_SOLARGLOBAL_H_ */
