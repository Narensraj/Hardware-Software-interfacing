/* SPI_master.c
 * Master code which sends data from the master to slave
 *
 */

#include <stdio.h>
#include <stdint.h>

#include "common.h"

volatile uint8_t rxData;

void spiinit(){
    //Enable the clock for GPIOA
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
//Reset SPI peripheral
RCC->APB2RSTR |= RCC_APB2RSTR_SPI1;
RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1;

//Enable SPI1 clock
RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

//Initialize to AF
//GPIOA->MODER &= 0;
//GPIOA->MODER = (GPIOA->MODER & 0xA8000000) | 0x0000AAAA;

//****************************************************************************************************
//this is the line which sir missed we have to reset pin 5 from gpio output to alternate operation
GPIOA->MODER &= ~GPIO_MODER_MODER5;

GPIOA->MODER |= GPIO_MODER_MODER5_1;
GPIOA->MODER |= GPIO_MODER_MODER6_1;
GPIOA->MODER |= GPIO_MODER_MODER7_1;

//set speed to the highest
GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5;
GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;
GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;


GPIOA->AFR[0] |= GPIO_AFRL_AFRL5_0 | GPIO_AFRL_AFRL5_2;
GPIOA->AFR[0] |= GPIO_AFRL_AFRL6_0 | GPIO_AFRL_AFRL6_2;
GPIOA->AFR[0] |= GPIO_AFRL_AFRL7_0 | GPIO_AFRL_AFRL7_2;


GPIO_InitTypeDef GPIO_InitStruct = {0};
GPIO_InitStruct.Pin = GPIO_PIN_10;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
GPIO_InitStruct.Alternate = 0;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//Data frane format 8bits
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

//select master
SPI1->CR1 |= SPI_CR1_MSTR;

//This is for master
SPI1->CR1 |= SPI_CR1_SSM;
SPI1->CR1 |= SPI_CR1_SSI;



//full duplex
SPI1->CR1 &= ~SPI_CR1_BIDIMODE;
SPI1->CR1 &= ~SPI_CR1_RXONLY;

//Enable Rx interrupt
SPI1->CR2 |= SPI_CR2_RXNEIE;
SPI1->CR2 |= SPI_CR2_TXEIE;

//Enable SPI1 interrupt
HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
HAL_NVIC_EnableIRQ(SPI1_IRQn);

//Enable SPI
SPI1->CR1 |= SPI_CR1_SPE;


}



void spiWrite(uint8_t *data,uint8_t length){
  uint8_t i;
  
  for(i = 0;i<length;i++){
    
    while(!(SPI1->SR & SPI_SR_TXE)){};
           SPI1->DR = data[i]; 
           //printf("%d\n",data[i]);  
  }
  //while (!((SPI1->SR)& SPI_SR_TXE));  // wait for TXE bit to set -> This will indicate that the buffer is empty
  while(SPI1->SR & SPI_SR_BSY);     // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication	
}




ParserReturnVal_t CMDSpiWrite(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  uint32_t val = 0;
  fetch_uint32_arg(&val);
  
  spiWrite((uint8_t *)&val,1);

  return CmdReturnOk;
}

ADD_CMD("SD",CMDSpiWrite,"this command sends the data.")
