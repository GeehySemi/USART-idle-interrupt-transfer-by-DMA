/*!
 * @file        usbd_core.h
 *
 * @brief       USB device core function
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
#ifndef __USBD_CORE_H_
#define __USBD_CORE_H_

/* Includes */
#include "drv_usb.h"
#include "drv_usb_device.h"
#include "usb_config.h"


/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Device Core Device
  @{
*/

/** @addtogroup Standrad
  @{
*/

/** @addtogroup Core
  @{
*/

/** @defgroup Core_Macros Macros
  @{
*/

/* Get minimum value */
#define USB_MIN(a, b)      (a >= b ? b : a)

/* Get maximum value */
#define USB_MAX(a, b)      (a >= b ? a : b)

/**@} end of group Core_Macros */

/** @defgroup Core_Enumerations Enumerations
  @{
*/

/**
 * @brief   USB device request type
 */
enum
{
    USBD_REQ_TYPE_STANDARD   = 0,
    USBD_REQ_TYPE_CLASS      = 1,
    USBD_REQ_TYPE_VENDOR     = 2,
    USBD_REQ_TYPE_RESERVED   = 3
};

/**
 * @brief   USB device request recipient
 */
enum
{
    USBD_RECIPIENT_DEVICE    = 0,
    USBD_RECIPIENT_INTERFACE = 1,
    USBD_RECIPIENT_ENDPOINT  = 2,
    USBD_RECIPIENT_OTHER     = 3
};

/**
 * @brief   USB standard device requests
 */
enum
{
    USBD_GET_STATUS          = 0,
    USBD_CLEAR_FEATURE       = 1,
    USBD_SET_FEATURE         = 3,
    USBD_SET_ADDRESS         = 5,
    USBD_GET_DESCRIPTOR      = 6,
    USBD_SET_DESCRIPTOR      = 7,
    USBD_GET_CONFIGURATION   = 8,
    USBD_SET_CONFIGURATION   = 9,
    USBD_GET_INTERFACE       = 10,
    USBD_SET_INTERFACE       = 11,
    USBD_SYNCH_FRAME         = 12
};

/**
 * @brief   USB descriptor types
 */
enum
{
    USBD_DESC_DEVICE             = 1,
    USBD_DESC_CONFIGURATION      = 2,
    USBD_DESC_STRING             = 3,
    USBD_DESC_INTERFACE          = 4,
    USBD_DESC_ENDPOINT           = 5,
    USBD_DESC_DEVICE_QUALIFIER   = 6,
    USBD_DESC_OTHER_SPEED        = 7,
    USBD_INTERFACE_POWER         = 8
};

/**
 * @brief   USB standard feature
 */
enum
{
    USBD_FEATURE_ENDPOINT_HALT   = 0,
    USBD_FEATURE_REMOTE_WAKEUP   = 1,
    USBD_FEATURE_TEST_MODE       = 2
};

/**
 * @brief   USB device state
 */
typedef enum
{
    USBD_DEVICE_STATE_DEFAULT,
    USBD_DEVICE_STATE_ADDRESS,
    USBD_DEVICE_STATE_CONFIGURED,
    USBD_DEVICE_STATE_SUSPENDED
} USBD_STATE_T;

/**
 * @brief   USB device state
 */
typedef enum
{
    USBD_CTRL_STATE_IN_DATA,
    USBD_CTRL_STATE_IN_STATUS,
    USBD_CTRL_STATE_OUT_DATA,
    USBD_CTRL_STATE_OUT_STATUS,
    USBD_CTRL_STATE_STALL
} USBD_CTRL_STATE_T;

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
} USBD_REQ_TYPE_T;

/**
 * @brief   USB device request data
 */
typedef struct
{
    union
    {
        uint8_t pack[8];

        struct
        {
            USBD_REQ_TYPE_T bmRequestType;
            uint8_t         bRequest;
            uint8_t         wValue[2];
            uint8_t         wIndex[2];
            uint8_t         wLength[2];
        } domain;
    };
} USBD_DevReqData_T;

/**
 * @brief   Descriptor structure
 */
typedef struct
{
    uint8_t* pDesc;
    uint8_t size;
} USBD_Descriptor_T;

/* USB request handler function define */
typedef void (*USBD_ReqHandler_T)(USBD_DevReqData_T*);

typedef void (*USBD_StdReqHandler_T)(void);

/* Endpoint handler function define */
typedef void (*USBD_EPHandler_T)(uint8_t ep);

/* Reset handler function define */
typedef void (*USBD_ResetHandler_T)(void);

/* Interrupt handler function define */
typedef void (*USBD_InterruptHandler_T)(void);

/* Ctrl Tx Status handler function define */
typedef void (*USBD_CtrlTxStatusHandler_T)(void);

/* Ctrl Rx Status handler function define */
typedef void (*USBD_CtrlRxStatusHandler_T)(void);


/**
 * @brief   Control transfer buffer
 */
typedef struct
{
    uint8_t* pBuf;         /*!< Data buffer */
    uint32_t bufLen;       /*!< Length of the data buffer */
    uint8_t  packNum;      /*!< Packet number of the data */
    uint8_t  zeroPackFill; /*!< Fill a zero pack for IN transfer or not */
    uint16_t maxPackSize;  /*!< Max pack size of this endpoint */
    uint32_t xferCnt;      /*!< Data count of one pack on from tansfer */
} USBD_CtrlBuf_T;

/**
 * @brief   USB Class Request handler
 */
typedef struct
{
    USBD_StdReqHandler_T getConfigurationHandler;
    USBD_StdReqHandler_T getDescriptorHandler;
    USBD_StdReqHandler_T getInterfaceHandler;
    USBD_StdReqHandler_T getStatusHandler;
    USBD_StdReqHandler_T setAddressHandler;

    USBD_StdReqHandler_T setConfigurationHandler;
    USBD_StdReqHandler_T setDescriptorHandler;
    USBD_StdReqHandler_T setFeatureHandler;
    USBD_StdReqHandler_T setInterfaceHandler;
    USBD_StdReqHandler_T clearFeatureHandler;
} USBD_StdReqCallback_T;

/**
 * @brief   USB init parameter
 */
typedef struct
{
    USBD_Descriptor_T* pDeviceDesc;           /*!< Device descriptor pointer */
    USBD_Descriptor_T* pConfigurationDesc;    /*!< Configuration descriptor pointer */
    USBD_Descriptor_T* pStringDesc;           /*!< String descriptor pointer */
    USBD_Descriptor_T* pQualifierDesc;        /*!< Device Qualifier descriptor pointer */

    USBD_StdReqCallback_T* pStdReqCallback;   /*!< Setup callback fuction structure */
    USBD_ReqHandler_T stdReqExceptionHandler; /*!< Standard request exception handler */
    USBD_ReqHandler_T classReqHandler;        /*!< Class request handler */
    USBD_ReqHandler_T vendorReqHandler;       /*!< Vendor request handler */

    USBD_CtrlTxStatusHandler_T txStatusHandler; /*!< Send IN status early handler */
    USBD_CtrlRxStatusHandler_T rxStatusHandler; /*!< Receive OUT status early handler */

    USBD_EPHandler_T        outEpHandler;     /*!< OUT EP transfer done handler except EP0 */
    USBD_EPHandler_T        inEpHandler;      /*!< IN EP transfer done handler except EP0 */
    USBD_ResetHandler_T     resetHandler;     /*!< USB Reset handler */
    USBD_InterruptHandler_T intHandler;       /*!< Hadler the rest of global interrupt. */
} USBD_InitParam_T;

/**
 * @brief   USB device infomation
 */
typedef struct
{
    USBD_STATE_T      devState;
    USBD_CTRL_STATE_T ctrlState;
    uint8_t curFeature;
    uint8_t curInterface;
    uint8_t curAlternateSetting;
    uint8_t curConfiguration;
    uint8_t configurationNum;

    /* Descriptor pointer */
    USBD_Descriptor_T* pDeviceDesc;
    USBD_Descriptor_T* pConfigurationDesc;
    USBD_Descriptor_T* pStringDesc;
    USBD_Descriptor_T* pQualifierDesc;

    USBD_DevReqData_T reqData;

    /* Setup stage handler */
    USBD_StdReqCallback_T* pStdReqCallback;
    USBD_ReqHandler_T stdReqExceptionHandler;
    USBD_ReqHandler_T classReqHandler;
    USBD_ReqHandler_T vendorReqHandler;

    /*  Control transfer status stage handler */
    USBD_CtrlTxStatusHandler_T txStatusHandler;
    USBD_CtrlRxStatusHandler_T rxStatusHandler;

    /* Handle ep transfer done stage except ep 0 */
    USBD_EPHandler_T outEpHandler;
    USBD_EPHandler_T inEpHandler;

    USBD_ResetHandler_T resetHandler;
    USBD_InterruptHandler_T intHandler;

    /* Endpoint management */
    USBD_CtrlBuf_T inBuf[USB_IN_EP_MAX_NUM];
    USBD_CtrlBuf_T outBuf[USB_OUT_EP_MAX_NUM];
} USBD_Info_T;

/**@} end of group Core_Structures */

/** @defgroup Core_Variables Variables
  @{
*/

/* USB information */
extern USBD_Info_T g_usbDev;

/**@} end of group Core_Variables */

/** @defgroup Core_Functions Functions
  @{
*/

void USBD_SetupProcess(void);
void USBD_CtrlOutProcess(void);
void USBD_CtrlInProcess(void);

void USBD_CtrlOutData(uint8_t* buf, uint32_t len);
void USBD_CtrlInData(uint8_t* buf, uint32_t len);
void USBD_CtrlRxStatus(void);
void USBD_CtrlTxStatus(void);

void USBD_RxData(uint8_t ep, uint8_t* buf, uint32_t len);
void USBD_TxData(uint8_t ep, uint8_t* buf, uint32_t len);
void USBD_PushDataToTxFIFO(uint8_t ep);

void USBD_Reset(void);

/**@} end of group Core_Functions */
/**@} end of group Core */
/**@} end of group Standard */
/**@} end of group Core_Device */
/**@} end of group USB_OTG_Library */

#endif
