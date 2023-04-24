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
#include "usbd_init.h"
#include "usb_board.h"
#include "usb_user.h"
#include "usbd_descriptor.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Device Core Device
  @{
*/

/** @addtogroup Standrad
  @{
*/

/** @addtogroup Init
  @{
*/

/** @defgroup Init_Functions Functions
  @{
*/

static void USB_GlobalInit(void);
static void USB_DeviceInit(void);

/*!
 * @brief       USB initialization
 *
 * @param       param: Initialization parameter
 *
 * @retval      None
 */
void USBD_Init(USBD_InitParam_T* param)
{
    /* User Configuration */
    g_usbDev.configurationNum = USBD_CONFIGURATION_NUM;

    g_usbDev.pStdReqCallback = param->pStdReqCallback;
    g_usbDev.classReqHandler = param->classReqHandler;
    g_usbDev.vendorReqHandler = param->vendorReqHandler;
    g_usbDev.stdReqExceptionHandler = param->stdReqExceptionHandler;

    g_usbDev.resetHandler = param->resetHandler;
    g_usbDev.outEpHandler = param->outEpHandler;
    g_usbDev.inEpHandler  = param->inEpHandler;
    g_usbDev.intHandler = param->intHandler;

    g_usbDev.pDeviceDesc = param->pDeviceDesc;
    g_usbDev.pConfigurationDesc = param->pConfigurationDesc;
    g_usbDev.pStringDesc = param->pStringDesc;
    g_usbDev.pQualifierDesc = param->pQualifierDesc;

    g_usbDev.txStatusHandler = param->txStatusHandler;
    g_usbDev.rxStatusHandler = param->rxStatusHandler;

    /* Config Serial string descriptor */
    USBD_ConfigSerialString(g_usbDev.pStringDesc[g_usbDev.pDeviceDesc->pDesc[16]].pDesc);

    /* Hardware configuration */
    USB_HardwareInit();

    /* USB Global Init */
    USB_GlobalInit();

    /* USB Device Init */
    USB_DeviceInit();

    /* Enable USB Global interrupt */
    USB_OTG_EnableAllGlobalInterrupt();

    /* Disable Soft disconnect */
    USB_OTG_D->DCTRL_B.SDCNNT = BIT_RESET;
}

/*!
 * @brief       Init parameter in param
 *
 * @param       param: Initialization parameter
 *
 * @retval      None
 */
void USBD_InitParamStructInit(USBD_InitParam_T* param)
{
    param->pStdReqCallback = NULL;
    param->stdReqExceptionHandler = NULL;
    param->classReqHandler = NULL;
    param->vendorReqHandler = NULL;

    param->txStatusHandler = NULL;
    param->rxStatusHandler = NULL;

    param->outEpHandler = NULL;
    param->inEpHandler = NULL;

    param->resetHandler = NULL;
    param->intHandler = NULL;
}


/*!
 * @brief       USB Global Init
 *
 * @param       None
 *
 * @retval      None
 */
static void USB_GlobalInit(void)
{
#ifdef USB_OTG_FS
    /* FS PHY */
    USB_OTG_ConfigPHY(USB_OTG_PHY_FS);

    USB_CoreSoftReset();

    /* Stop power down */
    USB_OTG_DisablePowerDown();

#elif (defined OTG_HS_PHY_UTMI)
    USB_OTG_ConfigPHY(USB_OTG_PHY_HS);

    USB_CoreSoftReset();

    USB_OTG_G->GUSBCFG_B.DPSEL = BIT_RESET;
    USB_OTG_G->GUSBCFG_B.ULPISEL = BIT_RESET;
    USB_OTG_G->GUSBCFG_B.ULPICLKP = BIT_RESET;
#else /*!< OTG_HS_PHY_ULPI */
    USB_OTG_EnablePowerDown();

    USB_OTG_ConfigPHY(USB_OTG_PHY_HS);

    USB_OTG_G->GUSBCFG_B.DPSEL = BIT_RESET;
    USB_OTG_G->GUSBCFG_B.ULPISEL = BIT_RESET;
    USB_OTG_G->GUSBCFG_B.ULPICLKP = BIT_RESET;

    USB_CoreSoftReset();
#endif

    /* Config USB Vbus sensing */
#if USB_VBUS_SWITCH
    USB_OTG_EnableDeviceVbus(USB_DEVICE_A);
    USB_OTG_EnableDeviceVbus(USB_DEVICE_B);
    USB_OTG_EnableVbusSensing();
#else
    USB_OTG_DisableVbusSensing();
#endif

    /* SOF output */
#if USB_SOF_OUTPUT_SWITCH
    USB_OTG_EnableSofOutput();
#else
    USB_OTG_DisableSofOutput();
#endif

    /* Config Global Interrupt */
    USB_OTG_DisableGlobalInterrupt(USB_INT_G_ALL); /*!< Disable all interrupt */
    USB_OTG_ClearOTGIntFlag(USB_INT_OTG_ALL);      /*!< Clear OTG flag */
    USB_OTG_ClearGlobalIntFlag(USB_INT_G_ALL);     /*!< Clear global interrupt flag */

    USB_OTG_EnableGlobalInterrupt(USB_INT_G_SOURCE);
}

/*!
 * @brief       USB Device Init
 *
 * @param       None
 *
 * @retval      None
 */
static void USB_DeviceInit(void)
{
    /* Device mode or host mode */
    USB_OTG_ConfigMode(USB_MODE_DEVICE);

    /* PHY Clock restart */
    USB_OTG_PWR->PCGCTRL = 0;

    /* Periodic frame interval */
    USB_OTG_ConfigPeriodicFrameInterval(USB_FRAME_INTERVAL_80);

#ifdef USB_OTG_FS
    /* Device Speed */
    USB_OTG_ConfigDeviceSpeed(USB_DEV_SPEED_FS);

    /* RxFIFO Configuration */
    USB_OTG_ConfigRxFifoDepth(USB_FS_RX_FIFO_SIZE);

    uint16_t txFifoAddr[4];
    uint16_t txFifoDepth[4] =
    {
        USB_FS_TX_FIFO_0_SIZE, USB_FS_TX_FIFO_1_SIZE,
        USB_FS_TX_FIFO_2_SIZE, USB_FS_TX_FIFO_3_SIZE
    };

    txFifoAddr[0] = USB_FS_RX_FIFO_SIZE;
    txFifoAddr[1] = txFifoAddr[0] + USB_FS_TX_FIFO_0_SIZE;
    txFifoAddr[2] = txFifoAddr[1] + USB_FS_TX_FIFO_1_SIZE;
    txFifoAddr[3] = txFifoAddr[2] + USB_FS_TX_FIFO_2_SIZE;

    /* TxFIFO Configuration */
    for (uint8_t ep = 0; ep < USB_IN_EP_MAX_NUM; ep++)
    {
        USB_OTG_ConfigDeviceTxFifo(ep, txFifoAddr[ep], txFifoDepth[ep]);
    }
#else /* HS */
    /* Device Speed */
    USB_OTG_ConfigDeviceSpeed(USB_DEV_SPEED_HS);

    /* RxFIFO Configuration */
    USB_OTG_ConfigRxFifoDepth(USB_HS_RX_FIFO_SIZE);

    uint16_t txFifoAddr[8];
    uint16_t txFifoDepth[8] =
    {
        USB_HS_TX_FIFO_0_SIZE, USB_HS_TX_FIFO_1_SIZE,
        USB_HS_TX_FIFO_2_SIZE, USB_HS_TX_FIFO_3_SIZE,
        USB_HS_TX_FIFO_4_SIZE, USB_HS_TX_FIFO_5_SIZE,
        USB_HS_TX_FIFO_6_SIZE, USB_HS_TX_FIFO_7_SIZE
    };

    txFifoAddr[0] = USB_HS_RX_FIFO_SIZE;
    txFifoAddr[1] = txFifoAddr[0] + USB_HS_TX_FIFO_0_SIZE;
    txFifoAddr[2] = txFifoAddr[1] + USB_HS_TX_FIFO_1_SIZE;
    txFifoAddr[3] = txFifoAddr[2] + USB_HS_TX_FIFO_2_SIZE;
    txFifoAddr[4] = txFifoAddr[3] + USB_HS_TX_FIFO_3_SIZE;
    txFifoAddr[5] = txFifoAddr[4] + USB_HS_TX_FIFO_4_SIZE;
    txFifoAddr[6] = txFifoAddr[5] + USB_HS_TX_FIFO_5_SIZE;
    txFifoAddr[7] = txFifoAddr[6] + USB_HS_TX_FIFO_6_SIZE;

    /* TxFIFO Configuration */
    for (uint8_t ep = 0; ep < 8; ep++)
    {
        USB_OTG_ConfigDeviceTxFifo(ep, txFifoAddr[ep], txFifoDepth[ep]);
    }
#endif
    /* Flush TxFIFO */
    USB_OTG_FlushTxFIFO(0x10);

    /* Flush RxFIFO */
    USB_OTG_FlushRxFIFO();

    USB_OTG_DisableOutEpInterrupt(USB_INT_EP_OUT_ALL);
    USB_OTG_DisableInEpInterrupt(USB_INT_EP_IN_ALL);

    USB_OTG_DisableOutEpInterruptMask(USB_EP_0);
    USB_OTG_D->DAEPIMASK_B.AOUTM = 0;

    uint8_t i;

    /* Init endpoint */
    for (i = 0; i < USB_MAX(USB_OUT_EP_MAX_NUM, USB_IN_EP_MAX_NUM); i++)
    {
        /* Reset OUT endpoint */
        if (USB_OTG_D->EP_OUT[i].DOEPCTRL_B.EPEN)
        {
            USB_OTG_D->EP_OUT[i].DOEPCTRL_B.EPDIS = BIT_SET;
            USB_OTG_D->EP_OUT[i].DOEPCTRL_B.NAKSET = BIT_SET;
        }
        else
        {
            USB_OTG_D->EP_OUT[i].DOEPCTRL = 0;
        }

        /* Reset IN endpoint */
        if (USB_OTG_D->EP_IN[i].DIEPCTRL_B.EPEN)
        {
            USB_OTG_D->EP_IN[i].DIEPCTRL_B.EPDIS = BIT_SET;
            USB_OTG_D->EP_IN[i].DIEPCTRL_B.NAKSET = BIT_SET;
        }
        else
        {
            USB_OTG_D->EP_IN[i].DIEPCTRL = 0;
        }

        /* Reset transfer size */
        USB_OTG_D->EP_OUT[i].DOEPTRS = 0;
        USB_OTG_D->EP_IN[i].DIEPTRS = 0;

        /* Clear endpoint Flag */
        USB_OTG_D->EP_OUT[i].DOEPINT = 0xFF;
        USB_OTG_D->EP_IN[i].DIEPINT = 0xFF;
    }
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
    while (USB_OTG_ReadAHBMasterIdleFlag() != BIT_SET)
    {
        if (++timeout > 0x30000)
        {
            return;
        }
    }

    timeout = 0;

    USB_OTG_EnableCoreSoftReset();

    /* Wait for Reset ok */
    while (USB_OTG_ReadCoreSoftResetFlag() != BIT_RESET)
    {
        if (++timeout > 0x30000)
        {
            break;
        }
    }

    USB_Delay_us(3);
}

/*!
 * @brief       Open OUT endpoint.
 *
 * @param       ep: OUT endpoint number
 *
 * @param       epType: OUT endpoint type.
 *                     This parameter can be one of the following value:
 *                     @arg USB_EP_TYPE_CONTROL   : Control endpoint
 *                     @arg USB_EP_TYPE_ISO       : Isochronous endpoint
 *                     @arg USB_EP_TYPE_BULK      : Bulk endpoint
 *                     @arg USB_EP_TYPE_INTERRUPT : Interrupt endpoint
 *
 * @param       packSize: ep max pack size
 *
 * @retval      None
 */
void USBD_OpenOutEP(uint8_t ep, USB_EP_TYPE_T epType, uint32_t packSize)
{
    g_usbDev.outBuf[ep].maxPackSize = packSize;
    USB_OTG_OutEpActive(ep, epType, packSize);
}

/*!
 * @brief       Open IN endpoint.
 *
 * @param       ep: IN endpoint number
 *
 * @param       epType: IN endpoint type.
 *                     This parameter can be one of the following value:
 *                     @arg USB_EP_TYPE_CONTROL   : Control endpoint
 *                     @arg USB_EP_TYPE_ISO       : Isochronous endpoint
 *                     @arg USB_EP_TYPE_BULK      : Bulk endpoint
 *                     @arg USB_EP_TYPE_INTERRUPT : Interrupt endpoint
 *
 * @param       packSize: endpoint max pack size
 *
 * @retval      None
 */
void USBD_OpenInEP(uint8_t ep, USB_EP_TYPE_T epType, uint32_t packSize)
{
    uint8_t txFifoNum = ep;

    g_usbDev.inBuf[ep].maxPackSize = packSize;
    USB_OTG_InEpActive(ep, epType, txFifoNum, packSize);
}

/*!
 * @brief       Close OUT endpoint.
 *
 * @param       ep: OUT endpoint Number
 *
 * @retval      None
 */
void USBD_CloseOutEP(uint8_t ep)
{
    g_usbDev.outBuf[ep].maxPackSize = 0;

    USB_OTG_OutEpDeactive(ep);
}

/*!
 * @brief       Close IN endpoint.
 *
 * @param       ep: IN endpoint Number
 *
 * @retval      None
 */
void USBD_CloseInEP(uint8_t ep)
{
    g_usbDev.inBuf[ep].maxPackSize = 0;

    USB_OTG_InEpDeactive(ep);
}

/**@} end of group Init_Functions */
/**@} end of group Init */
/**@} end of group Standard */
/**@} end of group Init_Device */
/**@} end of group USB_OTG_Library */
