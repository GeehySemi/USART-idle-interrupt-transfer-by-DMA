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

/* Includes */
#include "usbd_interrupt.h"
#include "usb_fifo.h"
#include "usbd_core.h"
#include "usbd_init.h"
#include "usb_user.h"

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

/** @defgroup Interrupt_Functions Functions
  @{
*/

static void USBD_OutEndpointIsrHandler(void);
static void USBD_InEndpointIsrHandler(void);
static void USBD_RxFifoNoEmptyIsrHandler(void);
static void USBD_ResetIsrHandler(void);

static void USBD_ResumeIsrHandler(void);
static void USBD_SuspendIsrHandler(void);
static void USBD_EnumDoneIsrHandler(void);
static void USBD_ModeMismatchIsrHandler(void);
static void USBD_SofIsrHanlder(void);
static void USBD_NewSessionIsrHandler(void);
static void USBD_OTGIsrHandler(void);

/*!
 * @brief       Handle USB Global interrupt
 *
 * @param       None
 *
 * @retval      None
 */
void USBD_OTG_IsrHandler(void)
{
    /* Handle OUT Endpoint interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_ONEP))
    {
        USBD_OutEndpointIsrHandler();
    }

    /* Handle IN Endpoint interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_INEP))
    {
        USBD_InEndpointIsrHandler();
    }

    /* Handle RxFIFO no empty interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_RXFNONE))
    {
        USBD_RxFifoNoEmptyIsrHandler();
    }

    /* Handle USB Reset interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_USBRST))
    {
        USBD_ResetIsrHandler();
    }

    /* Handle Resume/remote wakeup detected interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_RWAKE))
    {
        USBD_ResumeIsrHandler();
    }

    /* Handle USB suspend interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_USBSUS))
    {
        USBD_SuspendIsrHandler();
    }

    /* Handle Enumeration done interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_ENUMD))
    {
        USBD_EnumDoneIsrHandler();
    }

    /* Handle Mode mismatch interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_MMIS))
    {
        USBD_ModeMismatchIsrHandler();
    }

    /* Handle  interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_SOF))
    {
        USBD_SofIsrHanlder();
    }

    /* Handle Session request/new session detected interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_SREQ))
    {
        USBD_NewSessionIsrHandler();
    }

    /* Handle OTG interrupt */
    if (USB_OTG_ReadGlobalIntFlag(USB_INT_G_OTG))
    {
        USBD_OTGIsrHandler();
    }

    if (g_usbDev.intHandler)
    {
        g_usbDev.intHandler();
    }
}

/*!
 * @brief       Handle OUT endpiont interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBD_OutEndpointIsrHandler(void)
{
    uint16_t epIntNum;
    uint8_t ep;

    epIntNum = USB_OTG_ReadOutEpIntNumber();

    for (ep = 0; ep < 16; ep++)
    {
        if (!(epIntNum >> ep))
        {
            break;
        }
        /* Fine out all the interrupt OUT endpoint */
        else if ((epIntNum >> ep) & 0x01)
        {
            /* Transfer completed */
            if (USB_OTG_ReadOutEpIntStatus(ep, USB_INT_EP_OUT_TSFCMP))
            {
                USB_OTG_ClearOutEpIntStatus(ep, USB_INT_EP_OUT_TSFCMP);

                /* Handle control EP */
                if (ep == USB_EP_0)
                {
                    /* Control OUT */
                    USBD_CtrlOutProcess();
                }

                /* Handle other OUT EP */
                else if (g_usbDev.outEpHandler != NULL)
                {
                    g_usbDev.outEpHandler(ep);
                }
            }

            /* Setup completed */
            if (USB_OTG_ReadOutEpIntStatus(ep, USB_INT_EP_OUT_SETPCMP))
            {
                USB_OTG_ClearOutEpIntStatus(ep, USB_INT_EP_OUT_SETPCMP);

                /* SETUP */
                USBD_SetupProcess();
            }

            /* Endpoint disable */
            if (USB_OTG_ReadOutEpIntStatus(ep, USB_INT_EP_OUT_EPDIS))
            {
                USB_OTG_ClearOutEpIntStatus(ep, USB_INT_EP_OUT_EPDIS);
            }

            /* OUT token received when endpoint disabled */
            if (USB_OTG_ReadOutEpIntStatus(ep, USB_INT_EP_OUT_RXOTDIS))
            {
                USB_OTG_ClearOutEpIntStatus(ep, USB_INT_EP_OUT_RXOTDIS);
            }

            /* Received Back-to-back SETUP packets over 3 */
            if (USB_OTG_ReadOutEpIntStatus(ep, USB_INT_EP_OUT_RXBSP))
            {
                USB_OTG_ClearOutEpIntStatus(ep, USB_INT_EP_OUT_RXBSP);
            }
        }
    }
}

/*!
 * @brief       Handle IN endpiont interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBD_InEndpointIsrHandler(void)
{
    uint16_t epIntNum;
    uint8_t ep;

    epIntNum = USB_OTG_ReadInEpIntNumber();

    for (ep = 0; ep < 16; ep++)
    {
        if (!(epIntNum >> ep))
        {
            break;
        }
        /* Fine out all the interrupt IN endpoint */
        else if ((epIntNum >> ep) & 0x01)
        {
            /* Transfer completed */
            if (USB_OTG_ReadInEpIntStatus(ep, USB_INT_EP_IN_TSFCMP))
            {
                USB_OTG_ClearInEpIntStatus(ep, USB_INT_EP_IN_TSFCMP);

                USB_OTG_DisableInEpEmptyInterrupt(ep);

                if (ep == USB_EP_0)
                {
                    /* Control IN */
                    USBD_CtrlInProcess();
                }
                /* Handle other IN EP when Transfer completed */
                else if (g_usbDev.inEpHandler != NULL)
                {
                    g_usbDev.inEpHandler(ep);
                }
            }

            /* TxFIFO empty */
            if (USB_OTG_ReadInEpIntStatus(ep, USB_INT_EP_IN_TXFE))
            {
                USBD_PushDataToTxFIFO(ep);
            }
            /* Endpoint disabled */
            if (USB_OTG_ReadInEpIntStatus(ep, USB_INT_EP_IN_EPDIS))
            {
                USB_OTG_ClearInEpIntStatus(ep, USB_INT_EP_IN_EPDIS);
            }
            /* Timeout */
            if (USB_OTG_ReadInEpIntStatus(ep, USB_INT_EP_IN_TO))
            {
                USB_OTG_ClearInEpIntStatus(ep, USB_INT_EP_IN_TO);
            }
            /* IN endpoint NAK effective */
            if (USB_OTG_ReadInEpIntStatus(ep, USB_INT_EP_IN_IEPNAKE))
            {
                USB_OTG_ClearInEpIntStatus(ep, USB_INT_EP_IN_IEPNAKE);
            }
            /* IN token received when TxFIFO is empty */
            if (USB_OTG_ReadInEpIntStatus(ep, USB_INT_EP_IN_ITXEMP))
            {
                USB_OTG_ClearInEpIntStatus(ep, USB_INT_EP_IN_ITXEMP);
            }
        }
    }
}

/*!
 * @brief       Handle RxFifo No-Empty interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBD_RxFifoNoEmptyIsrHandler(void)
{
    USBD_FIFO_Status_T fifoStatus;
    uint8_t ep;

    /* Disable RxFIFO no empty Interrup */
    USB_OTG_DisableGlobalInterrupt(USB_INT_G_RXFNONE);

    /* Read and pop the RxFIFO status data */
    fifoStatus.word = USB_OTG_PopRxFifoStatus();
    ep = fifoStatus.bit.ep;

    /* Handle RxFIFO data */
    switch (fifoStatus.bit.status)
    {
        /* Received OUT data packet */
        case FIFO_STATUS_OUT_DATA :
            if ((fifoStatus.bit.byteCount != 0) && (g_usbDev.outBuf[ep].pBuf != NULL))
            {
                USB_FIFO_ReadRxFifoPacket(g_usbDev.outBuf[ep].pBuf, \
                                          fifoStatus.bit.byteCount);

                g_usbDev.outBuf[ep].xferCnt = fifoStatus.bit.byteCount;
                g_usbDev.outBuf[ep].pBuf   += fifoStatus.bit.byteCount;
                g_usbDev.outBuf[ep].bufLen -= fifoStatus.bit.byteCount;
                g_usbDev.outBuf[ep].packNum--;
            }
            break;

        /* Received SETUP data packet */
        case FIFO_STATUS_SETUP_DATA :
            USB_FIFO_ReadRxFifoPacket(g_usbDev.reqData.pack, 8);
            break;

        case FIFO_STATUS_OUT_COMP :
            break;
        case FIFO_STATUS_SETUP_COMP :
            break;
        case FIFO_STATUS_G_OUT_NAK :
            break;
        default:
            break;
    }
    /* Enable RxFIFO no empty Interrupt */
    USB_OTG_EnableGlobalInterrupt(USB_INT_G_RXFNONE);
}

/*!
 * @brief       Handle USB Reset interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBD_ResetIsrHandler(void)
{
    uint8_t ep;

    /* Clear the Remote wakeup signaling */
    USB_OTG_DisableRemoteWakeupSignal();

    USB_OTG_FlushTxFIFO(0);

    /* Clear endpoint interrupt flag */
    for (ep = 0; ep < USB_MAX(USB_OUT_EP_MAX_NUM, USB_IN_EP_MAX_NUM); ep++)
    {
        /* Clear endpoint Flag */
        USB_OTG_ClearOutEpIntStatus(ep, USB_INT_EP_OUT_ALL);
        USB_OTG_ClearInEpIntStatus(ep, USB_INT_EP_IN_ALL);
    }

    /* Enable enpoint 0 global interrupt */
    USB_OTG_EnableOutEpInterruptMask(USB_EP_0);
    USB_OTG_EnableInEpInterruptMask(USB_EP_0);

    /* Enable OUT EP interrupt */
    USB_OTG_EnableOutEpInterrupt(USB_INT_EP_OUT_SOURCE);

    /* Enable IN EP interrupt */
    USB_OTG_EnableInEpInterrupt(USB_INT_EP_IN_SOURCE);

    /* Reset device address */
    USB_OTG_ConfigDeviceAddress(0);

    USB_OTG_ReceiveSetupPacket(3);

    /* EP Active */
    USBD_OpenOutEP(USB_EP_0, USB_EP_TYPE_CONTROL, USB_EP0_PACKET_SIZE);
    USBD_OpenInEP(USB_EP_0, USB_EP_TYPE_CONTROL, USB_EP0_PACKET_SIZE);

    g_usbDev.curFeature = g_usbDev.pConfigurationDesc->pDesc[7];
    g_usbDev.devState = USBD_DEVICE_STATE_DEFAULT;

    USB_OTG_ClearGlobalIntFlag(USB_INT_G_USBRST);

    /* Reset handler */
    if (g_usbDev.resetHandler != NULL)
    {
        g_usbDev.resetHandler();
    }

    /* User reset callback function */
    g_userCallback.USER_DeviceReset();
}

/*!
 * @brief       Handle USB Resume interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBD_ResumeIsrHandler(void)
{
#if USB_LOW_POWER_SWITCH
    USB_OTG_PWR->PCGCTRL_B.PCLKSTOP = BIT_RESET;
    USB_OTG_PWR->PCGCTRL_B.GCLK = BIT_RESET;
#endif

    USB_OTG_DisableRemoteWakeupSignal();

    g_userCallback.USER_DeviceResume();

    g_usbDev.devState = USBD_DEVICE_STATE_CONFIGURED;

    USB_OTG_ClearGlobalIntFlag(USB_INT_G_RWAKE);
}

/*!
 * @brief       Handle USB Suspend interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBD_SuspendIsrHandler(void)
{
    USB_OTG_ClearGlobalIntFlag(USB_INT_G_USBSUS);

    g_userCallback.USER_DeviceSuspend();

    g_usbDev.devState = USBD_DEVICE_STATE_SUSPENDED;
}

/*!
 * @brief       Handle USB Enum Done interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBD_EnumDoneIsrHandler(void)
{
    uint32_t hClkFreq;

    hClkFreq = RCM_ReadHCLKFreq();

    if (hClkFreq < 48000000)
    {
        /* TurnaroundTime = 4 * (48M / hclk) + 1*/
        USB_OTG_ConfigTurnaroundTime(((((uint32_t)48000000 << 2) + (hClkFreq - 1)) / hClkFreq) + 1);
    }
    else
    {
        USB_OTG_ConfigTurnaroundTime(0x5);
    }

    USB_OTG_ClearGlobalIntFlag(USB_INT_G_ENUMD);
}

/*!
 * @brief       Handle USB Mode Mismatch interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBD_ModeMismatchIsrHandler(void)
{
    USB_OTG_ClearGlobalIntFlag(USB_INT_G_MMIS);
}

/*!
 * @brief       Handle USB Start of Frame interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBD_SofIsrHanlder(void)
{
    USB_OTG_ClearGlobalIntFlag(USB_INT_G_SOF);
}

/*!
 * @brief       Handle USB New Session detected interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBD_NewSessionIsrHandler(void)
{
    USB_OTG_ClearGlobalIntFlag(USB_INT_G_SREQ);
}

/*!
 * @brief       Handle USB OTG interrupt
 *
 * @param       None
 *
 * @retval      None
 */
static void USBD_OTGIsrHandler(void)
{
    USB_OTG_ClearGlobalIntFlag(USB_INT_G_OTG);
}

/**@} end of group Interrupt_Functions */
/**@} end of group Interrupt */
/**@} end of group Standard */
/**@} end of group Core_Device */
/**@} end of group USB_OTG_Library */
