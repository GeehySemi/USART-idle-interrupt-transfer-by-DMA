/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.0
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

/* Includes */
#include "main.h"
#include "stdio.h"
#include <string.h>
/** @addtogroup Examples
  @{
*/

/** @addtogroup USART_Interrupt
  @{
*/

/** @addtogroup USART_Interrupt_MACROS MACROS
  @{
*/


/**@} end of group USART_Interrupt_MACROS */

/** @addtogroup USART_Interrupt_Variables Variables
  @{
*/

#define MAX_LEN  256

u8 uart_tx_buf[MAX_LEN] = {0};
u8 uart_rx_buf[MAX_LEN] = {0};

void send_data(u8 *buf, u16 len);

/**@} end of group USART_Interrupt_Variables */

/** @addtogroup USART_Interrupt_Functions Functions
  @{
*/


void config_dma(void)
{
  DMA_Config_T dmaConfig;
  
  RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_DMA1);  
  
  /* USART1 TX DMA1 Channel (triggered by USART1 Tx event) Config */
  DMA_Reset(DMA1_Channel4);  
  dmaConfig.peripheralBaseAddr = (u32)(USART1_BASE+0x04);
  dmaConfig.memoryBaseAddr = (uint32_t)uart_tx_buf;
  dmaConfig.dir = DMA_DIR_PERIPHERAL_DST;
  dmaConfig.bufferSize = 0;
  dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
  dmaConfig.memoryInc = DMA_MEMORY_INC_ENABLE;
  dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_BYTE;
  dmaConfig.memoryDataSize = DMA_MEMORY_DATA_SIZE_BYTE;
  dmaConfig.loopMode = DMA_MODE_NORMAL;
  dmaConfig.priority = DMA_PRIORITY_HIGH;
  dmaConfig.M2M = DMA_M2MEN_DISABLE;
  DMA_Config(DMA1_Channel4, &dmaConfig);

  /* USART1 RX DMA1 Channel (triggered by USART1 Rx event) Config */
  DMA_Reset(DMA1_Channel5);  
  dmaConfig.peripheralBaseAddr = (u32)(USART1_BASE+0x04);
  dmaConfig.memoryBaseAddr = (uint32_t)uart_rx_buf;
  dmaConfig.dir = DMA_DIR_PERIPHERAL_SRC;
  dmaConfig.bufferSize = MAX_LEN;
  DMA_Config(DMA1_Channel5, &dmaConfig);
}

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 */
int main(void)
{

	char sbuf[] = "start test..\r\n";
	
    USART_Config_T USART_ConfigStruct;

    APM_MINI_LEDInit(LED2);

    RCM_EnableAPB2PeriphClock((RCM_APB2_PERIPH_T)(RCM_APB2_PERIPH_GPIOA | RCM_APB2_PERIPH_USART1));


    USART_ConfigStruct.baudRate = 115200;
    USART_ConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    USART_ConfigStruct.mode = USART_MODE_TX_RX;
    USART_ConfigStruct.parity = USART_PARITY_NONE;
    USART_ConfigStruct.stopBits = USART_STOP_BIT_1;
    USART_ConfigStruct.wordLength = USART_WORD_LEN_8B;
    APM_MINI_COMInit(COM1, &USART_ConfigStruct);

	USART_EnableDMA(USART1,USART_DMA_TX_RX);
	
	

    USART_EnableInterrupt(USART1, USART_INT_IDLE);
    NVIC_EnableIRQRequest(USART1_IRQn, 0, 0);
	
	config_dma();
	
	send_data((u8*)sbuf, strlen(sbuf));

    while (1)
    {
		
    }
}

/*!
 * @brief       Delay
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Delay(void)
{
    uint32_t tick = 0xfffff;

    while (tick--);
}

/*!
 * @brief       USART1_Interrupt
 *
 * @param       None
 *
 * @retval      None
 *
 */
void send_data(u8 *buf, u16 len)
{
  memcpy(uart_tx_buf, buf, len);
  while(USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);
  DMA_Disable(DMA1_Channel4);
  DMA_ConfigDataNumber(DMA1_Channel4, len);
  DMA_Enable(DMA1_Channel4);
	
}


void USART1_Isr(void)
{
	u16 tmp_len;
    if(USART_ReadIntFlag(USART1, USART_INT_IDLE))
    {
	   USART_RxData(USART1); 
       DMA_Disable(DMA1_Channel5);
  	   tmp_len = MAX_LEN - DMA_ReadDataNumber(DMA1_Channel5);
	   DMA_ConfigDataNumber(DMA1_Channel5, MAX_LEN);
	   send_data(uart_rx_buf, tmp_len);  //send received data;
	   DMA_Enable(DMA1_Channel5);
    }
}





/**@} end of group USART_Interrupt_Functions */
/**@} end of group USART_Interrupt */
/**@} end of group Examples */
