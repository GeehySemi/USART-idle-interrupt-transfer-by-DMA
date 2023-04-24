/*!
 * @file        usbd_class_msc.h
 *
 * @brief       MSC class header file
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

/** @addtogroup MSC
  @{
*/

/** @defgroup MSC_Macros Macros
  @{
*/

#define BOT_GET_MAX_LUN              0xFE
#define BOT_RESET                    0xFF

/**@} end of group MSC_Macros */

/** @defgroup MSC_Variables Variables
  @{
*/

extern USBD_StdReqCallback_T g_stdReqCallback;

/**@} end of group MSC_Variables */

/** @defgroup MSC_Functions Functions
  @{
*/
void USBD_MSC_ClassHandler(USBD_DevReqData_T* reqData);

/**@} end of group MSC_Functions */
/**@} end of group MSC */
/**@} end of group Class */
/**@} end of group Core_Device */
/**@} end of group USB_OTG_Library */
#endif
