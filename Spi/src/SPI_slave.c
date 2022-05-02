#include <stdio.h>
#include <stdint.h>
#include "common.h"

uint8_t RxData[6];

void SPIConfig (void)
{
  RCC->APB2ENR |= (1<<12);  // Enable SPI1 CLock
	
  SPI1->CR1 |= (1<<0)|(1<<1);   // CPOL=1, CPHA=1
	
  SPI1->CR1 |= (1<<2);  // Master Mode
	
  SPI1->CR1 |= (3<<3);  // BR[2:0] = 011: fPCLK/16, PCLK2 = 80MHz, SPI clk = 5MHz
	
  SPI1->CR1 &= ~(1<<7);  // LSBFIRST = 0, MSB first
	
  SPI1->CR1 |= (1<<8) | (1<<9);  // SSM=1, SSi=1 -> Software Slave Management
	
  SPI1->CR1 &= ~(1<<10);  // RXONLY = 0, full-duplex
	
  SPI1->CR1 &= ~(1<<11);  // DFF=0, 8 bit data
	
  SPI1->CR2 = 0;
}

void SPI_Transmit (uint8_t *data, int size)
{

	int i=0;
	while (i<size)
	{
	   while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	   SPI1->DR = data[i];  // load the data into the Data Register
	   i++;
	}	
	
	while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication	
	
	//  Clear the Overrun flag by reading DR and SR
	//uint8_t temp = SPI1->DR;
			//		temp = SPI1->SR;
	
}


void SPI_Receive (uint8_t *data, int size)
{

	while (size)
	{
		while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
		SPI1->DR = 0;  // send dummy data
		while (!((SPI1->SR) &(1<<0))){};  // Wait for RXNE to set -> This will indicate that the Rx buffer is not empty
	  *data++ = (SPI1->DR);
		size--;
	}	
  printf("%s\n",RxData);
}


void GPIOConfig (void)
{
	RCC->AHB1ENR |= (1<<0);  // Enable GPIO Clock
	
	GPIOA->MODER |= (2<<10)|(2<<12)|(2<<14)|(1<<18);  // Alternate functions for PA5, PA6, PA7 and Output for PA9
	
	GPIOA->OSPEEDR |= (3<<10)|(3<<12)|(3<<14)|(3<<18);  // HIGH Speed for PA5, PA6, PA7, PA9
	
	GPIOA->AFR[0] |= (5<<20)|(5<<24)|(5<<28);   // AF5(SPI1) for PA5, PA6, PA7
}


void SPI_Enable (void)
{
	SPI1->CR1 |= (1<<6);   // SPE=1, Peripheral enabled
}

void SPI_Disable (void)
{
	SPI1->CR1 &= ~(1<<6);   // SPE=0, Peripheral Disabled
}

void CS_Enable (void)
{
	GPIOA->BSRR |= (1<<9)<<16;
}

void CS_Disable (void)
{
	GPIOA->BSRR |= (1<<9);
}

ParserReturnVal_t spicmd(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
		int i = 0;
  GPIOConfig ();
  SPIConfig ();
  SPI_Enable ();
  CS_Enable ();
  //SPI_Receive (RxData,4);
  while(1)
  {

	while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
	SPI1->DR = 0;  // send dummy data
	while (!((SPI1->SR) &(1<<0))){};  // Wait for RXNE to set -> This will indicate that the Rx buffer is not empty
	  RxData[i] = (SPI1->DR);
	  printf("%d\n",RxData[i]);
	  i++;
  }
  
  return CmdReturnOk;
}

ADD_CMD("spi",spicmd,"                Example Command")