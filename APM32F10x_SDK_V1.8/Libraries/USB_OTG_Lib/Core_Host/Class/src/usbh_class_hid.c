/*!
 * @file        usbh_class_hid.c
 *
 * @brief       hid class handler
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
#include "usbh_class_hid.h"
#include "usbh_dataXfer.h"
#include "usbh_channel.h"
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

/** @addtogroup HID
  @{
*/

/** @defgroup HID_Variables Variables
  @{
*/

__IO uint8_t flagParseHID = 0;
uint16_t hidTimer = 0;

uint8_t mouseSeat;
char printMouse[21];
HID_MOUSE_Data_T mouseData;
uint8_t dat[4];
uint8_t hidDataInChannel;
uint8_t s_classReqState = 0;
uint8_t s_classState = 0;

/**@} end of group HID_Variables */

/** @defgroup HID_Functions Functions
  @{
*/
void ParseMouseData(uint8_t* rBuf);

static void HID_REQ_IdleHandler(void);
static void HID_REQ_GetDevDescHandler(void);
static void HID_REQ_GetReportDescHandler(void);
static void HID_REQ_SetIdleHandler(void);
static void HID_REQ_SetProtocolHandler(void);

static void HID_IdleHandler(void);
static void HID_SyncHandler(void);
static void HID_GetDataHandler(void);
static void HID_PollHandler(void);

/**@} end of group HID_Functions */

/** @defgroup HID_Variables Variables
  @{
*/

/* HID Request Handler function */
HID_REQ_Handler_T HID_REQ_Handler[] =
{
    HID_REQ_IdleHandler,           /*!< match with HID_REQ_IDLE */
    HID_REQ_GetDevDescHandler,     /*!< match with HID_REQ_GET_DEV_DESC */
    HID_REQ_GetReportDescHandler,  /*!< match with HID_REQ_GET_REQ_DEV_DESC */
    HID_REQ_SetIdleHandler,        /*!< match with HID_REQ_SET_IDLE */
    HID_REQ_SetProtocolHandler,    /*!< match with HID_REQ_SET_PROTOCOL */
};

/* HID Core Handler function */
HID_Handler_T HID_Handler[] =
{
    HID_IdleHandler,    /*!< match with HID_IDLE, */
    HID_SyncHandler,    /*!< match with HID_SYNC, */
    HID_GetDataHandler, /*!< match with HID_GET_DATA, */
    HID_PollHandler,    /*!< match with HID_POLL, */
};

/**@} end of group HID_Variables */

/** @defgroup HID_Functions Functions
  @{
*/

/*!
 * @brief     USB Host Class Initialization.
 *
 * @param     None
 *
 * @retval    None
 */
void USBH_HID_InitHandler(void)
{
    /* Prepare to Handler class request */
    g_usbHost.preHostState = USBH_HOST_CLASS_REQ;

    hidDataInChannel = USBH_CH_AllocChannel(0x81);
    /* Open and Config HID IN channel */
    USBH_CH_OpenChannel(hidDataInChannel,
                        USBH_DEVICE_CONFIGURED_ADDRESS,
                        USB_EP_TYPE_INTERRUPT,
                        8);

    g_usbHost.curHostState = USBH_HOST_CLASS_REQ;
}

/*!
 * @brief     USB Host Class Re-Initialization.
 *
 * @param     None
 *
 * @retval    None
 */
void USBH_HID_DeInitHandler(void)
{
    s_classReqState = HID_REQ_IDLE;
    s_classState = HID_IDLE;
}

/*!
 * @brief     USB Host Class Request handler.
 *
 * @param     None
 *
 * @retval    None
 */
void USBH_HID_ReqHandler(void)
{
    /* Hander enum function */
    HID_REQ_Handler[s_classReqState]();

    if (s_classReqState == HID_REQ_CONFIGURED_OK)
    {
        /* Prepare to Handler class request */
        s_classReqState = HID_REQ_IDLE;
        g_usbHost.curHostState = USBH_HOST_CLASS;
    }
}

/*!
 * @brief     USB Host Class Core handler.
 *
 * @param     None
 *
 * @retval    None
 */
void USBH_HID_CoreHandler(void)
{
    /* Hander HID class function */
    HID_Handler[s_classState]();
    g_userCallback.applicationHandler();
}

/*!
 * @brief     Work for Class request handler.
 *
 * @param     None
 *
 * @retval    None
 */
static void HID_REQ_IdleHandler(void)
{
    s_classReqState = HID_REQ_GET_DEV_DESC;
}

/*!
 * @brief     Work for Class request handler.
 *
 * @param     None
 *
 * @retval    None
 */
static void HID_REQ_GetDevDescHandler(void)
{
    USBH_CTRL_STATE_T   xferCtrlState;

    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        /* Get HID Descriptor */
        USBH_StandardGetDescriptor(USBH_DESC_HID, 0, 9);
        /* Recipient change to interface */
        g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_INTERFACE;

        g_usbHost.control.xferState = USBH_XFER_WAITING;
    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        /* Polling transaction handler */
        xferCtrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
                                                (uint8_t*)&g_usbHost.control.descriptor.device);
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
    /* If Get device descriptor OK */
    else if (g_usbHost.control.xferState == USBH_XFER_OK)
    {
        /* Parse HID Descriptor */

        g_usbHost.control.xferState = USBH_XFER_START;
        s_classReqState = HID_REQ_GET_REQ_DEV_DESC;
    }
}

/*!
 * @brief     Work for Class request handler.
 *
 * @param     None
 *
 * @retval    None
 */
static void HID_REQ_GetReportDescHandler(void)
{
    USBH_CTRL_STATE_T   xferCtrlState;

    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        /* Get HID Report Descriptor */
        USBH_StandardGetDescriptor(USBH_DESC_HID_REPORT, 0, 0x2E);
        /* Recipient change to interface */
        g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_INTERFACE;

        g_usbHost.control.xferState = USBH_XFER_WAITING;
    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        /* Polling transaction handler */
        xferCtrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
                                                (uint8_t*)&g_usbHost.control.descriptor.device);
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
    /* If Get device descriptor OK */
    else if (g_usbHost.control.xferState == USBH_XFER_OK)
    {
        /* Parse HID Report Descriptor */

        g_usbHost.control.xferState = USBH_XFER_START;
        s_classReqState = HID_REQ_SET_IDLE;
    }
}

/*!
 * @brief     Work for Class request handler.
 *
 * @param     None
 *
 * @retval    None
 */
static void HID_REQ_SetIdleHandler(void)
{
    USBH_CTRL_STATE_T   xferCtrlState;

    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        /* Get HID Report Descriptor */
        g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_OUT;
        g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_INTERFACE;
        g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_CLASS;

        g_usbHost.control.reqData.domain.bRequest = 0x0A;

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
        xferCtrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
                                                (uint8_t*)&g_usbHost.control.descriptor.device);
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
    /* If Get device descriptor OK */
    else if (g_usbHost.control.xferState == USBH_XFER_OK)
    {
        /* User can Parse HID Report Descriptor here */

        g_usbHost.control.xferState = USBH_XFER_START;
        s_classReqState = HID_REQ_SET_PROTOCOL;
    }
}

/*!
 * @brief     Work for Class request handler.
 *
 * @param     None
 *
 * @retval    None
 */
static void HID_REQ_SetProtocolHandler(void)
{
    USBH_CTRL_STATE_T   xferCtrlState;

    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        /* Get HID Report Descriptor */
        g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_OUT;
        g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_INTERFACE;
        g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_CLASS;

        g_usbHost.control.reqData.domain.bRequest = 0x0B;

        g_usbHost.control.reqData.domain.wValue[0] = 0x01;
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
        xferCtrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
                                                (uint8_t*)&g_usbHost.control.descriptor.device);
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
    /* If Get device descriptor OK */
    else if (g_usbHost.control.xferState == USBH_XFER_OK)
    {
        /* User can Parse HID Report Descriptor here */

        g_usbHost.control.xferState = USBH_XFER_START;
        s_classReqState = HID_REQ_CONFIGURED_OK;
    }
}

/*!
 * @brief     Work for Class Core handler.
 *
 * @param     None
 *
 * @retval    None
 */
static void HID_IdleHandler(void)
{
    s_classState = HID_SYNC;
}

/*!
 * @brief     Work for Class Core handler.
 *
 * @param     None
 *
 * @retval    None
 */
static void HID_SyncHandler(void)
{
    if ((USB_OTG_H_ReadFrameNum() & 0x1) == RESET)
    {
        s_classState = HID_GET_DATA;
    }
}

/*!
 * @brief     Work for Class Core handler.
 *
 * @param     None
 *
 * @retval    None
 */
static void HID_GetDataHandler(void)
{
    USBH_InterruptInData(hidDataInChannel, dat, 4);

    flagParseHID = 1;

    s_classState = HID_POLL;
    hidTimer = USB_OTG_H_ReadFrameNum();

}

/*!
 * @brief     Work for Class Core handler.
 *
 * @param     None
 *
 * @retval    None
 */
static void HID_PollHandler(void)
{
    USBH_URB_STATUS_T  urbStatus;
    USBH_CTRL_STATE_T   ctrlState;


    if (((USB_OTG_H_ReadFrameNum() - hidTimer) >= HID_POLL_TIME) || (USB_OTG_H_ReadFrameNum() < hidTimer))
    {
        s_classState = HID_GET_DATA;
    }
    else
    {
        urbStatus  = g_usbHost.xferPipe[2].urbStatus;

        switch (urbStatus)
        {
            case USBH_URB_OK:
                /* IN data OK */
                /* Parse HID data */
                if (flagParseHID == 1)
                {
                    flagParseHID = 0;
                    ParseMouseData(dat);
                }
                break;
            case USBH_URB_STALL:
                /* Config Standard Clear Feature request */
                if (g_usbHost.control.xferState == USBH_XFER_START)
                {
                    USBH_StandardClearFeature(USBH_RECIPIENT_ENDPOINT, USBH_FEATURE_ENDPOINT_HALT, USB_OTG_H_ReadEpNum(2), 2);
                    g_usbHost.control.xferState = USBH_XFER_WAITING;
                }
                /* Do the transaction */
                else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
                {
                    ctrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData, NULL);

                    switch (ctrlState)
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
                /* If Get Descriptor OK */
                else if (g_usbHost.control.xferState == USBH_XFER_OK)
                {
                    g_usbHost.control.xferState = USBH_XFER_START;
                    s_classState = HID_GET_DATA;
                }
                break;

            default :
                break;
        }
    }
}

/*!
 * @brief     Parse Mouse Data.
 *
 * @param     rBuf : Buffer to be Parsed.
 *
 * @retval    None
 */
void ParseMouseData(uint8_t* rBuf)
{
    int i;

    mouseData.button = rBuf[0];
    mouseData.x = rBuf[1];
    mouseData.y = rBuf[2];

    for (i = 0; i < 20; i++)
    {
        printMouse[i] = '.';
    }
    printMouse[i] = '\n';

    if ((mouseData.button & 0x01) == 0x01)
    {
        printMouse[0] = 'o';
    }

    if ((mouseData.button & 0x02) == 0x02)
    {
        printMouse[19] = 'o';
    }

    if (((mouseData.x & 0x80) == 0x80) && (mouseData.x > 0))
    {
        if (mouseSeat > 0)
        {
            mouseSeat --;
        }
        else
        {
            mouseSeat = 0;
        }
    }
    else if (((mouseData.x & 0x80) == 0x00) && (mouseData.x > 0))
    {
        if (mouseSeat < 17)
        {
            mouseSeat ++;
        }
        else
        {
            mouseSeat = 17;
        }
    }

    printMouse[mouseSeat + 1] = '|';
    printf("%s", printMouse);
}

/**@} end of group HID_Functions */
/**@} end of group HID */
/**@} end of group Class */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */
