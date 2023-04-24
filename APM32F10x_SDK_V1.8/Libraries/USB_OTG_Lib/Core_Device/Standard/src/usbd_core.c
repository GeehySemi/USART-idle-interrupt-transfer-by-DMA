/*!
 * @file        usbd_core.c
 *
 * @brief       USB device core function
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
#include "usbd_core.h"
#include "usbd_stdReq.h"
#include "usb_fifo.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Device Core Device
  @{
*/

/** @addtogroup Standrad
  @{
*/

/** @addtogroup Core
  @{
*/

/** @defgroup Core_Variables Variables
  @{
*/

/* USB information */
USBD_Info_T g_usbDev;

/**@} end of group Core_Variables */

/** @defgroup Core_Functions Functions
  @{
*/

/*!
 * @brief       Setup process
 *
 * @param       None
 *
 * @retval      None
 */
void USBD_SetupProcess(void)
{
    uint8_t reqType;

    reqType = g_usbDev.reqData.domain.bmRequestType.bit.type;

    if (reqType == USBD_REQ_TYPE_STANDARD)
    {
        USBD_StandardReqeust();
    }
    else if (reqType == USBD_REQ_TYPE_CLASS)
    {
        g_usbDev.classReqHandler(&g_usbDev.reqData);
    }
    else if (reqType == USBD_REQ_TYPE_VENDOR)
    {
        g_usbDev.vendorReqHandler(&g_usbDev.reqData);
    }
    else
    {
        USB_OTG_SetStall(USB_EP_0);
    }
}

/*!
 * @brief       OUT process for EP0
 *
 * @param       None
 *
 * @retval      None
 */
void USBD_CtrlOutProcess(void)
{
    uint16_t maxPackSize = g_usbDev.outBuf[USB_EP_0].maxPackSize;

    if (g_usbDev.ctrlState == USBD_CTRL_STATE_OUT_DATA)
    {
        /* Deal with OUT transfer */
        if (g_usbDev.outBuf[USB_EP_0].packNum)
        {
            /* Start OUT transfer */
            USB_OTG_EnableOutEpTransfer(USB_EP_0, 1, maxPackSize);
        }

        /* End of OUT transfer */
        else
        {
            USBD_CtrlTxStatus();
        }
    }
}

/*!
 * @brief       IN process
 *
 * @param       None
 *
 * @retval      None
 */
void USBD_CtrlInProcess(void)
{
    uint32_t len;
    uint16_t maxPackSize = g_usbDev.inBuf[USB_EP_0].maxPackSize;

    if (g_usbDev.ctrlState == USBD_CTRL_STATE_IN_DATA)
    {
        if (g_usbDev.inBuf[USB_EP_0].packNum)
        {
            len = USB_MIN(g_usbDev.inBuf[USB_EP_0].bufLen, maxPackSize);

            /* Enable IN transfer */
            USB_OTG_EnableInEpTransfer(USB_EP_0, 1, len);

            /* Enable IN EP TxFIFO Empty interrupt */
            USB_OTG_EnableInEpEmptyInterrupt(USB_EP_0);
        }

        /* End of the IN transfer */
        else
        {
            if (g_usbDev.inBuf[USB_EP_0].zeroPackFill)
            {
                USB_OTG_EnableInEpTransfer(USB_EP_0, 1, 0);

                g_usbDev.inBuf[USB_EP_0].zeroPackFill = 0;
            }
            else
            {
                /* Receive OUT Status */
                USBD_CtrlRxStatus();
            }
        }
    }
}

/*!
 * @brief       Config the buffer to receive OUT transaction data and start to transfer.
 *
 * @param       buf:    Buffer pointer
 *
 * @param       len:    Buffer length
 *
 * @retval      None
 */
void USBD_CtrlOutData(uint8_t* buf, uint32_t len)
{
    uint16_t maxPackSize = g_usbDev.outBuf[USB_EP_0].maxPackSize;

    /* Init Buffer to Receive */
    g_usbDev.outBuf[USB_EP_0].pBuf = buf;
    g_usbDev.outBuf[USB_EP_0].bufLen = len;
    g_usbDev.outBuf[USB_EP_0].packNum = (len + (maxPackSize - 1)) / maxPackSize;

    /* Enable OUT Transfer */
    USB_OTG_EnableOutEpTransfer(USB_EP_0, 1, maxPackSize);

    g_usbDev.ctrlState = USBD_CTRL_STATE_OUT_DATA;
}

/*!
 * @brief       Config the buffer for IN transaction and start to transfer.
 *
 * @param       buf:    Buffer pointer
 *
 * @param       len:    Buffer length
 *
 * @retval      None
 */
