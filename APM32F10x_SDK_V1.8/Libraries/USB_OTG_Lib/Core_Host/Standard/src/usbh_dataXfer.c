/*!
 * @file        usbh_dataXfer.c
 *
 * @brief       USB host enum hander function
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
#include "usbh_dataXfer.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Host Core Host
  @{
*/

/** @addtogroup Standard
  @{
*/

/** @addtogroup DataXfer
  @{
*/

/** @defgroup DataXfer_Functions Functions
  @{
*/

static USBH_CTRL_STATE_T USBH_CtrlXferSetup(USBH_HostReqData_T* reqData, uint8_t* buf);
static USBH_CTRL_STATE_T USBH_CtrlXferOutData(USBH_HostReqData_T* reqData, uint8_t* outBuf);
static USBH_CTRL_STATE_T USBH_CtrlXferInData(USBH_HostReqData_T* reqData, uint8_t* inBuf);
static USBH_CTRL_STATE_T USBH_CtrlXferOutStatus(USBH_HostReqData_T* reqData, uint8_t* buf);
static USBH_CTRL_STATE_T USBH_CtrlXferInStatus(USBH_HostReqData_T* reqData, uint8_t* buf);
static USBH_CTRL_STATE_T USBH_CtrlXferWaitUrbState(USBH_HostReqData_T* reqData, uint8_t* buf);

/**@} end of group DataXfer_Functions */

/** @defgroup DataXfer_Variables Variables
  @{
*/

/* USB Control transfer State Handler function */
static USBH_CtrlStateHandler_T USBH_CtrlStateHandler[] =
{
    USBH_CtrlXferSetup,
    USBH_CtrlXferOutData,
    USBH_CtrlXferInData,
    USBH_CtrlXferOutStatus,
    USBH_CtrlXferInStatus,
    USBH_CtrlXferWaitUrbState
};

static USBH_CTRL_XFER_STATE_T s_curXferState = USBH_CTRL_XFER_SETUP;
static USBH_CTRL_XFER_STATE_T s_preXferState;

/**@} end of group DataXfer_Variables */

/** @defgroup DataXfer_Functions Functions
  @{
*/

/*!
 * @brief     Handle Control Setup transfer.
 *
 * @param     reqData : Point to a USBH_HostReqData_T structure
 *
 * @param     buf : Point to a Buffer to be transfered(useless)
 *
 * @retval    Return state of USBH_CTRL_STATE_T type enum
 */
USBH_CTRL_STATE_T USBH_ControlXferHandler(USBH_HostReqData_T* reqData, uint8_t* buf)
{
    return USBH_CtrlStateHandler[s_curXferState](reqData, buf);
}

/*!
 * @brief     Start Control Setup transfer.
 *
 * @param     chNum : Channel number
 *
 * @param     reqData : Point to a USBH_HostReqData_T structure
 *
 * @retval    None
 */
void USBH_CtrlSetupReq(uint8_t chNum, USBH_HostReqData_T* reqData)
{
    g_usbHost.xferPipe[chNum].buf = (uint8_t*)reqData;
    g_usbHost.xferPipe[chNum].bufLen = 8;
    g_usbHost.xferPipe[chNum].bufCount = 0;

    /* Enable Setup transfer */
    USB_OTG_EnableOutChannelTransfer(chNum,
                                     (uint8_t*)reqData,
                                     8,
                                     g_usbHost.control.maxPackSize,
                                     SETUP,
                                     USB_EP_TYPE_CONTROL);

    USBH_ConfigDataPid(chNum, 1);
    g_usbHost.xferPipe[chNum].urbStatus = USBH_URB_IDLE;
}

/*!
 * @brief     Start Control OUT transfer.
 *
 * @param     chNum : Channel number
 *
 * @param     wBuf : Point to an OUT data Buffer
 *
 * @param     wLen : Transfer length.
 *
 * @retval    None
 */
