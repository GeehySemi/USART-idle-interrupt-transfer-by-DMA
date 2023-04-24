/*!
 * @file        usbh_enum.c
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
#include "usbh_enum.h"
#include "usbh_core.h"
#include "usbh_stdReq.h"
#include "usbh_channel.h"
#include "usbh_dataXfer.h"
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

/** @addtogroup Enum
  @{
*/

/** @defgroup Enum_Functions Functions
  @{
*/

static void ENUM_IdleHandler(void);
static void ENUM_GetDevDescHandler(void);
static void ENUM_GetFullDevDescHandler(void);
static void ENUM_SetAddressHandler(void);
static void ENUM_GetConfigurationDescHandler(void);
static void ENUM_GetFullConfigurationDescHandler(void);
static void ENUM_GetMFCStringHandler(void);
static void ENUM_GetProductStringHandler(void);
static void ENUM_GetSerialNumStringHandler(void);
static void ENUM_SetConfigurationHandler(void);

/**@} end of group Enum_Functions */

/** @defgroup Enum_Variables Variables
  @{
*/

uint8_t s_stringBuf[64];

/* USB Host State Handler function */
ENUM_CoreHandler_T ENUM_CoreHandler[] =
{
    ENUM_IdleHandler,
    ENUM_GetDevDescHandler,
    ENUM_GetFullDevDescHandler,
    ENUM_SetAddressHandler,
    ENUM_GetConfigurationDescHandler,
    ENUM_GetFullConfigurationDescHandler,
    ENUM_GetMFCStringHandler,
    ENUM_GetProductStringHandler,
    ENUM_GetSerialNumStringHandler,
    ENUM_SetConfigurationHandler
};

/**@} end of group Enum_Variables */

/** @defgroup Enum_Functions Functions
  @{
*/

/*!
 * @brief     Handle Enum Idle Stage.
 *
 * @param     None
 *
 * @retval    None
 */
static void ENUM_IdleHandler(void)
{
    USBH_ConfigEnumState(USBH_ENUM_GET_DEV_DESC);
}

/*!
 * @brief     Handle Enum Get 8 bytes device Descriptor Stage.
 *
 * @param     None
 *
 * @retval    None
 */
static void ENUM_GetDevDescHandler(void)
{
    USBH_CTRL_STATE_T   ctrlState;

    /* Prepare to Get device descriptor */
    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        /* Get device descriptor */
        USBH_StandardGetDescriptor(USBH_DESC_DEVICE, 0, 8);
        g_usbHost.control.xferState = USBH_XFER_WAITING;
    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        /* Polling transaction handler */
        ctrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
                                            (uint8_t*)&g_usbHost.control.descriptor.device);

        switch (ctrlState)
        {
            case USBH_CTRL_COMPLETE:
                g_usbHost.control.xferState = USBH_XFER_START;
                g_usbHost.control.maxPackSize = g_usbHost.control.descriptor.device.bMaxPacketSize;

                USBH_CH_OpenChannel(g_usbHost.control.outChannelNum,
                                    g_usbHost.devAddress,
                                    USB_EP_TYPE_CONTROL,
                                    g_usbHost.control.descriptor.device.bMaxPacketSize);

                USBH_CH_OpenChannel(g_usbHost.control.inChannelNum,
                                    g_usbHost.devAddress,
                                    USB_EP_TYPE_CONTROL,
                                    g_usbHost.control.descriptor.device.bMaxPacketSize);

                /* Prepare to Set device Address */
                USBH_ConfigEnumState(USBH_ENUM_SET_ADDR);
                break;
            case USBH_CTRL_STALL :
            case USBH_CTRL_ERROR :
                g_usbHost.control.xferState = USBH_XFER_START;
                break;
            default :
                break;
        }
    }
}

/*!
 * @brief     Handle Enum Get Full device Descriptor Stage.
 *
 * @param     None
 *
 * @retval    None
 */
static void ENUM_GetFullDevDescHandler(void)
{
    USBH_CTRL_STATE_T   ctrlState;

    /* Prepare to Get Full device descriptor */
    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        /* Get Full device descriptor */
        USBH_StandardGetDescriptor(USBH_DESC_DEVICE, 0, STD_DEV_DESC_SIZE);
        g_usbHost.control.xferState = USBH_XFER_WAITING;
    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        /* Polling transaction handler */
        ctrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
                                            (uint8_t*)&g_usbHost.control.descriptor.device);

        switch (ctrlState)
        {
            case USBH_CTRL_COMPLETE:
                g_userCallback.devDescHandler(&g_usbHost.control.descriptor.device);
                g_usbHost.control.xferState = USBH_XFER_START;
                /* Prepare to Get Configuration descriptor */
                USBH_ConfigEnumState(USBH_ENUM_GET_CFG_DESC);
                break;
            case USBH_CTRL_STALL :
            case USBH_CTRL_ERROR :
                g_usbHost.control.xferState = USBH_XFER_START;
                break;
            default :
                break;
        }
    }
}

/*!
 * @brief     Handle Enum Set Address Stage.
 *
 * @param     None
 *
 * @retval    None
 */
static void ENUM_SetAddressHandler(void)
{
    USBH_CTRL_STATE_T   ctrlState;

    /* Prepare to set address */
    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        /* Set device Address */
        USBH_StandardSetAddress(USBH_DEVICE_CONFIGURED_ADDRESS);
        g_usbHost.control.xferState = USBH_XFER_WAITING;
    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        /* Polling transaction handler */
        ctrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData, NULL);

        switch (ctrlState)
        {
            /* Control transfer Complete */
            case USBH_CTRL_COMPLETE:
                g_usbHost.control.xferState = USBH_XFER_START;
                g_usbHost.devAddress = USBH_DEVICE_CONFIGURED_ADDRESS;

                /* Open and Config out channel */
                USB_Delay_us(2000);
                USB_OTG_H_ConfigDeviceAddr(g_usbHost.control.outChannelNum, g_usbHost.devAddress);
                USB_OTG_H_ConfigDeviceAddr(g_usbHost.control.inChannelNum, g_usbHost.devAddress);

                /* Prepare to Get Full device Descriptor */
                USBH_ConfigEnumState(USBH_ENUM_GET_FULL_DEV_DESC);
                break;

            /* Control transfer Stall */
            case USBH_CTRL_STALL :
            /* Control transfer Error */
            case USBH_CTRL_ERROR :
                g_usbHost.control.xferState = USBH_XFER_START;
                break;
            /* Waiting */
            default :
                break;
        }
    }
}

/*!
 * @brief     Handle Enum Get Configuration Descriptor Stage.
 *
 * @param     None
 *
 * @retval    None
 */
static void ENUM_GetConfigurationDescHandler(void)
{
    USBH_CTRL_STATE_T   ctrlState;

    /* Prepare to Get Configuration descriptor */
    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        /* Config Get Descriptor request */
        USBH_StandardGetDescriptor(USBH_DESC_CONFIGURATION, 0, STD_CFG_DESC_SIZE);
        g_usbHost.control.xferState = USBH_XFER_WAITING;
    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        ctrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
                                            (uint8_t*)&g_usbHost.control.descriptor.configuration);

        switch (ctrlState)
        {
            case USBH_CTRL_COMPLETE:
                g_usbHost.control.xferState = USBH_XFER_START;
                /* Prepare to Get Full Configuration Descriptor */
                USBH_ConfigEnumState(USBH_ENUM_GET_FULL_CFG_DESC);
                break;
            case USBH_CTRL_STALL :
            case USBH_CTRL_ERROR :
                g_usbHost.control.xferState = USBH_XFER_START;
                break;
            default :
                break;
        }
    }
}

/*!
 * @brief     Handle Enum Get full Configuration descriptor Stage.
 *
 * @param     None
 *
 * @retval    None
 */
static void ENUM_GetFullConfigurationDescHandler(void)
{
    USBH_CTRL_STATE_T   ctrlState;
    uint16_t reqLen = ((uint16_t)g_usbHost.control.descriptor.configuration.wTotalLength[1] << 8) | \
                      (uint16_t)g_usbHost.control.descriptor.configuration.wTotalLength[0];

    /* Prepare to Get Configuration descriptor */
    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        /* Config Get Descriptor request */
        USBH_StandardGetDescriptor(USBH_DESC_CONFIGURATION,
                                   0,
                                   reqLen);
        g_usbHost.control.xferState = USBH_XFER_WAITING;
    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        ctrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
                                            g_usbHost.control.descriptor.cfgDescBuf);

        switch (ctrlState)
        {
            case USBH_CTRL_COMPLETE:
                g_usbHost.control.xferState = USBH_XFER_START;
                /* Parse Configuration Descriptor */
                USBH_ParseCfgDesc(g_usbHost.control.descriptor.cfgDescBuf,
                                  &g_usbHost.control.descriptor.configuration,
                                  g_usbHost.control.descriptor.interface);

                g_userCallback.cfgDescHandler(&g_usbHost.control.descriptor.configuration, \
                                              g_usbHost.control.descriptor.interface);


                USBH_ConfigEnumState(USBH_ENUM_GET_MFC_STRING_DESC);
                break;
            case USBH_CTRL_STALL :
            case USBH_CTRL_ERROR :
                g_usbHost.control.xferState = USBH_XFER_START;
                break;
            default :
                break;
        }
    }
}

static void ENUM_GetMFCStringHandler(void)
{
    USBH_CTRL_STATE_T   ctrlState;

    /* Prepare to Get Configuration descriptor */
    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        /* Config Get Descriptor request */
        USBH_StandardGetDescriptor(USBH_DESC_STRING,
                                   g_usbHost.control.descriptor.device.iManufacturer,
                                   0xFF);
        g_usbHost.control.xferState = USBH_XFER_WAITING;
    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        ctrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
                                            (uint8_t*)&g_usbHost.control.descriptor.stringBuf);

        switch (ctrlState)
        {
            case USBH_CTRL_COMPLETE:
                g_usbHost.control.xferState = USBH_XFER_START;
                /* Parse String Descriptor */
                USBH_ParseStrDescriptor(g_usbHost.control.descriptor.stringBuf, s_stringBuf);

                g_userCallback.manufacturerStringHandler(s_stringBuf);

                USBH_ConfigEnumState(USBH_ENUM_GET_PRODUCT_STRING_DESC);
                break;
            case USBH_CTRL_STALL :
            case USBH_CTRL_ERROR :
                g_usbHost.control.xferState = USBH_XFER_START;
                break;
            default :
                break;
        }
    }
}

static void ENUM_GetProductStringHandler(void)
{
    USBH_CTRL_STATE_T   ctrlState;

    /* Prepare to Get Configuration descriptor */
    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        /* Config Get Descriptor request */
        USBH_StandardGetDescriptor(USBH_DESC_STRING,
                                   g_usbHost.control.descriptor.device.iProduct,
                                   0xFF);
        g_usbHost.control.xferState = USBH_XFER_WAITING;
    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        ctrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
                                            (uint8_t*)&g_usbHost.control.descriptor.stringBuf);

        switch (ctrlState)
        {
            case USBH_CTRL_COMPLETE:
                g_usbHost.control.xferState = USBH_XFER_START;
                /* Parse String Descriptor */
                USBH_ParseStrDescriptor(g_usbHost.control.descriptor.stringBuf, s_stringBuf);

                g_userCallback.productStringHandler(s_stringBuf);

                USBH_ConfigEnumState(USBH_ENUM_GET_SERIALNUM_STRING_DESC);
                break;
            case USBH_CTRL_STALL :
            case USBH_CTRL_ERROR :
                g_usbHost.control.xferState = USBH_XFER_START;
                break;
            default :
                break;
        }
    }
}

static void ENUM_GetSerialNumStringHandler(void)
{
    USBH_CTRL_STATE_T   ctrlState;

	if(g_usbHost.control.descriptor.device.iSerialNumber)
	{
    	/* Prepare to Get Configuration descriptor */
	    if (g_usbHost.control.xferState == USBH_XFER_START)
	    {
	        /* Config Get Descriptor request */
	        USBH_StandardGetDescriptor(USBH_DESC_STRING,
	                                   g_usbHost.control.descriptor.device.iSerialNumber,
	                                   0xFF);
	        g_usbHost.control.xferState = USBH_XFER_WAITING;
	    }
	    /* Do the transaction */
	    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
	    {
	        ctrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData,
	                                            (uint8_t*)&g_usbHost.control.descriptor.stringBuf);

	        switch (ctrlState)
	        {
	            case USBH_CTRL_COMPLETE:
	                g_usbHost.control.xferState = USBH_XFER_START;

	                /* Parse String Descriptor */
	                USBH_ParseStrDescriptor(g_usbHost.control.descriptor.stringBuf, s_stringBuf);

	                g_userCallback.serialNumStringHandler(s_stringBuf);

	                USBH_ConfigEnumState(USBH_ENUM_SET_CONFIGURATION);
	                break;
	            case USBH_CTRL_STALL :
	            case USBH_CTRL_ERROR :
	                g_usbHost.control.xferState = USBH_XFER_START;
	                break;
	            default :
	                break;
	        }
	    }
    }
	else
	{
		g_usbHost.control.xferState = USBH_XFER_START;
		USBH_ConfigEnumState(USBH_ENUM_SET_CONFIGURATION);
	}
}

/*!
 * @brief     Handle Enum Set Configuration Stage.
 *
 * @param     None
 *
 * @retval    None
 */
static void ENUM_SetConfigurationHandler(void)
{
    USBH_CTRL_STATE_T   ctrlState;

    /* Prepare to Set Configuration  */
    if (g_usbHost.control.xferState == USBH_XFER_START)
    {
        USBH_StandardSetConfiguration(g_usbHost.control.descriptor.configuration.bConfigurationValue);
        g_usbHost.control.xferState = USBH_XFER_WAITING;
    }
    /* Do the transaction */
    else if (g_usbHost.control.xferState == USBH_XFER_WAITING)
    {
        ctrlState = USBH_ControlXferHandler(&g_usbHost.control.reqData, NULL);

        switch (ctrlState)
        {
            case USBH_CTRL_COMPLETE:
                g_usbHost.control.xferState = USBH_XFER_START;

                g_userCallback.enumDoneHandler();
                g_usbHost.control.enumState = USBH_ENUM_CONFIGURED_OK;
                break;
            case USBH_CTRL_STALL :
            case USBH_CTRL_ERROR :
                g_usbHost.control.xferState = USBH_XFER_START;
                break;
            default :
                break;
        }
    }
}

/**@} end of group Enum_Functions */
/**@} end of group Enum */
/**@} end of group Standard */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */
