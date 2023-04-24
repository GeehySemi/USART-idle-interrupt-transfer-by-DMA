/*!
 * @file        usbh_core.c
 *
 * @brief       USB host core function
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
#include "usbh_core.h"
#include "usbh_enum.h"
#include "usbh_dataXfer.h"
#include "usbh_init.h"
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

/** @addtogroup Core
  @{
*/

/** @defgroup Core_Variables Variables
  @{
*/

/* USB Host infomation */
USBH_Info_T g_usbHost;

/**@} end of group Core_Variables */

/** @defgroup Core_Functions Functions
  @{
*/

static void USBH_IdleHandler(void);
static void USBH_DevAttachedHandler(void);
static void USBH_DevDetachedHandler(void);
static void USBH_EnumHandler(void);
static void USBH_UserInputHandler(void);
static void USBH_ClassReqHandler(void);
static void USBH_ClassHandler(void);
static void USBH_SuspendHandler(void);
static void USBH_WakeUpHandler(void);
static void USBH_ErrorHandler(void);

static void ErrorManage(USBH_ERROR_STATE_T error);

/**@} end of group Core_Functions */

/** @defgroup Core_Variables Variables
  @{
*/

/* USB Host State Handler function */
static USBH_CoreHandler_T USBH_CoreHandler[] =
{
    USBH_IdleHandler,        /*!< Match with USBH_HOST_IDLE */
    USBH_DevAttachedHandler, /*!< Match with USBH_HOST_DEVICE_ATTACHED */
    USBH_DevDetachedHandler, /*!< Match with USBH_HOST_DEVICE_DETACHED */
    USBH_EnumHandler,        /*!< Match with USBH_HOST_ENUM */
    USBH_UserInputHandler,   /*!< Match with USBH_HOST_USER_INPUT */
    USBH_ClassReqHandler,    /*!< Match with USBH_HOST_CLASS_REQ */
    USBH_ClassHandler,       /*!< Match with USBH_HOST_CLASS */
    USBH_SuspendHandler,     /*!< Match with USBH_HOST_SUSPEND */
    USBH_WakeUpHandler,      /*!< Match with USBH_HOST_WAKE_UP */
    USBH_ErrorHandler        /*!< Match with USBH_HOST_ERROR */
};

/**@} end of group Core_Variables */

/** @defgroup Core_Functions Functions
  @{
*/

/*!
 * @brief     Host state machine polling Process
 *
 * @param     None
 *
 * @retval    None
 */
void USBH_PollingProcess(void)
{
    if (!(g_usbHost.connectedFlag || g_usbHost.portEnable) && \
            (USBH_ReadCurHostState() != USBH_HOST_IDLE))
    {
        USBH_ConfigHostState(USBH_HOST_DEVICE_DETACHED);
    }

    USBH_CoreHandler[g_usbHost.curHostState]();
}

/*!
 * @brief     Host idle state handler
 *
 * @param     None
 *
 * @retval    None
 */
static void USBH_IdleHandler(void)
{
    /* Check attached state */
    if (g_usbHost.connectedFlag)
    {
        g_usbHost.curHostState = USBH_HOST_DEVICE_ATTACHED;

        g_userCallback.devAttachedHandler();

        USB_Delay_us(100 * 1000);
    }
}

/*!
 * @brief     Host Attached state handler
 *
 * @param     None
 *
 * @retval    None
 */
static void USBH_DevAttachedHandler(void)
{
    /* Get free channel */
    g_usbHost.control.outChannelNum = USBH_CH_AllocChannel(0x00);
    g_usbHost.control.inChannelNum = USBH_CH_AllocChannel(0x80);

    /* Reset Port */
    USB_OTG_PortReset();

    g_userCallback.resetDevHandler();

    g_usbHost.errorState = USBH_ERROR_NO;

    g_usbHost.speed = USB_OTG_ReadPortSpeed();

    g_userCallback.devSpeedDetectedHandler(g_usbHost.speed);

    /* Open and Config out channel */
    USBH_CH_OpenChannel(g_usbHost.control.outChannelNum,
                        0,
                        USB_EP_TYPE_CONTROL,
                        g_usbHost.control.maxPackSize);

    /* Open and Config in channel */
    USBH_CH_OpenChannel(g_usbHost.control.inChannelNum,
                        0,
                        USB_EP_TYPE_CONTROL,
                        g_usbHost.control.maxPackSize);

    /* Prepare to handle enum */
    g_usbHost.curHostState = USBH_HOST_ENUM;
    g_usbHost.control.enumState = USBH_ENUM_IDLE;
}

/*!
 * @brief     Host Detached state handler
 *
 * @param     None
 *
 * @retval    None
 */
static void USBH_DevDetachedHandler(void)
{
    g_userCallback.devDetachedHandler();

    /* Config Global Interrupt */
    USB_OTG_G->GINTMASK = 0;       /*!< Disable all interrupt */
    USB_OTG_G->GINT = 0xFFFFFFFF;  /*!< Clear OTG flag */
    USB_OTG_G->GCINT = 0xFFFFFFFF; /*!< Clear global interrupt flag */

    /* Get free channel */
    USBH_CH_FreeAllChannel();

    /* stop host */
    USB_OTG_StopHost();

    /* clear relevant State */
    USBH_ConfigHostState(USBH_HOST_IDLE);
    USBH_ConfigDeviceAddr(USBH_DEVICE_DEFAULT_ADDRESS);

    g_usbHost.control.maxPackSize = USB_EP0_PACKET_SIZE;
    g_usbHost.control.ctrlState = USBH_CTRL_SETUP;
    g_usbHost.control.enumState = USBH_ENUM_IDLE;
    g_usbHost.control.xferState = USBH_XFER_START;

    USBH_DeInit();

    USB_OTG_DisableAllGlobalInterrupt();

    /* USB Global Init */
    USB_GlobalInit();

    /* USB Host Init */
    USB_HostInit();

    /* Enable USB Global interrupt */
    USB_OTG_EnableAllGlobalInterrupt();
}

/*!
 * @brief     Host Enum state handler
 *
 * @param     None
 *
 * @retval    None
 */
static void USBH_EnumHandler(void)
{
    /* Hander enum function */
    ENUM_CoreHandler[g_usbHost.control.enumState]();

    if (g_usbHost.control.enumState == USBH_ENUM_CONFIGURED_OK)
    {
        USBH_ConfigHostState(USBH_HOST_USER_INPUT);
    }
}

/*!
 * @brief     Through User input to go to class.
 *
 * @param     None
 *
 * @retval    None
 */
static void USBH_UserInputHandler(void)
{
    if (g_userCallback.userInputHandler() == USER_OK)
    {
        g_usbHost.classInitHandler();
    }
}

/*!
 * @brief     Host Class Request state handler
 *
 * @param     None
 *
 * @retval    None
 */
static void USBH_ClassReqHandler(void)
{
    g_usbHost.classReqHandler();

    if (g_usbHost.errorState != USBH_ERROR_NO)
    {
        ErrorManage(g_usbHost.errorState);
    }
}

/*!
 * @brief     Host Class state handler
 *
 * @param     None
 *
 * @retval    None
 */
static void USBH_ClassHandler(void)
{
    g_usbHost.classCoreHandler();

    if (g_usbHost.errorState != USBH_ERROR_NO)
    {
        ErrorManage(g_usbHost.errorState);
    }
}

/*!
 * @brief     Host Suspend state handler
 *
 * @param     None
 *
 * @retval    None
 */
static void USBH_SuspendHandler(void)
{
    if (g_usbHost.suspendHandler)
    {
        g_usbHost.suspendHandler();
    }
}

/*!
 * @brief     Host WakeUp state handler
 *
 * @param     None
 *
 * @retval    None
 */
static void USBH_WakeUpHandler(void)
{
    if (g_usbHost.wakeUpHandler)
    {
        g_usbHost.wakeUpHandler();
    }
}

/*!
 * @brief     Host Error state handler
 *
 * @param     None
 *
 * @retval    None
 */
static void USBH_ErrorHandler(void)
{
    USBH_DeInit();
}

/*!
 * @brief     Host Error management
 *
 * @param     error : Host Error state
 *
 * @retval    None
 */
static void ErrorManage(USBH_ERROR_STATE_T error)
{
    switch (error)
    {
        case USBH_ERROR_NOT_SUPPORT:
            g_userCallback.deviceNotSupportedHandler();
            break;
        case USBH_ERROR_UNRECOVERED:
            g_userCallback.unrecoveredErrHandler();
            break;
        case USBH_APP_RESET:
            break;
        default :
            break;
    }

    USBH_ConfigHostState(USBH_HOST_ERROR);
}

/**@} end of group Core_Functions */
/**@} end of group Core */
/**@} end of group Standard */
/**@} end of group Core_Host */
/**@} end of group USB_OTG_Library */