void USBH_CtrlOutData(uint8_t chNum, uint8_t* wBuf, uint32_t wLen)
{
    g_usbHost.xferPipe[chNum].buf = wBuf;
    g_usbHost.xferPipe[chNum].bufLen = wLen;
    g_usbHost.xferPipe[chNum].bufCount = 0;
    g_usbHost.xferPipe[chNum].pidToggle = wLen ? g_usbHost.xferPipe[chNum].pidToggle : 1;
    g_usbHost.xferPipe[chNum].dataPid = USBH_ReadDataPid(chNum);

    /* Enable control OUT transfer */
    USB_OTG_EnableOutChannelTransfer(chNum,
                                     wBuf,
                                     wLen,
                                     g_usbHost.control.maxPackSize,
                                     g_usbHost.xferPipe[chNum].dataPid,
                                     USB_EP_TYPE_CONTROL);

    g_usbHost.xferPipe[chNum].urbStatus = USBH_URB_IDLE;
}

/*!
 * @brief     Start Control IN transfer.
 *
 * @param     chNum : Channel number
 *
 * @param     rBuf : Point to an IN data Buffer
 *
 * @param     rLen : Transfer length.
 *
 * @retval    None
 */
void USBH_CtrlInData(uint8_t chNum, uint8_t* rBuf, uint32_t rLen)
{
    g_usbHost.xferPipe[chNum].buf = rBuf;
    g_usbHost.xferPipe[chNum].bufLen = rLen;
    g_usbHost.xferPipe[chNum].bufCount = 0;
    USBH_ConfigDataPid(chNum, 1);
    g_usbHost.xferPipe[chNum].dataPid = USBH_ReadDataPid(chNum);

    /* Enable control IN transfer */
    USB_OTG_EnableInChannelTransfer(chNum,
                                    rLen,
                                    g_usbHost.control.maxPackSize,
                                    g_usbHost.xferPipe[chNum].dataPid,
                                    USB_EP_TYPE_CONTROL);

    g_usbHost.xferPipe[chNum].urbStatus = USBH_URB_IDLE;
}

/*!
 * @brief     Start Interrupt OUT transfer.
 *
 * @param     chNum : Channel number
 *
 * @param     wBuf : Point to an OUT data Buffer
 *
 * @param     wLen : Transfer length.
 *
 * @retval    None
 */
void USBH_InterruptOutData(uint8_t chNum, uint8_t* wBuf, uint32_t wLen)
{
    g_usbHost.xferPipe[chNum].buf = wBuf;
    g_usbHost.xferPipe[chNum].bufLen = wLen;
    g_usbHost.xferPipe[chNum].bufCount = 0;
    g_usbHost.xferPipe[chNum].dataPid = USBH_ReadDataPid(chNum);
    USBH_ToggleDataPid(chNum);

    USB_OTG_EnableOutChannelTransfer(chNum,
                                     wBuf,
                                     wLen,
                                     g_usbHost.xferPipe[chNum].maxPackSize,
                                     g_usbHost.xferPipe[chNum].dataPid,
                                     USB_EP_TYPE_INTERRUPT);

    g_usbHost.xferPipe[chNum].urbStatus = USBH_URB_IDLE;
}

/*!
 * @brief     Start Interrupt IN transfer.
 *
 * @param     chNum : Channel number
 *
 * @param     rBuf : Point to an IN data Buffer
 *
 * @param     rLen : Transfer length.
 *
 * @retval    None
 */
void USBH_InterruptInData(uint8_t chNum, uint8_t* rBuf, uint32_t rLen)
{
    g_usbHost.xferPipe[chNum].buf = rBuf;
    g_usbHost.xferPipe[chNum].bufLen = rLen;
    g_usbHost.xferPipe[chNum].bufCount = 0;
    g_usbHost.xferPipe[chNum].dataPid = USBH_ReadDataPid(chNum);
    USBH_ToggleDataPid(chNum);

    /* Enable Interrupt IN transfer */
    USB_OTG_EnableInChannelTransfer(chNum,
                                    rLen,
                                    g_usbHost.xferPipe[chNum].maxPackSize,
                                    g_usbHost.xferPipe[chNum].dataPid,
                                    USB_EP_TYPE_INTERRUPT);

    g_usbHost.xferPipe[chNum].urbStatus = USBH_URB_IDLE;
}

/*!
 * @brief     Start Bulk OUT transfer.
 *
 * @param     chNum : Channel number
 *
 * @param     wBuf : Point to an OUT data Buffer
 *
 * @param     wLen : Transfer length.
 *
 * @retval    None
 */
void USBH_BulkOutData(uint8_t chNum, uint8_t* wBuf, uint32_t wLen)
{
    g_usbHost.xferPipe[chNum].buf = wBuf;
    g_usbHost.xferPipe[chNum].bufLen = wLen;
    g_usbHost.xferPipe[chNum].bufCount = 0;
    g_usbHost.xferPipe[chNum].dataPid = USBH_ReadDataPid(chNum);
    g_usbHost.xferPipe[chNum].urbStatus = USBH_URB_IDLE;
    /* Enable Bulk OUT transfer */
    USB_OTG_EnableOutChannelTransfer(chNum,
                                     wBuf,
                                     wLen,
                                     g_usbHost.xferPipe[chNum].maxPackSize,
                                     g_usbHost.xferPipe[chNum].dataPid,
                                     USB_EP_TYPE_BULK);

//149060    g_usbHost.xferPipe[chNum].urbStatus = USBH_URB_IDLE;
}

/*!
 * @brief     Start Bulk IN transfer.
 *
 * @param     chNum : Channel number
 *
 * @param     rBuf : Point to an IN data Buffer
 *
 * @param     rLen : Transfer length.
 *
 * @retval    None
 */
void USBH_BulkInData(uint8_t chNum, uint8_t* rBuf, uint32_t rLen)
{
    g_usbHost.xferPipe[chNum].buf = rBuf;
    g_usbHost.xferPipe[chNum].bufLen = rLen;
    g_usbHost.xferPipe[chNum].bufCount = 0;
    g_usbHost.xferPipe[chNum].dataPid = USBH_ReadDataPid(chNum);

    /* Enable BULK IN transfer */
    USB_OTG_EnableInChannelTransfer(chNum,
                                    rLen,
                                    g_usbHost.xferPipe[chNum].maxPackSize,
                                    g_usbHost.xferPipe[chNum].dataPid,
                                    USB_EP_TYPE_BULK);

    g_usbHost.xferPipe[chNum].urbStatus = USBH_URB_IDLE;
}

/*!
 * @brief     Start Isochronous OUT transfer.
 *
 * @param     chNum : Channel number
 *
 * @param     wBuf : Point to an OUT data Buffer
 *
 * @param     wLen : Transfer length.
 *
 * @retval    None
 */
void USBH_IsocOutData(uint8_t chNum, uint8_t* wBuf, uint32_t wLen)
{
    g_usbHost.xferPipe[chNum].buf = wBuf;
    g_usbHost.xferPipe[chNum].bufLen = wLen;
    g_usbHost.xferPipe[chNum].bufCount = 0;
    g_usbHost.xferPipe[chNum].dataPid = DATA_0;

    /* Enable Isochronous OUT transfer */
    USB_OTG_EnableOutChannelTransfer(chNum,
                                     wBuf,
                                     wLen,
                                     g_usbHost.xferPipe[chNum].maxPackSize,
                                     g_usbHost.xferPipe[chNum].dataPid,
                                     USB_EP_TYPE_ISO);

    g_usbHost.xferPipe[chNum].urbStatus = USBH_URB_IDLE;
}

/*!
 * @brief     Start Isochronous IN transfer.
 *
 * @param     chNum : Channel number
 *
 * @param     rBuf : Point to an OUT data Buffer
 *
 * @param     rLen : Transfer length.
 *
 * @retval    None
 */
void USBH_IsocInData(uint8_t chNum, uint8_t* rBuf, uint32_t rLen)
{
    g_usbHost.xferPipe[chNum].buf = rBuf;
    g_usbHost.xferPipe[chNum].bufLen = rLen;
    g_usbHost.xferPipe[chNum].bufCount = 0;
    g_usbHost.xferPipe[chNum].dataPid = DATA_0;

    /* Enable Isochronous IN transfer */
    USB_OTG_EnableInChannelTransfer(chNum,
                                    rLen,
                                    g_usbHost.xferPipe[chNum].maxPackSize,
                                    g_usbHost.xferPipe[chNum].dataPid,
                                    USB_EP_TYPE_ISO);

    g_usbHost.xferPipe[chNum].urbStatus = USBH_URB_IDLE;
}

/*!
 * @brief     Handle Control Setup transfer.
 *
 * @param     reqData : Point to a USBH_HostReqData_T structure
 *
 * @param     buf : Point to a Buffer to be transfered(useless)
 *
 * @retval    Return state of USBH_CTRL_STATE_T type enum
 */
static USBH_CTRL_STATE_T USBH_CtrlXferSetup(USBH_HostReqData_T* reqData, uint8_t* buf)
{
    g_usbHost.control.ctrlState = USBH_CTRL_SETUP;

    /* Sent setup packet */
    USBH_CtrlSetupReq(g_usbHost.control.outChannelNum, reqData);

    /* Backup state */
    s_preXferState = s_curXferState;
    /* Set Next state  */
    s_curXferState = USBH_CTRL_XFER_WAIT_URB;

    return g_usbHost.control.ctrlState;
}

/*!
 * @brief     Handle Control OUT transfer.
 *
 * @param     reqData : Point to a USBH_HostReqData_T structure
 *
 * @param     outBuf : Point to a Buffer to be transfered
 *
 * @retval    Return state of USBH_CTRL_STATE_T type enum
 */
static USBH_CTRL_STATE_T USBH_CtrlXferOutData(USBH_HostReqData_T* reqData, uint8_t* outBuf)
{
    uint16_t len = (uint16_t)reqData->domain.wLength[1] << 8 | \
                   ((uint16_t)reqData->domain.wLength[0]);
    g_usbHost.control.ctrlState = USBH_CTRL_DATA_OUT;

    /* Sent OUT packet */
    USBH_ConfigDataPid(g_usbHost.control.outChannelNum, 1);

    USBH_CtrlOutData(g_usbHost.control.outChannelNum, outBuf, len);

    /* Set next state  */
    s_preXferState = s_curXferState;
    s_curXferState = USBH_CTRL_XFER_WAIT_URB;

    return g_usbHost.control.ctrlState;
}

/*!
 * @brief     Handle Control IN transfer.
 *
 * @param     reqData : Point to a USBH_HostReqData_T structure
 *
 * @param     inBuf : Point to a Buffer to be transfered
 *
 * @retval    Return state of USBH_CTRL_STATE_T type enum
 */
static USBH_CTRL_STATE_T USBH_CtrlXferInData(USBH_HostReqData_T* reqData, uint8_t* inBuf)
{
    uint16_t len = (uint16_t)reqData->domain.wLength[1] << 8 | \
                   ((uint16_t)reqData->domain.wLength[0]);
    g_usbHost.control.ctrlState = USBH_CTRL_DATA_IN;

    /* Receive IN packet */
    USBH_CtrlInData(g_usbHost.control.inChannelNum, inBuf, len);

    /* Set next state  */
    s_preXferState = s_curXferState;
    s_curXferState = USBH_CTRL_XFER_WAIT_URB;

    return g_usbHost.control.ctrlState;
}

/*!
 * @brief     Handle Control OUT status transfer.
 *
 * @param     reqData : Point to a USBH_HostReqData_T structure
 *
 * @param     buf : Point to a Buffer to be transfered(useless)
 *
 * @retval    None
 */
static USBH_CTRL_STATE_T USBH_CtrlXferOutStatus(USBH_HostReqData_T* reqData, uint8_t* buf)
{
    g_usbHost.control.ctrlState = USBH_CTRL_STATUS_OUT;

    /* Sent OUT status */
    USBH_CtrlOutData(g_usbHost.control.outChannelNum, NULL, 0);

    /* Set next state  */
    s_preXferState = s_curXferState;
    s_curXferState = USBH_CTRL_XFER_WAIT_URB;

    return g_usbHost.control.ctrlState;
}

/*!
 * @brief     Handle Control IN status transfer.
 *
 * @param     reqData : Point to a USBH_HostReqData_T structure
 *
 * @param     buf : Point to a Buffer to be transfered(useless)
 *
 * @retval    Return state of USBH_CTRL_STATE_T type enum
 */
static USBH_CTRL_STATE_T USBH_CtrlXferInStatus(USBH_HostReqData_T* reqData, uint8_t* buf)
{
    g_usbHost.control.ctrlState = USBH_CTRL_STATUS_IN;
    /* Receive IN status */
    USBH_CtrlInData(g_usbHost.control.inChannelNum, NULL, 0);

    /* Set next state  */
    s_preXferState = s_curXferState;
    s_curXferState = USBH_CTRL_XFER_WAIT_URB;

    return g_usbHost.control.ctrlState;
}

/*!
 * @brief     Wating URB handler in Control transfer.
 *
 * @param     reqData : Point to a USBH_HostReqData_T structure
 *
 * @param     buf : Point to a Buffer to be transfered(useless)
 *
 * @retval    Return state of USBH_CTRL_STATE_T type enum
 */
static USBH_CTRL_STATE_T USBH_CtrlXferWaitUrbState(USBH_HostReqData_T* reqData, uint8_t* buf)
{
    uint16_t len = (uint16_t)reqData->domain.wLength[1] << 8 | \
                   ((uint16_t)reqData->domain.wLength[0]);
    uint8_t chNum = ((s_preXferState == USBH_CTRL_XFER_DATA_IN) || (s_preXferState == USBH_CTRL_XFER_STATUS_IN)) ? \
                    g_usbHost.control.inChannelNum : g_usbHost.control.outChannelNum;

    switch (g_usbHost.xferPipe[chNum].urbStatus)
    {
        case USBH_URB_OK: /*!< Setup request OK */
            switch (s_preXferState)
            {
                case USBH_CTRL_XFER_SETUP:
                    /* Device to Host */
                    if (reqData->domain.bmRequestType.bit.dir == USBH_REQ_DIR_IN)
                    {
                        s_curXferState = len ? USBH_CTRL_XFER_DATA_IN : USBH_CTRL_XFER_STATUS_OUT;
                    }
                    /* Host to Device */
                    else
                    {
                        s_curXferState = len ? USBH_CTRL_XFER_DATA_OUT : USBH_CTRL_XFER_STATUS_IN;
                    }
                    break;
                case USBH_CTRL_XFER_DATA_OUT:
                    s_curXferState = USBH_CTRL_XFER_STATUS_IN;
                    break;
                case USBH_CTRL_XFER_DATA_IN:
                    s_curXferState = USBH_CTRL_XFER_STATUS_OUT;
                    break;
                case USBH_CTRL_XFER_STATUS_OUT:
                case USBH_CTRL_XFER_STATUS_IN:
                    s_curXferState = USBH_CTRL_XFER_SETUP;
                    g_usbHost.control.ctrlState = USBH_CTRL_COMPLETE;
                    break;
                default :
                    break;
            }
            break;
        case USBH_URB_ERROR:
            if (g_usbHost.control.errCnt++ < CTRL_XFER_ERROR_CNT)
            {
                s_curXferState = USBH_CTRL_XFER_SETUP;
            }
            else
            {
                g_usbHost.control.ctrlState = USBH_CTRL_ERROR;
            }

        case USBH_URB_STALL:
            g_usbHost.control.ctrlState = USBH_CTRL_STALL;
            s_curXferState = USBH_CTRL_XFER_SETUP;
            break;
        case USBH_URB_NOREADY:
            /* If OUT data is not received */
            if (s_preXferState == USBH_CTRL_XFER_DATA_OUT)
            {
                s_curXferState = USBH_CTRL_XFER_DATA_OUT;
            }
        default :
            break;
    }

    return g_usbHost.control.ctrlState;
}

/**@} end of group DataXfer_Functions */
/**@} end of group DataXfer */
/**@} end of group Standard */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */
