/*!
 * @file        USBD_StReq.c
 *
 * @brief       USB standard request process
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
#include "usbd_stdReq.h"
#include "usbd_core.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Core_Device Core Device
  @{
*/

/** @addtogroup Standrad
  @{
*/

/** @addtogroup StdReq
  @{
*/

/** @defgroup StdReq_Functions Functions
  @{
*/
static uint8_t USBD_StandardGetConfiguration(void);
static uint8_t USBD_StandardGetDescriptor(void);
static uint8_t USBD_StandardGetInterface(void);
static uint8_t USBD_StandardGetStatus(void);

static uint8_t USBD_StandardSetAddress(void);
static uint8_t USBD_StandardSetConfiguration(void);
static uint8_t USBD_StandardSetDescriptor(void);
static uint8_t USBD_StandardSetFeature(void);
static uint8_t USBD_StandardSetInterface(void);

static uint8_t USBD_StandardClearFeature(void);

/*!
 * @brief       USB request standard request
 *
 * @param       None
 *
 * @retval      None
 */
void USBD_StandardReqeust(void)
{
    uint8_t result = 1;

    uint8_t bRequest = g_usbDev.reqData.domain.bRequest;

    switch (bRequest)
    {
        case USBD_GET_CONFIGURATION:
            result = USBD_StandardGetConfiguration();
            break;

        case USBD_GET_DESCRIPTOR:
            result = USBD_StandardGetDescriptor();
            break;

        case USBD_GET_INTERFACE:
            result = USBD_StandardGetInterface();
            break;

        case USBD_GET_STATUS:
            result = USBD_StandardGetStatus();
            break;

        case USBD_SET_ADDRESS:
            result = USBD_StandardSetAddress();
            break;

        case USBD_SET_CONFIGURATION:
            result = USBD_StandardSetConfiguration();
            break;

        case USBD_SET_DESCRIPTOR:
            result = USBD_StandardSetDescriptor();
            break;

        case USBD_SET_FEATURE:
            result = USBD_StandardSetFeature();
            break;

        case USBD_SET_INTERFACE:
            result = USBD_StandardSetInterface();

            break;

        case USBD_CLEAR_FEATURE:
            result = USBD_StandardClearFeature();
            break;

        default:
            USB_OTG_SetStall(USB_EP_0);
            break;
    }

    if (!result)
    {
        if (g_usbDev.stdReqExceptionHandler != NULL)
        {
            g_usbDev.stdReqExceptionHandler(&g_usbDev.reqData);
        }
    }
}

/*!
 * @brief       Standard request get configuration
 *
 * @param       None
 *
 * @retval      ERROR: 0;  SUCCESS : 1
 */
static uint8_t USBD_StandardGetConfiguration(void)
{
    /* Only for Device */
    if (g_usbDev.reqData.domain.bmRequestType.bit.recipient == USBD_RECIPIENT_DEVICE)
    {
        /* Configured State */
        if (g_usbDev.devState == USBD_DEVICE_STATE_CONFIGURED)
        {
            USBD_CtrlInData(&g_usbDev.curConfiguration, 1);
        }
        /* Address State */
        else if (g_usbDev.devState == USBD_DEVICE_STATE_ADDRESS)
        {
            g_usbDev.curConfiguration = 0;
            USBD_CtrlInData(&g_usbDev.curConfiguration, 1);
        }
        else
        {
            USB_OTG_SetStall(USB_EP_0);
            return ERROR;
        }

        /* User Handler */
        if (g_usbDev.pStdReqCallback->getConfigurationHandler != NULL)
        {
            g_usbDev.pStdReqCallback->getConfigurationHandler();
        }

        return SUCCESS;
    }

    USB_OTG_SetStall(USB_EP_0);

    return ERROR;
}

/*!
 * @brief       Standard request get descriptor
 *
 * @param       None
 *
 * @retval      ERROR: 0;  SUCCESS : 1
 */
static uint8_t USBD_StandardGetDescriptor(void)
{
    uint32_t len = 0;
    uint8_t wValue0 = g_usbDev.reqData.domain.wValue[0];
    uint8_t wValue1 = g_usbDev.reqData.domain.wValue[1];
    uint16_t reqLength = (g_usbDev.reqData.domain.wLength[1] << 8) | \
                         (g_usbDev.reqData.domain.wLength[0]);;

    /* Device Descriptor */
    if (wValue1 == USBD_DESC_DEVICE)
    {
        len = USB_MIN(reqLength, g_usbDev.pDeviceDesc->size);
        USBD_CtrlInData((uint8_t*)g_usbDev.pDeviceDesc->pDesc, len);
    }
    /* Configuration Descriptor */
    else if (wValue1 == USBD_DESC_CONFIGURATION)
    {
        len = USB_MIN(reqLength, g_usbDev.pConfigurationDesc->size);
        USBD_CtrlInData((uint8_t*)g_usbDev.pConfigurationDesc->pDesc, len);
    }
    /* String Descriptor */
    else if ((wValue1 == USBD_DESC_STRING) && (g_usbDev.pStringDesc[wValue0].pDesc != NULL))
    {
        len = USB_MIN(reqLength, g_usbDev.pStringDesc[wValue0].size);
        USBD_CtrlInData((uint8_t*)g_usbDev.pStringDesc[wValue0].pDesc, len);
    }
    /* Qualifier Descriptor */
    else if (wValue1 == USBD_DESC_DEVICE_QUALIFIER)
    {
#ifdef USB_OTG_FS
        USB_OTG_SetStall(USB_EP_0);

        return ERROR;
#else
        len = USB_MIN(reqLength, g_usbDev.pQualifierDesc->size);
        USBD_CtrlInData((uint8_t*)g_usbDev.pQualifierDesc->pDesc, len);
#endif
    }
    /* Other Descriptor */
    else
    {
        /* User Handler */
        if (g_usbDev.pStdReqCallback->getDescriptorHandler != NULL)
        {
            g_usbDev.pStdReqCallback->getDescriptorHandler();
        }
        else
        {
            USB_OTG_SetStall(USB_EP_0);

            return ERROR;
        }
    }

    return SUCCESS;
}

/*!
 * @brief       Standard request get interface
 *
 * @param       None
 *
 * @retval      ERROR: 0;  SUCCESS : 1
 */
static uint8_t USBD_StandardGetInterface(void)
{
    USBD_DevReqData_T* reqData = &g_usbDev.reqData;

    if ((reqData->domain.bmRequestType.bit.recipient) == USBD_RECIPIENT_INTERFACE)
    {
        /* Configured state */
        if (g_usbDev.devState == USBD_DEVICE_STATE_CONFIGURED)
        {
            USBD_CtrlInData(&g_usbDev.curAlternateSetting, 1);

            /* User Handler */
            if (g_usbDev.pStdReqCallback->getInterfaceHandler != NULL)
            {
                g_usbDev.pStdReqCallback->getInterfaceHandler();
            }

            return SUCCESS;
        }
    }

    return ERROR;
}

/*!
 * @brief       Standard request get status
 *
 * @param       None
 *
 * @retval      ERROR: 0; SUCCESS : 1
 */
static uint8_t USBD_StandardGetStatus(void)
{
    uint8_t ret = 1;
    USBD_DevReqData_T* reqData = &g_usbDev.reqData;
    uint8_t recipient = reqData->domain.bmRequestType.bit.recipient;

    uint8_t status[2] = {0, 0};

    /* User Handler */
    if (g_usbDev.pStdReqCallback->getStatusHandler != NULL)
    {
        g_usbDev.pStdReqCallback->getStatusHandler();
    }

    /* Device */
    if (recipient == USBD_RECIPIENT_DEVICE)
    {
        /* Remote Wakeup */
        if (g_usbDev.curFeature & (1 << 5))
        {
            status[0] |= 0x02;
        }
        /* Self power */
        if (g_usbDev.curFeature & (1 << 6))
        {
            status[0] |= 0x01;
        }

        USBD_CtrlInData(status, 2);
    }
    /* Interface */
    else if (recipient == USBD_RECIPIENT_INTERFACE)
    {
        USBD_CtrlInData(status, 2);
    }
    /* Endpoint */
    else if (recipient == USBD_RECIPIENT_ENDPOINT)
    {
        /* IN Endpoint */
        if (reqData->domain.wIndex[0] & 0x80)
        {
            status[0] = USB_OTG_ReadInEpStallStatus(reqData->domain.wIndex[0] & 0x7F);
        }
        /* OUT Endpoint */
        else
        {
            status[0] = USB_OTG_ReadOutEpStallStatus(reqData->domain.wIndex[0] & 0x7F);
        }

        USBD_CtrlInData(status, 2);
    }
    else
    {
        USB_OTG_SetStall(USB_EP_0);

        ret = 0;
    }

    return ret;
}

/*!
 * @brief       Standard request set address
 *
 * @param       None
 *
 * @retval      ERROR: 0; SUCCESS : 1
 */
static uint8_t USBD_StandardSetAddress(void)
{
    uint8_t wValue0 = g_usbDev.reqData.domain.wValue[0];
    uint8_t wValue1 = g_usbDev.reqData.domain.wValue[1];
    uint8_t recipient = g_usbDev.reqData.domain.bmRequestType.bit.recipient;

    if ((wValue0 < 127) && (wValue1 == 0) && (recipient == USBD_RECIPIENT_DEVICE))
    {
        USBD_CtrlTxStatus();

        if (g_usbDev.devState == USBD_DEVICE_STATE_CONFIGURED)
        {
            USB_OTG_SetStall(USB_EP_0);
            return ERROR;
        }

        if (wValue0)
        {
            g_usbDev.devState = USBD_DEVICE_STATE_ADDRESS;
        }
        else
        {
            g_usbDev.devState = USBD_DEVICE_STATE_DEFAULT;
        }

        /* Config device address */
        USB_OTG_ConfigDeviceAddress(wValue0);

        /* User Handler */
        if (g_usbDev.pStdReqCallback->setAddressHandler != NULL)
        {
            g_usbDev.pStdReqCallback->setAddressHandler();
        }

        return SUCCESS;
    }

    USB_OTG_SetStall(USB_EP_0);
    return ERROR;
}

/*!
 * @brief       Standard request set configuration
 *
 * @param       usbInfo: USB Information
 *
 * @retval      ERROR: 0;  SUCCESS : 1
 */
static uint8_t USBD_StandardSetConfiguration(void)
{
    uint8_t recipient = g_usbDev.reqData.domain.bmRequestType.bit.recipient;
    uint8_t wValue0 = g_usbDev.reqData.domain.wValue[0];

    if ((wValue0 <= g_usbDev.configurationNum) && (recipient == USBD_RECIPIENT_DEVICE))
    {
        g_usbDev.curConfiguration = wValue0;

        if (g_usbDev.curConfiguration == 0)
        {
            g_usbDev.devState = USBD_DEVICE_STATE_ADDRESS;
        }
        else
        {
            g_usbDev.devState = USBD_DEVICE_STATE_CONFIGURED;
        }

        /* User Handler */
        if (g_usbDev.pStdReqCallback->setConfigurationHandler != NULL)
        {
            g_usbDev.pStdReqCallback->setConfigurationHandler();
        }

        USBD_CtrlTxStatus();

        return SUCCESS;
    }

    USB_OTG_SetStall(USB_EP_0);
    return ERROR;
}

/*!
 * @brief       Standard request set descriptor
 *
 * @param       None
 *
 * @retval      0: Failed; 1: Success
 */
static uint8_t USBD_StandardSetDescriptor(void)
{
    /* User Handler */
    if (g_usbDev.pStdReqCallback->setDescriptorHandler != NULL)
    {
        g_usbDev.pStdReqCallback->setDescriptorHandler();
    }
    else
    {
        USB_OTG_SetStall(USB_EP_0);
    }

    return ERROR;
}

/*!
 * @brief       Standard request set feature
 *
 * @param       None
 *
 * @retval      0: Failed; 1: Success
 */
static uint8_t USBD_StandardSetFeature(void)
{
    uint8_t ret = 1;
    uint8_t ep;

    if ((g_usbDev.reqData.domain.wValue[0] == USBD_FEATURE_REMOTE_WAKEUP) &&
            (g_usbDev.reqData.domain.bmRequestType.bit.recipient) == USBD_DESC_DEVICE)
    {
        g_usbDev.curFeature |= (1 << 5);
    }
    else if ((g_usbDev.reqData.domain.wValue[0] == USBD_FEATURE_ENDPOINT_HALT) &&
             (g_usbDev.reqData.domain.bmRequestType.bit.recipient) == USBD_DESC_ENDPOINT)
    {
        ep = g_usbDev.reqData.domain.wIndex[0] & 0x7f;

        if ((ep < USB_MAX(USB_OUT_EP_MAX_NUM, USB_IN_EP_MAX_NUM)) && (!g_usbDev.curConfiguration))
        {
            if (g_usbDev.reqData.domain.wIndex[0] & 0x80)
            {
                USB_OTG_SetInEpStall(ep);
            }
            else
            {
                USB_OTG_SetOutEpStall(ep);
            }
        }
    }
    else
    {
        ret = 0;
    }

    /* User Handler */
    if (g_usbDev.pStdReqCallback->setFeatureHandler != NULL)
    {
        g_usbDev.pStdReqCallback->setFeatureHandler();
    }

    if (ret)
    {
        USBD_CtrlTxStatus();
    }
    else
    {
        USB_OTG_SetStall(USB_EP_0);
    }

    return ret;
}

/*!
 * @brief       Standard request set interface
 *
 * @param       None
 *
 * @retval      0: Failed; 1: Success
 */
static uint8_t USBD_StandardSetInterface(void)
{
    if ((g_usbDev.reqData.domain.bmRequestType.bit.recipient) == USBD_RECIPIENT_INTERFACE)
    {
        g_usbDev.curInterface = g_usbDev.reqData.domain.wIndex[0];
        g_usbDev.curAlternateSetting = g_usbDev.reqData.domain.wValue[0];

        /* User Handler */
        if (g_usbDev.pStdReqCallback->setInterfaceHandler != NULL)
        {
            g_usbDev.pStdReqCallback->setInterfaceHandler();
        }

        USBD_CtrlTxStatus();

        return 1;
    }

    return 0;
}

/*!
 * @brief       Standard request clear feature
 *
 * @param       None
 *
 * @retval      0: Failed; 1: Success
 */
static uint8_t USBD_StandardClearFeature(void)
{
    uint8_t ret = 1;
    uint8_t ep;

    if ((g_usbDev.reqData.domain.wValue[0] == USBD_FEATURE_REMOTE_WAKEUP) &&
            (g_usbDev.reqData.domain.bmRequestType.bit.recipient) == USBD_RECIPIENT_DEVICE)
    {
        g_usbDev.curFeature &= ~(1 << 5);
    }
    else if ((g_usbDev.reqData.domain.wValue[0] == USBD_FEATURE_ENDPOINT_HALT) &&
             (g_usbDev.reqData.domain.bmRequestType.bit.recipient) == USBD_RECIPIENT_ENDPOINT)
    {
        ep = g_usbDev.reqData.domain.wIndex[0] & 0x7f;

        if ((ep < USB_MAX(USB_OUT_EP_MAX_NUM, USB_IN_EP_MAX_NUM)) && (!g_usbDev.curConfiguration))
        {
            if (g_usbDev.reqData.domain.wIndex[0] & 0x80)
            {
                USB_OTG_ClearInEpStall(ep);
            }
            else
            {
                USB_OTG_ClearOutEpStall(ep);
            }
        }
    }
    else
    {
        ret = 0;
    }

    /* User Handler */
    if (g_usbDev.pStdReqCallback->clearFeatureHandler != NULL)
    {
        g_usbDev.pStdReqCallback->clearFeatureHandler();
    }

    if (ret)
    {
        USBD_CtrlTxStatus();
    }
    else
    {
        USB_OTG_SetStall(USB_EP_0);
    }

    return ret;
}
/**@} end of group StdReq_Functions */
/**@} end of group Interrupt */
/**@} end of group Standard */
/**@} end of group Core_Device */
/**@} end of group USB_OTG_Library */
