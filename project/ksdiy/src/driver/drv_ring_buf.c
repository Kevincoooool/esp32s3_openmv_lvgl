#include "drv_ring_buf.h"

void drv_ringbuf_init(RING_BUF_DEF_STRUCT* ring_buf, uint8_t* src_buf, uint16_t src_buf_maxsize)
{
    ring_buf->write_index = 0;
    ring_buf->read_index = 0;
    ring_buf->buff_addr = src_buf;
    ring_buf->max_size_mask = (src_buf_maxsize - 1);
}

void drv_ringbuf_read(RING_BUF_DEF_STRUCT* src_ringbuf, uint16_t len, uint8_t* dst_buf)
{
    uint16_t count = 0;

    if(src_ringbuf->buff_addr != NULL)
    {
        for(count = 0; count < len; count++)
        {
            dst_buf[count] = src_ringbuf->buff_addr[src_ringbuf->read_index++];
            src_ringbuf->read_index = (src_ringbuf->read_index & src_ringbuf->max_size_mask);
        }
    }
    else
    {
        //add error process
    }
}

void drv_ringbuf_peek(RING_BUF_DEF_STRUCT* src_ringbuf, uint16_t len, uint8_t* dst_buf)
{
    uint16_t count = 0;
    if(src_ringbuf->buff_addr != NULL)
    {
        for(count = 0; count < len; count++)
        {
            dst_buf[count] = src_ringbuf->buff_addr[src_ringbuf->peek_index++];
            src_ringbuf->peek_index = (src_ringbuf->peek_index & src_ringbuf->max_size_mask);
        }
    }
    else
    {
        //add error process
    }
}

void drv_ringbuf_reeked_flush(RING_BUF_DEF_STRUCT* src_ringbuf, uint16_t len)
{
    src_ringbuf->read_index = src_ringbuf->peek_index;
}

void drv_ringbuf_peeked_reset(RING_BUF_DEF_STRUCT* src_ringbuf)
{
    src_ringbuf->peek_index = src_ringbuf->read_index;
}

void drv_ringbuf_write(RING_BUF_DEF_STRUCT* des_ringbuf, uint8_t* src, uint16_t len)
{
    uint16_t count = 0;

    if(drv_ringbuf_unusedcount(des_ringbuf) < len)
    {
        count = len;
    }
    if(des_ringbuf->buff_addr != NULL)
    {
        for(count = 0; count < len; count++)
        {
            des_ringbuf->buff_addr[des_ringbuf->write_index++] = src[count];
            des_ringbuf->write_index = (des_ringbuf->write_index & des_ringbuf->max_size_mask);
        }
    }
    else
    {
        //add error  process
    }
}

uint16_t drv_ringbuf_unusedcount(RING_BUF_DEF_STRUCT* src_ringbuf)
{
    return((src_ringbuf->max_size_mask + 1) - drv_ringbuf_count(src_ringbuf));
}

uint16_t drv_ringbuf_count(RING_BUF_DEF_STRUCT* src_ringbuf)
{
    uint16_t count = 0;
    count = (src_ringbuf->write_index - src_ringbuf->read_index) & (src_ringbuf->max_size_mask);
    return count;
}

void drv_ringbuf_flush(RING_BUF_DEF_STRUCT* src_ringbuf)
{
    src_ringbuf->write_index = 0;
    src_ringbuf->read_index = 0;
    src_ringbuf->peek_index = 0;
}

uint16_t drv_ringbuf_peeked_counter(RING_BUF_DEF_STRUCT* src_ringbuf)
{
    uint16_t count = 0;
    count = (src_ringbuf->peek_index - src_ringbuf->read_index) & (src_ringbuf->max_size_mask);
    return count;
}

uint16_t drv_ringbuf_peek_count(RING_BUF_DEF_STRUCT* src_ringbuf)
{
    uint16_t count = 0;
    count = (src_ringbuf->write_index - src_ringbuf->peek_index) & (src_ringbuf->max_size_mask);
    return count;
}

void drv_ringbuf_peek_pos_change(RING_BUF_DEF_STRUCT* src_ringbuf, uint16_t offset)
{
    src_ringbuf->peek_index = (src_ringbuf->read_index+offset) & (src_ringbuf->max_size_mask);
}

//rx_ringbuff缓存区清空--tx也可以用此函数清空
void ringbuff_rx_reset(RING_BUF_DEF_STRUCT* src_ringbuf, uint16_t len)
{
    uint16_t count = 0;

    if(src_ringbuf->buff_addr != NULL)
    {
        for(count = 0; count < len; count++)
        {
            src_ringbuf->buff_addr[count] = 0;
        }
    }
    else
    {
        //add error process
    }
}