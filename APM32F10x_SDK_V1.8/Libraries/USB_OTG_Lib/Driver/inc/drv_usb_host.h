/*!
 * @file        drv_usb_host.h
 *
 * @brief       This file provides usb host driver of register
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
#ifndef __DRV_USB_HOST_H
#define __DRV_USB_HOST_H

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

/** @addtogroup USB_Host
  @{
*/

/** @defgroup USB_Host_Macros Macros
  @{
*/

#define HOST_CHANNEL_NUM  12  /*!<  Host channel number */

/* OTG_FS_HCFG  0x400 */
#define USB_OTG_H_ConfigHostClk(USB_H_PHYCLK_T)           (USB_OTG_H->HCFG_B.PHYCLKSEL = (USB_H_PHYCLK_T & 0x03))
#define USB_OTG_H_ReadHostClk()                           (USB_OTG_H->HCFG_B.PHYCLKSEL)

#define USB_OTG_H_EnableSupportHS()                       (USB_OTG_H->HCFG_B.FSSPT = 0)
#define USB_OTG_H_DisableSupportHS()                      (USB_OTG_H->HCFG_B.FSSPT = 1)
#define USB_OTG_H_IsNotSupportHS()                        ((USB_OTG_H->HCFG_B.FSSPT) ? SET : RESET)

/* OTG_FS_HFIVL  0x404 */
#define USB_OTG_H_SetFrameInterval(intervalValue)         (USB_OTG_H->HFIVL_B.FIVL = (intervalValue & 0XFFFF))
#define USB_OTG_H_ReadFrameInterval()                     (USB_OTG_H->HFIVL_B.FIVL)

/* OTG_HS1_HFIFM  0x408 */
#define USB_OTG_H_ReadFrameNum()                          (USB_OTG_H->HFIFM_B.FNUM)
#define USB_OTG_H_ReadFrameRemainTime()                   (USB_OTG_H->HFIFM_B.FRTIME)

/* OTG_HS1_HPTXSTS  0x410 */
#define USB_OTG_H_ReadTxFIFORemainSpace()                 (USB_OTG_H->HPTXSTS_B.FSPACE)
#define USB_OTG_H_ReadTxQueueRemainSpace()                (USB_OTG_H->HPTXSTS_B.QSPACE)
#define USB_OTG_H_TxQueueIsOddFrame()                     (((USB_OTG_H->HPTXSTS_B.QTOP) & 0x80) ? SET : RESET)
#define USB_OTG_H_TxQueueSerialNum()                      (((USB_OTG_H->HPTXSTS_B.QTOP) >>3)& 0x0F)
#define USB_OTG_H_TxQueueType()                           (USB_H_QUEUE_T)(((USB_OTG_H->HPTXSTS_B.QTOP) >>1)& 0x03)

/* OTG_HS1_HACHINT  0x414 */
#if !defined(__ICCARM__)
#define USB_OTG_H_ReadAllChannelsIntFlag(USB_H_CHANNEL_T) (((USB_OTG_H->HACHINT & USB_OTG_H->HACHIMASK) & (uint32_t)(USB_H_CHANNEL_T)) ? SET : RESET)
#endif

/* OTG_HS1_HACHIMASK 0x418 */
#define USB_OTG_H_EnableAllChannelsInt(USB_H_CHANNEL_T)   (USB_OTG_H->HACHIMASK_B.ACHIMASK |= (USB_H_CHANNEL_T))
#define USB_OTG_H_DisableAllChannelsInt(USB_H_CHANNEL_T)  (USB_OTG_H->HACHIMASK_B.ACHIMASK &= (~((uint32_t)USB_H_CHANNEL_T)))

/* OTG_HS1_HPORTCSTS  0x440 */
#define USB_OTG_H_PortIsConnect()                         ((USB_OTG_H->HPORTCSTS_B.PCNNTFLG) ? SET : RESET)
#define USB_OTG_H_ReadPortConnectIntTriFlag()             (USB_OTG_H->HPORTCSTS_B.PCINTFLG)
#define USB_OTG_H_ClearPortConnectIntTriFlag()            (USB_OTG_H->HPORTCSTS_B.PCINTFLG = 1)

#define USB_OTG_H_EnablePort()                            (USB_OTG_H->HPORTCSTS_B.PEN = 1)
#define USB_OTG_H_DisablePort()                           (USB_OTG_H->HPORTCSTS_B.PEN = 0)
#define USB_OTG_H_PortIsEnable()                          ((USB_OTG_H->HPORTCSTS_B.PEN) ? SET : RESET)
#define USB_OTG_H_PenBitIsChange()                        ((USB_OTG_H->HPORTCSTS_B.PENCHG) ? SET : RESET)
#define USB_OTG_H_ClearPenBitChangeFlag()                 (USB_OTG_H->HPORTCSTS_B.PENCHG = 1)

#define USB_OTG_H_PortIsOvercurrent()                     ((USB_OTG_H->HPORTCSTS_B.POVC) ? SET : RESET)
#define USB_OTG_H_PovcBitIsChange()                       ((USB_OTG_H->HPORTCSTS_B.POVCCHG) ? SET : RESET)
#define USB_OTG_H_ClearPovcBitChangeFlag()                (USB_OTG_H->HPORTCSTS_B.POVCCHG = 1)

#define USB_OTG_H_EnablePortResume()                      (USB_OTG_H->HPORTCSTS_B.PRS = 1)
#define USB_OTG_H_DisablePortResume()                     (USB_OTG_H->HPORTCSTS_B.PRS = 0)
#define USB_OTG_H_ReadPortResumeFlag()                    (USB_OTG_H->HPORTCSTS_B.PRS)

#define USB_OTG_H_EnablePortSuspend()                     (USB_OTG_H->HPORTCSTS_B.PSUS = 1)
#define USB_OTG_H_DisablePortSuspend()                    (USB_OTG_H->HPORTCSTS_B.PSUS = 0)
#define USB_OTG_H_PortIsSuspend()                         ((USB_OTG_H->HPORTCSTS_B.PSUS)? SET : RESET)

#define USB_OTG_H_EnablePortReset()                       (USB_OTG_H->HPORTCSTS_B.PRST = 1)
#define USB_OTG_H_DisablePortReset()                      (USB_OTG_H->HPORTCSTS_B.PRST = 0)

#define USB_OTG_H_PortDataLineStatus()                    (USB_OTG_H->HPORTCSTS_B.PDLSTS)

#define USB_OTG_H_EnablePortPower()                       (USB_OTG_H->HPORTCSTS_B.PP = 1)
#define USB_OTG_H_DisablePortPower()                      (USB_OTG_H->HPORTCSTS_B.PP = 0)
#define USB_OTG_H_ReadPortPowerFlag()                     (USB_OTG_H->HPORTCSTS_B.PP)

#define USB_OTG_H_ConfigPortTestMode(USB_H_PORT_TEST_T)   (USB_OTG_H->HPORTCSTS_B.PTSEL = (USB_H_PORT_TEST_T & 0x0F))

#define USB_OTG_H_ReadPortSpeedType()                     (USB_OTG_H->HPORTCSTS_B.PSPDSEL)
#define USB_OTG_H_ConfigPortSpeedType(USB_H_SPEED_TYPE_T) (USB_OTG_H->HPORTCSTS_B.PSPDSEL = (USB_H_SPEED_TYPE_T & 0x03))

/* OTG_HS1_HCHX   0x500+20*X */
#define USB_OTG_H_ReadEpMaxPSize(chNum)                   (USB_OTG_H->REGS_HCH[chNum].HCH_B.MAXPSIZE)
#define USB_OTG_H_ConfigEpMaxPSize(chNum,maxSize)         (USB_OTG_H->REGS_HCH[chNum].HCH_B.MAXPSIZE = (maxSize & 0x7FF))

#define USB_OTG_H_ReadEpNum(chNum)                        (USB_OTG_H->REGS_HCH[chNum].HCH_B.EDPNUM)
#define USB_OTG_H_ConfigEpNum(chNum,epNum)                (USB_OTG_H->REGS_HCH[chNum].HCH_B.EDPNUM = (epNum & 0xF))

#define USB_OTG_H_ReadEpDirection(chNum)                  (USB_OTG_H->REGS_HCH[chNum].HCH_B.EDPDRT)
#define USB_OTG_H_ConfigEpDirection(chNum,USB_H_EP_DIR_T) (USB_OTG_H->REGS_HCH[chNum].HCH_B.EDPDRT = (USB_H_EP_DIR_T & 0x1))

#define USB_OTG_H_ReadLowSpeedDeviceFlag(chNum)           (USB_OTG_H->REGS_HCH[chNum].HCH_B.LSDV)
#define USB_OTG_H_EnableLowSpeedDevice(chNum)             (USB_OTG_H->REGS_HCH[chNum].HCH_B.LSDV = 1)
#define USB_OTG_H_DisableLowSpeedDevice(chNum)            (USB_OTG_H->REGS_HCH[chNum].HCH_B.LSDV = 0)

#define USB_OTG_H_ReadEpType(chNum)                       (USB_OTG_H->REGS_HCH[chNum].HCH_B.EDPTYP)
#define USB_OTG_H_ConfigEpType(chNum, USB_H_EP_TYPE_T)    (USB_OTG_H->REGS_HCH[chNum].HCH_B.EDPTYP = (USB_H_EP_TYPE_T & 0x03))

#define USB_OTG_H_ConfigCNTSEL(chNum, value_1_3)          (USB_OTG_H->REGS_HCH[chNum].HCH_B.CNTSEL = (value_1_3 & 0x03))

#define USB_OTG_H_ReadDeviceAddr(chNum)                   (USB_OTG_H->REGS_HCH[chNum].HCH_B.DVADDR)
#define USB_OTG_H_ConfigDeviceAddr(chNum, deviceAddr)     (USB_OTG_H->REGS_HCH[chNum].HCH_B.DVADDR = (deviceAddr & 0x7F))

#define USB_OTG_H_TxUseOddFrame(chNum)                    (USB_OTG_H->REGS_HCH[chNum].HCH_B.ODDF = 1)
#define USB_OTG_H_TxUseEvenFrame(chNum)                   (USB_OTG_H->REGS_HCH[chNum].HCH_B.ODDF = 0)

#define USB_OTG_H_DisableChannel(chNum)                   (USB_OTG_H->REGS_HCH[chNum].HCH |= (BIT31|BIT30))
#define USB_OTG_H_ClearQueue(chNum)                       (USB_OTG_H->REGS_HCH[chNum].HCH_B.CHINT = 1)
#define USB_OTG_H_EnableChannel(chNum)                    (USB_OTG_H->REGS_HCH[chNum].HCH_B.CHEN = 1)

/* OTG_HS1_HCHSCTRLX    0x504+20*X */
#define USB_OTG_H_ReadPortAddr(chNum)                     (USB_OTG_H->REGS_HCH[chNum].HCHSCTRL_B.PADDR)
#define USB_OTG_H_ConfigPortAddr(chNum, portAddr)         (USB_OTG_H->REGS_HCH[chNum].HCHSCTRL_B.PADDR = (portAddr & 0x7F))

#define USB_OTG_H_ReadHubAddr(chNum)                      (USB_OTG_H->REGS_HCH[chNum].HCHSCTRL_B.HADDR)
#define USB_OTG_H_ConfigHubAddr(chNum, hubAddr)           (USB_OTG_H->REGS_HCH[chNum].HCHSCTRL_B.HADDR = (hubAddr & 0x7F))

#define USB_OTG_H_TransactionPosition(chNum, USB_H_POSITION_T)       (USB_OTG_H->REGS_HCH[chNum].HCHSCTRL_B.TPOST = (USB_H_POSITION_T & 0x03))

#define USB_OTG_H_DoCompleteSplit(chNum)                  (USB_OTG_H->REGS_HCH[chNum].HCHSCTRL_B.DO_CMP_SPL = 1)

#define USB_OTG_H_EnableSplit(chNum)                      (USB_OTG_H->REGS_HCH[chNum].HCHSCTRL_B.SPLEN = 1)
#define USB_OTG_H_DisableSplit(chNum)                     (USB_OTG_H->REGS_HCH[chNum].HCHSCTRL_B.SPLEN = 0)

/* OTG_HS1_HCHINTX  0x508+20*X */
#if !defined(__ICCARM__)
#define USB_OTG_H_ReadChannelAllIntFlag(chNum)                        (USB_OTG_H->REGS_HCH[chNum].HCHINT & USB_OTG_H->REGS_HCH[chNum].HCHIMASK)
#define USB_OTG_H_ReadChannelIntFlag(chNum, USB_H_CHANNEL_INT_T)      (((USB_OTG_H->REGS_HCH[chNum].HCHINT & USB_OTG_H->REGS_HCH[chNum].HCHIMASK) & \
                                                                        (uint32_t)(USB_H_CHANNEL_INT_T)) ? SET : RESET)
#endif

#define USB_OTG_H_ClearChannelIntFlag(chNum, USB_H_CHANNEL_INT_T)     (USB_OTG_H->REGS_HCH[chNum].HCHINT |= (USB_H_CHANNEL_INT_T))

/* OTG_HS1_HCHIMASKX  0x50C+20*X */
#define USB_OTG_H_EnableChannelInt(chNum, USB_H_CHANNEL_INT_T)        (USB_OTG_H->REGS_HCH[chNum].HCHIMASK |= (USB_H_CHANNEL_INT_T))
#define USB_OTG_H_DisableChannelInt(chNum, USB_H_CHANNEL_INT_T)       (USB_OTG_H->REGS_HCH[chNum].HCHIMASK &= (~(USB_H_CHANNEL_INT_T)))

/* OTG_HS1_HCHTSIZEX  0x510+20*X */
#define USB_OTG_H_ReadTransferSize(chNum)                  (USB_OTG_H->REGS_HCH[chNum].HCHTSIZE_B.TSFSIZE)
#define USB_OTG_H_ConfigTransferSize(chNum, size)          (USB_OTG_H->REGS_HCH[chNum].HCHTSIZE_B.TSFSIZE = (size & 0x7FFFF))

#define USB_OTG_H_ReadPacketCount(chNum)                   (USB_OTG_H->REGS_HCH[chNum].HCHTSIZE_B.PCKTCNT)
#define USB_OTG_H_ConfigPacketCount(chNum, packetNum)      (USB_OTG_H->REGS_HCH[chNum].HCHTSIZE_B.PCKTCNT = (packetNum & 0x3FF))

#define USB_OTG_H_ReadDataPID(chNum)                       (USB_OTG_H->REGS_HCH[chNum].HCHTSIZE_B.DATAPID)
#define USB_OTG_H_ConfigDataPID(chNum, DATA_PID_T)         (USB_OTG_H->REGS_HCH[chNum].HCHTSIZE_B.DATAPID = (DATA_PID_T & 0x03))

#define USB_OTG_H_EnableDoPing(chNum)                      (USB_OTG_H->REGS_HCH[chNum].HCHTSIZE_B.DO_PING = 1)
#define USB_OTG_H_DisableDoPing(chNum)                     (USB_OTG_H->REGS_HCH[chNum].HCHTSIZE_B.DO_PING = 0)

/* OTG_HS1_HCHDMAX  0x514+20*X */
#define USB_OTG_H_ReadDMAAddr(chNum)                       (USB_OTG_H->REGS_HCH[chNum].HCHDMA)
#define USB_OTG_H_ConfigDMAAddr(chNum, dmaAddr)            (USB_OTG_H->REGS_HCH[chNum].HCHDMA = (dmaAddr & 0xFFFFFFFF))

/**@} end of group USB_Host_Macros */

/** @defgroup USB_Host_Enumerates Enumerates
  @{
*/

typedef struct
{
    uint8_t  chNum;
    uint8_t  devAddr;
    uint8_t  devSpeed;
    uint8_t  epType;
    uint16_t maxPackSize;
    uint8_t  direction;
    uint8_t  epNum;
    uint8_t  enbalePing;
} USB_ChannelConfig_T;

typedef enum
{
    PHYCLK_30_60_MHZ,
    PHYCLK_48_MHZ,
    PHYCLK_6_MHZ
} USB_H_PHYCLK_T;

typedef enum
{
    SPEED_HS,
    SPEED_FS,
    SPEED_LS
} USB_H_SPEED_TYPE_T;

typedef enum
{
    IN_OUT,
    EMPTY_PACKET,
    DIEABLE_CHANNEL
} USB_H_QUEUE_T;

typedef enum
{
    EP_DIR_OUT,
    EP_DIR_IN
} USB_H_EP_DIR_T;

typedef enum
{
    OTG_HS_FS_DP = 2,
    OTG_HS_FS_DM
} PORT_DATA_LINE_T;

typedef enum
{
    TEST_DISABLE,
    TEST_J,
    TEST_K,
    TEST_SE0_NAK,
    TEST_PACKET,
    TEST_FORCE_ENABLE
} USB_H_PORT_TEST_T;

typedef enum
{
    EP_TYPE_CONTROL,
    EP_TYPE_ISO,
    EP_TYPE_BULK,
    EP_TYPE_INTERRUPT
} USB_H_EP_TYPE_T;

typedef enum
{
    POSITION_MIDD,
    POSITION_END,
    POSITION_START,
    POSITION_ALL
} USB_H_POSITION_T;

typedef enum
{
    USB_H_CHANNEL0  = BIT0,
    USB_H_CHANNEL1  = BIT1,
    USB_H_CHANNEL2  = BIT2,
    USB_H_CHANNEL3  = BIT3,
    USB_H_CHANNEL4  = BIT4,
    USB_H_CHANNEL5  = BIT5,
    USB_H_CHANNEL6  = BIT6,
    USB_H_CHANNEL7  = BIT7,
    USB_H_CHANNEL8  = BIT8,
    USB_H_CHANNEL9  = BIT9,
    USB_H_CHANNEL10 = BIT10,
    USB_H_CHANNEL11 = BIT11,
    USB_H_CHANNEL12 = BIT12,
    USB_H_CHANNEL13 = BIT13,
    USB_H_CHANNEL14 = BIT14,
    USB_H_CHANNEL15 = BIT15,
    USB_H_CHANNEL_ALL = (uint32_t)0XFFFF
} USB_H_CHANNEL_T;

typedef enum
{
    DATA_0,
    DATA_2,
    DATA_1,
    MDATA = 0X03,
    SETUP = 0X03
} DATA_PID_T;

typedef enum
{
    CHANNEL_INT_TSFCMPN   = BIT0,             /*!< Transfer Complete Normally */
    CHANNEL_INT_TSFCMPAN  = BIT1,             /*!< Transfer Complete Abnormally */
    CHANNEL_INT_AHBERR    = BIT2,             /*!< AHB Error */
    CHANNEL_INT_RXSTALL   = BIT3,             /*!< STALL Response Received Interrupt */
    CHANNEL_INT_RXNAK     = BIT4,             /*!< NAK Response Received Interrupt */
    CHANNEL_INT_RXTXACK   = BIT5,             /*!< ACK Response Received/Transmitted Interrupt */
    CHANNEL_INT_RXNYET    = BIT6,             /*!< NYET Response Received Interrupt */
    CHANNEL_INT_TERR      = BIT7,             /*!< Transaction Error */
    CHANNEL_INT_BABBLE    = BIT8,             /*!< Babble Error */
    CHANNEL_INT_FOVR      = BIT9,             /*!< Frame Overrun Error */
    CHANNEL_INT_DTOG      = BIT10,            /*!< Data Toggle Error */
    CHANNEL_INT_ALL       = (uint32_t)0X7FF   /*!< Channel all interrupt */
} USB_H_CHANNEL_INT_T;

typedef enum
{
    USBH_FIFO_IN_DATA         = 2,
    USBH_FIFO_IN_COMP         = 3,
    USBH_FIFO_DATA_TOGGLE_ERR = 5,
    USBH_FIFO_CH_HALT         = 7
} USBH_FIFO_STATUS_T;

/**@} end of group USB_Host_Enumerates */

/** @defgroup USB_Host_Structures Structures
  @{
*/

typedef struct
{
    union
    {
        __IO uint32_t word;

        struct
        {
            __IO uint32_t chNum       : 4;
            __IO uint32_t byteCount   : 11;
            __IO uint32_t dataPid     : 2;
            __IO uint32_t status      : 4;
        } bit;
    };
} USBH_FIFO_Status_T;

/**@} end of group USB_Host_Structures */

/** @defgroup USB_Host_Functions Functions
  @{
*/

uint8_t USB_OTG_ReadPortSpeed(void);
void USB_OTG_PortReset(void);
void USB_OTG_EnableOutChannelTransfer(uint16_t chNum,
                                      uint8_t* transferBuff,
                                      uint32_t transferLen,
                                      uint16_t maxPackSize,
                                      uint8_t dataPid,
                                      uint8_t epType);
void USB_OTG_EnableInChannelTransfer(uint16_t chNum,
                                     uint32_t transferLen,
                                     uint16_t maxPackSize,
                                     uint8_t dataPid,
                                     uint8_t epType);
void USB_OTG_HaltChannel(uint16_t chNum);
void USB_OTG_InitChannel(USB_ChannelConfig_T* channelConfig);
void USB_OTG_EnableChannelPing(uint8_t chNum);
void USB_OTG_StopHost(void);

#if defined(__ICCARM__)
uint8_t USB_OTG_H_ReadAllChannelsIntFlag(uint32_t flag);
uint32_t USB_OTG_H_ReadChannelAllIntFlag(uint8_t chNum);
uint8_t USB_OTG_H_ReadChannelIntFlag(uint8_t chNum, USB_H_CHANNEL_INT_T flag);
#endif

/**@} end of group USB_Host_Functions */
/**@} end of group USB_Host */
/**@} end of group Driver */
/**@} end of group USB_OTG_Library */
#ifdef __cplusplus
}
#endif

#endif
