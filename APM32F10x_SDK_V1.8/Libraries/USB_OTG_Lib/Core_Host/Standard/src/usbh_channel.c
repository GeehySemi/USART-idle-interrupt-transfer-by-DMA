/*!
 * @file        usbh_channel.c
 *
 * @brief       USB host channel handler function
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
#include "usbh_channel.h"
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

/** @addtogroup Channel
  @{
*/

/** @defgroup Channel_Functions Functions
  @{
*/

/*!
 * @brief     Check the minimum Free Channel Number.
 *
 * @param     None
 *
 * @retval    channel number.
 */
static int8_t USBH_CH_ReadFreeChannelNum(void)
{
    /* Read Free channel */
    for (uint8_t i = 0; i < USBH_CHANNEL_MAX_NUM; i++)
    {
        if (g_usbHost.xferPipe[i].channelUsedFlag == USBH_CHANNEL_FREE)
            return i;
    }

    return -1;
}

/*!
 * @brief     Alloc a channel for a endpoint by address.
 *
 * @param     epAddr : endpoint address to be alloc channel.
 *
 * @retval    channel number.
 */
int8_t USBH_CH_AllocChannel(uint8_t epAddr)
{
    int8_t chNum = USBH_CH_ReadFreeChannelNum();

    if (chNum >= 0)
    {
        g_usbHost.xferPipe[chNum].channelUsedFlag = USBH_CHANNEL_USED;
        g_usbHost.xferPipe[chNum].epAddr = epAddr;
    }

    return chNum;
}

/*!
 * @brief     Free the channel.
 *
 * @param     chNum : channel to be free.
 *
 * @retval    None.
 */
void USBH_CH_FreeChannel(uint8_t chNum)
{
    g_usbHost.xferPipe[chNum].channelUsedFlag = USBH_CHANNEL_FREE;
    g_usbHost.xferPipe[chNum].epAddr = 0;

    g_usbHost.xferPipe[chNum].buf = NULL;
    g_usbHost.xferPipe[chNum].bufLen = 0;
    g_usbHost.xferPipe[chNum].bufCount = 0;

    g_usbHost.xferPipe[chNum].enbalePing = 0;
    g_usbHost.xferPipe[chNum].urbStatus = USBH_URB_IDLE;
    g_usbHost.xferPipe[chNum].pipeState = PIPE_OK;
    g_usbHost.xferPipe[chNum].pidToggle = 0;

    g_usbHost.xferPipe[chNum].dataPid = 0;
    g_usbHost.xferPipe[chNum].dir = 0;
    g_usbHost.xferPipe[chNum].maxPackSize = 0;
}

/*!
 * @brief     Free All of the channel.
 *
 * @param     chNum : channel to be free.
 *
 * @retval    None.
 */
void USBH_CH_FreeAllChannel(void)
{
    uint8_t i = 0;

    for (i = 0; i < HOST_CHANNEL_NUM; i++)
    {
        USBH_CH_FreeChannel(i);
    }
}

/*!
 * @brief     Config the channel to transfer.
 *
 * @param     chNum : channel to be configed.
 *
 * @param     devAddr : device address.
 *
 * @param     epType : endpoint type of the channel.
 *
 * @param     maxPackSize : endpoint max pack size.
 *
 * @retval    USBH_CH_OK or USBH_CH_FAIL.
 */
USBH_CH_STATUS_T USBH_CH_OpenChannel(uint8_t  chNum,
                                     uint8_t  devAddr,
                                     uint8_t  epType,
                                     uint16_t maxPackSize)
{
    if (chNum >= USBH_CHANNEL_MAX_NUM)
        return USBH_CH_FAIL;

    USB_ChannelConfig_T chConfig;

    /* Config transfer Channel info */
    g_usbHost.xferPipe[chNum].maxPackSize = maxPackSize;
    g_usbHost.xferPipe[chNum].enbalePing = g_usbHost.speed ? 0 : 1;

    /* Config Channel */
    chConfig.chNum = chNum;
    chConfig.devAddr = devAddr;
    chConfig.epType = epType;
    chConfig.devSpeed = g_usbHost.speed;
    chConfig.direction = (g_usbHost.xferPipe[chNum].epAddr >> 7) & 0x01;
    chConfig.epNum = g_usbHost.xferPipe[chNum].epAddr & 0x7F;
    chConfig.maxPackSize = g_usbHost.xferPipe[chNum].maxPackSize;
    chConfig.enbalePing =  g_usbHost.xferPipe[chNum].enbalePing;//!< High speed enable Ping

    USB_OTG_InitChannel(&chConfig);

    return USBH_CH_OK;
}

/*!
 * @brief     Read the channel number of endpoint.
 *
 * @param     epAddr : endpoint address which you want to fine the channel.
 *
 * @retval    Return the channel number, if no channel, then return 0xFF.
 */
uint8_t USBH_ReadEpAddrChannel(uint8_t epAddr)
{
    uint8_t chNum = 0;

    while (1)
    {
        if (g_usbHost.xferPipe[chNum].channelUsedFlag)
        {
            if (g_usbHost.xferPipe[chNum].epAddr == epAddr)
            {
                return chNum;
            }
        }

        if (++chNum > USBH_CHANNEL_MAX_NUM)
            return 0xFF;
    }
}

/**@} end of group Channel_Functions */
/**@} end of group Channel */
/**@} end of group Standard */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */
