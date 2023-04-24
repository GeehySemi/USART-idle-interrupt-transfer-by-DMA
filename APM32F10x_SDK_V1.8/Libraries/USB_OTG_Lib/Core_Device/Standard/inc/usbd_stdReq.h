/*!
 * @file        usbd_stdReq.h
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
#ifndef __USBD_STDREQ_H_
#define __USBD_STDREQ_H_
#include "usbd_core.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Device Core Device
  @{
*/

/** @addtogroup Standrad
  @{
*/

/** @addtogroup StdReq
  @{
*/

/** @defgroup StdReq_Functions Functions
  @{
*/
void USBD_StandardReqeust(void);

/**@} end of group StdReq_Functions */
/**@} end of group Interrupt */
/**@} end of group Standard */
/**@} end of group Core_Device */
/**@} end of group USB_OTG_Library */
#endif
