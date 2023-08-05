#include "usbd_core.h"

#include "usbd_cdc.h"
#include "ksdiy_usb_cdc.h"
#include "usb_cdc.h"
#include "usbd_msc.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "extmod/vfs_fat.h"
// #include "usb_dc.h"
uint8_t tx_ringbuf_array[1024 * 100];
volatile ringbuf_t tx_ringbuf;
uint32_t read_buffer_len = 0;
volatile bool ep_tx_busy_flag = false;
volatile bool ep_open_flag = 0;
static volatile uint32_t host2dev_lenth = 0;
static volatile uint32_t dev2host_lenth = 0;
/*!< endpoint address */
#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x84

#define MSC_IN_EP  0x82
#define MSC_OUT_EP 0x01

#define USBD_VID 0X1209
#define USBD_PID 0XABD1
#define USBD_MAX_POWER 100
#define USBD_LANGID_STRING 1033

#define DBG_MAX_RECV_PACKET (1024 * 16)
#define DBG_MAX_PACKET (1024 * 8)
/*!< config descriptor size */
/*!< config descriptor size */

/*!< config descriptor size */
// #define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN + MSC_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

#ifdef CONFIG_USB_HS
#define MSC_MAX_MPS 512
#else
#define MSC_MAX_MPS 64
#endif

/*!< global descriptor */
static const uint8_t cdc_msc_descriptor[] = {
//      USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
//     USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
//     CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, 0x02),
//     ///////////////////////////////////////
//     /// string0 descriptor
//     ///////////////////////////////////////
//     USB_LANGID_INIT(USBD_LANGID_STRING),
//     ///////////////////////////////////////
//     /// string1 descriptor
//     ///////////////////////////////////////
//     0x14,                       /* bLength */
//     USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
//     'C', 0x00,                  /* wcChar0 */
//     'h', 0x00,                  /* wcChar1 */
//     'e', 0x00,                  /* wcChar2 */
//     'r', 0x00,                  /* wcChar3 */
//     'r', 0x00,                  /* wcChar4 */
//     'y', 0x00,                  /* wcChar5 */
//     'U', 0x00,                  /* wcChar6 */
//     'S', 0x00,                  /* wcChar7 */
//     'B', 0x00,                  /* wcChar8 */
//     ///////////////////////////////////////
//     /// string2 descriptor
//     ///////////////////////////////////////
//     0x26,                       /* bLength */
//     USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
//     'C', 0x00,                  /* wcChar0 */
//     'h', 0x00,                  /* wcChar1 */
//     'e', 0x00,                  /* wcChar2 */
//     'r', 0x00,                  /* wcChar3 */
//     'r', 0x00,                  /* wcChar4 */
//     'y', 0x00,                  /* wcChar5 */
//     'U', 0x00,                  /* wcChar6 */
//     'S', 0x00,                  /* wcChar7 */
//     'B', 0x00,                  /* wcChar8 */
//     ' ', 0x00,                  /* wcChar9 */
//     'C', 0x00,                  /* wcChar10 */
//     'D', 0x00,                  /* wcChar11 */
//     'C', 0x00,                  /* wcChar12 */
//     ' ', 0x00,                  /* wcChar13 */
//     'D', 0x00,                  /* wcChar14 */
//     'E', 0x00,                  /* wcChar15 */
//     'M', 0x00,                  /* wcChar16 */
//     'O', 0x00,                  /* wcChar17 */
//     ///////////////////////////////////////
//     /// string3 descriptor
//     ///////////////////////////////////////
//     0x16,                       /* bLength */
//     USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
//     '2', 0x00,                  /* wcChar0 */
//     '0', 0x00,                  /* wcChar1 */
//     '2', 0x00,                  /* wcChar2 */
//     '2', 0x00,                  /* wcChar3 */
//     '1', 0x00,                  /* wcChar4 */
//     '2', 0x00,                  /* wcChar5 */
//     '3', 0x00,                  /* wcChar6 */
//     '4', 0x00,                  /* wcChar7 */
//     '5', 0x00,                  /* wcChar8 */
//     '6', 0x00,                  /* wcChar9 */
// #ifdef CONFIG_USB_HS
//     ///////////////////////////////////////
//     /// device qualifier descriptor
//     ///////////////////////////////////////
//     0x0a,
//     USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
//     0x00,
//     0x02,
//     0x02,
//     0x02,
//     0x01,
//     0x40,
//     0x01,
//     0x00,
// #endif
//     0x00
   USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, 0x02),
    MSC_DESCRIPTOR_INIT(0x02, MSC_OUT_EP, MSC_IN_EP, 0x00),
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x26,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'C', 0x00,                  /* wcChar10 */
    '-', 0x00,                  /* wcChar11 */
    'M', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[4096];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[DBG_MAX_RECV_PACKET];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t send_buffer[DBG_MAX_PACKET];
typedef struct __attribute__((packed))
{
    uint8_t cmd;
    uint8_t request;
    uint32_t xfer_length;
} usbdbg_cmd_t;
#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif
static void openmv_send_data(uint8_t *data, uint32_t len)
{
    // ep_tx_busy_flag = true;
    usbd_ep_start_write(CDC_IN_EP, data, len);
    // while (ep_tx_busy_flag)
    //{

    //}
}
void usbd_configure_done_callback(void)
{
    /* setup first out ep read transfer */
    usbd_ep_start_read(CDC_OUT_EP, read_buffer, 4096);
}

void usbd_cdc_acm_bulk_out(uint8_t ep, uint32_t nbytes)
{
    uint8_t request = 0;
    uint32_t xfer_length = 0;
    read_buffer_len = nbytes;
    /* setup next out ep read transfer */
    usbd_ep_start_read(CDC_OUT_EP, read_buffer, DBG_MAX_RECV_PACKET);
    // USB_LOG_RAW("actual out len:%d 0x%02x 0x%02x \r\n", nbytes,read_buffer[0],read_buffer[1]);
    if (host2dev_lenth) // host send device msg
    {
        if (host2dev_lenth < nbytes)
            host2dev_lenth = 0;
        else
            host2dev_lenth -= nbytes;
        usbdbg_data_out(read_buffer, nbytes);
        return;
    }
    else
    {
        usbdbg_cmd_t *cmd = (usbdbg_cmd_t *)read_buffer;
        request = cmd->request;
        xfer_length = cmd->xfer_length;
        usbdbg_control(NULL, request, xfer_length);
    }
    // if(read_buffer[1] == USBDBG_FRAME_DUMP)
    // USB_LOG_RAW("usbdbg_cmd_t request:0x%02x xfer_length:0x%08x\r\n", request,xfer_length);
    if (request & 0x80)
    {
        // Device-to-host data phase
        int bytes = MIN(xfer_length, DBG_MAX_PACKET - 1);
        usbdbg_data_in(read_buffer, bytes);
        if (xfer_length >= bytes)
            dev2host_lenth = xfer_length - bytes;
        else
            dev2host_lenth = 0;
        openmv_send_data(read_buffer, bytes);
    }
    else
    // Host-to-device data phase
    {
        host2dev_lenth = xfer_length;
    }
}

void usbd_cdc_acm_bulk_in(uint8_t ep, uint32_t nbytes)
{
    // USB_LOG_RAW("actual in len:%d\r\n", nbytes);

    // if ((nbytes % CDC_MAX_MPS) == 0 && nbytes)
    // {
    //     /* send zlp */
    //     usbd_ep_start_write(CDC_IN_EP, NULL, 0);
    // }
    // else
    // {
    //     ep_tx_busy_flag = false;
    // }
    //  USB_LOG_RAW("actual in len:%d\r\n", nbytes);
    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes)
    {
        /* send zlp */
        usbd_ep_start_write(CDC_IN_EP, NULL, 0);
    }
    else
    {
        ep_tx_busy_flag = false;
        if (dev2host_lenth == 0)
            return;
        // USB_LOG_RAW("actual in len:%d\r\n", nbytes);
        int bytes = MIN(dev2host_lenth, DBG_MAX_PACKET - 1);
        usbdbg_data_in(send_buffer, bytes);
        openmv_send_data(send_buffer, bytes);
        if (dev2host_lenth >= bytes)
            dev2host_lenth -= bytes;
        else
            dev2host_lenth = 0;
    }
}

/*!< endpoint call back */
struct usbd_endpoint cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out};

struct usbd_endpoint cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in};

// struct usbd_interface intf0;
// struct usbd_interface intf1;

// void cdc_acm_init00(void)
// {
//     // const uint8_t data[10] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30 };

//     // memcpy(&write_buffer[0], data, 10);
//     // memset(&write_buffer[10], 'a', 2038);
//     tx_ringbuf.buf = tx_ringbuf_array;
//     tx_ringbuf.size = sizeof(tx_ringbuf_array);
//     // tx_ringbuf.size =500*1024;
//     tx_ringbuf.iget = 0;
//     tx_ringbuf.iput = 0;
//     usbd_desc_register(cdc_descriptor);
//     usbd_add_interface(usbd_cdc_acm_init_intf(&intf0));
//     usbd_add_interface(usbd_cdc_acm_init_intf(&intf1));
//     usbd_add_endpoint(&cdc_out_ep);
//     usbd_add_endpoint(&cdc_in_ep);
//     usbd_initialize();
//     xTaskCreatePinnedToCore(cdc_loop, "cdc_loop", 4096 / sizeof(StackType_t), NULL, 15, NULL, 1);
// }

void usbd_cdc_acm_set_rts(uint8_t intf, bool rts)
{
    ep_open_flag = 1;
}

bool usb_vcp_is_enabled(void)
{
    return ep_open_flag;
}

void send_cdc_data(uint8_t *data, uint32_t len)
{
    uint8_t _data[512];
    ep_tx_busy_flag = true;
    //   l1c_dc_invalidate((uint32_t)data, len);
    if (((data[len - 1] == 0x0a) && (len > 1) && (data[len - 2] != 0x0d)) ||
        ((data[len - 1] == 0x0a) && (len == 1)))
    {
        memcpy(_data, data, len - 1);
        _data[len - 1] = 0x0d;
        _data[len] = 0x0a;
        usbd_ep_start_write(CDC_IN_EP, _data, len + 1);
    }
    else
    {
        usbd_ep_start_write(CDC_IN_EP, data, len);
    }
    while (ep_tx_busy_flag)
    {
    }
}

uint32_t recv_cdc_data(uint8_t *data)
{
    volatile uint32_t len = 0;
    if (!data)
        return 0;
    len = read_buffer_len;
    if (len)
    {
        read_buffer_len = 0;
        memcpy(data, read_buffer, len);
    }
    return len;
}

volatile uint8_t dtr_enable = 0;

void usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr)
{
    if (dtr)
    {
        dtr_enable = 1;
    }
    else
    {
        dtr_enable = 0;
    }
}

void cdc_acm_data_send_with_dtr_test(void)
{
    if (dtr_enable)
    {
        memset(&write_buffer[10], 'a', 2038);
        ep_tx_busy_flag = true;
        usbd_ep_start_write(CDC_IN_EP, write_buffer, 2048);
        while (ep_tx_busy_flag)
        {
        }
    }
}
#define DBG_MAX_PACKET (64)
#define IDE_BAUDRATE_SLOW (921600)
#define IDE_BAUDRATE_FAST (12000000)
#define CONFIG_USB_CDC_RX_BUFSIZE 4096
// MicroPython runs as a task under FreeRTOS
#define USB_CDC_TASK_PRIORITY (ESP_TASK_PRIO_MIN + 10)
#define USB_CDC_TASK_STACK_SIZE (16 * 1024)

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

volatile uint8_t dbg_mode_enabled = 1;
static bool linecode_set = false, userial_open = false;
#define debug(fmt, ...) \
    if (0)              \
    printf(fmt, ##__VA_ARGS__)

extern void usbdbg_data_in(void *buffer, int length);
extern void usbdbg_data_out(void *buffer, int length);
extern void usbdbg_control(void *buffer, uint8_t brequest, uint32_t wlength);
//  uint8_t *tx_ringbuf_array=NULL;
// uint8_t tx_ringbuf_array[1024 * 200];
// volatile ringbuf_t tx_ringbuf;

uint32_t usb_cdc_buf_len()
{
    return ringbuf_avail((ringbuf_t *)&tx_ringbuf);
}

uint32_t usb_cdc_get_buf(uint8_t *buf, uint32_t len)
{
    int i = 0;
    for (; i < len; i++)
    {
        buf[i] = ringbuf_get((ringbuf_t *)&tx_ringbuf);
        if (buf[i] == -1)
        {
            break;
        }
    }
    return i;
}

#include "py/runtime.h"
#include "py/mphal.h"
void cdc_task_serial_mode(void)
{
    // if (tud_ready() && userial_open) {
    if (userial_open)
    {

        // connected and there are data available
        // if (tud_cdc_available())
        // {
        uint8_t usb_rx_buf[4096];
        usbd_ep_start_read(CDC_OUT_EP, usb_rx_buf, 4096);
        uint32_t len = 4096;
        // tud_cdc_read(usb_rx_buf, CONFIG_USB_CDC_RX_BUFSIZE);
        for (int i = 0; i < len; i++)
        {
            if (usb_rx_buf[i] == mp_interrupt_char)
            {
                debug("keyboard_interrupt. ");
                mp_sched_keyboard_interrupt();
            }
            else
            {
                ringbuf_put(&stdin_ringbuf, usb_rx_buf[i]);
            }
            debug("%c", usb_rx_buf[i]);
        }
        debug(" avail %d, put %d bytes\n", ringbuf_avail(&stdin_ringbuf), len);
        // }
        //  else {
        //   vTaskDelay(pdMS_TO_TICKS(1));
        // }
    }
    else
    {
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

#define CDC_WRDAT(w_buf, w_len)                       \
    do                                                \
    {                                                 \
        usbd_ep_start_write(CDC_IN_EP, w_buf, w_len); \
    } while (0)

void usb_tx_strn(const char *str, size_t len)
{
    debug("usb_tx_strn %d bytes\n", len);
    if (dbg_mode_enabled == false)
    {
        CDC_WRDAT(str, len);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    else
    {
        for (int i = 0; i < len; i++)
        {
            while (ringbuf_put(&tx_ringbuf, (uint8_t)str[i]) < 0)
            {
                printf("Error: usb_cdc tx_ringbuf overflow!\n");
                vTaskDelay(pdMS_TO_TICKS(1));
            }
        }
    }
}

void cdc_task_debug_mode(void)
{
    // if (tud_cdc_connected()) {
    //   if(tud_cdc_available()) {
    uint8_t buf[DBG_MAX_PACKET];
    uint32_t count = usbd_ep_start_read(CDC_OUT_EP, buf, 6);
    // if (count < 6) {
    //     //Shouldn't happen
    //     return;
    // }
    // assert buf[0] == '\x30';
    uint8_t request = buf[1];
    uint32_t xfer_length = *((uint32_t *)(buf + 2));
    usbdbg_control(buf + 6, request, xfer_length);

    while (xfer_length)
    {
        if (request & 0x80)
        {
            // Device-to-host data phase
            int bytes = MIN(xfer_length, DBG_MAX_PACKET);
            // if (bytes <= tud_cdc_write_available()) {
            xfer_length -= bytes;
            usbdbg_data_in(buf, bytes);
            // tud_cdc_write(buf, bytes);
            usbd_ep_start_write(CDC_IN_EP, buf, bytes);
            // }
        }
        else
        {
            // Host-to-device data phase
            int bytes = MIN(xfer_length, DBG_MAX_PACKET);
            usbd_ep_start_read(CDC_OUT_EP, buf, bytes);
            // uint32_t count = tud_cdc_read(buf, bytes);
            // if (count == bytes) {
            xfer_length -= bytes;
            usbdbg_data_out(buf, bytes);
            // }
        }
    }
    // tud_cdc_write_flush();
    //   }
    // }
    // else {
    //   vTaskDelay(pdMS_TO_TICKS(1));
    // }
}

// void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* p_line_coding)
// {
//     if (p_line_coding->bit_rate == IDE_BAUDRATE_SLOW ||
//             p_line_coding->bit_rate == IDE_BAUDRATE_FAST) {
//         dbg_mode_enabled = 1;
//     } else {
//         dbg_mode_enabled = 0;
//     }
//     tx_ringbuf.iget = 0;
//     tx_ringbuf.iput = 0;

//     linecode_set = true;
//     debug("tud_cdc_line_coding_cb dbg_mode_enabled %d\n", dbg_mode_enabled);
// }

// void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
// {
//   if(!dtr) {
//     if(linecode_set && !userial_open)
//       userial_open = true;
//     else if(userial_open && !linecode_set) {
//       userial_open = false;
//     }

//     if(linecode_set)
//       linecode_set = false;

//   } else {
//       userial_open = true;
//   }

//   debug("tud_cdc_line_state_cb userial_open %d, line_state %d\n", userial_open, (dtr|(rts<<1)));
// }

bool is_dbg_mode_enabled(void)
{
    return dbg_mode_enabled;
}

void cdc_loop()
{
    while (true)
    {
        if (is_dbg_mode_enabled())
        {
            cdc_task_debug_mode();
        }
        else
        {
            cdc_task_serial_mode();
        }
    }
}
#include "esp_ota_ops.h"
#include "esp_partition.h"
#define OP_READ     0
#define OP_WRITE    1

// TODO: Split the caching out of supervisor/shared/external_flash so we can use it.
#define SECTOR_SIZE 4096
#define FILESYSTEM_BLOCK_SIZE 512
STATIC uint8_t _cache[SECTOR_SIZE];
STATIC uint32_t _cache_lba = 0xffffffff;

#if CIRCUITPY_STORAGE_EXTEND
#if FF_MAX_SS == FF_MIN_SS
#define SECSIZE(fs) (FF_MIN_SS)
#else
#define SECSIZE(fs) ((fs)->ssize)
#endif // FF_MAX_SS == FF_MIN_SS
STATIC DWORD fatfs_bytes(void) {
    FATFS *fatfs = filesystem_circuitpy();
    return (fatfs->csize * SECSIZE(fatfs)) * (fatfs->n_fatent - 2);
}
STATIC bool storage_extended = true;
STATIC const esp_partition_t *_partition[2];
#else
STATIC const esp_partition_t *_partition[1];
#endif // CIRCUITPY_STORAGE_EXTEND

void supervisor_flash_init(void) {
    if (_partition[0] != NULL) {
        return;
    }
    _partition[0] = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_FAT,
        NULL);
    #if CIRCUITPY_STORAGE_EXTEND
    _partition[1] = esp_ota_get_next_update_partition(NULL);
    #endif
}


struct usbd_interface intf0;
struct usbd_interface intf1;
struct usbd_interface intf2;
typedef void *pdrv_t;
pdrv_t msc_pdrv;
void cdc_acm_init00(void)
{
    // const uint8_t data[10] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30 };
    supervisor_flash_init();
    // memcpy(&write_buffer[0], data, 10);
    // memset(&write_buffer[10], 'a', 2038);
    tx_ringbuf.buf = tx_ringbuf_array;
    tx_ringbuf.size = sizeof(tx_ringbuf_array);
    // tx_ringbuf.size =500*1024;
    tx_ringbuf.iget = 0;
    tx_ringbuf.iput = 0;
    usbd_desc_register(cdc_msc_descriptor);
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf1));
    usbd_add_endpoint(&cdc_out_ep);
    usbd_add_endpoint(&cdc_in_ep);
    usbd_add_interface(usbd_msc_init_intf(&intf2, MSC_OUT_EP, MSC_IN_EP));

    usbd_initialize();
    //     char *path = "/";
    // char *out_path = NULL;
    // mp_vfs_mount_t *vfs_mount = mp_vfs_lookup_path(path,&out_path);
    // msc_pdrv = ((fs_user_mount_t *)vfs_mount->obj);

    // xTaskCreatePinnedToCore(cdc_loop, "cdc_loop", 4096 / sizeof(StackType_t), NULL, 15, NULL, 1);
}
#if CONFIG_IDF_TARGET_ESP32S3
static void usb_otg_router_to_internal_phy()
{
    uint32_t *usb_phy_sel_reg = (uint32_t *)(0x60008000 + 0x120);
    *usb_phy_sel_reg |= BIT(19) | BIT(20);
}
#endif
// int usb_cdc_init(void)
// {
//     vTaskDelay(100 / portTICK_PERIOD_MS);
//     static bool initialized = false;
//     if (!initialized) {
//         initialized = true;
//         // tx_ringbuf_array = malloc(1024*300);
//         // tx_ringbuf_array = heap_caps_malloc(500*1024, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

//         tx_ringbuf.buf = tx_ringbuf_array;
//         tx_ringbuf.size = sizeof(tx_ringbuf_array);
//         // tx_ringbuf.size =500*1024;
//         tx_ringbuf.iget = 0;
//         tx_ringbuf.iput = 0;
//         tinyusb_config_t tusb_cfg = {
//             .descriptor = NULL,
//             .string_descriptor = NULL,
//             .external_phy = false // In the most cases you need to use a `false` value
//         };
// #if CONFIG_IDF_TARGET_ESP32S3
//         usb_otg_router_to_internal_phy();
// #endif
//         ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
//         xTaskCreatePinnedToCore(cdc_loop, "cdc_loop", USB_CDC_TASK_STACK_SIZE / sizeof(StackType_t), NULL, USB_CDC_TASK_PRIORITY, NULL, 1);
//     }
//     return 0;
// }

void cdc_printf2(const char *fmt, ...)
{
    va_list ap;
    char p_buf[1024];
    int p_len;

    va_start(ap, fmt);
    p_len = vsprintf(p_buf, fmt, ap);
    va_end(ap); // debug("cdc_printf size %d, cdc open %d\n", p_len, tud_cdc_connected());
    p_buf[p_len] = '\r';
    p_buf[p_len + 1] = '\n';
    p_buf[p_len + 2] = 0;
    if (!dbg_mode_enabled && userial_open)
        CDC_WRDAT(p_buf, p_len + 2);
    else
        printf("%s", p_buf);
}

void cdc_printf(const char *fmt, ...)
{
    if (!dbg_mode_enabled && userial_open)
    {
        va_list ap;
        char p_buf[256];
        int p_len;

        va_start(ap, fmt);
        p_len = vsprintf(p_buf, fmt, ap);
        va_end(ap); // debug("cdc_printf size %d, cdc open %d\n", p_len, tud_cdc_connected());
        CDC_WRDAT(p_buf, p_len);
    }
}


uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    #if CIRCUITPY_STORAGE_EXTEND
    return ((storage_extended) ? (_partition[0]->size + _partition[1]->size) : _partition[0]->size) / FILESYSTEM_BLOCK_SIZE;
    #else
    return _partition[0]->size / FILESYSTEM_BLOCK_SIZE;
    #endif
}

void port_internal_flash_flush(void) {
}

STATIC void single_partition_rw(const esp_partition_t *partition, uint8_t *data,
    const uint32_t offset, const uint32_t size_total, const bool op) {
    if (op == OP_READ) {
        esp_partition_read(partition, offset, data, size_total);
    } else {
        esp_partition_erase_range(partition, offset, size_total);
        esp_partition_write(partition, offset, _cache, size_total);
    }
}

#if CIRCUITPY_STORAGE_EXTEND
STATIC void multi_partition_rw(uint8_t *data,
    const uint32_t offset, const uint32_t size_total, const bool op) {
    if (offset > _partition[0]->size) {
        // only r/w partition 1
        single_partition_rw(_partition[1], data, (offset - _partition[0]->size), size_total, op);
    } else if ((offset + size_total) > _partition[0]->size) {
        // first r/w partition 0, then partition 1
        uint32_t size_0 = _partition[0]->size - offset;
        uint32_t size_1 = size_total - size_0;
        if (op == OP_READ) {
            esp_partition_read(_partition[0], offset, data, size_0);
            esp_partition_read(_partition[1], 0, (data + size_0), size_1);
        } else {
            esp_partition_erase_range(_partition[0], offset, size_0);
            esp_partition_write(_partition[0], offset, _cache, size_0);
            esp_partition_erase_range(_partition[1], 0, size_1);
            esp_partition_write(_partition[1], 0, (_cache + size_0), size_1);
        }
    } else {
        // only r/w partition 0
        single_partition_rw(_partition[0], data, offset, size_total, op);
    }
}
#endif

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    const uint32_t offset = block * FILESYSTEM_BLOCK_SIZE;
    const uint32_t read_total = num_blocks * FILESYSTEM_BLOCK_SIZE;
    #if CIRCUITPY_STORAGE_EXTEND
    multi_partition_rw(dest, offset, read_total, OP_READ);
    #else
    single_partition_rw(_partition[0], dest, offset, read_total, OP_READ);
    #endif
    return 0; // success
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t lba, uint32_t num_blocks) {
    uint32_t blocks_per_sector = SECTOR_SIZE / FILESYSTEM_BLOCK_SIZE;
    uint32_t block = 0;
    while (block < num_blocks) {
        uint32_t block_address = lba + block;
        uint32_t sector_offset = block_address / blocks_per_sector * SECTOR_SIZE;
        uint8_t block_offset = block_address % blocks_per_sector;
        if (_cache_lba != block_address) {
            supervisor_flash_read_blocks(_cache, sector_offset / FILESYSTEM_BLOCK_SIZE, blocks_per_sector);
            _cache_lba = sector_offset;
        }
        for (uint8_t b = block_offset; b < blocks_per_sector; b++) {
            // Stop copying after the last block.
            if (block >= num_blocks) {
                break;
            }
            memcpy(_cache + b * FILESYSTEM_BLOCK_SIZE,
                src + block * FILESYSTEM_BLOCK_SIZE,
                FILESYSTEM_BLOCK_SIZE);
            block++;
        }
        #if CIRCUITPY_STORAGE_EXTEND
        multi_partition_rw(_cache, sector_offset, SECTOR_SIZE, OP_WRITE);
        #else
        single_partition_rw(_partition[0], _cache, sector_offset, SECTOR_SIZE, OP_READ);
        #endif
    }
    return 0; // success
}

