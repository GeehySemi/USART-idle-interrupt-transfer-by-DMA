/*!
 * @file        drv_usb_device.h
 *
 * @brief       This file provides usb device driver of register
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
#ifndef __DRV_USB_DEVICE_H
#define __DRV_USB_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "drv_usb.h"

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Driver
  @{
*/

/** @addtogroup USB_Device
  @{
*/

/** @defgroup USB_Device_Macros Macros
  @{
*/

/*!
 ***************************
 *         Device          *
 ***************************
 * @{
 */

/*!
 * @brief     Config Device Speed.
 *
 * @param     speed : specific Device Speed.
 *                    This parameter can be one of the following values:
 *                    @arg USB_DEV_SPEED_FS
 *                    @arg USB_DEV_SPEED_HS
 *
 * @retval    None
 */
#define USB_OTG_ConfigDeviceSpeed(speed)       (USB_OTG_D->DCFG_B.DSPDSEL = speed & 0x3)

/*!
 * @brief     Config Device Address.
 *
 * @param     addr : specific Device Address.
 *                    This parameter can a 7-bit value.
 *
 * @retval    None
 */
#define USB_OTG_ConfigDeviceAddress(addr)       (USB_OTG_D->DCFG_B.DADDR = (uint8_t)(addr & 0x7F))

/*!
 * @brief     Config USB device Periodic frame interval.
 *
 * @param     interval: specific Device Address.
 *                     This parameter can be one of the following values:
 *                     @arg USB_FRAME_INTERVAL_80
 *                     @arg USB_FRAME_INTERVAL_85
 *                     @arg USB_FRAME_INTERVAL_90
 *                     @arg USB_FRAME_INTERVAL_95
 *
 * @retval    None
 */
#define USB_OTG_ConfigPeriodicFrameInterval(interval)  (USB_OTG_D->DCFG_B.PFITV = interval & 0x3)

/*!
 * @brief     Config USB Turnaround Time.
 *
 * @param     num: specific Turnaround Time number.
 *                 This parameter can be a 4-bit value.
 *
 * @retval    None
 */
#define USB_OTG_ConfigTurnaroundTime(num)              (USB_OTG_G->GUSBCFG_B.TRTIM = (num) & 0xF)

/*!
 * @brief     Enale USB Remote Wakeup.
 *
 * @param     None
 *
 * @retval    None
 */
#define USB_OTG_EnableRemoteWakeupSignal()             (USB_OTG_D->DCTRL_B.RWKUPS = BIT_SET)

/*!
 * @brief     Disale USB Remote Wakeup.
 *
 * @param     None
 *
 * @retval    None
 */
#define USB_OTG_DisableRemoteWakeupSignal()            (USB_OTG_D->DCTRL_B.RWKUPS = BIT_RESET)

/*!
 * @}
 */


/*!
 ***************************
 *         Endpoint        *
 ***************************
 * @{
 */

/*!
 * @brief     Read OUT endpoint Stall status
 *
 * @param     ep : endpiont number except EP0
 *
 * @retval    OUT endpoint Stall status
 */
#define USB_OTG_ReadOutEpStallStatus(ep)   (USB_OTG_D->EP_OUT[ep].DOEPCTRL_B.STALLH)

/*!
 * @brief     Read IN endpoint Stall status
 *
 * @param     ep : endpiont number except EP0
 *
 * @retval    IN endpoint Stall status
 */
#define USB_OTG_ReadInEpStallStatus(ep)   (USB_OTG_D->EP_IN[ep].DIEPCTRL_B.STALLH)

/*!
 * @brief     Enable OUT endpoint the whole interrupt mask
 *
 * @param     ep : OUT endpiont number
 *
 * @retval    None
 */
#define USB_OTG_EnableOutEpInterruptMask(ep)   (USB_OTG_D->DAEPIMASK_B.AOUTM |= (0x0001 << ep))

/*!
 * @brief     Disable OUT endpoint the whole interrupt mask
 *
 * @param     ep : OUT endpiont number
 *
 * @retval    None
 */
#define USB_OTG_DisableOutEpInterruptMask(ep)  (USB_OTG_D->DAEPIMASK_B.AOUTM &= ~(0x0001 << ep))

/*!
 * @brief     Enable IN endpoint the whole interrupt mask
 *
 * @param     ep : IN endpiont number
 *
 * @retval    None
 */
#define USB_OTG_EnableInEpInterruptMask(ep)    (USB_OTG_D->DAEPIMASK_B.AINM |= (0x0001 << ep))

/*!
 * @brief     Disable IN endpoint the whole interrupt mask
 *
 * @param     ep : IN endpiont number
 *
 * @retval    None
 */
#define USB_OTG_DisableInEpInterruptMask(ep)   (USB_OTG_D->DAEPIMASK_B.AINM &= ~(0x0001 << ep))

/*!
 * @brief     Enable the OUT endpoint interrupt.
 *
 * @param     interrupt: out endpoint interrupt flag.
 *                     This parameter can be any combination of the following value:
 *                     @arg USB_INT_EP_OUT_TSFCMP : Transfer completed.
 *                     @arg USB_INT_EP_OUT_EPDIS  : Endpoint disabled.
 *                     @arg USB_INT_EP_OUT_SETPCMP: setup completed.
 *                     @arg USB_INT_EP_OUT_RXOTDIS: OUT token received when endpoint disabled.
 *                     @arg USB_INT_EP_OUT_RXBSP  : Received Back-to-back SETUP packets over 3.
 *
 * @retval    None
 */
#define USB_OTG_EnableOutEpInterrupt(interrupt)  (USB_OTG_D->DOUTIMASK |= (interrupt))

/*!
 * @brief     Disable the OUT endpoint interrupt.
 *
 * @param     interrupt: out endpoint interrupt flag.
 *                     This parameter can be any combination of the following value:
 *                     @arg USB_INT_EP_OUT_TSFCMP : Transfer completed.
 *                     @arg USB_INT_EP_OUT_EPDIS  : Endpoint disabled.
 *                     @arg USB_INT_EP_OUT_SETPCMP: setup completed.
 *                     @arg USB_INT_EP_OUT_RXOTDIS: OUT token received when endpoint disabled.
 *                     @arg USB_INT_EP_OUT_RXBSP  : Received Back-to-back SETUP packets over 3.
 *                     @arg USB_INT_EP_OUT_ALL    : all of OUT EP interrupt.
 *
 * @retval    None
 */
#define USB_OTG_DisableOutEpInterrupt(interrupt)  (USB_OTG_D->DOUTIMASK &= ~(interrupt))

/*!
 * @brief     Enable the IN endpoint interrupt.
 *
 * @param     interrupt: IN EP interrupt flag.
 *                     This parameter can be any combination of the following value:
 *                     This parameter can any combination of the following value:
 *                     @arg USB_INT_EP_IN_TSFCMP  : Transfer completed.
 *                     @arg USB_INT_EP_IN_EPDIS   : Endpoint disabled.
 *                     @arg USB_INT_EP_IN_TO      : Timeout.
 *                     @arg USB_INT_EP_IN_ITXEMP  : IN token received when TxFIFO is empty.
 *                     @arg USB_INT_EP_IN_IEPNAKE : IN endpoint NAK effective.
 *                     @arg USB_INT_EP_IN_TXFE    : TxFIFO empty.
 *
 * @retval    None
 */
#define USB_OTG_EnableInEpInterrupt(interrupt)  (USB_OTG_D->DINIMASK |= (interrupt))

/*!
 * @brief     Disable the IN endpoint interrupt.
 *
 * @param     interrupt: IN endpoint interrupt flag.
 *                     This parameter can any combination of the following value:
 *                     @arg USB_INT_EP_IN_TSFCMP  : Transfer completed.
 *                     @arg USB_INT_EP_IN_EPDIS   : Endpoint disabled.
 *                     @arg USB_INT_EP_IN_TO      : Timeout.
 *                     @arg USB_INT_EP_IN_ITXEMP  : IN token received when TxFIFO is empty.
 *                     @arg USB_INT_EP_IN_IEPNAKE : IN endpoint NAK effective.
 *                     @arg USB_INT_EP_IN_TXFE    : TxFIFO empty.
 *                     @arg USB_INT_EP_IN_ALL     : All of IN endpoint interrupt.
 *
 * @retval    None
 */
#define USB_OTG_DisableInEpInterrupt(interrupt)  (USB_OTG_D->DINIMASK &= ~(interrupt))


/*!
 * @brief     Enable IN endpoint FIFO empty interrupt
 *
 * @param     ep : IN endpiont number
 *
 * @retval    None
 */
#define USB_OTG_EnableInEpEmptyInterrupt(ep)        (USB_OTG_D->DIEIMASK |= (uint32_t)(1 << ep))

/*!
 * @brief     Disable IN endpoint FIFO empty interrupt
 *
 * @param     ep : IN endpiont number
 *
 * @retval    None
 */
#define USB_OTG_DisableInEpEmptyInterrupt(ep)        (USB_OTG_D->DIEIMASK &= ~((uint32_t)(1 << ep)))

/*!
 * @brief     Read OUT endpoint interrupt number
 *
 * @param     None
 *
 * @retval    OUT endpoint interrupt number, each endpoint represents a bit
 */
#define USB_OTG_ReadOutEpIntNumber()                 ((uint16_t)(USB_OTG_D->DAEPINT >> 16))

/*!
 * @brief     Read IN endpoint interrupt number
 *
 * @param     None
 *
 * @retval    IN endpoint interrupt number, each endpoint represents a bit
 */
#define USB_OTG_ReadInEpIntNumber()                   ((uint16_t)(USB_OTG_D->DAEPINT))

/*!
 * @brief     Read the OUT endpoint interrupt status
 *
 * @param     ep : OUT endpiont number
 *
 * @param     flag: out ep interrupt flag.
 *                  This parameter can be any combination of the following value:
 *                  @arg USB_INT_EP_OUT_TSFCMP : Transfer completed.
 *                  @arg USB_INT_EP_OUT_EPDIS  : Endpoint disabled.
 *                  @arg USB_INT_EP_OUT_SETPCMP: setup completed.
 *                  @arg USB_INT_EP_OUT_RXOTDIS: OUT token received when endpoint disabled.
 *                  @arg USB_INT_EP_OUT_RXBSP  : Received Back-to-back SETUP packets over 3.
 *                  @arg USB_INT_EP_OUT_ALL    : all interrupt above.
 *
 * @retval    return Out endpoint interrupt status
 */
#define USB_OTG_ClearOutEpIntStatus(ep, interrupt)    (USB_OTG_D->EP_OUT[ep].DOEPINT |= (interrupt))

/*!
 * @brief     Read the IN endpoint interrupt status
 *
 * @param     ep : IN endpiont number
 *
 * @param     flag : IN ep interrupt flag.
 *                   This parameter can any combination of the following value:
 *                   @arg USB_INT_EP_IN_TSFCMP  : Transfer completed.
 *                   @arg USB_INT_EP_IN_EPDIS   : Endpoint disabled.
 *                   @arg USB_INT_EP_IN_TO      : Timeout.
 *                   @arg USB_INT_EP_IN_ITXEMP  : IN token received when TxFIFO is empty.
 *                   @arg USB_INT_EP_IN_IEPNAKE : IN endpoint NAK effective.
 *                   @arg USB_INT_EP_IN_TXFE    : TxFIFO empty.
 *                   @arg USB_INT_EP_IN_ALL     : all interrupt above.
 *
 * @retval    return Out endpoint interrupt status
 */
#define USB_OTG_ClearInEpIntStatus(ep, interrupt)      (USB_OTG_D->EP_IN[ep].DIEPINT |= (interrupt))

/**@} end of group USB_Device_Macros */

/** @defgroup USB_Device_Enumerates Enumerates
  @{
*/

/**
 * @brief   USB Device speed
 */
typedef enum
{
    USB_DEV_SPEED_HS = 0,
    USB_DEV_SPEED_FS = 3
} USB_DEV_SPEED_T;

/**
 * @brief   USB Periodic frame interval
 */
typedef enum
{
    USB_FRAME_INTERVAL_80,
    USB_FRAME_INTERVAL_85,
    USB_FRAME_INTERVAL_90,
    USB_FRAME_INTERVAL_95
} USB_FRAME_INTERVAL_T;

/**
 * @brief   Endpoint id
 */
typedef enum
{
    USB_EP_0,
    USB_EP_1,
    USB_EP_2,
    USB_EP_3
} USB_EP_T;

/**
 * @brief   USB OUT endpoint interrupt
 */
typedef enum
{
    USB_INT_EP_OUT_TSFCMP  = (uint32_t)BIT0, /*!< Transfer completed */
    USB_INT_EP_OUT_EPDIS   = (uint32_t)BIT1, /*!< Endpoint disabled */
    USB_INT_EP_OUT_SETPCMP = (uint32_t)BIT3, /*!< setup completed */
    USB_INT_EP_OUT_RXOTDIS = (uint32_t)BIT4, /*!< OUT token received when endpoint disabled */
    USB_INT_EP_OUT_RXBSP   = (uint32_t)BIT6, /*!< Received Back-to-back SETUP packets over 3 */
    USB_INT_EP_OUT_ALL     = (uint32_t)(0xFF)
} USB_INT_EP_OUT_T;

/**
 * @brief   USB IN endpoint interrupt
 */
typedef enum
{
    USB_INT_EP_IN_TSFCMP  = (uint32_t)BIT0, /*!< Transfer completed */
    USB_INT_EP_IN_EPDIS   = (uint32_t)BIT1, /*!< Endpoint disabled */
    USB_INT_EP_IN_TO      = (uint32_t)BIT3, /*!< Timeout */
    USB_INT_EP_IN_ITXEMP  = (uint32_t)BIT4, /*!< IN token received when TxFIFO is empty */
    USB_INT_EP_IN_IEPMM   = (uint32_t)BIT5, /*!< IN Token Received with Endpoint Mismatch */
    USB_INT_EP_IN_IEPNAKE = (uint32_t)BIT6, /*!< IN endpoint NAK effective */
    USB_INT_EP_IN_TXFE    = (uint32_t)BIT7, /*!< TxFIFO empty */
    USB_INT_EP_IN_ALL     = (uint32_t)(0xFF)
} USB_INT_EP_IN_T;

typedef enum
{
    FIFO_STATUS_G_OUT_NAK  = 1,
    FIFO_STATUS_OUT_DATA   = 2,
    FIFO_STATUS_OUT_COMP   = 3,
    FIFO_STATUS_SETUP_COMP = 4,
    FIFO_STATUS_SETUP_DATA = 6
} FIFO_STATUS_T;

/**@} end of group USB_Device_Enumerates */

/** @defgroup USB_Device_Structures Structures
  @{
*/

typedef struct
{
    union
    {
        __IO uint32_t word;

        struct
        {
            __IO uint32_t ep          : 4;
            __IO uint32_t byteCount   : 11;
            __IO uint32_t dataPid     : 2;
            __IO uint32_t status      : 4;
            __IO uint32_t frameNumber : 4;
        } bit;
    };
} USBD_FIFO_Status_T;

/**@} end of group USB_Device_Structures */

/** @defgroup USB_Device_Functions Functions
  @{
*/

void USB_OTG_ConfigDeviceTxFifo(uint8_t ep, uint16_t address, uint16_t depth);

/* Endpoint */
void USB_OTG_OutEpActive(uint8_t ep, USB_EP_TYPE_T epType, uint16_t packSize);
void USB_OTG_OutEpDeactive(uint8_t ep);
void USB_OTG_InEpActive(uint8_t ep, USB_EP_TYPE_T epType, \
                        uint8_t txFifoNum, uint32_t packSize);
void USB_OTG_InEpDeactive(uint8_t ep);
/* Endpoint Stall */
void USB_OTG_SetInEpStall(uint8_t ep);
void USB_OTG_ClearInEpStall(uint8_t ep);
void USB_OTG_SetOutEpStall(uint8_t ep);
void USB_OTG_ClearOutEpStall(uint8_t ep);
void USB_OTG_SetStall(uint8_t ep);
void USB_OTG_ClearStall(uint8_t ep);

/* Setup packet Transfer */
void USB_OTG_ReceiveSetupPacket(uint8_t packNum);

/* OUT/IN packet Transfer */
void USB_OTG_EnableOutEpTransfer(uint8_t ep, uint8_t packNum, uint32_t transferSize);
void USB_OTG_EnableInEpTransfer(uint8_t ep, uint8_t packNum, uint32_t transferSize);

uint8_t USB_OTG_ReadOutEpIntStatus(uint8_t ep, USB_INT_EP_OUT_T flag);
uint8_t USB_OTG_ReadInEpIntStatus(uint8_t ep, USB_INT_EP_IN_T flag);

/**@} end of group USB_Device_Functions */
/**@} end of group USB_Device */
/**@} end of group Driver */
/**@} end of group USB_OTG_Library */

#ifdef __cplusplus
}
#endif

#endif
