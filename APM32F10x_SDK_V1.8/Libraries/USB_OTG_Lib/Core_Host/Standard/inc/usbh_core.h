/*!
 * @file        usbh_core.h
 *
 * @brief       USB host core function
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
#ifndef __USBH_CORE_H_
#define __USBH_CORE_H_

/* Includes */
#include "usb_fifo.h"
#include "drv_usb_host.h"
#include "usbh_stdReq.h"
#include "usb_config.h"
#include "usbh_channel.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Host Core Host
  @{
*/

/** @addtogroup Standard
  @{
*/

/** @addtogroup Core
  @{
*/

/** @defgroup Core_Macros Macros
  @{
*/

/*!
 * @brief     Config Host state to change polling handler.
 *
 * @param     USBH_HOST_STATE_T : host state
 *
 * @retval    None
 */
#define USBH_ConfigHostState(USBH_HOST_STATE_T) \
do{ \
    g_usbHost.preHostState = g_usbHost.curHostState; \
    g_usbHost.curHostState = USBH_HOST_STATE_T; \
} while (0)

/*!
 * @brief     Read Current Host state.
 *
 * @param     None
 *
 * @retval    Return Current Host state.
 */
#define USBH_ReadCurHostState()                  (g_usbHost.curHostState)

/*!
 * @brief     Config Enum state.
 *
 * @param     USBH_ENUM_STATE_T : Enum state
 *
 * @retval    None
 */
#define USBH_ConfigEnumState(USBH_ENUM_STATE_T)  (g_usbHost.control.enumState = USBH_ENUM_STATE_T)

/*!
 * @brief     Config device address.
 *
 * @param     addr : device address
 *
 * @retval    None
 */
#define USBH_ConfigDeviceAddr(addr)             (g_usbHost.devAddress = addr)

/**@} end of group Core_Macros */

/** @defgroup Core_Enumerates Enumerates
  @{
*/

/**
 * @brief   USB Host state machine
 */
typedef enum
{
    USBH_HOST_IDLE,
    USBH_HOST_DEVICE_ATTACHED,
    USBH_HOST_DEVICE_DETACHED,
    USBH_HOST_ENUM,
    USBH_HOST_USER_INPUT,
    USBH_HOST_CLASS_REQ,
    USBH_HOST_CLASS,
    USBH_HOST_SUSPEND,
    USBH_HOST_WAKE_UP,
    USBH_HOST_ERROR
} USBH_HOST_STATE_T;

/**
 * @brief   USB Host enum state
 */
typedef enum
{
    USBH_ENUM_IDLE,                      /*!< enum idle */
    USBH_ENUM_GET_DEV_DESC,              /*!< Get device descriptor */
    USBH_ENUM_GET_FULL_DEV_DESC,         /*!< Get Full device descriptor */
    USBH_ENUM_SET_ADDR,                  /*!< Set address */
    USBH_ENUM_GET_CFG_DESC,              /*!< Get Configuration descriptor */
    USBH_ENUM_GET_FULL_CFG_DESC,         /*!< Get Full Configuration descriptor */
    USBH_ENUM_GET_MFC_STRING_DESC,       /*!< Get Manufacturer string */
    USBH_ENUM_GET_PRODUCT_STRING_DESC,   /*!< Get Product string */
    USBH_ENUM_GET_SERIALNUM_STRING_DESC, /*!< Get serial number string */
    USBH_ENUM_SET_CONFIGURATION,         /*!< Set Configuration */
    USBH_ENUM_CONFIGURED_OK              /*!< Configuration finish */
} USBH_ENUM_STATE_T;

/**
 * @brief   USB Host control transfer state
 */
typedef enum
{
    USBH_CTRL_SETUP,
    USBH_CTRL_DATA_IN,
    USBH_CTRL_DATA_OUT,
    USBH_CTRL_STATUS_IN,
    USBH_CTRL_STATUS_OUT,
    USBH_CTRL_ERROR,
    USBH_CTRL_STALL,
    USBH_CTRL_COMPLETE
} USBH_CTRL_STATE_T;

/**
 * @brief   USB transfer state
 */
typedef enum
{
    USBH_XFER_START,
    USBH_XFER_WAITING,
    USBH_XFER_OK
} USBH_XFER_STATE_T;

typedef enum
{
    USBH_URB_IDLE,
    USBH_URB_NOREADY,
    USBH_URB_STALL,
    USBH_URB_ERROR,
    USBH_URB_PING,
    USBH_URB_OK,
} USBH_URB_STATUS_T;


typedef enum
{
    PIPE_OK,
    PIPE_HALTED,
    PIPE_NAK,
    PIPE_NYET,
    PIPE_STALL,
    PIPE_XACTERR,
    PIPE_BBLERR,
    PIPE_DATATGLERR,
} USBH_PIPE_STATE_T;

typedef enum
{
    USBH_ERROR_NO,          /*!< No Error */
    USBH_ERROR_NOT_SUPPORT, /*!< Not Supported */
    USBH_ERROR_UNRECOVERED, /*!< Unrecovered Error */
    USBH_APP_RESET
} USBH_ERROR_STATE_T;

typedef enum
{
    USBH_DEVICE_SPEED_HS,
    USBH_DEVICE_SPEED_FS,
    USBH_DEVICE_SPEED_LS
} USBH_DEVICE_SPEED_T;

/**
 * @brief   USB channel state
 */
typedef enum
{
    USBH_CHANNEL_FREE,
    USBH_CHANNEL_USED
} USBH_CHANNEL_STATE_T;

/**@} end of group Core_Enumerates */

/** @defgroup Core_Functions Functions
  @{
*/

/* Host state handler function */
typedef void (*USBH_CoreHandler_T)(void);

/* Class callback function type define */
typedef void (*USBH_ClassCallBack_T)(void);

/* Suspend callback function type define */
typedef void (*USBH_SuspendCallBack_T)(void);

/* Wake up callback function type define */
typedef void (*USBH_WakeUpCallBack_T)(void);

/* Interrupt callback function type define */
typedef void (*USBH_IntCallBack_T)(void);

/**@} end of group Core_Functions */

/** @defgroup Core_Structures Structures
  @{
*/

/**
 * @brief   USB Host transfer buffer management
 */
typedef struct
{
    uint8_t*              buf;
    uint32_t             bufLen;
    uint32_t             bufCount;

    uint8_t              enbalePing;
    USBH_URB_STATUS_T    urbStatus;
    USBH_PIPE_STATE_T    pipeState;
    uint8_t              pidToggle;

    USBH_CHANNEL_STATE_T channelUsedFlag;
    uint8_t              epAddr;
    uint8_t              dataPid;
    uint8_t              dir;
    uint16_t             maxPackSize;
} USBH_XferPipe_T;

/**
 * @brief   USB Host Descriptor structure
 */
typedef struct
{
    USBH_DeviceDesc_T        device;
    USBH_ConfigurationDesc_T configuration;
    USBH_Interface_T         interface[INTERFACE_DESC_MAX_NUM];
    uint8_t                  cfgDescBuf[CFG_DESC_MAX_LEN];
    uint8_t                  stringBuf[STRING_DESC_MAX_LEN];
} USBH_Descriptor_T;

/**
 * @brief   USB Control thansfer info
 */
typedef struct
{
    USBH_CTRL_STATE_T   ctrlState;  /*!< USB control thansfer state */
    USBH_ENUM_STATE_T   enumState;  /*!< USB enum state */
    USBH_XFER_STATE_T   xferState;  /*!< USB transfer state */

    uint8_t errCnt;
    uint8_t inChannelNum;
    uint8_t outChannelNum;
    uint16_t maxPackSize;

    USBH_HostReqData_T reqData;    /*!< Setup Request data */
    USBH_Descriptor_T descriptor;  /*!< Descriptor structure */
} USBH_Control_T;

/**
 * @brief   USB Host Init Param
 */
typedef struct
{

    USBH_ClassCallBack_T   classInitHandler;
    USBH_ClassCallBack_T   classDeInitHandler;
    USBH_ClassCallBack_T   classReqHandler;
    USBH_ClassCallBack_T   classCoreHandler;

    USBH_SuspendCallBack_T suspendHandler;
    USBH_WakeUpCallBack_T  wakeUpHandler;

    USBH_IntCallBack_T     intHandler;
} USBH_InitParam_T;

/**
 * @brief   USB Host infomation
 */
typedef struct
{
    USBH_HOST_STATE_T curHostState;  /*!< USB Host State Machine */
    USBH_HOST_STATE_T preHostState;  /*!< USB Host State Machine */

    USBH_ERROR_STATE_T errorState;

    uint8_t         speed;         /*!< device speed */
    uint8_t         devAddress;    /*!< device address */

    uint8_t         connectedFlag; /*!< device connected flag */
    uint8_t         portEnable;    /*!< device speed */

    USBH_Control_T  control;       /*!< Control Thansfer info management */
    USBH_XferPipe_T xferPipe[HOST_CHANNEL_NUM]; /*!< Thansfer data management */

    USBH_ClassCallBack_T classInitHandler;
    USBH_ClassCallBack_T classDeInitHandler;
    USBH_ClassCallBack_T classReqHandler;
    USBH_ClassCallBack_T classCoreHandler;

    USBH_SuspendCallBack_T suspendHandler;
    USBH_WakeUpCallBack_T  wakeUpHandler;

    USBH_IntCallBack_T     intHandler;
} USBH_Info_T;

/**@} end of group Core_Structures */

/** @defgroup Core_Variables Variables
  @{
*/

/* USB Host infomation */
extern USBH_Info_T g_usbHost;

/**@} end of group Core_Variables */

/** @defgroup Core_Functions Functions
  @{
*/

void USBH_PollingProcess(void);

/**@} end of group Core_Functions */
/**@} end of group Core */
/**@} end of group Standard */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */

#endif
