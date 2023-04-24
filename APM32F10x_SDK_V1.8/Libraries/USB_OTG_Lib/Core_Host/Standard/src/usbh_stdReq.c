/*!
 * @file        USBH_StReq.c
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
#include "usbh_stdReq.h"
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

/** @addtogroup StdReq
  @{
*/

/** @defgroup StdReq_Functions Functions
  @{
*/

static void USBH_CopyBuffer(uint8_t* desBuffer, uint8_t* srcBuf, uint32_t len);

/*!
 * @brief     Config Standard Get Configuration request.
 *
 * @param     None
 *
 * @retval    None
 */
void USBH_GetStandardConfiguration(void)
{
    /* Config Request */
    g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_IN;
    g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_DEVICE;
    g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_STANDARD;

    g_usbHost.control.reqData.domain.bRequest = USBH_STD_GET_CONFIGURATION;

    g_usbHost.control.reqData.domain.wValue[0] = 0;
    g_usbHost.control.reqData.domain.wValue[1] = 0;

    g_usbHost.control.reqData.domain.wIndex[0] = 0;
    g_usbHost.control.reqData.domain.wIndex[1] = 0;

    g_usbHost.control.reqData.domain.wLength[0] = 1;
    g_usbHost.control.reqData.domain.wLength[1] = 0;

    g_usbHost.control.ctrlState = USBH_CTRL_SETUP;
}

/*!
 * @brief     Config Standard Get Descriptor request.
 *
 * @param     descType : Select descriptor type.
 *                  This value can be one of the following values:
 *                  @arg USBH_DESC_DEVICE
 *                  @arg USBH_DESC_CONFIGURATION
 *                  @arg USBH_DESC_STRING
 *                  @arg USBH_DESC_INTERFACE
 *                  @arg USBH_DESC_ENDPOINT
 *                  @arg USBH_DESC_DEVICE_QUALIFIER
 *                  @arg USBH_DESC_OTHER_SPEED
 *                  @arg USBH_INTERFACE_POWER
 *
 * @param     descIndex : Specifies descriptor index.
 *
 * @param     len : Specifies descriptor len of request.
 *
 * @retval    None
 */
void USBH_StandardGetDescriptor(USBH_DESC_TYPE_T descType, uint8_t descIndex, uint16_t len)
{
    /* Config Request */
    g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_IN;
    g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_DEVICE;
    g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_STANDARD;

    g_usbHost.control.reqData.domain.bRequest = USBH_STD_GET_DESCRIPTOR;

    g_usbHost.control.reqData.domain.wValue[0] = descIndex;
    g_usbHost.control.reqData.domain.wValue[1] = descType;

    if((descType & 0xFF) == USBH_DESC_STRING)
    {
        g_usbHost.control.reqData.domain.wIndex[0] = USBH_LANG_ID & 0xFF;
        g_usbHost.control.reqData.domain.wIndex[1] = USBH_LANG_ID >> 8;
    }
    else
    {
        g_usbHost.control.reqData.domain.wIndex[0] = 0;
        g_usbHost.control.reqData.domain.wIndex[1] = 0;
    }

    g_usbHost.control.reqData.domain.wLength[0] = len & 0xFF;
    g_usbHost.control.reqData.domain.wLength[1] = len >> 8;

    g_usbHost.control.ctrlState = USBH_CTRL_SETUP;
}

/*!
 * @brief     Config Standard Get Interface request..
 *
 * @param     interface : Specifies interface number to get its alternative setting.
 *
 * @retval    None
 */
void USBH_StandardGetInterface(uint16_t interface)
{
    /* Config Request */
    g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_IN;
    g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_INTERFACE;
    g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_STANDARD;

    g_usbHost.control.reqData.domain.bRequest = USBH_STD_GET_INTERFACE;

    g_usbHost.control.reqData.domain.wValue[0] = 0;
    g_usbHost.control.reqData.domain.wValue[1] = 0;

    g_usbHost.control.reqData.domain.wIndex[0] = interface & 0xFF;
    g_usbHost.control.reqData.domain.wIndex[1] = interface >> 8;

    g_usbHost.control.reqData.domain.wLength[0] = 1;
    g_usbHost.control.reqData.domain.wLength[1] = 0;

    g_usbHost.control.ctrlState = USBH_CTRL_SETUP;
}

/*!
 * @brief     Config Standard Get Status request..
 *
 * @param     recipient : Select request recipient.
 *                       This value can be one of the following values:
 *                       @arg USBH_RECIPIENT_DEVICE
 *                       @arg USBH_RECIPIENT_INTERFACE
 *                       @arg USBH_RECIPIENT_ENDPOINT
 *                       @arg USBH_RECIPIENT_OTHER
 *
 * @param     index : Specifies index of interface or endpoint.
 *
 * @retval    None
 */
void USBH_StandardGetStatus(USBH_RECIPIENT_T recipient, uint16_t index)
{
    /* Config Request */
    g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_IN;
    g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = recipient;
    g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_STANDARD;

    g_usbHost.control.reqData.domain.bRequest = USBH_STD_GET_INTERFACE;

    g_usbHost.control.reqData.domain.wValue[0] = 0;
    g_usbHost.control.reqData.domain.wValue[1] = 0;

    if (recipient == USBH_RECIPIENT_DEVICE)
    {
        g_usbHost.control.reqData.domain.wIndex[0] = 0;
        g_usbHost.control.reqData.domain.wIndex[1] = 0;
    }
    else
    {
        g_usbHost.control.reqData.domain.wIndex[0] = index & 0xFF;
        g_usbHost.control.reqData.domain.wIndex[1] = index >> 8;
    }

    g_usbHost.control.reqData.domain.wLength[0] = 2;
    g_usbHost.control.reqData.domain.wLength[1] = 0;

    g_usbHost.control.ctrlState = USBH_CTRL_SETUP;
}

/*!
 * @brief     Config Standard Set Address request.
 *
 * @param     devAddr : Specifies the device address.
 *
 * @retval    None
 */
void USBH_StandardSetAddress(uint8_t devAddr)
{
    /* Config Request */
    g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_OUT;
    g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_DEVICE;
    g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_STANDARD;

    g_usbHost.control.reqData.domain.bRequest = USBH_STD_SET_ADDRESS;

    g_usbHost.control.reqData.domain.wValue[0] = devAddr;
    g_usbHost.control.reqData.domain.wValue[1] = 0;

    g_usbHost.control.reqData.domain.wIndex[0] = 0;
    g_usbHost.control.reqData.domain.wIndex[1] = 0;

    g_usbHost.control.reqData.domain.wLength[0] = 0;
    g_usbHost.control.reqData.domain.wLength[1] = 0;

    g_usbHost.control.ctrlState = USBH_CTRL_SETUP;
}

/*!
 * @brief     Config Standard Set Configuration request..
 *
 * @param     configuration : Specifies the configuration value.
 *
 * @retval    None
 */
void USBH_StandardSetConfiguration(uint8_t configuration)
{
    /* Config Request */
    g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_OUT;
    g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_DEVICE;
    g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_STANDARD;

    g_usbHost.control.reqData.domain.bRequest = USBH_STD_SET_CONFIGURATION;

    g_usbHost.control.reqData.domain.wValue[0] = configuration;
    g_usbHost.control.reqData.domain.wValue[1] = 0;

    g_usbHost.control.reqData.domain.wIndex[0] = 0;
    g_usbHost.control.reqData.domain.wIndex[1] = 0;

    g_usbHost.control.reqData.domain.wLength[0] = 0;
    g_usbHost.control.reqData.domain.wLength[1] = 0;

    g_usbHost.control.ctrlState = USBH_CTRL_SETUP;
}


/*!
 * @brief     Config Standard Set Descriptor request.
 *
 * @param     descType : Select descriptor type.
 *                  This value can be one of the following values:
 *                  @arg USBH_DESC_DEVICE
 *                  @arg USBH_DESC_CONFIGURATION
 *                  @arg USBH_DESC_STRING
 *                  @arg USBH_DESC_INTERFACE
 *                  @arg USBH_DESC_ENDPOINT
 *                  @arg USBH_DESC_DEVICE_QUALIFIER
 *                  @arg USBH_DESC_OTHER_SPEED
 *                  @arg USBH_INTERFACE_POWER
 *
 * @param     descIndex : Specifies descriptor index.
 *
 * @param     len : Specifies descriptor len of request.
 *
 * @retval    None
 */
void USBH_StandardSetDescriptor(USBH_DESC_TYPE_T descType, uint8_t descIndex, uint16_t len)
{
    /* Config Request */
    g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_OUT;
    g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_DEVICE;
    g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_STANDARD;

    g_usbHost.control.reqData.domain.bRequest = USBH_STD_SET_DESCRIPTOR;

    g_usbHost.control.reqData.domain.wValue[0] = descIndex;
    g_usbHost.control.reqData.domain.wValue[1] = descType;

    g_usbHost.control.reqData.domain.wIndex[0] = 0;
    g_usbHost.control.reqData.domain.wIndex[1] = 0;

    g_usbHost.control.reqData.domain.wLength[0] = len & 0xFF;
    g_usbHost.control.reqData.domain.wLength[1] = len >> 8;

    g_usbHost.control.ctrlState = USBH_CTRL_SETUP;
}

/*!
 * @brief     Config Standard Set Interface request..
 *
 * @param     interface : Specifies interface number.
 *
 * @param     alternateSetting : Specifies the alternative setting.
 *
 * @retval    None
 */
void USBH_StandardSetInterface(uint16_t interface, uint16_t alternateSetting)
{
    /* Config Request */
    g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_OUT;
    g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = USBH_RECIPIENT_INTERFACE;
    g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_STANDARD;

    g_usbHost.control.reqData.domain.bRequest = USBH_STD_SET_INTERFACE;

    g_usbHost.control.reqData.domain.wValue[0] = alternateSetting & 0xFF;
    g_usbHost.control.reqData.domain.wValue[1] = alternateSetting >> 8;

    g_usbHost.control.reqData.domain.wIndex[0] = interface & 0xFF;
    g_usbHost.control.reqData.domain.wIndex[1] = interface >> 8;

    g_usbHost.control.reqData.domain.wLength[0] = 1;
    g_usbHost.control.reqData.domain.wLength[1] = 0;

    g_usbHost.control.ctrlState = USBH_CTRL_SETUP;
}

/*!
 * @brief     Config Standard Set Feature request..
 *
 * @param     recipient : Select request recipient.
 *                       This value can be one of the following values:
 *                       @arg USBH_RECIPIENT_DEVICE
 *                       @arg USBH_RECIPIENT_INTERFACE
 *                       @arg USBH_RECIPIENT_ENDPOINT
 *                       @arg USBH_RECIPIENT_OTHER
 *
 * @param     feature : Specifies the feature.
 *                     This value can be one of the following values:
 *                     @arg USBH_FEATURE_ENDPOINT_HALT
 *                     @arg USBH_FEATURE_REMOTE_WAKEUP
 *                     @arg USBH_FEATURE_TEST_MODE
 *
 * @param     index : index of interface or endpoint.
 *
 * @retval    None
 */
void USBH_StandardSetFeature(USBH_RECIPIENT_T recipient, USBH_FEATURE_TEST_T feature, uint16_t index)
{
    /* Config Request */
    g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_OUT;
    g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = recipient;
    g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_STANDARD;

    g_usbHost.control.reqData.domain.bRequest = USBH_STD_SET_FEATURE;

    g_usbHost.control.reqData.domain.wValue[0] = feature & 0xFF;
    g_usbHost.control.reqData.domain.wValue[1] = feature >> 8;

    g_usbHost.control.reqData.domain.wIndex[0] = index & 0xFF;
    g_usbHost.control.reqData.domain.wIndex[1] = index >> 8;

    g_usbHost.control.reqData.domain.wLength[0] = 0;
    g_usbHost.control.reqData.domain.wLength[1] = 0;

    g_usbHost.control.ctrlState = USBH_CTRL_SETUP;
}

/*!
 * @brief     Config Standard Clear Feature request..
 *
 * @param     recipient : Select request recipient.
 *                       This value can be one of the following values:
 *                       @arg USBH_RECIPIENT_DEVICE
 *                       @arg USBH_RECIPIENT_INTERFACE
 *                       @arg USBH_RECIPIENT_ENDPOINT
 *                       @arg USBH_RECIPIENT_OTHER
 *
 * @param     feature : Specifies the feature.
 *                     This value can be one of the following values:
 *                     @arg USBH_FEATURE_ENDPOINT_HALT
 *                     @arg USBH_FEATURE_REMOTE_WAKEUP
 *                     @arg USBH_FEATURE_TEST_MODE
 *
 * @param     index : index of interface or endpoint.
 *
 * @retval    None
 */
void USBH_StandardClearFeature(USBH_RECIPIENT_T recipient, USBH_FEATURE_TEST_T feature, uint16_t index, uint8_t channelNum)
{
    /* Config Request */
    g_usbHost.control.reqData.domain.bmRequestType.bit.dir = USBH_REQ_DIR_OUT;
    g_usbHost.control.reqData.domain.bmRequestType.bit.recipient = recipient;
    g_usbHost.control.reqData.domain.bmRequestType.bit.type = USBH_REQ_TYPE_STANDARD;

    g_usbHost.control.reqData.domain.bRequest = USBH_STD_CLEAR_FEATURE;

    g_usbHost.control.reqData.domain.wValue[0] = feature & 0xFF;
    g_usbHost.control.reqData.domain.wValue[1] = feature >> 8;

    g_usbHost.control.reqData.domain.wIndex[0] = index & 0xFF;
    g_usbHost.control.reqData.domain.wIndex[1] = index >> 8;

    g_usbHost.control.reqData.domain.wLength[0] = 0;
    g_usbHost.control.reqData.domain.wLength[1] = 0;

    g_usbHost.control.ctrlState = USBH_CTRL_SETUP;
}

void USBH_ParseCfgDesc(uint8_t* parseBuf,
                       USBH_ConfigurationDesc_T* cfgDescBuf,
                       USBH_Interface_T* interface)
{
    uint16_t parseIndex = 0;
    uint8_t  itfIndex = 0;
    uint8_t  epIndex = 0;
    uint16_t totalLen = ((uint16_t)parseBuf[3] << 8) | (uint16_t)parseBuf[2];
    uint8_t  subLen = 0;


    if (cfgDescBuf != NULL)
    {
        /* Parse Configuration Descriptor */
        USBH_CopyBuffer((uint8_t*)cfgDescBuf, parseBuf, STD_CFG_DESC_SIZE);
    }

    parseIndex = STD_CFG_DESC_SIZE;

    while (totalLen > parseIndex)
    {
        /* Get descriptor length at first byte */
        subLen = parseBuf[parseIndex];

        /* Check the descriptor Type at second byte */
        switch (parseBuf[parseIndex + 1])
        {
            case USBH_DESC_INTERFACE:
                if (itfIndex < INTERFACE_DESC_MAX_NUM)
                {
                    /* Move data from parse Buffer to Interface descriptor */
                    USBH_CopyBuffer((uint8_t*)&interface[itfIndex].itfDesc,
                                    &parseBuf[parseIndex],
                                    STD_INTERFACE_DESC_SIZE);
                    itfIndex++;
                    epIndex = 0;
                }
                break;
            case USBH_DESC_ENDPOINT:
                if ((0 < itfIndex) && (itfIndex < INTERFACE_DESC_MAX_NUM))
                {
                    if (epIndex < ENDPOINT_DESC_MAX_NUM)
                    {
                        /* Move data from parse Buffer to Endpoint descriptor */
                        USBH_CopyBuffer((uint8_t*)&interface[itfIndex - 1].epDesc[epIndex],
                                        &parseBuf[parseIndex],
                                        STD_EP_DESC_SIZE);

                        epIndex++;
                    }

                }
                break;
            default :
                break;
        }

        parseIndex += subLen;

        /* To avoid some useless data left */
        if ((totalLen - parseIndex) < STD_EP_DESC_SIZE)
            break;
    }
}

/*!
 * @brief       Parses the string descriptor
 *
 * @param       DescBuff: point to descriptor data
 *
 * @param       strBuff: Buffer to store the string descriptor
 *
 * @retval      None
 */
void USBH_ParseStrDescriptor(uint8_t* DescBuff, uint8_t* strBuff)
{
    uint16_t index;

    switch (DescBuff[1])
    {
        case 0X03:    /*!< String Type Descriptor */

            for (index = 2; index < DescBuff[0]; index += 2)
            {
                /* get string */
                *strBuff =  DescBuff[index];
                strBuff++;
            }
            /* string end mark*/
            *strBuff = 0x00;
            break;

        default:
            break;
    }
}

/*!
 * @brief       Copy data from source buffer to destination buffer.
 *
 * @param       desBuffer: point to destination buffer
 *
 * @param       srcBuf   : point to source buffer
 *
 * @param       len: copy length
 *
 * @retval      None
 */
static void USBH_CopyBuffer(uint8_t* desBuffer, uint8_t* srcBuf, uint32_t len)
{
    while (len--)
    {
        desBuffer[len] = srcBuf[len];
    }
}
/**@} end of group StdReq_Functions */
/**@} end of group StdReq */
/**@} end of group Standard */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */
