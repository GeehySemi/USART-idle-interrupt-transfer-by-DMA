/*!
 * @file        usb_reg.c
 *
 * @brief       This file contains the peripheral register of USB on-the-go full-speed and high-speed
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
#ifndef __USB_REG_H
#define __USB_REG_H

#ifdef __cplusplus
extern "C" {
#endif

#if !((defined (USB_OTG_FS)) ^ (defined (USB_OTG_HS)))
#error "Please define USB_OTG_FS or USB_OTG_HS"
#endif

/* Includes */
#if defined (APM32F40X)
#include "apm32f4xx.h"
#elif defined (APM32F10X_CL)
#include "apm32f10x.h"
#endif

#if defined (APM32F40X) || defined (APM32F10X_CL)

/** @addtogroup USB_OTG_Library USB OTG Library
  @{
*/

/** @addtogroup Driver
  @{
*/

/** @addtogroup USB_Reg
  @{
*/

/** @defgroup USB_Reg_Structures Structures
  @{
*/

/**
  * @brief USB on-the-go full-speed and high-speed Global regs(OTG_FS_GLOBAL/OTG_HS_GLOBAL)
  */
typedef struct
{
    union
    {
        __IOM uint32_t word;


        struct
        {
            __IOM uint32_t INEPTXFRSA : 16;
            __IOM uint32_t INEPTXFDEP : 16;
        } bit;
    } ;
} REG_DTXFIFO1_T;


typedef struct
{

    union
    {
        __IOM uint32_t GCTRLSTS;

        struct
        {
            __IM  uint32_t SREQSUC    : 1;
            __IOM uint32_t SREQ       : 1;
            __IM  uint32_t            : 6;
            __IM  uint32_t HNSUC      : 1;
            __IOM uint32_t HNPREQ     : 1;
            __IOM uint32_t HHNPEN     : 1;
            __IOM uint32_t DHNPEN     : 1;
            __IM  uint32_t            : 4;
            __IM  uint32_t CIDSTS     : 1;
            __IM  uint32_t LSDEBT     : 1;
            __IM  uint32_t ASVD       : 1;
            __IM  uint32_t BSVD       : 1;
        } GCTRLSTS_B;
    } ;

    union
    {
        __IOM uint32_t GINT;

        struct
        {
            __IM  uint32_t            : 2;
            __IOM uint32_t SEFLG      : 1;
            __IM  uint32_t            : 5;
            __IOM uint32_t SREQSUCCHG : 1;
            __IOM uint32_t HNSUCCHG   : 1;
            __IM  uint32_t            : 7;
            __IOM uint32_t HNFLG      : 1;
            __IOM uint32_t ADTOFLG    : 1;
            __IOM uint32_t DEBDFLG    : 1;
        } GINT_B;
    } ;

    union
    {
        __IOM uint32_t GAHBCFG;

        struct
        {
            __IOM uint32_t GINTMASK   : 1;
            __IOM uint32_t BLT        : 4;
            __IOM uint32_t DMAEN      : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t TXFEL      : 1;
            __IOM uint32_t PTXFEL     : 1;
        } GAHBCFG_B;
    } ;

    union
    {
        __IOM uint32_t GUSBCFG;

        struct
        {
            __IOM uint32_t SEFLG      : 3;
            __IM  uint32_t            : 3;
            __OM  uint32_t FSSTSEL    : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t SRPEN      : 1;
            __IOM uint32_t HNPEN      : 1;
            __IOM uint32_t TRTIM      : 4;
            __IM  uint32_t            : 1;
            __IOM uint32_t PHYLPSEL   : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t ULPISEL    : 1;
            __IOM uint32_t ULPIAR     : 1;
            __IOM uint32_t ULPICLKP   : 1;
            __IOM uint32_t ULPIEVDSEL : 1;
            __IOM uint32_t ULPIEVC    : 1;
            __IOM uint32_t DPSEL      : 1;
            __IOM uint32_t SINI       : 1;
            __IOM uint32_t NPTHQ      : 1;
            __IOM uint32_t ULPIIPCDIS : 1;
            __IM  uint32_t            : 3;
            __IOM uint32_t FHMODE     : 1;
            __IOM uint32_t FDMODE     : 1;
            __IOM uint32_t CTXP       : 1;
        } GUSBCFG_B;
    } ;

    union
    {
        __IOM uint32_t GRSTCTRL;

        struct
        {
            __IOM uint32_t CSRST      : 1;
            __IOM uint32_t HSRST      : 1;
            __IOM uint32_t HFCNTRST   : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t RXFFLU     : 1;
            __IOM uint32_t TXFFLU     : 1;
            __IOM uint32_t TXFNUM     : 5;
            __IM  uint32_t            : 19;
            __IM  uint32_t DMAREQ     : 1;
            __IM  uint32_t AHBMIDL    : 1;
        } GRSTCTRL_B;
    } ;

    union
    {
        __IOM uint32_t GCINT;

        struct
        {
            __IM  uint32_t CURMOSEL   : 1;
            __IOM uint32_t MMIS       : 1;
            __IM  uint32_t OTG        : 1;
            __IOM uint32_t SOF        : 1;
            __IM  uint32_t RXFNONE    : 1;
            __IM  uint32_t NPTXFEM    : 1;
            __IM  uint32_t GINNPNAKE  : 1;
            __IM  uint32_t GONAKE     : 1;
            __IM  uint32_t            : 2;
            __IOM uint32_t ESUS       : 1;
            __IOM uint32_t USBSUS     : 1;
            __IOM uint32_t USBRST     : 1;
            __IOM uint32_t ENUMD      : 1;
            __IOM uint32_t ISOPD      : 1;
            __IOM uint32_t EOPF       : 1;
            __IM  uint32_t            : 2;
            __IM  uint32_t INEP       : 1;
            __IM  uint32_t ONEP       : 1;
            __IOM uint32_t IIINTX     : 1;
            __IOM uint32_t IP_OUTTX   : 1;
            __IOM uint32_t DFSUS      : 1;
            __IM  uint32_t            : 1;
            __IM  uint32_t HPORT      : 1;
            __IM  uint32_t HCHAN      : 1;
            __IM  uint32_t PTXFE      : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t CINSTSCHG  : 1;
            __IOM uint32_t DEDIS      : 1;
            __IOM uint32_t SREQ       : 1;
            __IOM uint32_t RWAKE      : 1;
        } GCINT_B;
    } ;

    union
    {
        __IOM uint32_t GINTMASK;

        struct
        {
            __IM  uint32_t            : 1;
            __IOM uint32_t MMISM      : 1;
            __IOM uint32_t OTGM       : 1;
            __IOM uint32_t SOFM       : 1;
            __IOM uint32_t RXFNONEM   : 1;
            __IOM uint32_t NPTXFEMM   : 1;
            __IOM uint32_t GINNPNAKEM : 1;
            __IOM uint32_t GONAKEM    : 1;
            __IM  uint32_t            : 2;
            __IOM uint32_t ESUSM      : 1;
            __IOM uint32_t USBSUSM    : 1;
            __IOM uint32_t USBRSTM    : 1;
            __IOM uint32_t ENUMDM     : 1;
            __IOM uint32_t ISOPDM     : 1;
            __IOM uint32_t EOPFM      : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t EPMISM     : 1;
            __IOM uint32_t INEPM      : 1;
            __IOM uint32_t OUTEPM     : 1;
            __IOM uint32_t IIINTXM    : 1;
            __IOM uint32_t IP_OUTTXM  : 1;
            __IOM uint32_t DFSUSM     : 1;
            __IM  uint32_t            : 1;
            __IM  uint32_t HPORTM     : 1;
            __IOM uint32_t HCHM       : 1;
            __IOM uint32_t PTXFEM     : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t CIDSTSTCM  : 1;
            __IOM uint32_t DEDISM     : 1;
            __IOM uint32_t SREQM      : 1;
            __IOM uint32_t RWAKEM     : 1;
        } GINTMASK_B;
    } ;

    union
    {
        __IM  uint32_t GRXSTS;

        struct
        {
            __IM  uint32_t CHNUM    : 4;
            __IM  uint32_t BCNT     : 11;
            __IM  uint32_t DPID     : 2;
            __IM  uint32_t PSTS     : 4;
        } GRXSTSR_H_B;

        struct
        {
            __IM  uint32_t EPNUM    : 4;
            __IM  uint32_t BCNT     : 11;
            __IM  uint32_t DPID     : 2;
            __IM  uint32_t PSTS     : 4;
            __IM  uint32_t FNUM     : 4;
        } GRXSTS_D_B;
    } ;

    union
    {

        __IM  uint32_t GRXSTSP;

        struct
        {
            __IM  uint32_t CHNUM    : 4;
            __IM  uint32_t BCNT     : 11;
            __IM  uint32_t DPID     : 2;
            __IM  uint32_t PSTS     : 4;
        } GRXSTSP_H_B;

        struct
        {
            __IM  uint32_t EPNUM    : 4;
            __IM  uint32_t BCNT     : 11;
            __IM  uint32_t DPID     : 2;
            __IM  uint32_t PSTS     : 4;
            __IM  uint32_t FNUM     : 4;
        } GRXSTSP_D_B;

    };

    union
    {
        __IOM uint32_t GRXFIFO;

        struct
        {
            __IOM uint32_t RXFDEP     : 16;
        } GRXFIFO_B;
    } ;

    union
    {
        __IOM uint32_t GTXFCFG;

        struct
        {
            __IOM uint32_t NPTXSA   : 16;
            __IOM uint32_t NPTXFDEP : 16;
        } GTXFCFG_H_B;

        struct
        {
            __IOM uint32_t EPTXSA   : 16;
            __IOM uint32_t EPTXFDEP : 16;
        } GTXFCFG_D_B;
    };

    union
    {
        __IM  uint32_t GNPTXFQSTS;


        struct
        {
            __IM  uint32_t NPTXFSA    : 16;
            __IM  uint32_t NPTXRSA    : 8;
            __IM  uint32_t NPTXRQ     : 7;
        } GNPTXFQSTS_B;
    } ;
    __IM  uint32_t  RESERVED[2];

    union
    {
        __IOM uint32_t GGCCFG;

        struct
        {
            __IM  uint32_t            : 16;
            __IOM uint32_t PWEN       : 1;
            __IOM uint32_t I2CBEN     : 1;
            __IOM uint32_t ADVBSEN    : 1;
            __IOM uint32_t BDVBSEN    : 1;
            __IOM uint32_t SOFPOUT    : 1;
            __IOM uint32_t VBSDIS     : 1;
        } GGCCFG_B;
    } ;

    union
    {
        __IOM uint32_t GCID;

        struct
        {
            __IOM uint32_t PID        : 32;
        } GCID_B;
    } ;
    __IM  uint32_t  RESERVED1[48];

    union
    {
        __IOM uint32_t GHPTXFSIZE;

        struct
        {
            __IOM uint32_t HPDTXFSA   : 16;
            __IOM uint32_t HPDTXFDEP  : 16;
        } GHPTXFSIZE_B;
    } ;

    REG_DTXFIFO1_T DTXFIFO[6];
} USB_OTG_GLOBAL_T;


/**
  * @brief USB on the go high speed (OTG_HS2)
  */

typedef struct
{

    union
    {
        __IOM uint32_t USB_SWITCH;

        struct
        {
            __IOM uint32_t usb_switch : 1;
        } USB_SWITCH_B;
    } ;

    union
    {
        __IOM uint32_t POWERON_CORE;

        struct
        {
            __IOM uint32_t poweron_core : 1;
        } POWERON_CORE_B;
    } ;

    union
    {
        __IOM uint32_t USB_PLL_EN;

        struct
        {
            __IOM uint32_t usb_pll_en : 1;
        } USB_PLL_EN_B;
    } ;

    union
    {
        __IOM uint32_t SHORT_5V_ENABLE;

        struct
        {
            __IOM uint32_t short_5v_enable : 1;
        } SHORT_5V_ENABLE_B;
    } ;

    union
    {
        __IOM uint32_t OTG_SUSPENDM;

        struct
        {
            __IOM uint32_t otg_suspendm : 1;
        } OTG_SUSPENDM_B;
    } ;

    union
    {
        __IOM uint32_t TXBITSTUFFENABLE;

        struct
        {
            __IOM uint32_t txbitstuffenable : 1;
        } TXBITSTUFFENABLE_B;
    } ;

    union
    {
        __IOM uint32_t PLLICP_SEL_I2C;

        struct
        {
            __IOM uint32_t pllicp_sel_i2c : 1;
        } PLLICP_SEL_I2C_B;
    } ;

    union
    {
        __IOM  uint32_t HSZR_CNTL_I2C;

        struct
        {
            __IOM  uint32_t hszr_cntl_i2c : 3;
        } HSZR_CNTL_I2C_B;
    } ;

    union
    {
        __IOM  uint32_t SQVTH_CNTL_I2C;

        struct
        {
            __IOM  uint32_t sqvth_cntl_i2c : 2;
        } SQVTH_CNTL_I2C_B;
    } ;

    union
    {
        __IOM  uint32_t SW_RREF_I2C;

        struct
        {
            __IOM  uint32_t sw_rref_i2c : 3;
        } SW_RREF_I2C_B;
    } ;

    union
    {
        __IOM  uint32_t SW_BUF_I2C;

        struct
        {
            __IOM  uint32_t sw_buf_i2c : 1;
        } SW_BUF_I2C_B;
    } ;

    union
    {
        __IOM uint32_t TX2RX_T_CFG_I2C;

        struct
        {
            __IOM uint32_t tx2rx_t_cfg_i2c : 2;
        } TX2RX_T_CFG_I2C_B;
    } ;

    union
    {
        __IOM uint32_t TEST_ANA_FAST_I2C;

        struct
        {
            __IOM uint32_t test_ana_fast_i2c : 3;
        } TEST_ANA_FAST_I2C_B;
    } ;

    union
    {
        __IOM uint32_t CLK_MODE_I2C;

        struct
        {
            __IOM uint32_t clk_mode_i2c : 3;
        } CLK_MODE_I2C_B;
    } ;

    union
    {
        __IOM  uint32_t USB_DBNCE_FLTR_BYPASS;


        struct
        {
            __IOM  uint32_t usb_dbnce_fltr_bypass : 1;

        } USB_DBNCE_FLTR_BYPASS_B;
    } ;

    union
    {
        __IOM uint32_t USB_SS_SCALEDOWN_MODE;

        struct
        {
            __IOM uint32_t usb_ss_scaledown_mode : 2;
        } USB_SS_SCALEDOWN_MODE_B;
    } ;
} USB_OTG_HS2_T;



typedef struct
{
    union
    {
        __IOM uint32_t HCH;

        struct
        {
            __IOM uint32_t MAXPSIZE   : 11;
            __IOM uint32_t EDPNUM     : 4;
            __IOM uint32_t EDPDRT     : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t LSDV       : 1;
            __IOM uint32_t EDPTYP     : 2;
            __IOM uint32_t CNTSEL     : 2;
            __IOM uint32_t DVADDR     : 7;
            __IOM uint32_t ODDF       : 1;
            __IOM uint32_t CHINT      : 1;
            __IOM uint32_t CHEN       : 1;
        } HCH_B;
    } ;

    union
    {
        __IOM uint32_t HCHSCTRL;

        struct
        {
            __IOM uint32_t PADDR      : 7;
            __IOM uint32_t HADDR      : 7;
            __IOM uint32_t TPOST      : 2;
            __IOM uint32_t DO_CMP_SPL : 1;
            __IM  uint32_t            : 14;
            __IOM uint32_t SPLEN      : 1;
        } HCHSCTRL_B;
    } ;

    union
    {
        __IOM uint32_t HCHINT;

        struct
        {
            __IOM uint32_t TSFCMPN    : 1;
            __IOM uint32_t TSFCMPAN   : 1;
            __IOM uint32_t AHBERR     : 1;
            __IOM uint32_t RXSTALL    : 1;
            __IOM uint32_t RXNAK      : 1;
            __IOM uint32_t RXTXACK    : 1;
            __IOM uint32_t RXNYET     : 1;
            __IOM uint32_t TERR       : 1;
            __IOM uint32_t BABBLE     : 1;
            __IOM uint32_t FOVR       : 1;
            __IOM uint32_t DTOG       : 1;
        } HCHINT_B;
    } ;

    union
    {
        __IOM uint32_t HCHIMASK;

        struct
        {
            __IOM uint32_t TSFCMPNM   : 1;
            __IOM uint32_t TSFCMPANM  : 1;
            __IOM uint32_t AHBERRM    : 1;
            __IOM uint32_t RXSTALLM   : 1;
            __IOM uint32_t RXNAKM     : 1;
            __IOM uint32_t RXTXACKM   : 1;
            __IOM uint32_t RXNYETM    : 1;
            __IOM uint32_t TERRM      : 1;
            __IOM uint32_t BABBLEM    : 1;
            __IOM uint32_t FOVRM      : 1;
            __IOM uint32_t DTOGM      : 1;
        } HCHIMASK_B;
    } ;

    union
    {
        __IOM uint32_t HCHTSIZE;

        struct
        {
            __IOM uint32_t TSFSIZE    : 19;
            __IOM uint32_t PCKTCNT    : 10;
            __IOM uint32_t DATAPID    : 2;
            __IOM uint32_t DO_PING    : 1;
        } HCHTSIZE_B;
    } ;

    union
    {
        __IOM uint32_t HCHDMA;

        struct
        {
            __IOM uint32_t ADDR       : 32;
        } HCHDMA_B;
    } ;
    __IM  uint32_t  RESERVED[2];
} HCH_REGS_T;



/**
  * @brief USB on-the-go full-speed and high-speed (OTG_FS_HOST/OTG_HS_HOST)
  */

typedef struct
{

    union
    {
        __IOM uint32_t HCFG;

        struct
        {
            __IOM uint32_t PHYCLKSEL  : 2;
            __IM  uint32_t FSSPT      : 1;
        } HCFG_B;
    } ;

    union
    {
        __IOM uint32_t HFIVL;

        struct
        {
            __IOM uint32_t FIVL       : 16;
        } HFIVL_B;
    } ;

    union
    {
        __IM  uint32_t HFIFM;


        struct
        {
            __IM  uint32_t FNUM       : 16;
            __IM  uint32_t FRTIME     : 16;
        } HFIFM_B;
    } ;
    __IM  uint32_t  RESERVED;

    union
    {
        __IOM uint32_t HPTXSTS;


        struct
        {
            __IOM uint32_t FSPACE     : 16;
            __IM  uint32_t QSPACE     : 8;
            __IM  uint32_t QTOP       : 8;
        } HPTXSTS_B;
    } ;

    union
    {
        __IM  uint32_t HACHINT;

        struct
        {
            __IM  uint32_t ACHINT     : 16;
        } HACHINT_B;
    } ;

    union
    {
        __IOM uint32_t HACHIMASK;

        struct
        {
            __IOM uint32_t ACHIMASK   : 16;
        } HACHIMASK_B;
    } ;
    __IM  uint32_t  RESERVED1[9];

    union
    {
        __IOM uint32_t HPORTCSTS;

        struct
        {
            __IM  uint32_t PCNNTFLG   : 1;
            __IOM uint32_t PCINTFLG   : 1;
            __IOM uint32_t PEN        : 1;
            __IOM uint32_t PENCHG     : 1;
            __IM  uint32_t POVC       : 1;
            __IOM uint32_t POVCCHG    : 1;
            __IOM uint32_t PRS        : 1;
            __IOM uint32_t PSUS       : 1;
            __IOM uint32_t PRST       : 1;
            __IM  uint32_t            : 1;
            __IM  uint32_t PDLSTS     : 2;
            __IOM uint32_t PP         : 1;
            __IOM uint32_t PTSEL      : 4;
            __IM  uint32_t PSPDSEL    : 2;
        } HPORTCSTS_B;
    } ;
    __IM  uint32_t  RESERVED2[47];

    HCH_REGS_T REGS_HCH[12];

} USB_OTG_HOST_T;


/**
  * @brief IN endpoint type define
  */
typedef struct
{
    /*!< (offset Add: 0x100 + 0x20 * m; m = 0~3) OTG_FS device control IN endpoint x control register (OTG_FS_DIEPCTLx)*/
    union
    {
        __IOM uint32_t DIEPCTRL;

        struct
        {
            __IOM uint32_t MAXPS      : 11;
            __IM  uint32_t            : 4;
            __IOM uint32_t USBAEP     : 1;
            __IM  uint32_t EOF        : 1;
            __IM  uint32_t NAKSTS     : 1;
            __IOM uint32_t EPTYPE     : 2;
            __IM  uint32_t            : 1;
            __IOM uint32_t STALLH     : 1;
            __IOM uint32_t TXFNUM     : 4;
            __OM  uint32_t NAKCLR     : 1;
            __OM  uint32_t NAKSET     : 1;
            __OM  uint32_t DPIDSET    : 1;
            __OM  uint32_t OFSET      : 1;
            __IOM uint32_t EPDIS      : 1;
            __IOM uint32_t EPEN       : 1;
        } DIEPCTRL_B;
    } ;

    __IM  uint32_t  RESERVED1;

    /*!< (offset Add: 0x108 + 0x20 * m; m = 0~3) device endpoint-x interrupt register */
    union
    {
        __IOM uint32_t DIEPINT;

        struct
        {
            __IOM uint32_t TSFCMP     : 1;
            __IOM uint32_t EPDIS      : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t TO         : 1;
            __IOM uint32_t ITXEMP     : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t IEPNAKE    : 1;
            __IM  uint32_t TXFE       : 1;
        } DIEPINT_B;
    } ;

    __IM  uint32_t  RESERVED2;

    /*!< (offset Add: 0x110 + 0x20 * m; m = 0~3) device endpoint-0 transfer size register */
    union
    {
        __IOM uint32_t DIEPTRS;

        struct
        {
            __IOM uint32_t EPTRS      : 19;
            __IOM uint32_t EPPCNT     : 10;
            __IOM uint32_t TXDCNT     : 2;
        } DIEPTRS_B;
    } ;

    union
    {
        __IOM uint32_t DIEPDMA;

        struct
        {
            __IOM uint32_t DMAADDR    : 32;
        } DIEPDMA_B;
    } ;

    /*!< (offset Add: 0x118 + 0x20 * m; m = 0~3) OTG_FS device IN endpoint transmit FIFO status
                        register                                                   */
    union
    {
        __IM  uint32_t DITXFSTS;

        struct
        {
            __IM  uint32_t INEPTXFSA : 16;
        } DITXFSTS_B;
    } ;

    __IM  uint32_t  RESERVED4;

} IN_EP_T;

/**
  * @brief OUT endpoint type define
  */
typedef struct
{
    /*!< (offset Add: 0x300 + 0x20 * m; m = 0~3) device endpoint control register */
    union
    {
        __IOM uint32_t DOEPCTRL;

        struct
        {
            __IOM uint32_t MAXPS          : 11;
            __IM  uint32_t                : 4;
            __IOM uint32_t USBAEP         : 1;
            __IM  uint32_t EOF            : 1;
            __IM  uint32_t NAKSTS         : 1;
            __IOM uint32_t EPTYP          : 2;
            __IOM uint32_t SNMEN          : 1;
            __IOM uint32_t STALLH         : 1;
            __IM  uint32_t                : 4;
            __OM  uint32_t NAKCLR         : 1;
            __OM  uint32_t NAKSET         : 1;
            __OM  uint32_t DPIDSET        : 1;
            __OM  uint32_t OFSET          : 1;
            __IOM uint32_t EPDIS          : 1;
            __IOM uint32_t EPEN           : 1;
        } DOEPCTRL_B;
    } ;

    __IM  uint32_t  RESERVED1;

    /*!< (offset Add: 0x308 + 0x20 * m; m = 0~3) device endpoint interrupt register */
    union
    {
        __IOM uint32_t DOEPINT;

        struct
        {
            __IOM uint32_t TSFCMP     : 1;
            __IOM uint32_t EPDIS      : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t SETPCMP    : 1;
            __IOM uint32_t RXOTDIS    : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t RXBSP      : 1;
            __IM  uint32_t            : 7;
            __IOM uint32_t NYET       : 1;
        } DOEPINT_B;
    } ;

    __IM  uint32_t  RESERVED2;

    /*!< (offset Add: 0x310 + 0x20 * m; m = 1~3) device OUT endpoint transfer size register */
    union
    {
        __IOM uint32_t DOEPTRS;

        struct
        {
            __IOM uint32_t EPTRS        : 19;
            __IOM uint32_t EPPCNT       : 10;
            __IOM uint32_t PID_SPCNT    : 2;
        } DOEPTRS_B;

    } ;

    union
    {
        __IOM uint32_t DOEPDMA;

        struct
        {
            __IOM uint32_t DMAADDR    : 32;
        } DOEPDMA_B;
    } ;
    __IM  uint32_t  RESERVED3[2];

} OUT_EP_T;

/**
  * @brief USB on-the-go full-speed and high-speed (OTG_FS_DEVICE/OTG_HS_DEVICE)
  */

typedef struct
{

    union
    {
        __IOM uint32_t DCFG;

        struct
        {
            __IOM uint32_t DSPDSEL    : 2;
            __IOM uint32_t SENDOUT    : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t DADDR      : 7;
            __IOM uint32_t PFITV      : 2;
        } DCFG_B;
    } ;

    union
    {
        __IOM uint32_t DCTRL;

        struct
        {
            __IOM uint32_t RWKUPS     : 1;
            __IOM uint32_t SDCNNT     : 1;
            __IM  uint32_t GINAKSTS   : 1;
            __IM  uint32_t GONAKSTS   : 1;
            __IOM uint32_t TESTSEL    : 3;
            __OM  uint32_t GINAKSET   : 1;
            __OM  uint32_t GINAKCLR   : 1;
            __OM  uint32_t GONAKSET   : 1;
            __OM  uint32_t GONAKCLR   : 1;
            __IOM uint32_t POPROGCMP  : 1;
        } DCTRL_B;
    } ;

    union
    {
        __IM  uint32_t DSTS;

        struct
        {
            __IM  uint32_t SUSSTS     : 1;
            __IM  uint32_t ENUMSPD    : 2;
            __IM  uint32_t ERTERR     : 1;
            __IM  uint32_t            : 4;
            __IM  uint32_t SOFNUM     : 14;
        } DSTS_B;
    } ;
    __IM  uint32_t  RESERVED;

    union
    {
        __IOM uint32_t DINIMASK;


        struct
        {
            __IOM uint32_t TSFCMPM    : 1;
            __IOM uint32_t EPDISM     : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t TOM        : 1;
            __IOM uint32_t ITXEMPM    : 1;
            __IOM uint32_t IEPMMM     : 1;
            __IOM uint32_t IEPNAKEM   : 1;
        } DINIMASK_B;
    } ;

    union
    {
        __IOM uint32_t DOUTIMASK;


        struct
        {
            __IOM uint32_t TSFCMPM    : 1;
            __IOM uint32_t EPDISM     : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t SETPCMPM   : 1;
            __IOM uint32_t OTXEMPM    : 1;
        } DOUTIMASK_B;
    } ;

    union
    {
        __IM  uint32_t DAEPINT;

        struct
        {
            __IM  uint32_t INEPINT    : 16;
            __IM  uint32_t OUTEPINT   : 16;
        } DAEPINT_B;
    } ;

    union
    {
        __IOM uint32_t DAEPIMASK;

        struct
        {
            __IOM uint32_t AINM       : 16;
            __IOM uint32_t AOUTM      : 16;
        } DAEPIMASK_B;
    } ;
    __IM  uint32_t  RESERVED1[2];

    union
    {
        __IOM uint32_t DVBUSDTIM;

        struct
        {
            __IOM uint32_t VBUSDTIM   : 16;
        } DVBUSDTIM_B;
    } ;

    union
    {
        __IOM uint32_t DVBUSPTIM;

        struct
        {
            __IOM uint32_t VBUSPTIM   : 12;
        } DVBUSPTIM_B;
    } ;

    union
    {
        __IOM uint32_t DTHCTRL;

        struct
        {
            __IOM uint32_t NSINTHEN   : 1;
            __IOM uint32_t SINTHEN    : 1;
            __IOM uint32_t TXTHLTH    : 9;
            __IM  uint32_t            : 5;
            __IOM uint32_t RXTHEN     : 1;
            __IOM uint32_t RXTHLTH    : 9;
            __IM  uint32_t            : 1;
            __IOM uint32_t APARKEN    : 1;
        } DTHCTRL_B;
    } ;

    union
    {
        __IOM uint32_t DIEIMASK;


        struct
        {
            __IOM uint32_t INEM       : 16;
        } DIEIMASK_B;
    } ;

    union
    {
        __IOM uint32_t DEPINT;

        struct
        {
            __IM  uint32_t            : 1;
            __IOM uint32_t IN1INT     : 1;
            __IM  uint32_t            : 15;
            __IOM uint32_t OUT1INT    : 1;
        } DEPINT_B;
    } ;

    union
    {
        __IOM uint32_t DEPIMASK;


        struct
        {
            __IM  uint32_t            : 1;
            __IOM uint32_t IN1M       : 1;
            __IM  uint32_t            : 15;
            __IOM uint32_t OUT1M      : 1;
        } DEPIMASK_B;
    } ;
    __IM  uint32_t  RESERVED2;

    union
    {
        __IOM uint32_t DIN1IMASK;

        struct
        {
            __IOM uint32_t TSFCMPM    : 1;
            __IOM uint32_t EPDISM     : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t TOM        : 1;
            __IOM uint32_t ITXEMPM    : 1;
            __IOM uint32_t IEPMMM     : 1;
            __IOM uint32_t IEPNAKEM   : 1;
            __IM  uint32_t            : 1;
            __IOM uint32_t TXFUDRM    : 1;
            __IOM uint32_t BNAM       : 1;
            __IM  uint32_t            : 3;
            __IOM uint32_t NAKM       : 1;
        } DIN1IMASK_B;
    } ;
    __IM  uint32_t  RESERVED3[15];

    union
    {
        __IOM uint32_t DOUT1MASK;

        struct
        {
            __IOM uint32_t TSFCMPM    : 1;
            __IOM uint32_t EPDISM     : 1;
            __IOM uint32_t AHBERRIM   : 1;
            __IM  uint32_t            : 5;
            __IOM uint32_t OPERRM     : 1;
            __IOM uint32_t BNAM       : 1;
            __IM  uint32_t            : 2;
            __IOM uint32_t BERRM      : 1;
            __IOM uint32_t NAKM       : 1;
            __IOM uint32_t NYETM      : 1;
        } DOUT1MASK_B;
    } ;
    __IM  uint32_t  RESERVED4[30];

    IN_EP_T EP_IN[8];

    __IM  uint32_t  RESERVED5[64];

    OUT_EP_T EP_OUT[8];

} USB_OTG_DEVICE_T;


/**
  * @brief USB on-the-go full-speed and high-speed (OTG_FS_PWRCLK/OTG_HS_PWRCLK)
  */

typedef struct
{

    union
    {
        __IOM uint32_t PCGCTRL;

        struct
        {
            __IOM uint32_t PCLKSTOP   : 1;
            __IOM uint32_t GCLK       : 1;
            __IM  uint32_t            : 2;
            __IOM uint32_t PHYSUS     : 1;
        } PCGCTRL_B;
    } ;
} USB_OTG_PWRCLK_T;

/**
  * @brief USB on-the-go full-speed and high-speed (OTG_FS_FIFO/OTG_HS_FIFO)
  */
typedef struct
{
    __IOM uint32_t DATA;

    __IM uint32_t RESERVED[1023];
} USB_OTG_FIFO_DATA_T;

typedef struct
{
    USB_OTG_FIFO_DATA_T FIFO[11];

} USB_OTG_FIFO_T;

/**@} end of group USB_Reg_Structures */

/** @defgroup USB_Reg_Macros Macros
  @{
*/

/* USB OTG base address in the bit-band region */
#define USB_OTG_FS_BASE        ((uint32_t)0x50000000)
#define USB_OTG_HS_BASE        ((uint32_t)0x40040000)

#ifdef USB_OTG_FS
#define USB_OTG_BASE            USB_OTG_FS_BASE
#else
#define USB_OTG_BASE            USB_OTG_HS_BASE
#endif

#define USB_OTG_HS2_BASE       ((uint32_t)USB_OTG_BASE + 0x200)
#define USB_OTG_HOST_BASE      ((uint32_t)USB_OTG_BASE + 0x400)
#define USB_OTG_DEVICE_BASE    ((uint32_t)USB_OTG_BASE + 0x800)
#define USB_OTG_PWRCLK_BASE    ((uint32_t)USB_OTG_BASE + 0xE00)
#define USB_OTG_FIFO_BASE      ((uint32_t)USB_OTG_BASE + 0x1000)
#define USB_OTG_FIFO_OFFSET     0x1000

/* USB on-the-go registers */
#define USB_OTG_G              ((USB_OTG_GLOBAL_T *) USB_OTG_BASE)
#define USB_OTG_H              ((USB_OTG_HOST_T *) USB_OTG_HOST_BASE)
#define USB_OTG_D              ((USB_OTG_DEVICE_T *) USB_OTG_DEVICE_BASE)
#define USB_OTG_PWR            ((USB_OTG_PWRCLK_T *) USB_OTG_PWRCLK_BASE)
#define USB_OTG_HS2            ((USB_OTG_HS2_T *) USB_OTG_HS2_BASE)

#define USB_OTG_FIFO           ((USB_OTG_FIFO_T *) USB_OTG_FIFO_BASE)

#else
#error "Please define APM32F40X or APM32F10X_CL"
#endif

/**@} end of group USB_Reg_Macros */
/**@} end of group USB_Reg */
/**@} end of group Driver */
/**@} end of group USB_OTG_Library */
#ifdef __cplusplus
}
#endif

#endif

