/*!
 * @file        usbd_class_cdc.h
 *
 * @brief       CDC class header file
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
#ifndef __USBD_CLASS_VCP
#define __USBD_CLASS_VCP

/* Includes */
#include "usb_user.h"
#include "usbd_core.h"

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

extern USBD_StdReqCallback_T stdReqCallback;

/**@} end of group CDC_Variables */

/** @defgroup CDC_Functions Functions
  @{
*/

void USBD_ClassHandler(USBD_DevReqData_T* reqData);

/**@} end of group CDC_Functions */
/**@} end of group CDC */
/**@} end of group Class */
/**@} end of group Core_Device */
/**@} end of group USB_OTG_Library */

#endif
