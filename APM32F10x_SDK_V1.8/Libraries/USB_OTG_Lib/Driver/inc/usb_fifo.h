/*!
 * @file        usb_fifo.h
 *
 * @brief       USB fifo header file
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
#ifndef __USB_FIFO_H_
#define __USB_FIFO_H_

/* Includes */
#include "drv_usb.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Driver
  @{
*/

/** @addtogroup USB_FIFO
  @{
*/

/** @defgroup USB_FIFO_Macros Macros
  @{
*/

/*!
 * @brief       Write the data to TxFIFO.
 *
 * @param       fifoNum: TxFIFO number
 *
 * @param       data: data to be writed in TxFIFO.
 *
 * @retval      None
 */
#define USB_OTG_WriteTxFifoData(fifoNum, data)   (USB_OTG_FIFO->FIFO[fifoNum].DATA = data)

/*!
 * @brief     Read the RxFIFO data
 *
 * @param     None
 *
 * @retval    Return RxFIFO data
 */
#define USB_OTG_ReadRxFifoData()                 (USB_OTG_FIFO->FIFO[0].DATA)

/**@} end of group USB_FIFO_Macros */

struct __attribute__((packed, aligned(1))) USB_FIFO_PACKET { uint32_t v; };

/** @defgroup USB_FIFO_Functions Functions
  @{
*/

void USB_FIFO_ReadRxFifoPacket(uint8_t* buf, uint32_t len);
void USB_FIFO_WriteFifoPacket(uint8_t fifoNum, uint8_t* buf, uint32_t len);

/**@} end of group USB_FIFO_Functions */
/**@} end of group USB_FIFO */
/**@} end of group Driver */
/**@} end of group USB_OTG_Library */
#endif
