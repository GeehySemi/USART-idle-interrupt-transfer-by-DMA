/*!
 * @file        drv_usb_device.c
 *
 * @brief       This file driver of usb register
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
#include "drv_usb_device.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Driver
  @{
*/

/** @addtogroup USB_Device
  @{
*/

/** @defgroup USB_Device_Functions Functions
  @{
*/

/*!
 * @brief       Active OUT endpoint.
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
void USB_OTG_OutEpActive(uint8_t ep, USB_EP_TYPE_T epType, uint16_t packSize)
{
    if (ep == 0)
    {
        switch (packSize)
        {
            case 64:
                packSize = 0;
                break;
            case 32:
                packSize = 1;
                break;
            case 16:
                packSize = 2;
                break;
            case 8:
                packSize = 3;
                break;
            default:
                packSize = 0;
                break;
        }
    }
    USB_OTG_D->DAEPIMASK_B.AOUTM |= 1 << ep;

    USB_OTG_D->EP_OUT[ep].DOEPCTRL_B.MAXPS = packSize;
    USB_OTG_D->EP_OUT[ep].DOEPCTRL_B.EPTYP = epType;
    USB_OTG_D->EP_OUT[ep].DOEPCTRL_B.USBAEP = BIT_SET;
}

/*!
 * @brief       Deactive OUT endpoint.
 *
 * @param       ep: OUT endpoint Number
 *
 * @retval      None
 */
void USB_OTG_OutEpDeactive(uint8_t ep)
{
    if (USB_OTG_D->EP_OUT[ep].DOEPCTRL_B.EPEN)
    {
        USB_OTG_D->EP_OUT[ep].DOEPCTRL_B.EPDIS = BIT_SET;
        USB_OTG_D->EP_OUT[ep].DOEPCTRL_B.NAKSET = BIT_SET;
    }
    else
    {
        USB_OTG_D->EP_OUT[ep].DOEPCTRL = 0;
    }
}

/*!
 * @brief       Active IN endpoint.
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
 * @param       txFifoNum: endpoint specific txFIFO number.
 *
 * @param       packSize: endpoint max pack size
 *
 * @retval      None
 */
void USB_OTG_InEpActive(uint8_t ep, USB_EP_TYPE_T epType, uint8_t txFifoNum, uint32_t packSize)
{
    if (ep == 0)
    {
        switch (packSize)
        {
            case 64:
                packSize = 0;
                break;
            case 32:
                packSize = 1;
                break;
            case 16:
                packSize = 2;
                break;
            case 8:
                packSize = 3;
                break;
            default:
                packSize = 0;
                break;
        }
    }

    USB_OTG_D->DAEPIMASK_B.AINM |= 1 << ep;

    USB_OTG_D->EP_IN[ep].DIEPCTRL_B.MAXPS  = packSize;
    USB_OTG_D->EP_IN[ep].DIEPCTRL_B.EPTYPE = epType;
    USB_OTG_D->EP_IN[ep].DIEPCTRL_B.TXFNUM = txFifoNum;
    USB_OTG_D->EP_IN[ep].DIEPCTRL_B.USBAEP = BIT_SET;
}

/*!
 * @brief       Deactive IN endpoint.
 *
 * @param       ep: IN endpoint Number
 *
 * @retval      None
 */
void USB_OTG_InEpDeactive(uint8_t ep)
{
    if (USB_OTG_D->EP_IN[ep].DIEPCTRL_B.EPEN)
    {
        USB_OTG_D->EP_IN[ep].DIEPCTRL_B.EPDIS = BIT_SET;
        USB_OTG_D->EP_IN[ep].DIEPCTRL_B.NAKSET = BIT_SET;
    }
    else
    {
        USB_OTG_D->EP_IN[ep].DIEPCTRL = 0;
    }
}
/*!
 * @brief       Read the endpoint active status.
 *
 * @param       ep: IN endpoint number
 *
 * @retval      Return endpoint Active status,
 *              1: Actice
 */
uint8_t USB_OTG_ReadEPActive(uint8_t ep)
{
    return USB_OTG_D->EP_OUT[ep].DOEPCTRL_B.USBAEP;
}

/*!
 * @brief       Stall in endpoint.
 *
 * @param       ep: IN endpoint number
 *
 * @retval      None
 */
void USB_OTG_SetInEpStall(uint8_t ep)
{
    USB_OTG_D->EP_IN[ep].DIEPCTRL_B.STALLH = BIT_SET;
    USB_OTG_D->EP_IN[ep].DIEPCTRL_B.EPDIS = BIT_SET;
}

/*!
 * @brief       Clear in endpoint Stall.
 *
 * @param       ep: IN endpoint
 *
 * @retval      None
 */
void USB_OTG_ClearInEpStall(uint8_t ep)
{
    USB_OTG_D->EP_IN[ep].DIEPCTRL_B.STALLH = BIT_RESET;
}

/*!
 * @brief       Stall out endpoint.
 *
 * @param       ep: OUT endpoint number
 *
 * @retval      None
 */
void USB_OTG_SetOutEpStall(uint8_t ep)
{
    USB_OTG_D->EP_OUT[ep].DOEPCTRL_B.STALLH = BIT_SET;
}

/*!
 * @brief       Clear OUT endpoint Stall.
 *
 * @param       ep: OUT endpoint number
 *
 * @retval      None
 */
void USB_OTG_ClearOutEpStall(uint8_t ep)
{
    USB_OTG_D->EP_OUT[ep].DOEPCTRL_B.STALLH = BIT_RESET;
}

/*!
 * @brief       Stall out endpoint and in endpoint.
 *
 * @param       ep: endpoint number
 *
 * @retval      None
 */
void USB_OTG_SetStall(uint8_t ep)
{
    USB_OTG_SetInEpStall(ep);
    USB_OTG_SetOutEpStall(ep);

    USB_OTG_ReceiveSetupPacket(3);
}

/*!
 * @brief       Clear Stall of out endpoint and in endpoint.
 *
 * @param       ep: endpoint number
 *
 * @retval      None
 */
void USB_OTG_ClearStall(uint8_t ep)
{
    USB_OTG_ClearInEpStall(ep);
    USB_OTG_ClearOutEpStall(ep);

    USB_OTG_ReceiveSetupPacket(3);
}

/*!
 * @brief       Start to receive Setup request packet by RxFIFO
 *
 * @param       packNum: setup pack number to receive.
 *
 * @retval      None
 */
void USB_OTG_ReceiveSetupPacket(uint8_t packNum)
{
    USB_OTG_D->EP_OUT[0].DOEPTRS_B.PID_SPCNT = packNum;
    USB_OTG_D->EP_OUT[0].DOEPTRS_B.EPPCNT = 1;
    USB_OTG_D->EP_OUT[0].DOEPTRS_B.EPTRS = packNum * 8;
}

/*!
 * @brief       Start to receive OUT data by RxFIFO
 *
 * @param       ep: OUT endpoint number
 *
 * @param       packNum: Set pack number
 *
 * @param       transferSize: Set transfer size
 *
 * @retval      None
 */
void USB_OTG_EnableOutEpTransfer(uint8_t ep, uint8_t packNum, uint32_t transferSize)
{
    /* Config EP packet counter and transfer size */
    USB_OTG_D->EP_OUT[ep].DOEPTRS_B.EPPCNT = packNum;
    USB_OTG_D->EP_OUT[ep].DOEPTRS_B.EPTRS = transferSize;

    /* Enable OUT EP and Clear NAK */
    USB_OTG_D->EP_OUT[ep].DOEPCTRL_B.NAKCLR = BIT_SET;
    USB_OTG_D->EP_OUT[ep].DOEPCTRL_B.EPEN = BIT_SET;
}

/*!
 * @brief       Start to Send data through TxFIFO
 *
 * @param       ep: IN endpoint number
 *
 * @param       packNum: Set pack number
 *
 * @param       transferSize: Set transfer size
 *
 * @retval      None
 */
void USB_OTG_EnableInEpTransfer(uint8_t ep, uint8_t packNum, uint32_t transferSize)
{
    /* Config EP packet counter and transfer size */
    USB_OTG_D->EP_IN[ep].DIEPTRS_B.EPPCNT = packNum;
    USB_OTG_D->EP_IN[ep].DIEPTRS_B.EPTRS = transferSize;

    /* Enable IN EP and Clear NAK */
    USB_OTG_D->EP_IN[ep].DIEPCTRL_B.NAKCLR = BIT_SET;
    USB_OTG_D->EP_IN[ep].DIEPCTRL_B.EPEN = BIT_SET;
}

/*!
 * @brief     Config Depth and start address of TxFifo
 *
 * @param     ep : IN EP number
 *
 * @param     address : start address of TxFIFO
 *
 * @param     depth : depth of TxFIFO
 *
 * @retval    None
 */
void USB_OTG_ConfigDeviceTxFifo(uint8_t ep, uint16_t address, uint16_t depth)
{
    uint32_t txFifoConfig = (((uint32_t)depth) << 16) | ((uint32_t)address);

    if (ep)
    {
        USB_OTG_G->DTXFIFO[ep - 1].word = txFifoConfig;
    }
    else
    {
        USB_OTG_G->GTXFCFG = txFifoConfig;
    }
}

/*!
 * @brief       Read the out endpoint interrupt status
 *
 * @param       ep : out endpiont number
 *
 * @param       flag : out ep interrupt flag.
 *                     This parameter can be one of the following value:
 *                     @arg USB_INT_EP_OUT_TSFCMP : Transfer completed
 *                     @arg USB_INT_EP_OUT_EPDIS  : Endpoint disabled
 *                     @arg USB_INT_EP_OUT_SETPCMP: setup completed
 *                     @arg USB_INT_EP_OUT_RXOTDIS: OUT token received when endpoint disabled
 *                     @arg USB_INT_EP_OUT_RXBSP  : Received Back-to-back SETUP packets over 3
 *
 * @retval      return Out endpoint interrupt status
 */
uint8_t USB_OTG_ReadOutEpIntStatus(uint8_t ep, USB_INT_EP_OUT_T flag)
{
    uint8_t status = RESET;

    if ((((uint8_t)USB_OTG_D->EP_OUT[ep].DOEPINT) & flag) != RESET)
    {
        status = SET;
    }
    else
    {
        status = RESET;
    }

    return status;
}

/*!
 * @brief       Read the IN endpoint interrupt status
 *
 * @param       ep : IN endpiont number
 *
 * @param       flag : IN ep interrupt flag.
 *                     This parameter can be one of the following value:
 *                     @arg USB_INT_EP_IN_TSFCMP : Transfer completed
 *                     @arg USB_INT_EP_IN_EPDIS  : Endpoint disabled
 *                     @arg USB_INT_EP_IN_TO     : Timeout
 *                     @arg USB_INT_EP_IN_ITXEMP : IN token received when TxFIFO is empty
 *                     @arg USB_INT_EP_IN_IEPNAKE: IN endpoint NAK effective
 *                     @arg USB_INT_EP_IN_TXFE   : TxFIFO empty
 *
 * @retval      Return IN endpoint interrupt status
 */
uint8_t USB_OTG_ReadInEpIntStatus(uint8_t ep, USB_INT_EP_IN_T flag)
{
    return (((uint8_t)USB_OTG_D->EP_IN[ep].DIEPINT) & flag) ? SET : RESET;
}

/**@} end of group USB_Device_Functions */
/**@} end of group USB_Device */
/**@} end of group Driver */
/**@} end of group USB_OTG_Library */
