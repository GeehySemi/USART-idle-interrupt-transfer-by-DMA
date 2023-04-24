/*!
 * @file        usbh_class_msc.c
 *
 * @brief       usb host msc class handler
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
#include "usbh_class_msc.h"
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

/** @addtogroup MSC
  @{
*/

/** @defgroup MSC_Variables Variables
  @{
*/

/* MSC state machine */
USBH_MSC_Info_T g_MSCInfo;

/**@} end of group MSC_Variables */

/** @defgroup MSC_Functions Functions
  @{
*/
static void MSC_GetMaxLunHandler(void);
static void MSC_CtrlErrorStateHandler(void);
static void MSC_BotResetHandler(void);

/* private function */
static void MSC_ResetBotHandler(void);
static void MSC_TestUnitReadyHandler(void);
static void MSC_ReadCapacity10Handler(void);
static void MSC_ModeSense6Handler(void);
static void MSC_RequestSenseHandler(void);
static void MSC_BotTransferHandler(void);
static void MSC_AppStateHandler(void);
static void MSC_UnrecoveredStateHandler(void);

static void MSC_ErrorHandler(void);

/**@} end of group MSC_Functions */

/** @defgroup MSC_Variables Variables
  @{
*/
/* MSC handler callback function */
MSC_Handler_T MSC_Handler[] =
{
    MSC_ResetBotHandler,         /*!< Match with USBH_MSC_INIT_BOT_STATE */
    MSC_TestUnitReadyHandler,    /*!< Match with USBH_MSC_TEST_UNIT_READY */
    MSC_ReadCapacity10Handler,   /*!< Match with USBH_MSC_READ_CAPACITY10 */
    MSC_ModeSense6Handler,       /*!< Match with USBH_MSC_MODE_SENSE6 */
    MSC_RequestSenseHandler,     /*!< Match with USBH_MSC_REQUEST_SENSE */
    MSC_BotTransferHandler,      /*!< Match with USBH_MSC_BOT_TRANSFER */
    MSC_AppStateHandler,         /*!< Match with USBH_MSC_APP_STATE */
    MSC_UnrecoveredStateHandler, /*!< Match with USBH_MSC_UNRECOVERED_STATE */
};

/**@} end of group MSC_Variables */

/** @defgroup MSC_Functions Functions
  @{
*/

/*!
 * @brief       USB host MSC configuration
 *
 * @param       None
 *
 * @retval      None
 */
void USBH_MSC_ClassInitHandler(void)
{
    uint8_t itfNum, epNum;

    itfNum = USBH_ReadConfigurationItfNum(&g_usbHost);

    while (itfNum--)
    {
        if ((USBH_ReadInterfaceClass(&g_usbHost, itfNum) != USBH_CLASS_MSC) || \
                (USBH_ReadInterfaceProtocol(&g_usbHost, itfNum) != USBH_MSC_PROTOCOL_BBB))
        {
            g_userCallback.deviceNotSupportedHandler();
            continue;
        }

        epNum = USBH_ReadInterfaceEpNum(&g_usbHost, itfNum);
        while (epNum--)
        {
            /* Get endpoint and size */
            if (USBH_ReadEndpointAddress(&g_usbHost, itfNum, epNum) & 0x80)
            {
                g_BOTInfo.bulkInEpAddr = USBH_ReadEndpointAddress(&g_usbHost, itfNum, epNum);
                g_BOTInfo.bulkInEpSize = USBH_ReadEndpointMPS(&g_usbHost, itfNum, epNum);
            }
            else
            {
                g_BOTInfo.bulkOutEpAddr = USBH_ReadEndpointAddress(&g_usbHost, itfNum, epNum);
                g_BOTInfo.bulkOutEpSize = USBH_ReadEndpointMPS(&g_usbHost, itfNum, epNum);
            }
        }
    }

    /* Out channels */
    g_BOTInfo.outChNum = USBH_CH_AllocChannel(g_BOTInfo.bulkOutEpAddr);
    /* In channels */
    g_BOTInfo.inChNum = USBH_CH_AllocChannel(g_BOTInfo.bulkInEpAddr);

    /* Open the new channels */
    USBH_CH_OpenChannel(g_BOTInfo.outChNum,
                        g_usbHost.devAddress,
                        USB_EP_TYPE_BULK,
                        g_BOTInfo.bulkOutEpSize);

    /* Open the new channels */
    USBH_CH_OpenChannel(g_BOTInfo.inChNum,
                        g_usbHost.devAddress,
                        USB_EP_TYPE_BULK,
                        g_BOTInfo.bulkInEpSize);

    g_MSCInfo.storageInfo.maxLun = 0;
    g_MSCInfo.storageInfo.sectorSize = 512;
    g_MSCInfo.storageInfo.blockSize  = 512;
    g_MSCInfo.stallCnt = 0;

    /* Specify the next state of the USB host state machine */
    USBH_ConfigHostState(USBH_HOST_CLASS);
    USBH_ConfigClassCoreState(USBH_MSC_RESET_BOT);
}

/*!
 * @brief       USB host MSC Class Reset
 *
 * @param       None
 *
 * @retval      None
 */
void USBH_MSC_ClassDeInitHandler(void)
{
    g_MSCInfo.preMSCState = USBH_MSC_RESET_BOT;
    g_MSCInfo.curMSCState = USBH_MSC_RESET_BOT;

    g_MSCInfo.preClassReqState = USBH_MSC_REQ_GET_MAX_LUN;
    g_MSCInfo.curClassReqState = USBH_MSC_REQ_GET_MAX_LUN;
    g_MSCInfo.errorCnt = 0;
    g_MSCInfo.stallCnt = 0;


    g_MSCInfo.storageInfo.maxLun = 0;
    g_MSCInfo.storageInfo.capacity = 64;
    g_MSCInfo.storageInfo.sectorSize = 512;
    g_MSCInfo.storageInfo.blockSize  = 512;
}

/*!
 * @brief       USB host MSC Class reguest handler
 *
 * @param       None
 *
 * @retval      None
 */
void USBH_MSC_ClassReqHandler(void)
{
    switch (g_MSCInfo.curClassReqState)
    {
        case USBH_MSC_REQ_GET_MAX_LUN:
            MSC_GetMaxLunHandler();
            break;
        case USBH_MSC_REQ_BOT_RESET:
            MSC_BotResetHandler();
            break;
        case USBH_MSC_REQ_CTRL_ERROR:
            MSC_CtrlErrorStateHandler();
            break;
        default :
            break;
    }
}

/*!
 * @brief       USB host MSC handler
 *
 * @param       None
 *
 * @retval      None
 */
void USBH_MSC_CoreHandler(void)
{
    /* Hander enum function */
    MSC_Handler[g_MSCInfo.curMSCState]();
}


/*!
 * @brief       Get the max lun of MSC
 *
 * @param       None
 *
 * @retval      None
 */
static void MSC_GetMaxLunHandler(void)
{
    USBH_CTRL_STATE_T   xferCtrlState;

    /* Config Request */
    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_IN;
        g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_INTERFACE;
        g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_CLASS;

        g_usbHost.control.reqData.domain.bRequest = USBH_CLASS_GET_MAX_LUN;

        g_usbHost.control.reqData.domain.wValue[0] = 0x00;
        g_usbHost.control.reqData.domain.wValue[1] = 0x00;

        g_usbHost.control.reqData.domain.wIndex[0] = 0x00;
        g_usbHost.control.reqData.domain.wIndex[1] = 0x00;

        g_usbHost.control.reqData.domain.wLength[0] = 0x01;
        g_usbHost.control.reqData.domain.wLength[1] = 0x00;

        g_usbHost.control.ctrlState = USBH_CTRL_SETUP;

        g_usbHost.control.xferState = USBH_XFER_WAITING;

    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        /* Polling transaction handler */
        xferCtrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
                                                (uint8_t*)&g_MSCInfo.storageInfo.maxLun);

        switch (xferCtrlState)
        {
            case USBH_CTRL_COMPLETE:
                g_MSCInfo.stallCnt = 0;
                g_usbHost.control.xferState = USBH_XFER_START;
                /* Get device max LUN */
                if (g_MSCInfo.storageInfo.maxLun == 0)
                {
                    /* Go to next state */
                    USBH_ConfigClassCoreState(USBH_MSC_TEST_UNIT_READY);
                }
                else
                {
                    g_usbHost.errorState = USBH_ERROR_NOT_SUPPORT;
                }
                break;
            case USBH_CTRL_STALL :
                if (g_MSCInfo.stallCnt++ < USBH_MSC_SATLL_RETRY_CNT)
                {
                    g_MSCInfo.preClassReqState = USBH_MSC_REQ_GET_MAX_LUN;
                    g_MSCInfo.curClassReqState = USBH_MSC_REQ_CTRL_ERROR;
                }
                else
                {
                    g_MSCInfo.stallCnt = 0;
                    USBH_ConfigClassCoreState(USBH_MSC_TEST_UNIT_READY);
                }

                g_usbHost.control.xferState = USBH_XFER_START;
                g_MSCInfo.storageInfo.maxLun = 0;
            case USBH_CTRL_ERROR :
                g_usbHost.control.xferState = USBH_XFER_START;
                break;
            default :
                break;
        }
    }
}


/*!
 * @brief       MSC BOT Reset request
 *
 * @param       None
 *
 * @retval      None
 */
static void MSC_BotResetHandler(void)
{
    USBH_CTRL_STATE_T   xferCtrlState;

    /* Config Request */
    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_OUT;
        g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_INTERFACE;
        g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_CLASS;

        g_usbHost.control.reqData.domain.bRequest = USBH_CLASS_BOT_RESET;

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
                g_MSCInfo.stallCnt = 0;
                g_usbHost.control.xferState = USBH_XFER_OK;
                break;
            case USBH_CTRL_STALL :
                if (g_MSCInfo.stallCnt++ < USBH_MSC_SATLL_RETRY_CNT)
                {
                    g_MSCInfo.preClassReqState = USBH_MSC_REQ_BOT_RESET;
                    g_MSCInfo.curClassReqState = USBH_MSC_REQ_CTRL_ERROR;
                }
                else
                {
                    g_MSCInfo.stallCnt = 0;
                    USBH_ConfigClassCoreState(USBH_MSC_TEST_UNIT_READY);
                }
            case USBH_CTRL_ERROR :
                g_usbHost.control.xferState = USBH_XFER_START;
                break;
            default :
                break;
        }
    }
    /* If transfer OK */
    else if (g_usbHost.control.xferState == USBH_XFER_OK)
    {
        g_usbHost.control.xferState = USBH_XFER_START;

        g_MSCInfo.curClassReqState = USBH_MSC_REQ_GET_MAX_LUN;
    }
}



/*!
 * @brief       Control error state handler
 *
 * @param       None
 *
 * @retval      None
 */
static void MSC_CtrlErrorStateHandler(void)
{
    USBH_CTRL_STATE_T   xferCtrlState;

    /* Config Request */
    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        USBH_StandardClearFeature(USBH_RECIPIENT_ENDPOINT,
                                  USBH_FEATURE_ENDPOINT_HALT,
                                  0,
                                  g_usbHost.control.outChannelNum);

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
    /* If transfer OK */
    else if (g_usbHost.control.xferState == USBH_XFER_OK)
    {
        g_usbHost.control.xferState = USBH_XFER_START;

        g_MSCInfo.storageInfo.maxLun = 0;

        /* Go to backup state */
        g_MSCInfo.curClassReqState = g_MSCInfo.preClassReqState;
    }
}

/*!
 * @brief       Init BOT state
 *
 * @param       None
 *
 * @retval      None
 */
static void MSC_ResetBotHandler(void)
{
    USBH_MSC_BOT_Init();

    /* Go to next state */
    USBH_ConfigHostState(USBH_HOST_CLASS_REQ);
    USBH_ConfigClassReqState(USBH_MSC_REQ_BOT_RESET);
}

/*!
 * @brief       Process Test Unit Ready command
 *
 * @param       None
 *
 * @retval      None
 */
static void MSC_TestUnitReadyHandler(void)
{
    switch (MSC_SCSI_TestUnitReady())
    {
        case USBH_BOT_OK:
            /* Clear error counter */
            g_MSCInfo.errorCnt = 0;
            /* Go to next state */
            USBH_ConfigClassCoreState(USBH_MSC_READ_CAPACITY10);
            break;
        case USBH_BOT_FAIL:
            MSC_ErrorHandler();
            break;
        case USBH_BOT_PHASE_ERROR:
            /* Phase error, Go to Unrecovered state */
            USBH_ConfigClassCoreState(USBH_MSC_UNRECOVERED_STATE);
            break;
        case USBH_BOT_BUSY:
        default :
            break;
    }
}

/*!
 * @brief       Process Read Capacity 10
 *
 * @param       None
 *
 * @retval      None
 */
static void MSC_ReadCapacity10Handler(void)
{
    switch (MSC_SCSI_ReadCapacity10())
    {
        case USBH_BOT_OK:
            /* Clear error counter */
            g_MSCInfo.errorCnt = 0;

            /* Go to next state */
            USBH_ConfigClassCoreState(USBH_MSC_MODE_SENSE6);
            break;
        case USBH_BOT_FAIL:
            MSC_ErrorHandler();
            break;
        case USBH_BOT_PHASE_ERROR:
            /* Phase error, Go to Unrecovered state */
            USBH_ConfigClassCoreState(USBH_MSC_UNRECOVERED_STATE);
            break;
        case USBH_BOT_BUSY:
        default :
            break;
    }
}

/*!
 * @brief       Process Mode Sense 6
 *
 * @param       None
 *
 * @retval      None
 */
static void MSC_ModeSense6Handler(void)
{
    switch (MSC_SCSI_ModeSense6())
    {
        case USBH_BOT_OK:
            /* Clear error counter */
            g_MSCInfo.errorCnt = 0;
            /* Go to next state */
            USBH_ConfigClassCoreState(USBH_MSC_APP_STATE);
            break;
        case USBH_BOT_FAIL:
            MSC_ErrorHandler();
            break;
        case USBH_BOT_PHASE_ERROR:
            /* Phase error, Go to Unrecovered state */
            USBH_ConfigClassCoreState(USBH_MSC_UNRECOVERED_STATE);
            break;
        case USBH_BOT_BUSY:
        default :
            break;
    }
}

/*!
 * @brief       Process Request Sense
 *
 * @param       None
 *
 * @retval      None
 */
static void MSC_RequestSenseHandler(void)
{
    switch (MSC_SCSI_RequestSense())
    {
        case USBH_BOT_OK:
            /* Go to next state */
            g_MSCInfo.curMSCState = USBH_MSC_TEST_UNIT_READY;
            break;
        case USBH_BOT_FAIL:
            MSC_ErrorHandler();
            break;
        case USBH_BOT_PHASE_ERROR:
            /* Phase error, Go to Unrecovered state */
            USBH_ConfigClassCoreState(USBH_MSC_UNRECOVERED_STATE);
            break;
        case USBH_BOT_BUSY:
        default :
            break;
    }
}

/*!
 * @brief       Transfer BOT command
 *
 * @param       None
 *
 * @retval      None
 */
static void MSC_BotTransferHandler(void)
{
    USBH_MSC_BOTXferHandler();
}

/*!
 * @brief       Application Handler
 *
 * @param       None
 *
 * @retval      None
 */
static void MSC_AppStateHandler(void)
{
    g_userCallback.applicationHandler();
}


/*!
 * @brief       Unrecoverd State handler
 *
 * @param       None
 *
 * @retval      None
 */
static void MSC_UnrecoveredStateHandler(void)
{
    g_usbHost.errorState = USBH_ERROR_UNRECOVERED;
}

static void MSC_ErrorHandler(void)
{
    g_MSCInfo.errorCnt++;
    if (g_MSCInfo.errorCnt < USBH_MSC_ERROR_RETRY_CNT)
    {
        /* request Sense  */
        USBH_ConfigClassCoreState(USBH_MSC_REQUEST_SENSE);
        g_MSCInfo.SCSIStates = MSC_SCSI_SEND;
    }
    else
    {
        g_MSCInfo.errorCnt = 0;
        /* Error trials exceeded the limit, go to unrecovered state */
        g_MSCInfo.curMSCState = USBH_MSC_UNRECOVERED_STATE;
    }
}

/**@} end of group MSC_Functions */
/**@} end of group MSC */
/**@} end of group Class */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */
