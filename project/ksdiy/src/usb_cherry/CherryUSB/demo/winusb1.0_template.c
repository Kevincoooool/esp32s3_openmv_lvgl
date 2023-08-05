#include "usbd_core.h"
#include "usbd_cdc.h"

#define WCID_VENDOR_CODE 0x17

__ALIGN_BEGIN const uint8_t WCID_StringDescriptor_MSOS[18] __ALIGN_END = {
    ///////////////////////////////////////
    /// MS OS string descriptor
    ///////////////////////////////////////
    0x12,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    /* MSFT100 */
    'M', 0x00, 'S', 0x00, 'F', 0x00, 'T', 0x00, /* wcChar_7 */
    '1', 0x00, '0', 0x00, '0', 0x00,            /* wcChar_7 */
    WCID_VENDOR_CODE,                           /* bVendorCode */
    0x00,                                       /* bReserved */
};

__ALIGN_BEGIN const uint8_t WINUSB_WCIDDescriptor[40] __ALIGN_END = {
    ///////////////////////////////////////
    /// WCID descriptor
    ///////////////////////////////////////
    0x28, 0x00, 0x00, 0x00,                   /* dwLength */
    0x00, 0x01,                               /* bcdVersion */
    0x04, 0x00,                               /* wIndex */
    0x01,                                     /* bCount */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* bReserved_7 */

    ///////////////////////////////////////
    /// WCID function descriptor
    ///////////////////////////////////////
    0x00, /* bFirstInterfaceNumber */
    0x01, /* bReserved */
    /* WINUSB */
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00, /* cCID_8 */
    /*  */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* cSubCID_8 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             /* bReserved_6 */
};

__ALIGN_BEGIN const uint8_t WINUSB_IF0_WCIDProperties[142] __ALIGN_END = {
    ///////////////////////////////////////
    /// WCID property descriptor
    ///////////////////////////////////////
    0x8e, 0x00, 0x00, 0x00, /* dwLength */
    0x00, 0x01,             /* bcdVersion */
    0x05, 0x00,             /* wIndex */
    0x01, 0x00,             /* wCount */

    ///////////////////////////////////////
    /// registry propter descriptor
    ///////////////////////////////////////
    0x84, 0x00, 0x00, 0x00, /* dwSize */
    0x01, 0x00, 0x00, 0x00, /* dwPropertyDataType */
    0x28, 0x00,             /* wPropertyNameLength */
    /* DeviceInterfaceGUID */
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00,  /* wcName_20 */
    'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00,  /* wcName_20 */
    't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00,  /* wcName_20 */
    'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00,  /* wcName_20 */
    'U', 0x00, 'I', 0x00, 'D', 0x00, 0x00, 0x00, /* wcName_20 */
    0x4e, 0x00, 0x00, 0x00,                      /* dwPropertyDataLength */
    /* {CDB3B5AD-293B-4663-AA36-1AAE46463776} */
    '{', 0x00, 'C', 0x00, 'D', 0x00, 'B', 0x00, /* wcData_39 */
    '3', 0x00, 'B', 0x00, '5', 0x00, 'A', 0x00, /* wcData_39 */
    'D', 0x00, '-', 0x00, '2', 0x00, '9', 0x00, /* wcData_39 */
    '3', 0x00, 'B', 0x00, '-', 0x00, '4', 0x00, /* wcData_39 */
    '6', 0x00, '6', 0x00, '3', 0x00, '-', 0x00, /* wcData_39 */
    'A', 0x00, 'A', 0x00, '3', 0x00, '6', 0x00, /* wcData_39 */
    '-', 0x00, '1', 0x00, 'A', 0x00, 'A', 0x00, /* wcData_39 */
    'E', 0x00, '4', 0x00, '6', 0x00, '4', 0x00, /* wcData_39 */
    '6', 0x00, '3', 0x00, '7', 0x00, '7', 0x00, /* wcData_39 */
    '6', 0x00, '}', 0x00, 0x00, 0x00,           /* wcData_39 */
};

struct usb_msosv1_descriptor msosv1_desc = {
    .string = (uint8_t *)WCID_StringDescriptor_MSOS,
    .string_len = 18,
    .vendor_code = WCID_VENDOR_CODE,
    .compat_id = (uint8_t *)WINUSB_WCIDDescriptor,
    .compat_id_len = sizeof(WINUSB_WCIDDescriptor),
    .comp_id_property = (uint8_t *)WINUSB_IF0_WCIDProperties,
    .comp_id_property_len = sizeof(WINUSB_IF0_WCIDProperties),
};

#define WINUSB_IN_EP  0x81
#define WINUSB_OUT_EP 0x02

#define USBD_VID           0x0d28
#define USBD_PID           0x0404
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE (9 + 9 + 7 + 7)

#ifdef CONFIG_USB_HS
#define WINUSB_EP_MPS 512
#else
#define WINUSB_EP_MPS 64
#endif

const uint8_t winusb_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xff, 0xff, 0xff, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, 0x02, 0xff, 0x01, 0x00, 0x02),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_IN_EP, 0x02, WINUSB_EP_MPS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_OUT_EP, 0x02, WINUSB_EP_MPS, 0x00),
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
    0x2C,                       /* bLength */
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
    'W', 0x00,                  /* wcChar10 */
    'I', 0x00,                  /* wcChar11 */
    'N', 0x00,                  /* wcChar12 */
    'U', 0x00,                  /* wcChar13 */
    'S', 0x00,                  /* wcChar14 */
    'B', 0x00,                  /* wcChar15 */
    ' ', 0x00,                  /* wcChar16 */
    'D', 0x00,                  /* wcChar17 */
    'E', 0x00,                  /* wcChar18 */
    'M', 0x00,                  /* wcChar19 */
    'O', 0x00,                  /* wcChar20 */
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
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[2048];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[2048];

volatile bool ep_tx_busy_flag = false;

void usbd_configure_done_callback(void)
{
    /* setup first out ep read transfer */
    usbd_ep_start_read(WINUSB_OUT_EP, read_buffer, 2048);
}

void usbd_winusb_out(uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual out len:%d\r\n", nbytes);
    // for (int i = 0; i < 100; i++) {
    //     printf("%02x ", read_buffer[i]);
    // }
    // printf("\r\n");
    /* setup next out ep read transfer */
    usbd_ep_start_read(CDC_OUT_EP, read_buffer, 2048);
}

void usbd_winusb_in(uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual in len:%d\r\n", nbytes);

    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(CDC_IN_EP, NULL, 0);
    } else {
        ep_tx_busy_flag = false;
    }
}

struct usbd_endpoint winusb_out_ep = {
    .ep_addr = WINUSB_OUT_EP,
    .ep_cb = usbd_winusb_out
};

struct usbd_endpoint winusb_in_ep = {
    .ep_addr = WINUSB_IN_EP,
    .ep_cb = usbd_winusb_in
};

struct usbd_interface intf0;

void winusb_init(void)
{
    usbd_desc_register(winusb_descriptor);
    usbd_msosv1_desc_register(&msosv1_desc);
    usbd_add_interface(&intf0);
    usbd_add_endpoint(&winusb_out_ep);
    usbd_add_endpoint(&winusb_in_ep);

    usbd_initialize();
}