void supervisor_flash_release_cache(void) {
}

void supervisor_flash_set_extended(bool extended) {
    #if CIRCUITPY_STORAGE_EXTEND
    storage_extended = extended;
    #endif
}

bool supervisor_flash_get_extended(void) {
    #if CIRCUITPY_STORAGE_EXTEND
    return storage_extended;
    #else
    return false;
    #endif
}

void supervisor_flash_update_extended(void) {
    #if CIRCUITPY_STORAGE_EXTEND
    storage_extended = (_partition[0]->size < fatfs_bytes());
    #endif
}
#define BLOCK_SIZE 512
#define BLOCK_COUNT 10

typedef struct
{
    uint8_t BlockSpace[BLOCK_SIZE];
} BLOCK_TYPE;

BLOCK_TYPE mass_block[BLOCK_COUNT];

void usbd_msc_get_cap(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    // *block_num = 1000; // Pretend having so many buffer,not has actually.
    // *block_size = BLOCK_SIZE;
    *block_num = supervisor_flash_get_block_count(); // Pretend having so many buffer,not has actually.
    *block_size = supervisor_flash_get_block_size();
    //     ESP_LOGD(__func__, "");

    // if (unlikely(lun >= 1)) {
    //     ESP_LOGE(__func__, "invalid lun number %u", lun);
    //     return;
    // }

    // // disk_ioctl(msc_pdrv, GET_SECTOR_COUNT, block_num);
    // // disk_ioctl(msc_pdrv, GET_SECTOR_SIZE, block_size);
    // // s_disk_block_size[lun] = *block_size;
    ESP_LOGI(__func__, "lun = %u GET_SECTOR_COUNT = %d，GET_SECTOR_SIZE = %d",lun, *block_num, *block_size);
}
int usbd_msc_sector_read(uint32_t sector, uint8_t *buffer, uint32_t length)
{
    // const uint32_t offset = sector * FILESYSTEM_BLOCK_SIZE;
    if (sector < 10)
        memcpy(buffer, mass_block[sector].BlockSpace, length);
        // supervisor_flash_read_blocks(buffer,sector,length);
            // single_partition_rw(_partition[0], buffer, offset, length, OP_READ);

    return 0;
    //     ESP_LOGD(__func__, "");

    // if (unlikely(lun >= LOGICAL_DISK_NUM)) {
    //     ESP_LOGE(__func__, "invalid lun number %u", lun);
    //     return 0;
    // }

    // const uint32_t block_count = length / s_disk_block_size[lun];
    // disk_read(msc_pdrv, buffer, lba, block_count);
    // return block_count * s_disk_block_size[lun];
}

int usbd_msc_sector_write(uint32_t sector, uint8_t *buffer, uint32_t length)
{
    if (sector < 10)
        memcpy(mass_block[sector].BlockSpace, buffer, length);
    // const uint32_t offset = sector * FILESYSTEM_BLOCK_SIZE;
    // single_partition_rw(_partition[0], buffer, offset, length, OP_WRITE);

    return 0;
}