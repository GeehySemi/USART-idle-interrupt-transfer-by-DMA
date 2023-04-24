/*!
 * @file        drv_usb.c
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
#include "drv_usb.h"
#include "usb_user.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Driver
  @{
*/

/** @addtogroup USB
  @{
*/

/** @defgroup USB_Functions Functions
  @{
*/

/*!
 * @brief       Force USB to device ot host mode.
 *
 * @param       mode : Select mode.
 *                    This value can be one of the following values:
 *                    @arg USB_MODE_DEVICE : Force device mode.
 *                    @arg USB_MODE_HOST   : Force host mode.
 *
 * @retval      None
 */
void USB_OTG_ConfigMode(USB_MODE_T mode)
{
    if (mode == USB_MODE_HOST)
    {
        USB_OTG_G->GUSBCFG_B.FHMODE = BIT_SET;
    }
    else
    {
        USB_OTG_G->GUSBCFG_B.FDMODE = BIT_SET;
    }
}

/*!
 * @brief       Config HS PHY or FS Serial Transceiver.
 *
 * @param       phy : Select PHY.
 *                    This value can be one of the following values:
 *                    @arg USB_OTG_PHY_FS : FS Serial Transceiver
 *                    @arg USB_OTG_PHY_HS : HS PHY
 *
 * @retval      None
 */
void USB_OTG_ConfigPHY(USB_OTG_PHY_T phy)
{
    /* Select PHY  */
    USB_OTG_G->GUSBCFG_B.FSSTSEL = phy;
}

/*!
 * @brief       Enale USB Vbus Sensing.
 *
 * @param       None
 *
 * @retval      None
 */
void USB_OTG_EnableVbusSensing(void)
{
    USB_OTG_G->GGCCFG_B.VBSDIS = BIT_RESET;
}

/*!
 * @brief       Disale USB Vbus Sensing.
 *
 * @param       None
 *
 * @retval      None
 */
void USB_OTG_DisableVbusSensing(void)
{
    USB_OTG_G->GGCCFG_B.VBSDIS = BIT_SET;
}

/*!
 * @brief       Enale USB device Vbus Sensing.
 *
 * @param       type : Select "A" device or "B" device
 *                    This value can be one of the following values:
 *                    @arg USB_DEVICE_A : "A" device
 *                    @arg USB_DEVICE_B : "B" device
 *
 * @retval      None
 */
void USB_OTG_EnableDeviceVbus(USB_DEVICE_TYPE_T type)
{
    if (type == USB_DEVICE_A)
    {
        USB_OTG_G->GGCCFG_B.ADVBSEN = BIT_SET;
    }
    else
    {
        USB_OTG_G->GGCCFG_B.BDVBSEN = BIT_SET;
    }
}

/*!
 * @brief       Disale USB device Vbus Sensing.
 *
 * @param       type : Select "A" device or "B" device
 *                    This value can be one of the following values:
 *                    @arg USB_DEVICE_A : "A" device
 *                    @arg USB_DEVICE_B : "B" device
 *
 * @retval      None
 */
void USB_OTG_DisableDeviceVbus(USB_DEVICE_TYPE_T type)
{
    if (type == USB_DEVICE_A)
    {
        USB_OTG_G->GGCCFG_B.ADVBSEN = BIT_RESET;
    }
    else
    {
        USB_OTG_G->GGCCFG_B.BDVBSEN = BIT_RESET;
    }
}

/*!
 * @brief       Flush specific TxFIFO
 *
 * @param       num: TxFIFO number
 *
 * @retval      None
 */
void USB_OTG_FlushTxFIFO(uint8_t num)
{
    __IO uint32_t timeout = 0;

    /* Wait for AHB master idle */
    while ((USB_OTG_G->GRSTCTRL_B.AHBMIDL != BIT_SET) && \
            (USB_OTG_G->GRSTCTRL_B.TXFFLU != BIT_RESET))
    {
        if (++timeout > 0x200)
        {
            break;
        }
    }

    USB_OTG_G->GRSTCTRL_B.TXFNUM = num;
    USB_OTG_G->GRSTCTRL_B.TXFFLU = BIT_SET;

    timeout = 0;
    while (USB_OTG_G->GRSTCTRL_B.TXFFLU != BIT_RESET)
    {
        if (++timeout > 0x30000)
        {
            break;
        }
    }

    USB_Delay_us(3);
}

/*!
 * @brief       Flush RxFIFO
 *
 * @param       None
 *
 * @retval      None
 */
void USB_OTG_FlushRxFIFO(void)
{
    __IO uint32_t timeout = 0;

    USB_OTG_G->GRSTCTRL_B.RXFFLU = BIT_SET;
    while (USB_OTG_G->GRSTCTRL_B.RXFFLU != BIT_RESET)
    {
        if (++timeout > 0x30000)
        {
            break;
        }
    }

    USB_Delay_us(3);
}

/*!
 * @brief     Read the specified OTG interrupt flag.
 *
 * @param     USB_INT_OTG_T :point to USB_INT_OTG_T enum typedef
 *
 * @retval    Flag status. 1 to set and 0 to not set
 */
#if defined(__ICCARM__)
uint8_t USB_OTG_ReadOTGIntFlag(USB_INT_OTG_T flag)
{
    uint32_t temp1 = USB_OTG_G->GINT;
    uint8_t temp2 = USB_OTG_G->GINTMASK_B.OTGM;

    return (((temp1 & flag) ? SET : RESET) && temp2);
}
#endif

/*!
 * @brief     Read the specified Global interrupt flag
 *
 * @param     USB_INT_G_T: Point to USB_INT_G_T enum typedef
 *
 * @retval    Flag status. 1 to set and 0 to not set
 */
#if defined(__ICCARM__)
uint8_t USB_OTG_ReadGlobalIntFlag(USB_INT_G_T flag)
{
    uint32_t temp1 = USB_OTG_G->GCINT;
    uint32_t temp2 = USB_OTG_G->GINTMASK;

    return (((temp1 & temp2) & (uint32_t)(flag)) ? SET : RESET);
}
#endif

/*!
 * @brief       USB delay
 *
 * @param       cnt : delay counter
 *
 * @retval      None
 */
void USB_Delay_us(uint32_t cnt)
{
    if (g_userCallback.delay)
    {
        g_userCallback.delay(cnt);
    }
    else
    {
        __IO uint32_t i = 0;
        __IO uint32_t j = 0;
        __IO uint32_t delayCnt;

        delayCnt = (cnt * 6);

        do
        {
            i++;
            j++;
        }
        while (i < cnt);
    }
}

/**@} end of group USB_Functions */
/**@} end of group USB */
/**@} end of group Driver */
/**@} end of group USB_OTG_Library */
