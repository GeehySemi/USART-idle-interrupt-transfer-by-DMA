/*!
 * @file        usbh_class_hid.h
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
#ifndef __USBH_CLASS_HID
#define __USBH_CLASS_HID

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

/** @addtogroup HID
  @{
*/

/** @defgroup HID_Macros Macros
  @{
*/

#define HID_POLL_TIME          10

/**@} end of group HID_Macros */

/** @defgroup HID_Enumerates Enumerates
  @{
*/

typedef enum
{
    HID_REQ_IDLE,
    HID_REQ_GET_DEV_DESC,
    HID_REQ_GET_REQ_DEV_DESC,
    HID_REQ_SET_IDLE,
    HID_REQ_SET_PROTOCOL,
    HID_REQ_CONFIGURED_OK,
} USBH_HID_REQ_STATE_T;

typedef enum
{
    HID_IDLE,
    HID_SYNC,
    HID_GET_DATA,
    HID_POLL,
} USBH_HID_STATE_T;

/**@} end of group HID_Enumerates */

/** @defgroup HID_Enumerates Enumerates
  @{
*/

typedef struct
{
    uint8_t              x;
    uint8_t              y;
    uint8_t              button;
} HID_MOUSE_Data_T;

/**@} end of group HID_Enumerates */

/** @defgroup HID_Functions Functions
  @{
*/
typedef void (*HID_REQ_Handler_T)(void);
typedef void (*HID_Handler_T)(void);


void USBH_HID_InitHandler(void);
void USBH_HID_DeInitHandler(void);
void USBH_HID_ReqHandler(void);
void USBH_HID_CoreHandler(void);

/**@} end of group HID_Functions */
/**@} end of group HID */
/**@} end of group Class */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */

#endif
