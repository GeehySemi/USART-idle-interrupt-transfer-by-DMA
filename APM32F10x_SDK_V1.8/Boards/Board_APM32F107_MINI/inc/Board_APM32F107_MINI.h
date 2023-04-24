/*!
 * @file        Board_APM32F107_MINI.h
 *
 * @brief       This file contains definitions for APM32F107_MINI's Leds, push-buttons hardware resources
 *
 * @version     V1.0.3
 *
 * @date        2022-12-01
 *
 * @attention
 *
 *  Copyright (C) 2020-2022 Geehy Semiconductor
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
#ifndef __BOARD_APM32F107_MINI_H
#define __BOARD_APM32F107_MINI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f10x.h"
#include "apm32f10x_gpio.h"
#include "apm32f10x_rcm.h"
#include "apm32f10x_eint.h"
#include "apm32f10x_usart.h"
#include "apm32f10x_misc.h"

/** @addtogroup Board
  @{
*/

/** @addtogroup Board_APM32F107_MINI
  @{
*/

/** @defgroup APM32F107_MINI_Enumerations
  * @{
  */

typedef enum
{
    LED2 = 0,
    LED3 = 1
} Led_TypeDef;

typedef enum
{
    BUTTON_KEY1 = 0,
    BUTTON_KEY2 = 1
} Button_TypeDef;

typedef enum
{
    BUTTON_MODE_GPIO = 0,
    BUTTON_MODE_EINT = 1
} ButtonMode_TypeDef;

typedef enum
{
    COM1 = 0,
    COM2 = 1
} COM_TypeDef;

/**@} end of group APM32F107_MINI_Enumerations */

/**
  * @}
  */

/** @addtogroup APM32F107_MINI_LED
  * @{
  */
#define LEDn                             2

#define LED2_PIN                         GPIO_PIN_6
#define LED2_GPIO_PORT                   GPIOE
#define LED2_GPIO_CLK                    RCM_APB2_PERIPH_GPIOE

#define LED3_PIN                         GPIO_PIN_5
#define LED3_GPIO_PORT                   GPIOE
#define LED3_GPIO_CLK                    RCM_APB2_PERIPH_GPIOE

/**@} end of group APM32F107_MINI_LED */

/** @addtogroup APM32F107_MINI_BUTTON
  * @{
  */
#define BUTTONn                          2

/**
 * @brief Key1 push-button
 */
#define KEY1_BUTTON_PIN                   GPIO_PIN_1
#define KEY1_BUTTON_GPIO_PORT             GPIOA
#define KEY1_BUTTON_GPIO_CLK              RCM_APB2_PERIPH_GPIOA
#define KEY1_BUTTON_EINT_LINE             EINT_LINE_1
#define KEY1_BUTTON_EINT_PORT_SOURCE      GPIO_PORT_SOURCE_A
#define KEY1_BUTTON_EINT_PIN_SOURCE       GPIO_PIN_SOURCE_1
#define KEY1_BUTTON_EINT_IRQn             EINT1_IRQn
/**
 * @brief Key2 push-button
 */
#define KEY2_BUTTON_PIN                   GPIO_PIN_0
#define KEY2_BUTTON_GPIO_PORT             GPIOA
#define KEY2_BUTTON_GPIO_CLK              RCM_APB2_PERIPH_GPIOA
#define KEY2_BUTTON_EINT_LINE             EINT_LINE_0
#define KEY2_BUTTON_EINT_PORT_SOURCE      GPIO_PORT_SOURCE_A
#define KEY2_BUTTON_EINT_PIN_SOURCE       GPIO_PIN_SOURCE_0
#define KEY2_BUTTON_EINT_IRQn             EINT0_IRQn

/**@} end of group APM32F107_MINI_BUTTON */

/** @addtogroup APM32F107_MINI_COM
  * @{
  */

#define COMn                            2

/**
 * @brief Definition for COM port1, connected to USART1
 */
#define MINI_COM1                        USART1
#define MINI_COM1_CLK                    RCM_APB2_PERIPH_USART1
#define MINI_COM1_TX_PIN                 GPIO_PIN_9
#define MINI_COM1_TX_GPIO_PORT           GPIOA
#define MINI_COM1_TX_GPIO_CLK            RCM_APB2_PERIPH_GPIOA
#define MINI_COM1_RX_PIN                 GPIO_PIN_10
#define MINI_COM1_RX_GPIO_PORT           GPIOA
#define MINI_COM1_RX_GPIO_CLK            RCM_APB2_PERIPH_GPIOA
#define MINI_COM1_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port2, connected to USART2
 */
#define MINI_COM2                        USART2
#define MINI_COM2_CLK                    RCM_APB1_PERIPH_USART2
#define MINI_COM2_TX_PIN                 GPIO_PIN_2
#define MINI_COM2_TX_GPIO_PORT           GPIOA
#define MINI_COM2_TX_GPIO_CLK            RCM_APB2_PERIPH_GPIOA
#define MINI_COM2_RX_PIN                 GPIO_PIN_3
#define MINI_COM2_RX_GPIO_PORT           GPIOA
#define MINI_COM2_RX_GPIO_CLK            RCM_APB2_PERIPH_GPIOA
#define MINI_COM2_IRQn                   USART2_IRQn

/**@} end of group APM32F107_MINI_COM */

/** @defgroup APM32F107_MINI_Functions
  * @{
  */

void APM_MINI_LEDInit(Led_TypeDef Led);
void APM_MINI_LEDOn(Led_TypeDef Led);
void APM_MINI_LEDOff(Led_TypeDef Led);
void APM_MINI_LEDToggle(Led_TypeDef Led);

void APM_MINI_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint32_t APM_MINI_PBGetState(Button_TypeDef Button);

void APM_MINI_COMInit(COM_TypeDef COM, USART_Config_T* configStruct);

/**@} end of group APM32F107_MINI_Functions */
/**@} end of group Board_APM32F107_MINI */
/**@} end of group Board */

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_APM32F107_MINI_H */
