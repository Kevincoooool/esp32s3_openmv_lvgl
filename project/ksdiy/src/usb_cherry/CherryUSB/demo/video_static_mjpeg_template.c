#include "usbd_core.h"
#include "usbd_video.h"
#include "pic_data.h"

#define VIDEO_IN_EP 0x81

#ifdef CONFIG_USB_HS
#define MAX_PAYLOAD_SIZE  1024 // for high speed with one transcations every one micro frame
#define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 1)) | (0x00 << 11))

// #define MAX_PAYLOAD_SIZE  2048 // for high speed with two transcations every one micro frame
// #define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 2)) | (0x01 << 11))

// #define MAX_PAYLOAD_SIZE  3072 // for high speed with three transcations every one micro frame
// #define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 3)) | (0x02 << 11))

#else
#define MAX_PAYLOAD_SIZE  1020
#define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 1)) | (0x00 << 11))
#endif

#define WIDTH  (unsigned int)(640)
#define HEIGHT (unsigned int)(480)

#define CAM_FPS        (30)
#define INTERVAL       (unsigned long)(10000000 / CAM_FPS)
#define MIN_BIT_RATE   (unsigned long)(WIDTH * HEIGHT * 16 * CAM_FPS) //16 bit
#define MAX_BIT_RATE   (unsigned long)(WIDTH * HEIGHT * 16 * CAM_FPS)
#define MAX_FRAME_SIZE (unsigned long)(WIDTH * HEIGHT * 2)

#define USB_VIDEO_DESC_SIZ (unsigned long)(9 +  \
                                           8 +  \
                                           9 +  \
                                           13 + \
                                           18 + \
                                           9 +  \
                                           12 + \
                                           9 +  \
                                           14 + \
                                           11 + \
                                           30 + \
                                           9 +  \
                                           7)

#define VC_TERMINAL_SIZ (unsigned int)(13 + 18 + 12 + 9)
#define VS_HEADER_SIZ   (unsigned int)(13 + 1 + 11 + 30)

#define USBD_VID           0xffff
#define USBD_PID           0xffff
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

const uint8_t video_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xef, 0x02, 0x01, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_VIDEO_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    VIDEO_VC_DESCRIPTOR_INIT(0x00, 0, 0x0100, VC_TERMINAL_SIZ, 48000000, 0x02),
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x00, 0x00),
    VIDEO_VS_HEADER_DESCRIPTOR_INIT(0x01, VS_HEADER_SIZ, VIDEO_IN_EP, 0x00),
    VIDEO_VS_FORMAT_MJPEG_DESCRIPTOR_INIT(0x01, 0x01),
    VIDEO_VS_FRAME_MJPEG_DESCRIPTOR_INIT(0x01, WIDTH, HEIGHT, MIN_BIT_RATE, MAX_BIT_RATE, MAX_FRAME_SIZE, DBVAL(INTERVAL), 0x01, DBVAL(INTERVAL)),
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x01, 0x01),
    /* 1.2.2.2 Standard VideoStream Isochronous Video Data Endpoint Descriptor */
    0x07,                         /* bLength */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: ENDPOINT */
    0x81,                         /* bEndpointAddress: IN endpoint 2 */
    0x01,                         /* bmAttributes: Isochronous transfer type. Asynchronous synchronization type. */
    WBVAL(VIDEO_PACKET_SIZE),     /* wMaxPacketSize */
    0x01,                         /* bInterval: One frame interval */

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
    'U', 0x00,                  /* wcChar10 */
    'V', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
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
    '1', 0x00,                  /* wcChar3 */
    '0', 0x00,                  /* wcChar4 */
    '3', 0x00,                  /* wcChar5 */
    '1', 0x00,                  /* wcChar6 */
    '0', 0x00,                  /* wcChar7 */
    '0', 0x00,                  /* wcChar8 */
    '0', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

void usbd_configure_done_callback(void)
{
    /* no out ep, so do nothing */
}

volatile bool tx_flag = 0;
volatile bool iso_tx_busy = false;

void usbd_video_open(uint8_t intf)
{
    tx_flag = 1;
    USB_LOG_RAW("OPEN\r\n");
    iso_tx_busy = false;
}
void usbd_video_close(uint8_t intf)
{
    USB_LOG_RAW("CLOSE\r\n");
    tx_flag = 0;
    iso_tx_busy = false;
}

void usbd_video_iso_callback(uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual in len:%d\r\n", nbytes);
    iso_tx_busy = false;
}

static struct usbd_endpoint video_in_ep = {
    .ep_cb = usbd_video_iso_callback,
    .ep_addr = VIDEO_IN_EP
};

struct usbd_interface intf0;
struct usbd_interface intf1;

void video_init()
{
    usbd_desc_register(video_descriptor);
    usbd_add_interface(usbd_video_init_intf(&intf0, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_interface(usbd_video_init_intf(&intf1, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_endpoint(&video_in_ep);

    usbd_initialize();
}

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t packet_buffer[10 * 1024];

void video_test()
{
    uint32_t out_len;
    uint32_t packets;
    memset(packet_buffer, 0, 10 * 1024);
    while (1) {
        if (tx_flag) {
            packets = usbd_video_mjpeg_payload_fill((uint8_t *)jpeg_data, sizeof(jpeg_data), packet_buffer, &out_len);
#if 0
            iso_tx_busy = true;
            usbd_ep_start_write(VIDEO_IN_EP, packet_buffer, out_len);
            while (iso_tx_busy) {
                if (tx_flag == 0) {
                    break;
                }
            }
#else
            /* dwc2 must use this method */
            for (uint32_t i = 0; i < packets; i++) {
                if (i == (packets - 1)) {
                    iso_tx_busy = true;
                    usbd_ep_start_write(VIDEO_IN_EP, &packet_buffer[i * MAX_PAYLOAD_SIZE], out_len - (packets - 1) * MAX_PAYLOAD_SIZE);
                    while (iso_tx_busy) {
                        if (tx_flag == 0) {
                            break;
                        }
                    }
                } else {
                    iso_tx_busy = true;
                    usbd_ep_start_write(VIDEO_IN_EP, &packet_buffer[i * MAX_PAYLOAD_SIZE], MAX_PAYLOAD_SIZE);
                    while (iso_tx_busy) {
                        if (tx_flag == 0) {
                            break;
                        }
                    }
                }
            }
#endif
        }
    }
}