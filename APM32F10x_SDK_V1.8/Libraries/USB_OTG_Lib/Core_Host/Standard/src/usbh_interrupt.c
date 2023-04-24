/*!
 * @file        usbh_interrupt.h
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

/* Includes */
#include "usbh_interrupt.h"
#include "usb_fifo.h"
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

/** @addtogroup Interrupt
  @{
*/

/** @defgroup Interrupt_Functions Functions
  @{
*/

static void USBH_RxFifoNoEmptyIsrHandler(void);
static void USBH_NonPerTxFifoEmptyIsrHandler(void);
static void USBH_PerTxFifoEmptyIsrHandler(void);
static void USBH_FrameStartIsrHandler(void);
static void USBH_HostChannelsIsrHandler(void);
static void USBH_ISOOutTXIsrHandler(void);
static void USBH_HostPortIsrHandler(void);
static void USBH_DisconnectIsrHandler(void);
static void USBH_IN_HostChannelIsrHandler(uint16_t channelNum);
static void USBH_OUT_HostChannelIsrHandler(uint16_t channelNum);

/*!
 * @brief       Handle HS USB Global interrupt
 *
 * @param       None
 *
 * @retval      None
 */
void USBH_OTG_IsrHandler(void)
{
    /* Handle RxFIFO no empty interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_RXFNONE))
    {
        USBH_RxFifoNoEmptyIsrHandler();
    }
    /* Handle USB Non-periodic TxFIFO empty interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_NPTXFEM))
    {
        USBH_NonPerTxFifoEmptyIsrHandler();
    }

    /* Handle USB Periodic TxFIFO empty interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_PTXFE))
    {
        USBH_PerTxFifoEmptyIsrHandler();
    }

    /* Handle Start of Frame interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_SOF))
    {
        USBH_FrameStartIsrHandler();
    }

    /* Handle Host channels interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_HCHAN))
    {
        USBH_HostChannelsIsrHandler();
    }

    /* Handle Incomplete isochronous OUT transfer interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_IP_OUTTX))
    {
        USBH_ISOOutTXIsrHandler();
    }

    /* Handle Host port interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_HPORT))
    {
        USBH_HostPortIsrHandler();
    }

    /* Handle Disconnect detected interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_DEDIS))
    {
        USBH_DisconnectIsrHandler();
    }

    /* Hadler the rest of global interrupt */
    if (g_usbHost.intHandler)
    {
        g_usbHost.intHandler();
    }
}

/*!
 * @brief       Handle RxFIFO no empty interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBH_RxFifoNoEmptyIsrHandler(void)
{
    USBH_FIFO_Status_T fifoStatus;
    uint16_t channelNum;
    uint8_t packetStatus;

    /* Disable RxFIFO no empty Interrup */
    USB_OTG_DisableGlobalInterrupt(USB_INT_G_RXFNONE);

    /* Read and pop the RxFIFO status data */
    fifoStatus.word = USB_OTG_PopRxFifoStatus();
    channelNum = fifoStatus.bit.chNum;
    packetStatus = fifoStatus.bit.status;

    if (packetStatus == PKTSTS_IN)
    {
        /* Read the data into the host buffer */
        if ((fifoStatus.bit.byteCount > 0) && (g_usbHost.xferPipe[channelNum].buf != NULL))
        {
            USB_FIFO_ReadRxFifoPacket(g_usbHost.xferPipe[channelNum].buf, fifoStatus.bit.byteCount);
            /* manage multiple Xfer */
            g_usbHost.xferPipe[channelNum].buf      += fifoStatus.bit.byteCount;
            g_usbHost.xferPipe[channelNum].bufCount += fifoStatus.bit.byteCount;

            if (USB_OTG_H_ReadPacketCount(channelNum) > 0)
            {
                /* re-activate the channel when more packets are expected */
                USB_OTG_H_EnableChannel(channelNum);
            }
        }
    }
    else if (packetStatus == PKTSTS_IN_XFER_COMPLETE)
    {
    }
    else if (packetStatus == PKTSTS_DATA_ISO_ERR)
    {
    }
    else if (packetStatus == PKTSTS_CHANNEL_HALT)
    {
    }
    else
    {
    }

    /* Enable RxFIFO no empty Interrupt */
    USB_OTG_EnableGlobalInterrupt(USB_INT_G_RXFNONE);
}

/*!
 * @brief       Handle USB Non-periodic TxFIFO empty interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBH_NonPerTxFifoEmptyIsrHandler(void)
{
    uint32_t txFifoSpace;
    uint16_t len;
    uint8_t channelNum;

    channelNum = ((USB_OTG_G->GNPTXFQSTS_B.NPTXRQ >> 3) & 0x0F);
    len = g_usbHost.xferPipe[channelNum].bufLen;
    txFifoSpace = USB_OTG_G->GNPTXFQSTS_B.NPTXFSA * 4;

    while ((txFifoSpace > len) && (g_usbHost.xferPipe[channelNum].bufLen != 0))
    {
        if (txFifoSpace > g_usbHost.xferPipe[channelNum].bufLen)
        {
            /* Last packet */
            len = g_usbHost.xferPipe[channelNum].bufLen;

            USB_OTG_DisableGlobalInterrupt(USB_INT_G_NPTXFEM);
        }
        else
        {
            len = txFifoSpace;
        }

        USB_FIFO_WriteFifoPacket(channelNum, g_usbHost.xferPipe[channelNum].buf, len);

        g_usbHost.xferPipe[channelNum].buf      += len;
        g_usbHost.xferPipe[channelNum].bufLen   -= len;
        g_usbHost.xferPipe[channelNum].bufCount += len;

        txFifoSpace = USB_OTG_G->GNPTXFQSTS_B.NPTXFSA * 4;
    }
}

/*!
 * @brief       Handle USB Periodic TxFIFO empty interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBH_PerTxFifoEmptyIsrHandler(void)
{
    uint32_t txFifoSpace;
    uint16_t len;
    uint16_t channelNum;

    channelNum = USB_OTG_H_TxQueueSerialNum();

    txFifoSpace = USB_OTG_H_ReadTxFIFORemainSpace() * 4;
    len = g_usbHost.xferPipe[channelNum].bufLen;

    while ((txFifoSpace > len) && (g_usbHost.xferPipe[channelNum].bufLen != 0))
    {
        if (txFifoSpace > g_usbHost.xferPipe[channelNum].bufLen)
        {
            /* Last packet */
            len = g_usbHost.xferPipe[channelNum].bufLen;

            USB_OTG_DisableGlobalInterrupt(USB_INT_G_PTXFE);
        }
        else
        {
            len = txFifoSpace;
        }

        USB_FIFO_WriteFifoPacket(channelNum, g_usbHost.xferPipe[channelNum].buf, len);

        g_usbHost.xferPipe[channelNum].buf  += len;
        g_usbHost.xferPipe[channelNum].bufLen   -= len;
        g_usbHost.xferPipe[channelNum].bufCount  += len;

        txFifoSpace = USB_OTG_H_ReadTxFIFORemainSpace();
    }
}

/*!
 * @brief       Handle Frame Start of interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBH_FrameStartIsrHandler(void)
{
    /* Clear interrupt */
    USB_OTG_ClearGlobalIntFlag(USB_INT_G_SOF);
}


/*!
 * @brief       Handle Host channels interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBH_HostChannelsIsrHandler(void)
{
    uint32_t i = 0;

    for (i = 0; i < HOST_CHANNEL_NUM; i++)
    {
        if (USB_OTG_H_ReadAllChannelsIntFlag(1 << i) == SET)
        {
            if (USB_OTG_H_ReadEpDirection(i) == EP_DIR_IN)
            {
                USBH_IN_HostChannelIsrHandler(i);
            }
            else
            {
                USBH_OUT_HostChannelIsrHandler(i);
            }
        }
    }
}

/*!
 * @brief       Handle Incomplete isochronous OUT transfer interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBH_ISOOutTXIsrHandler(void)
{
    USB_OTG_H_DisableChannel(0);

    /* Clear interrupt */
    USB_OTG_ClearGlobalIntFlag(USB_INT_G_IP_OUTTX);
}


/*!
 * @brief       Handle Host port interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBH_HostPortIsrHandler(void)
{
    /* Read Port Connect Flag */
    if (USB_OTG_H_ReadPortConnectIntTriFlag() == SET)
    {
        USB_OTG_H_ClearPortConnectIntTriFlag();
        /* USBH Connected Handler */
        g_usbHost.connectedFlag = 1;
    }

    /* Port Enable is Changed */
    if (USB_OTG_H_PenBitIsChange() == SET)
    {
        /* Port is Enable */
        if (USB_OTG_H_PortIsEnable() == SET)
        {
            if (USB_OTG_H_ReadPortSpeedType() == SPEED_LS)
            {
                USB_OTG_H_SetFrameInterval(6000);
                if (USB_OTG_H_ReadHostClk() != PHYCLK_6_MHZ)
                {
#ifdef OTG_HS_PHY_ULPI /*!< ULPI PHY */
                    USB_OTG_H_SetFrameInterval(48000);
                    if (USB_OTG_H_ReadHostClk() != PHYCLK_48_MHZ)
                    {
                        USB_OTG_H_ConfigHostClk(PHYCLK_48_MHZ);
                    }
#else /*!< UTMI PHY (OTG_HS_PHY_UTMI) */
                    USB_OTG_H_ConfigHostClk(PHYCLK_6_MHZ);
#endif
                    USB_OTG_PortReset();
                }
            }
            else if (USB_OTG_H_ReadPortSpeedType() == SPEED_FS)
            {
                USB_OTG_H_SetFrameInterval(48000);
                if(USB_OTG_H_ReadHostClk() != PHYCLK_48_MHZ)
                {
                    USB_OTG_H_ConfigHostClk(PHYCLK_48_MHZ);
                    USB_OTG_PortReset();
                }
            }
            else
            {
                USB_OTG_PortReset();
            }

            /* USBH Port Enabled Handler */
            g_usbHost.portEnable = 1;

            /* Enable Disconnect detected interrupt */
            USB_OTG_EnableGlobalInterrupt(USB_INT_G_DEDIS);
        }
        else
        {
            /* USBH Port Disabled Handler */
            g_usbHost.portEnable = 0;
        }
    }

    /* Port overcurrent is Changed */
    if (USB_OTG_H_PovcBitIsChange() == SET)
    {
    }
    /* Clear Port Interrupts */
    USB_OTG_H_DisablePort();
}

/*!
 * @brief       Handle Disconnect detected interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBH_DisconnectIsrHandler(void)
{
    /* USBH Disconnected Handler */
    USB_OTG_ClearGlobalIntFlag(0xFFFFFFFF);
    USB_OTG_DisableGlobalInterrupt(0xFFFFFFFF);
    USB_OTG_DisableAllGlobalInterrupt();
    g_usbHost.connectedFlag = 0;

    /* Clear interrupt */
    USB_OTG_ClearGlobalIntFlag(USB_INT_G_DEDIS);
}

/*!
 * @brief       Handle Host port in interrupt
 *
 * @param       channelNum : Select channel.
 *
 * @retval      None
 */
static void USBH_IN_HostChannelIsrHandler(uint16_t channelNum)
{
    uint8_t intNak;
    intNak = USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_DTOG);

    if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_AHBERR))
    {
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_AHBERR);
        USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);
    }

    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_RXTXACK))
    {
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_RXTXACK);
    }

    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_RXSTALL))
    {
        USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);
        g_usbHost.xferPipe[channelNum].pipeState = PIPE_STALL;
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_RXNAK);
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_RXSTALL);
        intNak = 0;

        USB_OTG_HaltChannel(channelNum);
    }

    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_DTOG))
    {
        USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);
        USB_OTG_HaltChannel(channelNum);
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_RXNAK);
        g_usbHost.xferPipe[channelNum].pipeState = PIPE_DATATGLERR;
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_DTOG);
    }


    if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_FOVR))
    {
        USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);
        USB_OTG_HaltChannel(channelNum);
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_FOVR);
    }

    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_TSFCMPN))
    {
        g_usbHost.xferPipe[channelNum].pipeState = PIPE_OK;
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_TSFCMPN);

        if ((USB_OTG_H_ReadEpType(channelNum) == EP_TYPE_CONTROL)
                || (USB_OTG_H_ReadEpType(channelNum) == EP_TYPE_BULK))
        {
            USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);
            USB_OTG_HaltChannel(channelNum);
            USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_RXNAK);
            g_usbHost.xferPipe[channelNum].pidToggle ^= 1;
        }

        else if (USB_OTG_H_ReadEpType(channelNum) == EP_TYPE_INTERRUPT)
        {
            USB_OTG_H_TxUseOddFrame(channelNum);
            g_usbHost.xferPipe[channelNum].urbStatus = USBH_URB_OK;
        }
    }
    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_TSFCMPAN))
    {
        USB_OTG_H_DisableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);

        if (g_usbHost.xferPipe[channelNum].pipeState == PIPE_OK)
        {
            g_usbHost.xferPipe[channelNum].urbStatus = USBH_URB_OK;
        }

        else if (g_usbHost.xferPipe[channelNum].pipeState == PIPE_STALL)
        {
            g_usbHost.xferPipe[channelNum].urbStatus = USBH_URB_STALL;
        }

        else if ((g_usbHost.xferPipe[channelNum].pipeState == PIPE_XACTERR)
                 || (g_usbHost.xferPipe[channelNum].pipeState == PIPE_DATATGLERR))
        {
            g_usbHost.xferPipe[channelNum].urbStatus = USBH_URB_ERROR;
        }
        if (USB_OTG_H_ReadEpType(channelNum) == EP_TYPE_INTERRUPT)
        {
            g_usbHost.xferPipe[channelNum].pidToggle ^= 1;
        }

        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_TSFCMPAN);

    }
    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_TERR))
    {
        USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);
        g_usbHost.xferPipe[channelNum].pipeState = PIPE_XACTERR;
        USB_OTG_HaltChannel(channelNum);
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_TERR);
    }
    else if ((USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_RXNAK)) || (intNak == 1))
    {
        if (USB_OTG_H_ReadEpType(channelNum) == EP_TYPE_INTERRUPT)
        {
            USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);

            USB_OTG_HaltChannel(channelNum);
        }
        else if ((USB_OTG_H_ReadEpType(channelNum) == EP_TYPE_CONTROL)
                 || (USB_OTG_H_ReadEpType(channelNum) == EP_TYPE_BULK))
        {
            /* Enable the channel */
            USB_OTG_H_EnableChannel(channelNum);
        }
        g_usbHost.xferPipe[channelNum].pipeState = PIPE_NAK;
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_RXNAK);
    }
}

/*!
 * @brief       Handle Host port out interrupt
 *
 * @param       channelNum : Select channel.
 *
 * @retval      None
 */
static void USBH_OUT_HostChannelIsrHandler(uint16_t channelNum)
{
    if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_AHBERR))
    {
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_AHBERR);
        USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);
    }

    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_FOVR))
    {
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_FOVR);
        USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);
        USB_OTG_HaltChannel(channelNum);
    }

    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_TSFCMPN))
    {
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_TSFCMPN);
        USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);
        USB_OTG_HaltChannel(channelNum);

        g_usbHost.xferPipe[channelNum].pipeState = PIPE_OK;
    }

    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_RXTXACK))
    {
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_RXTXACK);
    }
    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_RXSTALL))
    {
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_RXSTALL);
        USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);
        USB_OTG_HaltChannel(channelNum);

        g_usbHost.xferPipe[channelNum].pipeState = PIPE_STALL;
    }

    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_RXNAK))
    {
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_RXNAK);
        USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);

        USB_OTG_HaltChannel(channelNum);

        g_usbHost.xferPipe[channelNum].pipeState = PIPE_NAK;
    }

    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_TERR))
    {
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_TERR);
        USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);
        USB_OTG_HaltChannel(channelNum);

        g_usbHost.xferPipe[channelNum].pipeState = PIPE_XACTERR;
    }

    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_RXNYET))
    {
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_RXNYET);
        USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);

        USB_OTG_HaltChannel(channelNum);

        g_usbHost.xferPipe[channelNum].pipeState = PIPE_NYET;
    }

    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_DTOG))
    {
        USB_OTG_H_EnableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);
        USB_OTG_HaltChannel(channelNum);
        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_RXNAK);

        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_DTOG);

        g_usbHost.xferPipe[channelNum].pipeState = PIPE_DATATGLERR;
    }

    else if (USB_OTG_H_ReadChannelIntFlag(channelNum, CHANNEL_INT_TSFCMPAN))
    {
        USB_OTG_H_DisableChannelInt(channelNum, CHANNEL_INT_TSFCMPAN);

        if (g_usbHost.xferPipe[channelNum].pipeState == PIPE_OK)
        {
            g_usbHost.xferPipe[channelNum].urbStatus = USBH_URB_OK;

            if (USB_OTG_H_ReadEpType(channelNum) == EP_TYPE_BULK)
            {
                g_usbHost.xferPipe[channelNum].pidToggle ^= 1;
            }
        }
        else if (g_usbHost.xferPipe[channelNum].pipeState == PIPE_NAK)
        {
            g_usbHost.xferPipe[channelNum].urbStatus = USBH_URB_NOREADY;
        }
        else if (g_usbHost.xferPipe[channelNum].pipeState == PIPE_NYET)
        {
            if (g_usbHost.xferPipe[channelNum].enbalePing == 1)
            {
                USB_OTG_EnableChannelPing(channelNum);
            }
            g_usbHost.xferPipe[channelNum].urbStatus = USBH_URB_NOREADY;
        }
        else if (g_usbHost.xferPipe[channelNum].pipeState == PIPE_STALL)
        {
            g_usbHost.xferPipe[channelNum].urbStatus = USBH_URB_STALL;
        }
        else if (g_usbHost.xferPipe[channelNum].pipeState == PIPE_XACTERR)
        {
            g_usbHost.xferPipe[channelNum].urbStatus = USBH_URB_ERROR;
        }

        USB_OTG_H_ClearChannelIntFlag(channelNum, CHANNEL_INT_TSFCMPAN);
    }
}

/**@} end of group Interrupt_Functions */
/**@} end of group Interrupt */
/**@} end of group Standard */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */
