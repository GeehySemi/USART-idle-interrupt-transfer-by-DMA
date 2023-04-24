/*!
 * @file        usbd_interrupt.h
 *
 * @brief       USB interrupt handle
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
#ifndef __USBD_INTERRUPT_H_
#define __USBD_INTERRUPT_H_

/* Includes */
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

/** @addtogroup Interrupt
  @{
*/

/** @defgroup Interrupt_Macros Macros
  @{
*/

/* Endpoint register mask value default */
#define USB_EP_MASK_DEFAULT     (USB_EP_BIT_CTFR | USB_EP_BIT_SETUP | USB_EP_BIT_TYPE | USB_EP_BIT_KIND | USB_EP_BIT_CTFT |USB_EP_BIT_ADDR)

/**@} end of group Interrupt_Macros */

/** @defgroup Interrupt_Functions Functions
  @{
*/

void USBD_OTG_IsrHandler(void);

/**@} end of group Interrupt_Functions */
/**@} end of group Interrupt */
/**@} end of group Standard */
/**@} end of group Core_Device */
/**@} end of group USB_OTG_Library */

#endif
