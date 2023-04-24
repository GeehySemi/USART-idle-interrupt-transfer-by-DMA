/*!
 * @file        drv_usb.h
 *
 * @brief       This file provides usb driver of register
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
#ifndef __DRV_USB_H
#define __DRV_USB_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "usb_reg.h"
#include <stdio.h>

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Driver
  @{
*/

/** @addtogroup USB
  @{
*/

/** @defgroup USB_Macros Macros
  @{
*/

/*!
 ***************************
 *         Conmon          *
 ***************************
 * @{
 */

/*!
 * @brief     Enable SOF Output.
 *
 * @param     None
 *
 * @retval    None
 */
#define USB_OTG_EnableSofOutput()                      (USB_OTG_G->GGCCFG_B.SOFPOUT = BIT_SET)

/*!
 * @brief     Disable SOF Output.
 *
 * @param     None
 *
 * @retval    None
 */
#define USB_OTG_DisableSofOutput()                     (USB_OTG_G->GGCCFG_B.SOFPOUT = BIT_RESET)

/*!
 * @brief     Enable Power down.
 *
 * @param     None
 *
 * @retval    None
 */
#define USB_OTG_EnablePowerDown()                      (USB_OTG_G->GGCCFG_B.PWEN = BIT_RESET)

/*!
 * @brief     Enable Power down.
 *
 * @param     None
 *
 * @retval    None
 */
#define USB_OTG_DisablePowerDown()                     (USB_OTG_G->GGCCFG_B.PWEN = BIT_SET)

/*!
 * @brief     Read the AHB Master Idle flag.
 *
 * @param     None
 *
 * @retval    return AHBMasterIdle flag
 */
#define USB_OTG_ReadAHBMasterIdleFlag()                (USB_OTG_G->GRSTCTRL_B.AHBMIDL)

/*!
 * @brief     Enable Core soft reset.
 *
 * @param     None
 *
 * @retval    None
 */
#define USB_OTG_EnableCoreSoftReset()                  (USB_OTG_G->GRSTCTRL_B.CSRST = BIT_SET)

/*!
 * @brief     Read the Core soft reset flag.
 *
 * @param     None
 *
 * @retval    return Core soft reset flag
 */
#define USB_OTG_ReadCoreSoftResetFlag()                (USB_OTG_G->GRSTCTRL_B.CSRST)


/*!
 * @brief     Read the specified OTG interrupt flag.
 *
 * @param     USB_INT_OTG_T :point to USB_INT_OTG_T enum typedef
 *
 * @retval    Flag status. 1 to set and 0 to not set
 */
#if !defined(__ICCARM__)
#define USB_OTG_ReadOTGIntFlag(USB_INT_OTG_T)         (((USB_OTG_G->GINT & USB_INT_OTG_T) ? SET : RESET) && USB_OTG_G->GINTMASK_B.OTGM)
#endif

/*!
 * @brief     Clear the specified OTG interrupt flag(Write 1 to clear)
 *
 * @param     USB_INT_OTG_T :point to USB_INT_OTG_T enum typedef
 *
 * @retval    None
 */
#define USB_OTG_ClearOTGIntFlag(USB_INT_OTG_T)         (USB_OTG_G->GINT = (uint32_t)(USB_INT_OTG_T))

/*!
 ***************************
 *         RxFIFO          *
 ***************************
 * @{
 */

/*!
 * @brief     Config receive FIFO size
 *
 * @param     size : a FIFO size which is not more than 256
 *
 * @retval    None
 */
#define USB_OTG_ConfigRxFifoSize(size)           (OTG_FS_G->GRXFIFO_B.RXFDEP = size)

/*!
 * @brief     Pop the RxFIFO status with data
 *
 * @param     None
 *
 * @retval    Return RxFIFO status
 */
#define USB_OTG_PopRxFifoStatus()                 ((uint32_t)USB_OTG_G->GRXSTSP)

/*!
 ***************************
 *         TxFIFO          *
 ***************************
 * @{
 */

/*!
 * @brief     Read the Available space of TxFIFO
 *
 * @param     ep : IN EP number
 *
 * @retval    return Available space of TxFIFO
 */
#define USB_OTG_ReadTxFifoAvailable(ep)          (USB_OTG_D->EP_IN[ep].DITXFSTS)

/*!
 * @brief     Config Depth of RxFifo
 *
 * @param     depth : value of depth
 *
 * @retval    None
 */
#define USB_OTG_ConfigRxFifoDepth(depth)         (USB_OTG_G->GRXFIFO_B.RXFDEP = (uint16_t)depth)

/*!
 ***************************
 *    Global Interrupt     *
 ***************************
 * @{
 */

/*!
 * @brief     Enable All the Global interrupt
 *
 * @param     None
 *
 * @retval    None
 */
#define USB_OTG_EnableAllGlobalInterrupt()            (USB_OTG_G->GAHBCFG_B.GINTMASK = BIT_SET)

/*!
 * @brief     Disable All the Global interrupt
 *
 * @param     None
 *
 * @retval    None
 */
#define USB_OTG_DisableAllGlobalInterrupt()           (USB_OTG_G->GAHBCFG_B.GINTMASK = BIT_RESET)

/*!
 * @brief       Enable the specified Global interrupt
 *
 * @param       USB_INT_G_T: Point to USB_INT_G_T enum typedef
 *
 * @retval      None
 */
#define USB_OTG_EnableGlobalInterrupt(USB_INT_G_T)     (USB_OTG_G->GINTMASK |= (uint32_t)(USB_INT_G_T))

/*!
 * @brief     Disable the specified Global interrupt
 *
 * @param     USB_INT_G_T: Point to USB_INT_G_T enum typedef
 *
 * @retval    None
 */
#define USB_OTG_DisableGlobalInterrupt(USB_INT_G_T)    (USB_OTG_G->GINTMASK &= (~(uint32_t)(USB_INT_G_T)))

/*!
 * @brief     Read the specified Global interrupt flag
 *
 * @param     USB_INT_G_T: Point to USB_INT_G_T enum typedef
 *
 * @retval    Flag status. 1 to set and 0 to not set
 */
#if !defined(__ICCARM__)
#define USB_OTG_ReadGlobalIntFlag(USB_INT_G_T)        (((USB_OTG_G->GCINT & USB_OTG_G->GINTMASK) & (uint32_t)(USB_INT_G_T)) ? SET : RESET)
#endif

/*!
 * @brief     Clear the specified Global interrupt flag(Write 1 to clear)
 *
 * @param     USB_INT_G_T: Point to USB_INT_G_T enum typedef
 *
 * @retval    None
 */
#define USB_OTG_ClearGlobalIntFlag(USB_INT_G_T)       (USB_OTG_G->GCINT = (uint32_t)(USB_INT_G_T))

/**@} end of group USB_Macros */

/** @defgroup USB_Enumerates Enumerates
  @{
*/

/**
 * @brief   USB Device mode
 */
typedef enum
{
    USB_MODE_DEVICE,
    USB_MODE_HOST
} USB_MODE_T;

/**
 * @brief   USB PHY
 */
typedef enum
{
    USB_OTG_PHY_HS,
    USB_OTG_PHY_FS
} USB_OTG_PHY_T;

/**
 * @brief   USB "A" device or "B" device
 */
typedef enum
{
    USB_DEVICE_A,
    USB_DEVICE_B
} USB_DEVICE_TYPE_T;

/**
 * @brief   USBD Endpoint type
 */
typedef enum
{
    USB_EP_TYPE_CONTROL,
    USB_EP_TYPE_ISO,
    USB_EP_TYPE_BULK,
    USB_EP_TYPE_INTERRUPT
} USB_EP_TYPE_T;

/**
 * @brief   USB Global interrupt source
 */
typedef enum
{
    USB_INT_G_MMIS      = BIT1,           /*!< Mode mismatch interrupt */
    USB_INT_G_OTG       = BIT2,           /*!< OTG interrupt */
    USB_INT_G_SOF       = BIT3,           /*!< Frame Start of interrupt */
    USB_INT_G_RXFNONE   = BIT4,           /*!< RxFIFO non-empty interrupt */
    USB_INT_G_NPTXFEM   = BIT5,           /*!< Non-periodic TxFIFO empty interrupt */
    USB_INT_G_GINNPNAKE = BIT6,           /*!< Global IN non-periodic NAK effective interrupt */
    USB_INT_G_GONAKE    = BIT7,           /*!< Global OUT NAK effective interrupt */

    USB_INT_G_ESUS      = BIT10,          /*!< Early suspend interrupt */
    USB_INT_G_USBSUS    = BIT11,          /*!< USB suspend interrupt */
    USB_INT_G_USBRST    = BIT12,          /*!< USB reset interrupt */
    USB_INT_G_ENUMD     = BIT13,          /*!< Enumeration done interrupt */
    USB_INT_G_ISOPD     = BIT14,          /*!< Isochronous OUT packet dropped interrupt */
    USB_INT_G_EOPF      = BIT15,          /*!< End of periodic frame interrupt */

    USB_INT_G_INEP      = BIT18,          /*!< IN endpoint interrupt */
    USB_INT_G_ONEP      = BIT19,          /*!< OUT endpoint interrupt */
    USB_INT_G_IIINTX    = BIT20,          /*!< Incomplete isochronous IN transfer interrupt */

    USB_INT_G_IP_OUTTX  = BIT21,          /*!< Incomplete isochronous OUT transfer interrupt */

    USB_INT_G_HPORT     = BIT24,          /*!< Host port interrupt */
    USB_INT_G_HCHAN     = BIT25,          /*!< Host channels interrupt */
    USB_INT_G_PTXFE     = BIT26,          /*!< Periodic TxFIFO empty */

    USB_INT_G_CIDSTSTC  = BIT28,          /*!< Connector ID status change */
    USB_INT_G_DEDIS     = BIT29,          /*!< Disconnect detected interrupt */
    USB_INT_G_SREQ      = BIT30,          /*!< Session request/new session detected interrupt */
    USB_INT_G_RWAKE     = (int32_t)BIT31, /*!< Resume/remote wakeup detected interrupt */
    USB_INT_G_ALL       = (int32_t)0xFFFFFFFF
} USB_INT_G_T;

/**
 * @brief   USB OTG interrupt source
 */
typedef enum
{
    USB_INT_OTG_SEFLG       = BIT2,  /*!< Session end flag */
    USB_INT_OTG_SREQSUCCHG  = BIT8,  /*!< Session request success status change flag */
    USB_INT_OTG_HNSUCCHG    = BIT9,  /*!< Host negotiation success status change flag */
    USB_INT_OTG_HNFLG       = BIT17, /*!< Host negotiation flag */
    USB_INT_OTG_ADTOFLG     = BIT18, /*!< A-device timeout change flag */
    USB_INT_OTG_DEBDFLG     = BIT19, /*!< Debounce done flag */
    USB_INT_OTG_ALL         = (int32_t)0xFFFFFFFF
} USB_INT_OTG_T;

/**@} end of group USB_Enumerates */

/** @defgroup USB_Functions Functions
  @{
*/

void USB_OTG_ConfigMode(USB_MODE_T mode);

/* PHY */
void USB_OTG_ConfigPHY(USB_OTG_PHY_T phy);
void USB_OTG_EnableVbusSensing(void);
void USB_OTG_DisableVbusSensing(void);
void USB_OTG_EnableDeviceVbus(USB_DEVICE_TYPE_T type);
void USB_OTG_DisableDeviceVbus(USB_DEVICE_TYPE_T type);

/* FIFO */
void USB_OTG_FlushTxFIFO(uint8_t num);
void USB_OTG_FlushRxFIFO(void);

#if defined(__ICCARM__)
uint8_t USB_OTG_ReadOTGIntFlag(USB_INT_OTG_T flag);
uint8_t USB_OTG_ReadGlobalIntFlag(USB_INT_G_T flag);
#endif

void USB_Delay_us(uint32_t cnt);

/**@} end of group USB_Functions */
/**@} end of group USB */
/**@} end of group Driver */
/**@} end of group USB_OTG_Library */
#ifdef __cplusplus
}
#endif

#endif
