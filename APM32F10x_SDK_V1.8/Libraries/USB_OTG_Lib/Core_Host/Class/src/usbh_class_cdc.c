/*!
 * @file        usbh_class_cdc.c
 *
 * @brief       cdc class handler
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
#include "usbh_class_cdc.h"
#include "usbh_dataXfer.h"
#include "usbh_channel.h"
#include "usbh_cdc.h"
#include "usb_board.h"
#include "usb_user.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Host Core Host
  @{
*/

/** @addtogroup Class
  @{
*/

/** @addtogroup CDC
  @{
*/

/** @defgroup CDC_Variables Variables
  @{
*/

CDC_Xfer_T CDC_Tx;
CDC_Xfer_T CDC_Rx;

uint8_t txBuf [CDC_BUFFER_SIZE];
uint8_t rxBuf [CDC_BUFFER_SIZE];

uint8_t cdcDataInBuff[8];
uint8_t cdcDataOutBuff[64] = "This is a CDC Test.\n";

uint8_t flagEnableGet = 0;
uint8_t flagEnableSend = 0;

uint8_t setLineCode[7] = {0x00, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x08};
uint8_t getLineCode[7];
USBH_CDC_REQ_STATE_T s_classReqState = CDC_REQ_SET_LINE_CODING;

/**@} end of group CDC_Variables */

/** @defgroup CDC_Functions Functions
  @{
*/
static void CDC_REQ_SetLineCoding(void);
static void CDC_REQ_GetLineCoding(void);
static void CDC_REQ_SetControlLineState(void);

/**@} end of group CDC_Functions */

/** @defgroup CDC_Variables Variables
  @{
*/

/* USB Host CDC Request Handler function */
CDC_REQ_Handler_T CDC_REQ_Handler[] =
{
    CDC_REQ_SetLineCoding,
    CDC_REQ_GetLineCoding,
    CDC_REQ_SetControlLineState,
};

/**@} end of group CDC_Variables */

/** @defgroup CDC_Functions Functions
  @{
*/
/*!
 * @brief     USB Host Class Initialization.
 *
 * @param     None
 *
 * @retval    None
 */
void USBH_CDC_ClassInitHandler(void)
{
    uint8_t itfNum, epNum;

    /* Initialize cdc buff */
    CDC_Tx.cdcState = USBH_CDC_IDIE;
    CDC_Tx.dataLen = 0;
    CDC_Tx.buff = txBuf;

    CDC_Rx.cdcState = USBH_CDC_IDIE;
    CDC_Rx.dataLen = 0;
    CDC_Rx.buff = rxBuf;
    CDC_Rx.pFillBuff = CDC_Rx.buff;
    CDC_Rx.pEmptyBuff = CDC_Rx.buff;
    CDC_Rx.buffLen = CDC_BUFFER_SIZE;

    /* Prepare to Handler class request */
    g_usbHost.preHostState = USBH_HOST_CLASS_REQ;

    itfNum = USBH_ReadConfigurationItfNum(&g_usbHost);

    while (itfNum--)
    {
        if (USBH_ReadInterfaceClass(&g_usbHost, itfNum) != USBH_CLASS_CDCC)
        {
            if (USBH_ReadInterfaceClass(&g_usbHost, itfNum) != USBH_CLASS_CDCD)
            {
                g_userCallback.deviceNotSupportedHandler();
                continue;
            }
        }

        epNum = USBH_ReadInterfaceEpNum(&g_usbHost, itfNum);
        while (epNum--)
        {
            if (USBH_ReadEndpointType(&g_usbHost, itfNum, epNum) == USB_EP_TYPE_BULK)
            {
                /* Get endpoint and size */
                if (USBH_ReadEndpointAddress(&g_usbHost, itfNum, epNum) & 0x80)
                {
                    CDC_Rx.chNum = USBH_CH_AllocChannel(USBH_ReadEndpointAddress(&g_usbHost, itfNum, epNum));
                    CDC_Rx.maxPackSize = USBH_ReadEndpointMPS(&g_usbHost, itfNum, epNum);

                    /* Open and Config data in channel */
                    USBH_CH_OpenChannel(CDC_Rx.chNum,
                                        USBH_DEVICE_CONFIGURED_ADDRESS,
                                        USB_EP_TYPE_BULK,
                                        CDC_Rx.maxPackSize);
                }
                else
                {
                    CDC_Tx.chNum = USBH_CH_AllocChannel(USBH_ReadEndpointAddress(&g_usbHost, itfNum, epNum));
                    CDC_Tx.maxPackSize = USBH_ReadEndpointMPS(&g_usbHost, itfNum, epNum);

                    /* Open and Config data in channel */
                    USBH_CH_OpenChannel(CDC_Tx.chNum,
                                        USBH_DEVICE_CONFIGURED_ADDRESS,
                                        USB_EP_TYPE_BULK,
                                        CDC_Tx.maxPackSize);
                }
            }
        }
    }

    /* Go to Class request handler */
    g_usbHost.curHostState = USBH_HOST_CLASS_REQ;

    CDC_EnableGet();
}
/*!
 * @brief     USB Host Class Re-Initialization.
 *
 * @param     None
 *
 * @retval    None
 */
void USBH_CDC_ClassDeInitHandler(void)
{

    /* Initialize cdc buff */
    CDC_Tx.cdcState = USBH_CDC_IDIE;
    CDC_Tx.dataLen = 0;
    CDC_Tx.buff = txBuf;

    CDC_Rx.cdcState = USBH_CDC_IDIE;
    CDC_Rx.dataLen = 0;
    CDC_Rx.buff = rxBuf;
}

/*!
 * @brief     Class request handler
 *
 * @param     None
 *
 * @retval    None
 */
void USBH_CDC_ClassReqHandler(void)
{
    if (s_classReqState != CDC_REQ_CONFIGURED_OK)
    {
        /* Hander Class request function */
        CDC_REQ_Handler[s_classReqState]();
    }
    else
    {
        s_classReqState = CDC_REQ_SET_LINE_CODING;
        /* Prepare to Handler class */
        g_usbHost.curHostState = USBH_HOST_CLASS;
    }
}

/*!
 * @brief     CDC Class handler
 *
 * @param     None
 *
 * @retval    None
 */
void USBH_CDC_CoreHandler(void)
{
    g_userCallback.applicationHandler();

}

/*!
 * @brief     This Class request to config the line coding
 *
 * @param     None
 *
 * @retval    None
 */
static void CDC_REQ_SetLineCoding(void)
{
    USBH_CTRL_STATE_T   xferCtrlState;

    if (g_usbHost.control.xferState == USBH_XFER_START)
    {

        g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_OUT;
        g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_INTERFACE;
        g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_CLASS;

        g_usbHost.control.reqData.domain.bRequest = 0x20;

        g_usbHost.control.reqData.domain.wValue[0] = 0x00;
        g_usbHost.control.reqData.domain.wValue[1] = 0x00;

        g_usbHost.control.reqData.domain.wIndex[0] = 0x00;
        g_usbHost.control.reqData.domain.wIndex[1] = 0x00;

        g_usbHost.control.reqData.domain.wLength[0] = 0x07;
        g_usbHost.control.reqData.domain.wLength[1] = 0x00;

        g_usbHost.control.ctrlState = USBH_CTRL_SETUP;

        g_usbHost.control.xferState = USBH_XFER_WAITING;
    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        /* Polling transaction handler */
        xferCtrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
                                                (uint8_t*)&setLineCode);
        switch (xferCtrlState)
        {
            case USBH_CTRL_COMPLETE:
                g_usbHost.control.xferState = USBH_XFER_OK;
                break;
            case USBH_CTRL_STALL :
            case USBH_CTRL_ERROR :
                g_usbHost.control.xferState = USBH_XFER_START;
                break;
            default :
                break;
        }
    }
    /* If config the line coding OK */
    else if (g_usbHost.control.xferState == USBH_XFER_OK)
    {
        g_usbHost.control.xferState = USBH_XFER_START;
        /* Prepare to get the currently configured line coding */
        s_classReqState = CDC_REQ_GET_LINE_CODING;
    }
}

/*!
 * @brief     This Class request to get the currently configured line coding
 *
 * @param     None
 *
 * @retval    None
 */
static void CDC_REQ_GetLineCoding(void)
{
    USBH_CTRL_STATE_T   xferCtrlState;

    if (g_usbHost.control.xferState == USBH_XFER_START)
    {

        g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_IN;
        g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_INTERFACE;
        g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_CLASS;

        g_usbHost.control.reqData.domain.bRequest = 0x21;

        g_usbHost.control.reqData.domain.wValue[0] = 0x00;
        g_usbHost.control.reqData.domain.wValue[1] = 0x00;

        g_usbHost.control.reqData.domain.wIndex[0] = 0x00;
        g_usbHost.control.reqData.domain.wIndex[1] = 0x00;

        g_usbHost.control.reqData.domain.wLength[0] = 0x07;
        g_usbHost.control.reqData.domain.wLength[1] = 0x00;

        g_usbHost.control.ctrlState = USBH_CTRL_SETUP;


        g_usbHost.control.xferState = USBH_XFER_WAITING;
    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        /* Polling transaction handler */
        xferCtrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
                                                (uint8_t*)&getLineCode);
        switch (xferCtrlState)
        {
            case USBH_CTRL_COMPLETE:
                g_usbHost.control.xferState = USBH_XFER_OK;
                break;
            case USBH_CTRL_STALL :
            case USBH_CTRL_ERROR :
                g_usbHost.control.xferState = USBH_XFER_START;
                break;
            default :
                break;
        }
    }
    /* If get the line coding OK */
    else if (g_usbHost.control.xferState == USBH_XFER_OK)
    {
        g_usbHost.control.xferState = USBH_XFER_START;
        /* Prepare to set line state */
        s_classReqState = CDC_SET_CONTROL_LINE_STATE;
    }
}

/*!
 * @brief     This Class request to set line state.
 *
 * @param     None
 *
 * @retval    None
 */
static void CDC_REQ_SetControlLineState(void)
{
    USBH_CTRL_STATE_T   xferCtrlState;

    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_OUT;
        g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_INTERFACE;
        g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_CLASS;

        g_usbHost.control.reqData.domain.bRequest = 0x22;

        g_usbHost.control.reqData.domain.wValue[0] = 0x00;
        g_usbHost.control.reqData.domain.wValue[1] = 0x00;

        g_usbHost.control.reqData.domain.wIndex[0] = 0x00;
        g_usbHost.control.reqData.domain.wIndex[1] = 0x00;

        g_usbHost.control.reqData.domain.wLength[0] = 0x00;
        g_usbHost.control.reqData.domain.wLength[1] = 0x00;

        g_usbHost.control.ctrlState = USBH_CTRL_SETUP;


        g_usbHost.control.xferState = USBH_XFER_WAITING;
    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        /* Polling transaction handler */
        xferCtrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData, NULL);
        switch (xferCtrlState)
        {
            case USBH_CTRL_COMPLETE:
                g_usbHost.control.xferState = USBH_XFER_OK;
                break;
            case USBH_CTRL_STALL :
            case USBH_CTRL_ERROR :
                g_usbHost.control.xferState = USBH_XFER_START;
                break;
            default :
                break;
        }
    }
    /* If set line state OK */
    else if (g_usbHost.control.xferState == USBH_XFER_OK)
    {
        g_usbHost.control.xferState = USBH_XFER_START;
        s_classReqState = CDC_REQ_CONFIGURED_OK;
    }
}

/*!
 * @brief     CDC Process Get data.
 *
 * @param     None
 *
 * @retval    None
 */
uint8_t USBH_CDCProcessGet(void)
{
    USBH_URB_STATUS_T  urbState;

    if (flagEnableGet == 1)
    {
        if ((CDC_Rx.cdcState == USBH_CDC_COMPLETE) || (CDC_Rx.cdcState == USBH_CDC_IDIE))
        {
            CDC_Rx.cdcState = USBH_CDC_GET_DATA;
        }
    }
    else
    {
        CDC_DisableGet();
        CDC_Rx.cdcState = USBH_CDC_IDIE;
        CDC_Rx.cdcState = CDC_Rx.cdcState;
        return CDC_Rx.cdcState;
    }

    switch (CDC_Rx.cdcState)
    {
        case USBH_CDC_IDIE:
            break;

        case USBH_CDC_GET_DATA:
            /* Sent OUT packet */
            if (CDC_Rx.dataLen < (CDC_Rx.buffLen - g_usbHost.xferPipe[CDC_Rx.chNum].maxPackSize))
            {
                USBH_BulkInData(CDC_Rx.chNum, CDC_Rx.pFillBuff, g_usbHost.xferPipe[CDC_Rx.chNum].maxPackSize);
                CDC_Rx.cdcState = USBH_CDC_GET_WAIT;
            }
            break;
        /* Wait for OUT stage complete */
        case USBH_CDC_GET_WAIT:
            urbState  = g_usbHost.xferPipe[CDC_Rx.chNum].urbStatus;
            switch (urbState)
            {
                case USBH_URB_OK: /*!< OUT data OK */
                    CDC_Rx.dataLen += g_usbHost.xferPipe[CDC_Rx.chNum].bufCount;
                    CDC_Rx.pFillBuff += g_usbHost.xferPipe[CDC_Rx.chNum].bufCount;
                    CDC_ReceiveData();
                    CDC_Rx.cdcState = USBH_CDC_COMPLETE;
                    break;
                case USBH_URB_ERROR:
                case USBH_URB_STALL:
                    CDC_Rx.cdcState = USBH_CDC_ERROR;
                    break;
                case USBH_URB_NOREADY:
                    CDC_Rx.cdcState = USBH_CDC_GET_DATA;;
                    break;
                default :
                    break;
            }
            break;
        default :
            break;
    }

    if (CDC_Rx.cdcState == USBH_CDC_ERROR)
    {
        CDC_Rx.cdcState = USBH_CDC_GET_DATA;
    }
    else if (CDC_Rx.cdcState == USBH_CDC_COMPLETE)
    {
    }

    return CDC_Rx.cdcState;
}

/*!
 * @brief     CDC Process Send data.
 *
 * @param     None
 *
 * @retval    None
 */
uint8_t USBH_CDCProcessSend(void)
{
    USBH_URB_STATUS_T  urbState;
    static uint16_t len;

    switch (CDC_Tx.cdcState)
    {
        case USBH_CDC_IDIE:
            break;

        case USBH_CDC_SEND_DATA:
            /* Sent OUT packet */
            if (CDC_Tx.dataLen > g_usbHost.xferPipe[CDC_Tx.chNum].maxPackSize)
            {
                len = g_usbHost.xferPipe[CDC_Tx.chNum].maxPackSize;
            }
            else
            {
                len = CDC_Tx.dataLen;
            }
            USBH_BulkOutData(CDC_Tx.chNum, CDC_Tx.buff, len);
            CDC_Tx.cdcState = USBH_CDC_SEND_WAIT;
            break;
        /* Wait for OUT stage complete */
        case USBH_CDC_SEND_WAIT:
            urbState  = g_usbHost.xferPipe[CDC_Tx.chNum].urbStatus;
            switch (urbState)
            {
                case USBH_URB_OK: /*!< OUT data OK */
                    CDC_Tx.buff += len ;
                    CDC_Tx.dataLen -= len;

                    if (CDC_Tx.dataLen == 0)
                    {
                        CDC_Tx.cdcState = USBH_CDC_COMPLETE;
                    }
                    else
                    {
                        CDC_Tx.cdcState = USBH_CDC_SEND_DATA;
                    }
                    break;
                case USBH_URB_ERROR:
                case USBH_URB_STALL:
                    CDC_Tx.cdcState = USBH_CDC_ERROR;
                    break;
                case USBH_URB_NOREADY:
                    CDC_Tx.cdcState = USBH_CDC_SEND_DATA;;
                    break;
                default :
                    break;
            }
            break;
        default :
            break;
    }

    if (CDC_Tx.cdcState == USBH_CDC_ERROR)
    {
        CDC_Tx.cdcState = USBH_CDC_SEND_DATA;
    }
    else if (CDC_Tx.cdcState == USBH_CDC_COMPLETE)
    {

    }

    return CDC_Tx.cdcState;
}

/*!
 * @brief     CDC Enable send flag.
 *
 * @param     None
 *
 * @retval    None
 */
void CDC_EnableSend(void)
{
    flagEnableSend = 1;
    CDC_Tx.cdcState = USBH_CDC_SEND_DATA;

}

/*!
 * @brief     CDC Disable send flag.
 *
 * @param     None
 *
 * @retval    None
 */
void CDC_DisableSend(void)
{
    flagEnableSend = 0;
    CDC_Tx.cdcState = USBH_CDC_IDIE;
}

/*!
 * @brief     CDC Enable to get data.
 *
 * @param     None
 *
 * @retval    None
 */
void CDC_EnableGet(void)
{
    if ((flagEnableGet == 0) && (CDC_Rx.cdcState == USBH_CDC_IDIE))
    {
        flagEnableGet = 1;
        CDC_Rx.cdcState = USBH_CDC_GET_DATA;
    }
}

/*!
 * @brief     CDC Disable get data.
 *
 * @param     None
 *
 * @retval    None
 */
void CDC_DisableGet(void)
{
    flagEnableGet = 0;
    CDC_Rx.cdcState = USBH_CDC_IDIE;

    USB_OTG_HaltChannel(CDC_Rx.chNum);
}

/*!
 * @brief     CDC data Receive handler.
 *
 * @param     None
 *
 * @retval    None
 */
void CDC_ReceiveData(void)
{
    uint8_t* ptr;

    if (CDC_Rx.pEmptyBuff < CDC_Rx.pFillBuff)
    {
        ptr = CDC_Rx.pFillBuff;
        *ptr = 0x00;

        /* Rx Fill Buff Pointer point to rxBuf Empty place */
        CDC_Rx.pFillBuff  = CDC_Rx.pEmptyBuff ;
        /* Reset the data length to zero */
        CDC_Rx.dataLen = 0;
    }
}

/*!
 * @brief     CDC send data handler.
 *
 * @param     None
 *
 * @retval    None
 */
uint8_t CDC_SendData(uint8_t* data, uint16_t length)
{
    if (CDC_Tx.cdcState == USBH_CDC_IDIE)
    {
        CDC_Tx.buff = data;
        CDC_Tx.dataLen = length;
        CDC_EnableSend();
        return 1;
    }
    return 0;
}

/*!
 * @brief     CDC tranfer data demo.
 *
 * @param     None
 *
 * @retval    None
 */
void CDC_DEMO(void)
{
    if (CDC_Tx.cdcState == USBH_CDC_COMPLETE)
    {
        printf(">> Transfer Complete.\n");

        USB_OTG_FlushTxFIFO(0X10);
        CDC_DisableSend();
    }

    if (CDC_Rx.cdcState == USBH_CDC_COMPLETE)
    {
        printf(">> Receive data: %s\n", CDC_Rx.pEmptyBuff);
    }
}

/**@} end of group CDC_Functions */
/**@} end of group CDC */
/**@} end of group Class */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */
