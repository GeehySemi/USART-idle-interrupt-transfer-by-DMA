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
#ifndef __USBH_INTERRUPT_H_
#define __USBH_INTERRUPT_H_

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Host Core Host
  @{
*/

/** @addtogroup Standard
  @{
*/

/** @addtogroup Interrupt
  @{
*/

typedef enum
{
    PKTSTS_IN                = 2,  /*!< recevied IN packet */
    PKTSTS_IN_XFER_COMPLETE  = 3,  /*!< Transfer IN packet Complete */
    PKTSTS_DATA_ISO_ERR      = 5,  /*!< Packer data Isochronous Error */
    PKTSTS_CHANNEL_HALT      = 7,  /*!< channel halt */
} GRXSTS_PKTSTS_T;

/**@} end of group Interrupt_Enumerates */

/** @defgroup Interrupt_Functions Functions
  @{
*/

void USBH_OTG_IsrHandler(void);

/**@} end of group Interrupt_Functions */
/**@} end of group Interrupt */
/**@} end of group Standard */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */

#endif
