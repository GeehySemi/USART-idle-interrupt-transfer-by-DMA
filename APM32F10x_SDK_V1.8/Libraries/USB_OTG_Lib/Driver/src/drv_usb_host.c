/*!
 * @file       drv_usb_host.c
 *
 * @brief      This file host driver of usb register
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
#include "drv_usb_host.h"
#include "drv_usb.h"
#include "usb_fifo.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Driver
  @{
*/

/** @addtogroup USB_Host
  @{
*/

/** @defgroup USB_Host_Functions Functions
  @{
*/

/*!
 * @brief       Read Port Speed
 *
 * @param       None
 *
 * @retval      Return the Port Speed(SPEED_HS or SPEED_FS or SPEED_LS)
 */
uint8_t USB_OTG_ReadPortSpeed(void)
{
    return USB_OTG_H_ReadPortSpeedType();
}

/*!
 * @brief       Reset Host Port
 *
 * @param       None
 *
 * @retval      None
 */
void USB_OTG_PortReset(void)
{
    USB_OTG_H_DisablePort();

    USB_OTG_H_ClearPenBitChangeFlag();

    USB_OTG_H_ClearPovcBitChangeFlag();

    USB_OTG_H_EnablePortReset();

    /* More than 60ms */
    USB_Delay_us(100 * 1000);

    USB_OTG_H_DisablePortReset();

    USB_Delay_us(70 * 1000);
}

/*!
 * @brief       Enable OUT channel to start transfer
 *
 * @param       chNum : Select channel.
 *
 * @param       transferBuff : point to transfer buff.
 *
 * @param       transferLen : transfer data length.
 *
 * @param       maxPackSize : The Max sizeof Packet to transfer.
 *
 * @param       dataPid : Channel data PID.
 *                       This value can be one of the following values:
 *                       @arg DATA_0   : DATA0.
 *                       @arg DATA_1   : DATA1.
 *                       @arg DATA_2   : DATA2.
 *                       @arg MDATA    : Controled transmission.
 *                       @arg SETUP    : UnControled transmission.

 *
 * @param       epType : Endpoint type.
 *                     This value can be one of the following values:
 *                     @arg EP_TYPE_CONTROL     : Control type.
 *                     @arg EP_TYPE_ISO         : Isochronous type.
 *                     @arg EP_TYPE_BULK        : Batch type.
 *                     @arg EP_TYPE_INTERRUPT   : Interrupt type.
 *
 * @retval      None
 */
void USB_OTG_EnableOutChannelTransfer(uint16_t chNum,
                                      uint8_t* transferBuff,
                                      uint32_t transferLen,
                                      uint16_t maxPackSize,
                                      uint8_t dataPid,
                                      uint8_t epType)
{
    uint16_t transferWordLen = 0;
    uint16_t packetNum;
    /* the number of packets to transfer */
    if (transferLen > 0)
    {
        packetNum = (transferLen + maxPackSize - 1) / maxPackSize;

        if (packetNum > 256)
        {
            packetNum = 256;
            transferLen = packetNum * maxPackSize;
        }
    }
    else
    {
        packetNum = 1;
    }

    /* Config the HCHTSIZE register */
    USB_OTG_H_ConfigTransferSize(chNum, transferLen);
    USB_OTG_H_ConfigPacketCount(chNum, packetNum);
    USB_OTG_H_ConfigDataPID(chNum, dataPid);
    USB_OTG_H_DisableDoPing(chNum);

    if ((USB_OTG_H_ReadFrameNum() % 2) == 1)
    {
        USB_OTG_H_TxUseEvenFrame(chNum);
    }
    else
    {
        USB_OTG_H_TxUseOddFrame(chNum);
    }
    /* enable channel */
    USB_OTG_H_EnableChannel(chNum);

    if (transferLen > 0)
    {
        if ((epType == USB_EP_TYPE_CONTROL) || \
                (epType == USB_EP_TYPE_BULK))
        {
            transferWordLen = (transferLen + 3) / 4;

            /* check FIFO space */
            if (transferWordLen > USB_OTG_G->GNPTXFQSTS_B.NPTXFSA)
            {
                /* enable Non-periodic TxFIFO empty interrupt */
                USB_OTG_EnableGlobalInterrupt(USB_INT_G_NPTXFEM);
            }
            else
            {
                /* Write packet into FIFO */
                USB_FIFO_WriteFifoPacket(chNum, transferBuff, transferLen);
            }
        }
        else if ((epType == USB_EP_TYPE_ISO) || \
                 (epType == USB_EP_TYPE_INTERRUPT))
        {
            transferWordLen = (transferLen + 3) / 4;
            /* check FIFO space */
            if (transferWordLen > USB_OTG_H->HPTXSTS_B.FSPACE)
            {
                /* enable Periodic TxFIFO empty interrupt */
                USB_OTG_EnableGlobalInterrupt(USB_INT_G_PTXFE);
            }
            else
            {
                /* Write packet into FIFO */
                USB_FIFO_WriteFifoPacket(chNum, transferBuff, transferLen);
            }
        }


    }
}

/*!
 * @brief       Enable IN channel to start transfer
 *
 * @param       chNum : Select channel.
 *
 * @param       transferLen : transfer data length.
 *
 * @param       maxPackSize : The Max sizeof Packet to transfer.
 *
 * @param       dataPid : Channel data PID.
 *                       This value can be one of the following values:
 *                       @arg DATA_0   : DATA0.
 *                       @arg DATA_1   : DATA1.
 *                       @arg DATA_2   : DATA2.
 *                       @arg MDATA    : Controled transmission.
 *                       @arg SETUP    : UnControled transmission.
 *
 * @param       epType : Endpoint type.
 *                     This value can be one of the following values:
 *                     @arg EP_TYPE_CONTROL     : Control type.
 *                     @arg EP_TYPE_ISO         : Isochronous type.
 *                     @arg EP_TYPE_BULK        : Batch type.
 *                     @arg EP_TYPE_INTERRUPT   : Interrupt type.
 *
 * @retval      None
 */
void USB_OTG_EnableInChannelTransfer(uint16_t chNum,
                                     uint32_t transferLen,
                                     uint16_t maxPackSize,
                                     uint8_t dataPid,
                                     uint8_t epType)
{
    uint16_t packetNum;

    /* the number of packets to transfer */
    if (transferLen > 0)
    {
        packetNum = (transferLen + maxPackSize - 1) / maxPackSize;

        if (packetNum > 256)
        {
            packetNum = 256;
            transferLen = packetNum * maxPackSize;
        }
    }
    else
    {
        packetNum = 1;
    }

    transferLen = packetNum * maxPackSize;

    /* Config the HCHTSIZE register */
    USB_OTG_H_ConfigTransferSize(chNum, transferLen);
    USB_OTG_H_ConfigPacketCount(chNum, packetNum);
    USB_OTG_H_ConfigDataPID(chNum, dataPid);
    USB_OTG_H_DisableDoPing(chNum);

    if ((USB_OTG_H_ReadFrameNum() % 2) == 1)
    {
        USB_OTG_H_TxUseEvenFrame(chNum);
    }
    else
    {
        USB_OTG_H_TxUseOddFrame(chNum);
    }

    /* enable channel */
    USB_OTG_H_EnableChannel(chNum);
}


/*!
 * @brief       Halt channel
 *
 * @param       chNum : Select channel.
 *
 * @retval      None
 */
void USB_OTG_HaltChannel(uint16_t chNum)
{
    /* Read Remain space */
    if ((USB_OTG_H_ReadEpType(chNum) == EP_TYPE_CONTROL)
            || (USB_OTG_H_ReadEpType(chNum) == EP_TYPE_BULK))
    {
        if (USB_OTG_G->GNPTXFQSTS_B.NPTXRSA == 0)
        {
            USB_OTG_H_ClearQueue(chNum);
        }
    }
    else
    {
        /* TxFIFO full*/
        if (USB_OTG_H_ReadTxQueueRemainSpace() == 0)
        {
            USB_OTG_H_ClearQueue(chNum);
        }
    }

    /* Channel Disable */
    USB_OTG_H_DisableChannel(chNum);
}


/*!
 * @brief       Init channel
 *
 * @param       channelConfig : Config channel struct.
 *
 * @retval      None
 */
void USB_OTG_InitChannel(USB_ChannelConfig_T* channelConfig)
{
    /* Clear interrupt conditions */
    USB_OTG_H_ClearChannelIntFlag(channelConfig->chNum, CHANNEL_INT_ALL);

    /* Enable channel interrupts required for this transfer. */
    USB_OTG_H_DisableChannelInt(channelConfig->chNum, CHANNEL_INT_ALL);

    if ((channelConfig->epType == USB_EP_TYPE_CONTROL)
            || (channelConfig->epType == USB_EP_TYPE_BULK))
    {
        USB_OTG_H_EnableChannelInt(channelConfig->chNum,
                                   (CHANNEL_INT_TSFCMPN |
                                    CHANNEL_INT_RXSTALL |
                                    CHANNEL_INT_TERR |
                                    CHANNEL_INT_DTOG |
                                    CHANNEL_INT_RXNAK));

        if (channelConfig->direction == EP_DIR_IN)
        {
            USB_OTG_H_EnableChannelInt(channelConfig->chNum, CHANNEL_INT_BABBLE);
        }
        else
        {
            USB_OTG_H_EnableChannelInt(channelConfig->chNum, CHANNEL_INT_RXNYET);
            if (channelConfig->enbalePing == 1)
            {
                USB_OTG_H_EnableChannelInt(channelConfig->chNum, CHANNEL_INT_RXTXACK);
            }
        }
    }
    else if (channelConfig->epType == USB_EP_TYPE_ISO)
    {
        USB_OTG_H_EnableChannelInt(channelConfig->chNum,
                                   (CHANNEL_INT_TSFCMPN |
                                    CHANNEL_INT_FOVR |
                                    CHANNEL_INT_RXTXACK));


        if (channelConfig->direction == EP_DIR_IN)
        {
            USB_OTG_H_EnableChannelInt(channelConfig->chNum,
                                       (CHANNEL_INT_TERR |
                                        CHANNEL_INT_BABBLE));
        }
    }
    else if (channelConfig->epType == USB_EP_TYPE_INTERRUPT)
    {
        USB_OTG_H_EnableChannelInt(channelConfig->chNum,
                                   (CHANNEL_INT_TSFCMPN |
                                    CHANNEL_INT_RXNAK |
                                    CHANNEL_INT_RXSTALL |
                                    CHANNEL_INT_TERR |
                                    CHANNEL_INT_DTOG |
                                    CHANNEL_INT_FOVR));

        if (channelConfig->direction == EP_DIR_IN)
        {
            USB_OTG_H_EnableChannelInt(channelConfig->chNum, CHANNEL_INT_BABBLE);
        }
    }

    /* Enable the top level host channel interrupt. */
    USB_OTG_H_EnableAllChannelsInt(1 << channelConfig->chNum);

    /* Enabled global host channel interrupts */
    USB_OTG_EnableGlobalInterrupt(USB_INT_G_HCHAN);

    /* Config the OTG_HS1_HCHX register */
    USB_OTG_H_ConfigDeviceAddr(channelConfig->chNum, channelConfig->devAddr);

    USB_OTG_H_ConfigEpNum(channelConfig->chNum, channelConfig->epNum);

    USB_OTG_H_ConfigEpDirection(channelConfig->chNum, channelConfig->direction);

    if (channelConfig->devSpeed == SPEED_LS)
    {
        USB_OTG_H_EnableLowSpeedDevice(channelConfig->chNum);
    }
    else
    {
        USB_OTG_H_DisableLowSpeedDevice(channelConfig->chNum);
    }

    USB_OTG_H_ConfigEpType(channelConfig->chNum, channelConfig->epType);

    USB_OTG_H_ConfigEpMaxPSize(channelConfig->chNum, channelConfig->maxPackSize);

    if (channelConfig->epType == USB_EP_TYPE_INTERRUPT)
    {
        USB_OTG_H_TxUseOddFrame(channelConfig->chNum);
    }
    else
    {
        USB_OTG_H_TxUseEvenFrame(channelConfig->chNum);
    }
}

/*!
 * @brief       Enable channel ping
 *
 * @param       chNum : Select channel.
 *
 * @retval      None
 */
void USB_OTG_EnableChannelPing(uint8_t chNum)
{
    USB_OTG_H->REGS_HCH[chNum].HCHTSIZE = 0;

    USB_OTG_H_ConfigPacketCount(chNum, 1);

    USB_OTG_H_EnableDoPing(chNum);

    USB_OTG_H_EnableChannel(chNum);
}

/*!
 * @brief       Stop Host
 *
 * @param       None
 *
 * @retval      None
 */
void USB_OTG_StopHost(void)
{
    uint8_t i = 0;

    USB_OTG_H_DisableAllChannelsInt(USB_H_CHANNEL_ALL);

    /* disable channel */
    for (i = 0; i < HOST_CHANNEL_NUM; i++)
    {
        USB_OTG_H_ClearQueue(i);

        USB_OTG_H_ConfigEpDirection(i, EP_DIR_OUT);
    }

    /* Flush the FIFO */
    USB_OTG_FlushRxFIFO();
    USB_OTG_FlushTxFIFO(0x10);
}

#if defined(__ICCARM__)
/*!
 * @brief       Reads the interrupt flag of the All the channel
 *
 * @param       flag : USB_H_CHANNEL_T type.
 *
 * @retval      Flag status. 1 to set and 0 to not set
 */
uint8_t USB_OTG_H_ReadAllChannelsIntFlag(uint32_t flag)
{
    uint32_t temp1 = USB_OTG_H->HACHINT;
    uint32_t temp2 = USB_OTG_H->HACHIMASK;

    return (((temp1 & temp2) & (uint32_t)flag) ? SET : RESET);
}

/*!
 * @brief       Reads All the interrupt flag of the specified channel.
 *
 * @param       chNum : Select channel.
 *
 * @retval      All the interrupt status.
 */

uint32_t USB_OTG_H_ReadChannelAllIntFlag(uint8_t chNum)
{
    uint32_t temp1 = USB_OTG_H->REGS_HCH[chNum].HCHINT;
    uint32_t temp2 = USB_OTG_H->REGS_HCH[chNum].HCHIMASK;

    return (temp1 & temp2);
}

/*!
 * @brief       Reads All the interrupt flag of the channel
 *
 * @param       chNum : Select channel.
 *
 * @param       flag : USB_H_CHANNEL_INT_T type.
 *
 * @retval      Flag status. 1 to set and 0 to not set
 */
uint8_t USB_OTG_H_ReadChannelIntFlag(uint8_t chNum, USB_H_CHANNEL_INT_T flag)
{
    uint32_t temp1 = USB_OTG_H->REGS_HCH[chNum].HCHINT;
    uint32_t temp2 = USB_OTG_H->REGS_HCH[chNum].HCHIMASK;

    return (((temp1 & temp2) & (uint32_t)flag) ? SET : RESET);
}
/**@} end of group USB_Host_Functions */
/**@} end of group USB_Host */
/**@} end of group Driver */
/**@} end of group USB_OTG_Library */
#endif
