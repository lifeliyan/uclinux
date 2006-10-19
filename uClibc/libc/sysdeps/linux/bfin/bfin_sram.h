/* File: bfin_sram.h
 *
 * Some kernel system calls to allow userspace to easily work
 * with L1 and DMA memory operations.
 */

#ifndef BFIN_SRAM_H
#define BFIN_SRAM_H

/* need size_t defines */
#include <sys/types.h>

#define L1_INST_SRAM            0x00000001
#define L1_DATA_A_SRAM          0x00000002
#define L1_DATA_B_SRAM          0x00000004
#define L1_DATA_SRAM            0x00000006
extern void *sram_alloc(size_t size, unsigned long flags);
extern int sram_free(const void *addr);
extern void *dma_memcpy(void *dest, const void *src, size_t len);

#endif
