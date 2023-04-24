/*!
 * @file        usbh_class_msc.h
 *
 * @brief       usb host msc class handler header file
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
#ifndef __USBH_CLASS_MSC
#define __USBH_CLASS_MSC

/* Includes */
#include "usbh_core.h"
#include "usbh_msc.h"
#include "usbh_msc_bot.h"
#include "usbh_msc_scsi.h"
#include "usbh_stdReq.h"
#include "usbh_dataXfer.h"
#include "usbh_channel.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Host Core Host
  @{
*/

/** @addtogroup Class
  @{
*/

/** @addtogroup MSC
  @{
*/

/** @defgroup MSC_Macros Macros
  @{
*/

#define USBH_MSC_ERROR_RETRY_CNT            10
#define USBH_MSC_SATLL_RETRY_CNT            3

#define USBH_ConfigClassReqState(state) \
do { \
    USBH_ConfigHostState(USBH_HOST_CLASS_REQ); \
    g_MSCInfo.curClassReqState = state; \
} while (0)


#define USBH_ConfigClassCoreState(state) \
do { \
    USBH_ConfigHostState(USBH_HOST_CLASS); \
    g_MSCInfo.preMSCState = g_MSCInfo.curMSCState; \
    g_MSCInfo.curMSCState = state; \
} while (0)

/**@} end of group MSC_Macros */

/** @defgroup MSC_Enumerates Enumerates
  @{
*/

/**
 * @brief    MSC state table
 */
typedef enum
{
    USBH_MSC_RESET_BOT,
    USBH_MSC_TEST_UNIT_READY,
    USBH_MSC_READ_CAPACITY10,
    USBH_MSC_MODE_SENSE6,
    USBH_MSC_REQUEST_SENSE,
    USBH_MSC_BOT_TRANSFER,
    USBH_MSC_APP_STATE,
    USBH_MSC_UNRECOVERED_STATE
} USBH_MSC_STATE_T;

/**
 * @brief    MSC class request state table
 */
typedef enum
{
    USBH_MSC_REQ_GET_MAX_LUN,
    USBH_MSC_REQ_BOT_RESET,
    USBH_MSC_REQ_CTRL_ERROR
} USBH_MSC_REQ_STATE_T;


typedef enum
{
    USBH_MSC_CLASS_SCSI_NOT_REPORTED = 0x00,
    USBH_MSC_CLASS_RBC               = 0x01,
    USBH_MSC_CLASS_MMC5              = 0x02,
    USBH_MSC_CLASS_UFI               = 0x04,
    USBH_MSC_CLASS_SCSI_TRANSPARENT  = 0x06,
    USBH_MSC_CLASS_LSD_FS            = 0x07,
    USBH_MSC_CLASS_IEEE1667          = 0x08
} USBH_MSC_CLASS_CODE_T;

typedef enum
{
    USBH_MSC_PROTOCOL_CBI_00 = 0x00,
    USBH_MSC_PROTOCOL_CBI_01 = 0x01,
    USBH_MSC_PROTOCOL_BBB    = 0x50,
    USBH_MSC_PROTOCOL_UAS    = 0x62
} USBH_MSC_PROTOCOL_CODE_T;

/**@} end of group MSC_Enumerates */

/** @defgroup MSC_Structures Structures
  @{
*/

/**
 * @brief    MSC Storage info
 */
typedef struct
{
    uint32_t capacity;
    uint32_t senseKey;
    uint32_t sectorSize;
    uint32_t blockSize;
    uint8_t  writeProtect;
    uint8_t  maxLun;
} USBH_MSC_StorageInfo_T;

/**
 * @brief    MSC information management
 */
typedef struct
{
    USBH_MSC_STATE_T curMSCState;
    USBH_MSC_STATE_T preMSCState;
    uint8_t errorCnt;
    uint8_t stallCnt;

    USBH_MSC_REQ_STATE_T preClassReqState;
    USBH_MSC_REQ_STATE_T curClassReqState;

    MSC_SCSI_STATE_T SCSIStates;

    USBH_MSC_StorageInfo_T storageInfo;
} USBH_MSC_Info_T;

/**@} end of group MSC_Structures */

/** @defgroup MSC_Variables Variables
  @{
*/

/* extern variables*/
extern USBH_MSC_Info_T g_MSCInfo;

/**@} end of group MSC_Variables */

/** @defgroup MSC_Functions Functions
  @{
*/
/* define callback function type */
typedef void (*MSC_Handler_T)(void);

void USBH_MSC_ClassInitHandler(void);
void USBH_MSC_ClassDeInitHandler(void);
void USBH_MSC_ClassReqHandler(void);
void USBH_MSC_CoreHandler(void);

/**@} end of group MSC_Functions */
/**@} end of group MSC */
/**@} end of group Class */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */
#endif