void USBD_CtrlInData(uint8_t* buf, uint32_t len)
{
    uint16_t maxPackSize = g_usbDev.inBuf[USB_EP_0].maxPackSize;
    uint16_t reqLen = ((uint16_t)g_usbDev.reqData.domain.wLength[1] << 8) | \
                      (g_usbDev.reqData.domain.wLength[0]);

    /* Init Buffer to Send */
    g_usbDev.inBuf[USB_EP_0].pBuf = buf;
    g_usbDev.inBuf[USB_EP_0].bufLen = len;
    g_usbDev.inBuf[USB_EP_0].packNum = (len + (maxPackSize - 1)) / maxPackSize;

    if ((len < reqLen) && ((len % maxPackSize) == 0))
    {
        g_usbDev.inBuf[USB_EP_0].zeroPackFill = 1;
    }

    if (len)
    {
        len = USB_MIN(g_usbDev.inBuf[USB_EP_0].bufLen, maxPackSize);
        /* Enable IN Transfer */
        USB_OTG_EnableInEpTransfer(USB_EP_0, 1, len);

        /* Enable TxFIFO empty interrupt */
        USB_OTG_EnableInEpEmptyInterrupt(USB_EP_0);

        g_usbDev.ctrlState = USBD_CTRL_STATE_IN_DATA;
    }
    /* Zero packet */
    else
    {
        USBD_CtrlTxStatus();
    }
}

/*!
 * @brief       Receive OUT status in control transfer.
 *
 * @param       None
 *
 * @retval      None
 */
void USBD_CtrlRxStatus(void)
{
    if (g_usbDev.rxStatusHandler)
    {
        g_usbDev.rxStatusHandler();
    }

    USB_OTG_EnableOutEpTransfer(USB_EP_0, 1, g_usbDev.inBuf[USB_EP_0].maxPackSize);

    /* Start to receive setup packet */
    USB_OTG_ReceiveSetupPacket(3);
}

/*!
 * @brief       Send IN status in control transfer.
 *
 * @param       None
 *
 * @retval      None
 */
void USBD_CtrlTxStatus(void)
{
    if (g_usbDev.txStatusHandler)
    {
        g_usbDev.txStatusHandler();
    }

    USB_OTG_EnableInEpTransfer(USB_EP_0, 1, 0);

    /* Start to receive setup packet */
    USB_OTG_ReceiveSetupPacket(3);
}

/*!
 * @brief       Receive OUT data.
 *
 * @param       ep : except EP0
 *
 * @param       buf: Buffer pointer
 *
 * @param       len: Buffer length
 *
 * @retval      None
 */
void USBD_RxData(uint8_t ep, uint8_t* buf, uint32_t len)
{
    uint16_t maxPackSize = g_usbDev.outBuf[ep].maxPackSize;

    g_usbDev.outBuf[ep].pBuf = buf;
    g_usbDev.outBuf[ep].bufLen = len;
    g_usbDev.outBuf[ep].packNum = (len + (maxPackSize - 1)) / maxPackSize;

    /* Enable OUT transfer */
    USB_OTG_EnableOutEpTransfer(ep, g_usbDev.outBuf[ep].packNum,
                                maxPackSize * g_usbDev.outBuf[ep].packNum);
}

/*!
 * @brief       Send IN data.
 *
 * @param       ep : except EP0
 *
 * @param       buf: Buffer pointer
 *
 * @param       len: Buffer length
 *
 * @retval      None
 */
void USBD_TxData(uint8_t ep, uint8_t* buf, uint32_t len)
{
    uint16_t maxPackSize = g_usbDev.inBuf[ep].maxPackSize;

    g_usbDev.inBuf[ep].pBuf = buf;
    g_usbDev.inBuf[ep].bufLen = len;
    g_usbDev.inBuf[ep].packNum = (len + (maxPackSize - 1)) / maxPackSize;;

    USB_OTG_EnableInEpTransfer(ep, g_usbDev.inBuf[ep].packNum, len);

    USB_OTG_EnableInEpEmptyInterrupt(ep);
}

/*!
 * @brief       Push data to TxFIFO
 *
 * @param       ep : EP number for TxFIFO
 *
 * @retval      None
 */
void USBD_PushDataToTxFIFO(uint8_t ep)
{
    uint32_t txFifoSpace;
    uint32_t len;
    uint16_t maxPackSize = g_usbDev.inBuf[ep].maxPackSize;

    if ((!g_usbDev.inBuf[ep].bufLen) || (g_usbDev.inBuf[ep].pBuf == NULL))
    {
        return;
    }

    len = USB_MIN(g_usbDev.inBuf[ep].bufLen, maxPackSize);

    /* Check the Available space of TxFifo */
    txFifoSpace = 4 * USB_OTG_ReadTxFifoAvailable(ep);

    /* Push data to TxFIFO */
    if (txFifoSpace >= len)
    {
        USB_FIFO_WriteFifoPacket(ep, g_usbDev.inBuf[ep].pBuf, len);

        g_usbDev.inBuf[ep].pBuf += len;
        g_usbDev.inBuf[ep].bufLen -= len;
        g_usbDev.inBuf[ep].packNum--;

        USB_OTG_DisableInEpEmptyInterrupt(ep);
    }
}

/**@} end of group Core_Functions */
/**@} end of group Core */
/**@} end of group Standard */
/**@} end of group Core_Device */
/**@} end of group USB_OTG_Library */
