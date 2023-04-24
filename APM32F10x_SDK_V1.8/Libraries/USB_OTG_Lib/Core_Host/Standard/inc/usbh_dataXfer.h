/*!
 * @file        usbh_dataXfer.h
 *
 * @brief       USB host enum hander function head file
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
#ifndef USBH_DATA_XFER_H
#define USBH_DATA_XFER_H

/* Includes */
#include "drv_usb.h"
#include "usbh_core.h"


/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Host Core Host
  @{
*/

/** @addtogroup Standard
  @{
*/

/** @addtogroup DataXfer
  @{
*/

/** @defgroup DataXfer_Macros Macros
  @{
*/

#define CTRL_XFER_ERROR_CNT                      2

#define USBH_ConfigDataPid(chNum, dataPid)      (g_usbHost.xferPipe[chNum].pidToggle = dataPid)
#define USBH_ToggleDataPid(chNum)               (g_usbHost.xferPipe[chNum].pidToggle ^= 1)
#define USBH_ReadDataPid(chNum)                 (g_usbHost.xferPipe[chNum].pidToggle ? DATA_1 : DATA_0)

/**@} end of group DataXfer_Macros */

/** @defgroup DataXfer_Enumerates Enumerates
  @{
*/

typedef enum
{
    USBH_CTRL_XFER_SETUP,
    USBH_CTRL_XFER_DATA_OUT,
    USBH_CTRL_XFER_DATA_IN,
    USBH_CTRL_XFER_STATUS_OUT,
    USBH_CTRL_XFER_STATUS_IN,
    USBH_CTRL_XFER_WAIT_URB
} USBH_CTRL_XFER_STATE_T;

/**@} end of group DataXfer_Enumerates */

/** @defgroup DataXfer_Functions Functions
  @{
*/

/* Control transfer state handler */
typedef USBH_CTRL_STATE_T(*USBH_CtrlStateHandler_T)(USBH_HostReqData_T*, uint8_t*);


USBH_CTRL_STATE_T USBH_ControlXferHandler(USBH_HostReqData_T* reqData, uint8_t* buf);

void USBH_CtrlSetupReq(uint8_t chNum, USBH_HostReqData_T* reqData);
void USBH_CtrlOutData(uint8_t chNum, uint8_t* wBuf, uint32_t wLen);
void USBH_CtrlInData(uint8_t chNum, uint8_t* rBuf, uint32_t rLen);

void USBH_InterruptOutData(uint8_t chNum, uint8_t* wBuf, uint32_t wLen);
void USBH_InterruptInData(uint8_t chNum, uint8_t* rBuf, uint32_t rLen);

void USBH_BulkOutData(uint8_t chNum, uint8_t* wBuf, uint32_t wLen);
void USBH_BulkInData(uint8_t chNum, uint8_t* rBuf, uint32_t rLen);

void USBH_IsocOutData(uint8_t chNum, uint8_t* wBuf, uint32_t wLen);
void USBH_IsocInData(uint8_t chNum, uint8_t* rBuf, uint32_t rLen);

/**@} end of group DataXfer_Functions */
/**@} end of group DataXfer */
/**@} end of group Standard */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */

#endif
