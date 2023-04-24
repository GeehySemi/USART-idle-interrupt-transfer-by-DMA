/*!
 * @file        usb_StandardRequest.h
 *
 * @brief       USB standard request process
 *
 * @version     V1.0.2
 *
 * @date        2022-12-01
 *
 * @attention
 *
 *  Copyright (C) 2021-2022 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Define to prevent recursive inclusion */
#ifndef __USBH_STDREQ_H_
#define __USBH_STDREQ_H_

/* Includes */
#include "drv_usb.h"
#include "usb_config.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Host Core Host
  @{
*/

/** @addtogroup Standard
  @{
*/

/** @addtogroup StdReq
  @{
*/

/** @defgroup StdReq_Macros Macros
  @{
*/

#define STD_DEV_DESC_SIZE          18
#define STD_CFG_DESC_SIZE          9
#define STD_INTERFACE_DESC_SIZE    9
#define STD_EP_DESC_SIZE           7
#define STD_HID_DESC_SIZE          9
#define USBH_LANG_ID               0x0409

#define USBH_ReadConfigurationItfNum(pUsbHost) \
        (((USBH_Info_T *)pUsbHost)->control.descriptor.configuration.bNumInterfaces)


#define USBH_ReadInterfaceClass(pUsbHost, itfIndex) \
        (((USBH_Info_T *)pUsbHost)->control.descriptor.interface[itfIndex].itfDesc.bInterfaceClass)

#define USBH_ReadInterfaceSubClass(pUsbHost, itfIndex) \
        (((USBH_Info_T *)pUsbHost)->control.descriptor.interface[itfIndex].itfDesc.bInterfaceSubClass)

#define USBH_ReadInterfaceProtocol(pUsbHost, itfIndex) \
        (((USBH_Info_T *)pUsbHost)->control.descriptor.interface[itfIndex].itfDesc.bInterfaceProtocol)

#define USBH_ReadInterfaceEpNum(pUsbHost, itfIndex) \
        (((USBH_Info_T *)pUsbHost)->control.descriptor.interface[itfIndex].itfDesc.bNumEndpoints)

#define USBH_ReadEndpointType(pUsbHost, itfIndex, epIndex) \
        (((USBH_Info_T *)pUsbHost)->control.descriptor.interface[itfIndex].epDesc[epIndex].bmAttributes)

#define USBH_ReadEndpointAddress(pUsbHost, itfIndex, epIndex) \
        (((USBH_Info_T *)pUsbHost)->control.descriptor.interface[itfIndex].epDesc[epIndex].bEndpointAddress)

#define USBH_ReadEndpointMPS(pUsbHost, itfIndex, epIndex) \
        ((uint16_t)((USBH_Info_T *)pUsbHost)->control.descriptor.interface[itfIndex].epDesc[epIndex].wMaxPacketSize[0] | \
         (uint16_t)((USBH_Info_T *)pUsbHost)->control.descriptor.interface[itfIndex].epDesc[epIndex].wMaxPacketSize[1] << 8)

/**@} end of group StdReq_Macros */

/** @defgroup StdReq_Enumerates Enumerates
  @{
*/

/**
 * @brief   USB device request direction
 */
enum
{
    USBH_REQ_DIR_OUT,
    USBH_REQ_DIR_IN
};

/**
 * @brief   USB device request type
 */
enum
{
    USBH_REQ_TYPE_STANDARD   = 0,
    USBH_REQ_TYPE_CLASS      = 1,
    USBH_REQ_TYPE_VENDOR     = 2,
    USBH_REQ_TYPE_RESERVED   = 3
};

/**
 * @brief   USB device request recipient
 */
typedef enum
{
    USBH_RECIPIENT_DEVICE    = 0,
    USBH_RECIPIENT_INTERFACE = 1,
    USBH_RECIPIENT_ENDPOINT  = 2,
    USBH_RECIPIENT_OTHER     = 3
} USBH_RECIPIENT_T;

/**
 * @brief   USB standard device standard requests type
 */
enum
{
    USBH_STD_GET_STATUS          = 0,
    USBH_STD_CLEAR_FEATURE       = 1,
    USBH_STD_SET_FEATURE         = 3,
    USBH_STD_SET_ADDRESS         = 5,
    USBH_STD_GET_DESCRIPTOR      = 6,
    USBH_STD_SET_DESCRIPTOR      = 7,
    USBH_STD_GET_CONFIGURATION   = 8,
    USBH_STD_SET_CONFIGURATION   = 9,
    USBH_STD_GET_INTERFACE       = 10,
    USBH_STD_SET_INTERFACE       = 11,
    USBH_STD_SYNCH_FRAME         = 12
};

/**
 * @brief   USB descriptor types
 */
typedef enum
{
    USBH_DESC_DEVICE             = 1,
    USBH_DESC_CONFIGURATION      = 2,
    USBH_DESC_STRING             = 3,
    USBH_DESC_INTERFACE          = 4,
    USBH_DESC_ENDPOINT           = 5,
    USBH_DESC_DEVICE_QUALIFIER   = 6,
    USBH_DESC_OTHER_SPEED        = 7,
    USBH_INTERFACE_POWER         = 8,
    USBH_DESC_HID                = 0x21,
    USBH_DESC_HID_REPORT         = 0x22
} USBH_DESC_TYPE_T;

/**
 * @brief   USB standard feature
 */
typedef enum
{
    USBH_FEATURE_ENDPOINT_HALT   = 0,
    USBH_FEATURE_REMOTE_WAKEUP   = 1,
    USBH_FEATURE_TEST_MODE       = 2
} USBH_FEATURE_TEST_T;

/**
 * @brief   USB Class
 */
typedef enum
{
    USBH_CLASS_AUDIO     = 0x01,  /*!< Audio */
    USBH_CLASS_CDCC      = 0x02,  /*!< Communications and CDC Control */
    USBH_CLASS_HID       = 0x03,  /*!< HID (Human Interface Device) */
    USBH_CLASS_PRINTER   = 0x07,  /*!< Printer */
    USBH_CLASS_MSC       = 0x08,  /*!< Mass Storage */
    USBH_CLASS_HUB       = 0x09,  /*!< Hub */
    USBH_CLASS_CDCD      = 0x0A,  /*!< CDC-Data */
    USBH_CLASS_SMARTCARD = 0x0B,  /*!< Smart Card */
    USBH_CLASS_VIDEO     = 0x0E,  /*!< Video */
    USBH_CLASS_AVD       = 0x10   /*!< Audio/Video Devices */
} USBH_CLASS_T;

/**@} end of group StdReq_Enumerates */

/** @defgroup StdReq_Structures Structures
  @{
*/

/**
 * @brief   USB device descriptor
 */
typedef struct
{
    uint8_t bLength;            /*!< Descriptor length */
    uint8_t bDescriptorType;    /*!< Descriptor Type */
    uint8_t bcdUSB[2];          /*!< BCD of the supported USB specification */
    uint8_t bDeviceClass;       /*!< USB device class */
    uint8_t bDeviceSubClass;    /*!< USB device subclass */
    uint8_t bDeviceProtocol;    /*!< USB device protocol */
    uint8_t bMaxPacketSize;     /*!< Max Packet Size */
    uint8_t idVendor[2];        /*!< Vendor ID */
    uint8_t idProduct[2];       /*!< Product ID */
    uint8_t bcdDevice[2];       /*!< Device Release Number */
    uint8_t iManufacturer;      /*!< Index of Manufacturer String Descriptor */
    uint8_t iProduct;           /*!< Index of Product String Descriptor */
    uint8_t iSerialNumber;      /*!< Index of Serial Number String Descriptor */
    uint8_t bNumConfigurations; /*!< Number of Possible Configurations */
} USBH_DeviceDesc_T;

/**
 * @brief   USB Configuration descriptor
 */
typedef struct
{
    uint8_t  bLength;             /*!< Descriptor length */
    uint8_t  bDescriptorType;     /*!< Descriptor Type */
    uint8_t  wTotalLength[2];     /*!< Total Length of Configuration Descriptor gather */
    uint8_t  bNumInterfaces;      /*!< Total number of interfaces in the configuration */
    uint8_t  bConfigurationValue; /*!< Configuration index of the current configuration */
    uint8_t  iConfiguration;      /*!< Index of a string descriptor describing the configuration */
    uint8_t  bmAttributes;        /*!< Configuration attributes */
    uint8_t  bMaxPower;           /*!< Maximum power consumption */
} USBH_ConfigurationDesc_T;

/**
 * @brief   USB Interface descriptor
 */
typedef struct
{
    uint8_t bLength;             /*!< Descriptor length */
    uint8_t bDescriptorType;     /*!< Descriptor Type */
    uint8_t bInterfaceNumber;    /*!< Interface Number */
    uint8_t bAlternateSetting;   /*!< Value used to select alternative setting */
    uint8_t bNumEndpoints;       /*!< Number of Endpoints used for this interface */
    uint8_t bInterfaceClass;     /*!< Class Code */
    uint8_t bInterfaceSubClass;  /*!< Sub class Code */
    uint8_t bInterfaceProtocol;  /*!< Protocol Code */
    uint8_t iInterface;          /*!< Index of String Descriptor of this interface */
} USBH_InterfaceDesc_T;

/**
 * @brief   USB Endpoint descriptor
 */
typedef struct
{
    uint8_t bLength;           /*!< Descriptor length */
    uint8_t bDescriptorType;   /*!< Descriptor Type */
    uint8_t bEndpointAddress;  /*!< Indicates what endpoint this descriptor is describing */
    uint8_t bmAttributes;      /*!< Specifies the transfer type */
    uint8_t wMaxPacketSize[2]; /*!< Maximum Packet Size this endpoint is capable of sending or receiving */
    uint8_t bInterval;         /*!< Polling interval in milliseconds for the endpoint */
} USBH_EndpointDesc_T;

typedef struct
{
    USBH_InterfaceDesc_T itfDesc;
    USBH_EndpointDesc_T  epDesc[ENDPOINT_DESC_MAX_NUM];
} USBH_Interface_T;

/**
 * @brief   USB request type
 */
typedef union
{
    uint8_t byte;

    struct
    {
        uint8_t recipient       : 5;
        uint8_t type            : 2;
        uint8_t dir             : 1;
    } bit;
} USBH_REQ_TYPE_T;

/**
 * @brief   USB Host request data
 */
typedef struct
{
    union
    {
        uint8_t pack[8];

        struct
        {
            USBH_REQ_TYPE_T bmRequestType;
            uint8_t         bRequest;
            uint8_t         wValue[2];
            uint8_t         wIndex[2];
            uint8_t         wLength[2];
        } domain;
    };
} USBH_HostReqData_T;

/**@} end of group StdReq_Structures */

/** @defgroup StdReq_Functions Functions
  @{
*/
void USBH_StandardGetConfiguration(void);
void USBH_StandardGetDescriptor(USBH_DESC_TYPE_T descType, uint8_t descIndex, uint16_t len);
void USBH_StandardGetInterface(uint16_t interface);
void USBH_StandardGetStatus(USBH_RECIPIENT_T recipient, uint16_t index);

void USBH_StandardSetAddress(uint8_t devAddr);
void USBH_StandardSetConfiguration(uint8_t configuration);
void USBH_StandardSetDescriptor(USBH_DESC_TYPE_T descType, uint8_t descIndex, uint16_t len);
void USBH_StandardSetInterface(uint16_t interface, uint16_t alternateSetting);
void USBH_StandardSetFeature(USBH_RECIPIENT_T recipient, USBH_FEATURE_TEST_T feature, uint16_t index);
void USBH_StandardClearFeature(USBH_RECIPIENT_T recipient,
                               USBH_FEATURE_TEST_T feature,
                               uint16_t index,
                               uint8_t channelNum);

void USBH_ParseCfgDesc(uint8_t* parseBuf,
                       USBH_ConfigurationDesc_T* cfgDescBuf,
                       USBH_Interface_T* interface);
void USBH_ParseStrDescriptor(uint8_t* DescBuff, uint8_t* strBuff);
/**@} end of group StdReq_Functions */
/**@} end of group StdReq */
/**@} end of group Standard */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */

#endif
