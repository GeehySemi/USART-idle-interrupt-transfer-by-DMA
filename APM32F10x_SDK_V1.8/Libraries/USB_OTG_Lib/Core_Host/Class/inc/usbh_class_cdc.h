/*!
 * @file        usbh_class_cdc.h
 *
 * @brief       id class handler header file
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
#ifndef __USBH_CLASS_CDC
#define __USBH_CLASS_CDC

/* Includes */
#include "usbh_core.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Host Core Host
  @{
*/

/** @addtogroup Class
  @{
*/

/** @addtogroup CDC
  @{
*/

/** @defgroup CDC_Macros Macros
  @{
*/

#define CDC_BUFFER_SIZE       1024

/**@} end of group CDC_Macros */

/** @defgroup CDC_Enumerates Enumerates
  @{
*/

typedef enum
{
    CDC_REQ_SET_LINE_CODING,
    CDC_REQ_GET_LINE_CODING,
    CDC_SET_CONTROL_LINE_STATE,
    CDC_REQ_CONFIGURED_OK,
} USBH_CDC_REQ_STATE_T;

typedef enum
{
    USBH_CDC_IDIE,
    USBH_CDC_SEND_DATA,
    USBH_CDC_SEND_WAIT,
    USBH_CDC_GET_DATA,
    USBH_CDC_GET_WAIT,
    USBH_CDC_ERROR,
    USBH_CDC_STALL,
    USBH_CDC_COMPLETE
} USBH_CDC_STATE_T;

/**@} end of group CDC_Enumerates */

/** @defgroup CDC_Structures Structures
  @{
*/

typedef struct
{
    uint8_t chNum;
    USBH_CDC_STATE_T cdcState;
    uint8_t* buff;
    uint8_t* pFillBuff;
    uint8_t* pEmptyBuff;
    uint32_t buffLen;
    uint16_t dataLen;
    uint16_t maxPackSize;
} CDC_Xfer_T;

/**@} end of group CDC_Structures */

/** @defgroup CDC_Variables Variables
  @{
*/

extern CDC_Xfer_T CDC_Tx;
extern CDC_Xfer_T CDC_Rx;

extern uint8_t cdcDataOutBuff[64];

/**@} end of group CDC_Variables */

/** @defgroup CDC_Functions Functions
  @{
*/

typedef void (*CDC_REQ_Handler_T)(void);
typedef void (*CDC_Handler_T)(void);

void USBH_CDC_ClassInitHandler(void);
void USBH_CDC_ClassDeInitHandler(void);
void USBH_CDC_ClassReqHandler(void);
void USBH_CDC_CoreHandler(void);

uint8_t USBH_CDCProcessSend(void);
uint8_t USBH_CDCProcessGet(void);

void CDC_EnableSend(void);
void CDC_EnableGet(void);
void CDC_DisableSend(void);
void CDC_DisableGet(void);
uint8_t CDC_SendData(uint8_t* data, uint16_t length);
void CDC_ReceiveData(void);
void CDC_DEMO(void);

/**@} end of group CDC_Functions */
/**@} end of group CDC */
/**@} end of group Class */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */

#endif
