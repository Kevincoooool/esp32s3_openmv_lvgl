#ifndef  __DRV_RING_BUF_H
#define  __DRV_RING_BUF_H

#include "stdint.h"
#include "stdio.h"


#define  USB_RX_BUFSIZE  128

typedef struct 
{
 void*      uart;
 volatile   uint16_t  write_index;
 volatile   uint16_t  read_index;
 volatile   uint16_t  peek_index;
            uint16_t  max_size_mask;
            uint8_t*  buff_addr;
//            uint8_t*  cache_buf;
//            uint16_t  cache_index;
//            uint16_t  cache_maxindex;
            uint8_t   port_num;
}RING_BUF_DEF_STRUCT;


void      drv_ringbuf_init(RING_BUF_DEF_STRUCT* ring_buf, uint8_t* src_buf, uint16_t src_buf_maxsize);
void      drv_ringbuf_read(RING_BUF_DEF_STRUCT* src_ringbuf, uint16_t len, uint8_t* dst_buf);
void      drv_ringbuf_peek(RING_BUF_DEF_STRUCT* src_ringbuf, uint16_t len, uint8_t* dst_buf);
void      drv_ringbuf_peek_pos_change(RING_BUF_DEF_STRUCT* src_ringbuf, uint16_t offset);
uint16_t  drv_ringbuf_peeked_counter(RING_BUF_DEF_STRUCT* src_ringbuf);
void      drv_ringbuf_reeked_flush(RING_BUF_DEF_STRUCT* src_ringbuf, uint16_t len);
void      drv_ringbuf_peeked_reset(RING_BUF_DEF_STRUCT* src_ringbuf);
void      drv_ringbuf_write(RING_BUF_DEF_STRUCT* des_ringbuf, uint8_t* src, uint16_t len);
void      drv_ringbuf_flush(RING_BUF_DEF_STRUCT* src_ringbuf);
uint16_t  drv_ringbuf_count(RING_BUF_DEF_STRUCT* src_ringbuf);
uint16_t  drv_ringbuf_unusedcount(RING_BUF_DEF_STRUCT* src_ringbuf);
uint16_t  drv_ringbuf_peek_count (RING_BUF_DEF_STRUCT* src_ringbuf);
void ringbuff_rx_reset(RING_BUF_DEF_STRUCT* src_ringbuf, uint16_t len);

#endif 
