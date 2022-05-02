/* example.c:
 *
 *   Template code for implementing a simple task, using the ADD_TASK()
 *   macro.  Also contains template code for a simple monitor command.
 *
 */

#include <stdio.h>
#include <stdint.h>

#include "common.h"

volatile uint8_t rxData;

const char ok[] = {'o','k'};
uint8_t ok_index;

void spiinit(){
 // printf("init");
    //Enable the clock for GPIOA
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

GPIO_InitTypeDef GPIO_InitStruct = {0};
GPIO_InitStruct.Pin = GPIO_PIN_10;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
GPIO_InitStruct.Alternate = 0;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//Reset SPI peripheral
RCC->APB2RSTR |= RCC_APB2RSTR_SPI1;
RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1;

//Enable SPI1 clock
RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

//Initialize to AF
GPIOA->MODER |= GPIO_MODER_MODER5_1;
GPIOA->MODER |= GPIO_MODER_MODER6_1;
GPIOA->MODER |= GPIO_MODER_MODER7_1;

//Set speed to the highest
GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5;
GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;
GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;


GPIOA->AFR[0] |= GPIO_AFRL_AFRL5_0 | GPIO_AFRL_AFRL5_2;
GPIOA->AFR[0] |= GPIO_AFRL_AFRL6_0 | GPIO_AFRL_AFRL6_2;
GPIOA->AFR[0] |= GPIO_AFRL_AFRL7_0 | GPIO_AFRL_AFRL7_2;

//Data frane format Bbits
SPI1->CR1 &= ~SPI_CR1_DFF;
//Clock low while idling
SPI1->CR1 |= SPI_CR1_CPOL;
//Capture at the rising edge
SPI1->CR1 &= ~SPI_CR1_CPHA;
//LSB first
SPI1->CR1 |= SPI_CR1_LSBFIRST;
//baudrate 100MHz/ 128
SPI1->CR1 |= SPI_CR1_BR_1 | SPI_CR1_BR_2;


//Use software to control the NSS
//This is for slave
//SPI1->CR1 | = SPI_CR1_SSM;
//This is for slave
SPI1->CR1 &= ~SPI_CR1_MSTR;
SPI1->CR1 |= SPI_CR1_SSM;
SPI1->CR1 &= ~SPI_CR1_SSI;

//full duplex
SPI1->CR1 &= ~SPI_CR1_BIDIMODE;
SPI1->CR1 &= ~SPI_CR1_RXONLY;
//Enable Rx interrupt
SPI1->CR2 |= SPI_CR2_RXNEIE;
//Enable SPI1 interrupt
NVIC_EnableIRQ(SPI1_IRQn);
//Enable SPI
SPI1->CR1 |= SPI_CR1_SPE;

}

void SPI1_IRQHandler(void)
{
   // printf("test");
  if(SPI1->DR & SPI_SR_RXNE)
  {
    rxData = SPI1->DR;
    printf("%d\n",rxData);
  }
}


void spi_respone(void)
{
    printf("test\n");
  if(SPI1->DR & SPI_SR_TXE)
  {
    printf("test1\n");
    SPI1->DR = ok[ok_index++];
    if(ok_index == sizeof(ok)){
ok_index = 0;
//printf("test");
//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_SET);
    }
    
  }
}


void spiWrite(uint8_t *data,uint8_t length){
  uint8_t i;
  
  for(i = 0;i<length;i++){
    while(!(SPI1->SR & SPI_SR_TXE)){
      SPI1->DR = data[i];
    }
  }
  while(SPI1->SR & SPI_SR_BSY);
}


ParserReturnVal_t CMDSpiWrite(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  uint32_t val = 0;
  fetch_uint32_arg(&val);
  spiWrite((uint8_t *)&val,1);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_SET);
  return CmdReturnOk;

}

ADD_CMD("SD",CMDSpiWrite,"this command sends the data.")
