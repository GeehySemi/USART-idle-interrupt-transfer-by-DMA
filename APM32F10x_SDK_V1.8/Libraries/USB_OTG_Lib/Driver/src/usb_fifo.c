/*!
 * @file        usb_fifo.c
 *
 * @brief       USB fifo handler
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
#include "usb_fifo.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Driver
  @{
*/

/** @addtogroup USB_FIFO
  @{
*/

/** @defgroup USB_FIFO_Functions Functions
  @{
*/

/*!
 * @brief       Read the RxFIFO packet to buffer
 *
 * @param       rBuf:    Buffer pointer
 *
 * @param       rLen:    Buffer length
 *
 * @retval      None
 */
void USB_FIFO_ReadRxFifoPacket(uint8_t* rBuf, uint32_t rLen)
{
    uint32_t i = 0;
    __IO uint32_t temp = 0;

    for (i = 0; i < rLen / 4; i++)
    {
        ((( struct USB_FIFO_PACKET *)(const void *)(rBuf))->v) = USB_OTG_ReadRxFifoData();
        rBuf += 4;
    }

    if (rLen & 0x3)
    {
        temp = USB_OTG_ReadRxFifoData();
        for (i = 0; i < (rLen & 0x3); i++)
        {
            rBuf[i] = temp >> (i << 3);
        }
    }
}

/*!
 * @brief       Write a buffer of data to a selected endpoint
 *
 * @param       ep:     Endpoint number
 *
 * @param       wBuf:   The pointer to the buffer of data to be written to the endpoint
 *
 * @param       wLen:   Number of data to be written (in bytes)
 *
 * @retval      None
 */
void USB_FIFO_WriteFifoPacket(uint8_t ep, uint8_t* wBuf, uint32_t wlen)
{
    uint32_t wordCnt = 0, i = 0;

    wordCnt = (wlen + 3) / 4;

    for (i = 0; i < wordCnt; i++)
    {
        USB_OTG_WriteTxFifoData(ep, (((const struct USB_FIFO_PACKET *)(const void *)(wBuf))->v));

        wBuf+=4;
    }

}
/**@} end of group USB_FIFO_Functions */
/**@} end of group USB_FIFO */
/**@} end of group Driver */
/**@} end of group USB_OTG_Library */
