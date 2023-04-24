/*!
 * @file        usbd_class_cdc.c
 *
 * @brief       CDC class source file
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

/* Includes */
#include "usbd_class_cdc.h"
#include "usbd_init.h"
#include "usb_user.h"
#include "usbd_descriptor.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Device Core Device
  @{
*/

/** @addtogroup Class
  @{
*/

/** @addtogroup CDC
  @{
*/

/** @defgroup CDC_Variables Variables
  @{
*/

static uint8_t cmdBuf[8] = {0};

/**@} end of group CDC_Variables */

/** @defgroup CDC_Functions Functions
  @{
*/

/*!
 * @brief       Class Request handler
 *
 * @param       reqData : point to setup request
 *
 * @retval      None
 */
void USBD_ClassHandler(USBD_DevReqData_T* reqData)
{
    uint16_t length = ((uint16_t)reqData->domain.wLength[1] << 8) | \
                      reqData->domain.wLength[0] ;

    if (length)
    {
        if (reqData->domain.bmRequestType.bit.dir != 0)
        {
            USBD_CtrlInData(cmdBuf, length);
        }
        /* Host to Device */
        else
        {
            USBD_CtrlOutData(cmdBuf, length);
        }
    }
    else
    {
        USBD_CtrlTxStatus();
    }
}

/**@} end of group CDC_Functions */
/**@} end of group CDC */
/**@} end of group Class */
/**@} end of group Core_Device */
/**@} end of group USB_OTG_Library */
