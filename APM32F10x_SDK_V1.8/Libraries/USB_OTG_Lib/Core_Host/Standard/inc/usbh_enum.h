/*!
 * @file        usbh_enum.h
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
#ifndef USBH_ENUM_H
#define USBH_ENUM_H

/* Includes */
#include "drv_usb.h"
#include "usbh_stdReq.h"
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

/** @addtogroup Enum
  @{
*/

/** @defgroup Enum_Functions Functions
  @{
*/

/* Enum handler */
typedef void (*ENUM_CoreHandler_T)(void);

extern ENUM_CoreHandler_T ENUM_CoreHandler[];

/**@} end of group Enum_Functions */
/**@} end of group Enum */
/**@} end of group Standard */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */

#endif

