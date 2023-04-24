/*!
 * @file        usbd_init.c
 *
 * @brief       USB device initialization
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
#include "usbh_init.h"
#include "usb_user.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Host Core Host
  @{
*/

/** @addtogroup Standard
  @{
*/

/** @addtogroup Init
  @{
*/

/** @defgroup Init_Functions Functions
  @{
*/

/*!
 * @brief     USB Host initialization
 *
 * @param     param: point to USBH_InitParam_T structure
 *
 * @retval    None
 */
void USBH_Init(USBH_InitParam_T* param)
{
    g_usbHost.control.maxPackSize = USB_EP0_PACKET_SIZE;

    g_usbHost.classInitHandler = param->classInitHandler;
    g_usbHost.classDeInitHandler = param->classDeInitHandler;
    g_usbHost.classReqHandler = param->classReqHandler;
    g_usbHost.classCoreHandler = param->classCoreHandler;
    g_usbHost.suspendHandler = param->suspendHandler;
    g_usbHost.wakeUpHandler = param->wakeUpHandler;
    g_usbHost.intHandler = param->intHandler;

    g_usbHost.devAddress = USBH_DEVICE_DEFAULT_ADDRESS;

    /* Hardware configuration */
    USB_HardwareInit();

    g_userCallback.initHandler();

    /* USB Global Init */
    USB_GlobalInit();

    /* USB Host Init */
    USB_HostInit();

    /* Enable USB Global interrupt */
    USB_OTG_EnableAllGlobalInterrupt();
}

/*!
 * @brief     USB Host initialization
 *
 * @param     param: point to USBH_InitParam_T structure
 *
 * @retval    None
 */
void USBH_DeInit(void)
{
    USBH_ConfigHostState(USBH_HOST_IDLE);
    USBH_ConfigEnumState(USBH_ENUM_IDLE);

    USBH_ConfigDeviceAddr(USBH_DEVICE_DEFAULT_ADDRESS);

    g_usbHost.control.maxPackSize = USB_EP0_PACKET_SIZE;
    g_usbHost.control.ctrlState = USBH_CTRL_SETUP;
    g_usbHost.control.xferState = USBH_XFER_START;

    USBH_CH_FreeChannel(g_usbHost.control.inChannelNum);
    USBH_CH_FreeChannel(g_usbHost.control.outChannelNum);

    g_usbHost.classDeInitHandler();
    g_userCallback.deInitHandler();

}

/*!
 * @brief     USB Host initialization
 *
 * @param     None
 *
 * @retval    None
 */
void USB_GlobalInit(void)
{
#ifdef USB_OTG_FS
    USB_OTG_ConfigPHY(USB_OTG_PHY_FS);
    /* Reset after a PHY select and set Host mode */
    USB_CoreSoftReset();
    /* Deactivate the power down*/
    USB_OTG_G->GGCCFG_B.PWEN = 1;
#elif (defined OTG_HS_PHY_UTMI)
    USB_OTG_ConfigPHY(USB_OTG_PHY_HS);

    USB_OTG_G->GUSBCFG_B.DPSEL = BIT_RESET;
    USB_OTG_G->GUSBCFG_B.ULPISEL = BIT_RESET;
    USB_OTG_G->GUSBCFG_B.ULPICLKP = BIT_RESET;

    USB_CoreSoftReset();
#endif

#if USB_VBUS_SWITCH
    USB_OTG_G->GGCCFG_B.ADVBSEN = 1;
    USB_OTG_G->GGCCFG_B.BDVBSEN = 1;
    USB_OTG_G->GGCCFG_B.VBSDIS  = 0;
#else
    USB_OTG_G->GGCCFG_B.VBSDIS  = 1;
#endif

#if USB_SOF_OUTPUT_SWITCH
    USB_OTG_G->GGCCFG_B.SOFPOUT = 1;
#endif

    USB_Delay_us(20 * 1000);
}

/*!
 * @brief     USB Host initialization
 *
 * @param     None
 *
 * @retval    None
 */
void USB_HostInit(void)
{
    uint8_t i = 0;

    /* Device mode or host mode */
    USB_OTG_ConfigMode(USB_MODE_HOST);

    /* PHY Clock restart */
    USB_OTG_PWR->PCGCTRL = 0;

    /* Initialize Host Configuration Register */
#ifdef OTG_HS_PHY_ULPI /*!< ULPI PHY */
    USB_OTG_H_ConfigHostClk(PHYCLK_30_60_MHZ);
#else /*!< UTMI PHY (OTG_HS_PHY_UTMI) */
    USB_OTG_H_ConfigHostClk(PHYCLK_48_MHZ);
#endif

    USB_OTG_PortReset();
#ifdef USB_OTG_FS
    USB_Delay_us(200 * 1000);
#endif

    /* Configure data FIFO sizes */
    /* Rx FIFO */
#ifdef USB_OTG_FS
    /* set Rx FIFO size */
    USB_OTG_G->GRXFIFO_B.RXFDEP = USB_FS_RX_FIFO_SIZE;
    USB_OTG_G->GTXFCFG_H_B.NPTXSA = USB_FS_RX_FIFO_SIZE;
    USB_OTG_G->GTXFCFG_H_B.NPTXFDEP = USB_FS_NP_TXH_FIFO_SIZE;

    USB_OTG_G->GHPTXFSIZE_B.HPDTXFSA = USB_FS_RX_FIFO_SIZE + USB_FS_NP_TXH_FIFO_SIZE;
    USB_OTG_G->GHPTXFSIZE_B.HPDTXFDEP = USB_FS_P_TXH_FIFO_SIZE;

#else //!< USB_OTG_HS_CORE
    USB_OTG_G->GRXFIFO_B.RXFDEP = USB_HS_RX_FIFO_SIZE;
    USB_OTG_G->GTXFCFG_H_B.NPTXSA = USB_HS_RX_FIFO_SIZE;
    USB_OTG_G->GTXFCFG_H_B.NPTXFDEP = USB_HS_NP_TXH_FIFO_SIZE;

    USB_OTG_G->GHPTXFSIZE_B.HPDTXFSA = USB_HS_RX_FIFO_SIZE + USB_HS_NP_TXH_FIFO_SIZE;
    USB_OTG_G->GHPTXFSIZE_B.HPDTXFDEP = USB_HS_P_TXH_FIFO_SIZE;
#endif

    /* Flush all Tx FIFOs */
    USB_OTG_FlushTxFIFO(0x10);
    USB_OTG_FlushRxFIFO();

    /* Clear all Channel Interrupts */
    for (i = 0; i < HOST_CHANNEL_NUM; i++)
    {
        USB_OTG_H_ClearChannelIntFlag(i, CHANNEL_INT_ALL);
        USB_OTG_H_DisableChannelInt(i, CHANNEL_INT_ALL);
    }

    USB_OTG_H->HPORTCSTS_B.PP = 1;


    /* Config Global Interrupt */
    USB_OTG_G->GINTMASK = 0;       /*!< Disable all interrupt */
    USB_OTG_G->GINT = 0xFFFFFFFF;  /*!< Clear OTG flag */
    USB_OTG_G->GCINT = 0xFFFFFFFF; /*!< Clear global interrupt flag */

    /* Enable the common interrupts */
    USB_OTG_G->GINTMASK = (USB_INT_G_USBSUS |
                           USB_INT_G_RWAKE  |
                           USB_INT_G_RXFNONE |
                           USB_INT_G_HPORT  |
                           USB_INT_G_HCHAN  |
                           USB_INT_G_IP_OUTTX);

    USB_OTG_G->GINTMASK_B.SOFM = 0;
    USB_OTG_G->GINTMASK_B.DEDISM = 0;
}

/*!
 * @brief       USB Soft reset
 *
 * @param       None
 *
 * @retval      None
 */
void USB_CoreSoftReset(void)
{
    __IO uint32_t timeout = 0;

    /* Wait for AHB master idle */
    while (USB_OTG_G->GRSTCTRL_B.AHBMIDL != BIT_SET)
    {
        if (++timeout > 0x30000)
        {
            return;
        }
    }

    timeout = 0;

    USB_OTG_G->GRSTCTRL_B.CSRST = BIT_SET;

    /* Wait for Reset ok */
    while (USB_OTG_G->GRSTCTRL_B.CSRST != BIT_RESET)
    {
        if (++timeout > 0x30000)
        {
            break;
        }
    }

    USB_Delay_us(3);
}

/**@} end of group Init_Functions */
/**@} end of group Init */
/**@} end of group Standard */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */
