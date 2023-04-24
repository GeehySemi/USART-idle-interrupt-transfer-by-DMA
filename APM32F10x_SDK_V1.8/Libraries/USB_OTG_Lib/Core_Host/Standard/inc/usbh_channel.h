/*!
 * @file        usbh_channel.h
 *
 * @brief       USB host channel handler function head file
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
#ifndef __USBH_CHANNEL_H
#define __USBH_CHANNEL_H

/* Includes */
#include "drv_usb.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Host Core Host
  @{
*/

/** @addtogroup Standard
  @{
*/

/** @addtogroup Channel
  @{
*/

/** @defgroup Channel_Macros Macros
  @{
*/

#define USBH_CHANNEL_MAX_NUM    8

/**@} end of group Channel_Macros */

/** @defgroup Channel_Enumerates Enumerates
  @{
*/

typedef enum
{
    USBH_CH_FAIL,
    USBH_CH_OK
} USBH_CH_STATUS_T;

/**@} end of group Channel_Enumerates */

/** @defgroup Channel_Functions Functions
  @{
*/

int8_t USBH_CH_AllocChannel(uint8_t epAddr);

void USBH_CH_FreeChannel(uint8_t channelNum);
void USBH_CH_FreeAllChannel(void);

USBH_CH_STATUS_T USBH_CH_OpenChannel(uint8_t  channelNum,
                                     uint8_t  devAddr,
                                     uint8_t  epType,
                                     uint16_t maxPackSize);

uint8_t USBH_ReadEpAddrChannel(uint8_t epAddr);

/**@} end of group Channel_Functions */
/**@} end of group Channel */
/**@} end of group Standard */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */

#endif
