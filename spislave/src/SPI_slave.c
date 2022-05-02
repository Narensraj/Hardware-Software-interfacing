/* SPI_slave.c:
 * This code is for spi slave which receives the value from the master and control the brightness of the led
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "common.h"

TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim2;
volatile uint8_t rxData;
uint16_t pulse = 0;
  int count  = 0;
  int count2 = 0;
  int count3 = 0;
  int count2A = 0;
  int count3A = 0;  
int myindex[1000];
int flag = 0; 

int8_t Brightness=0;
const char ok[] = {'o','k'};
uint8_t ok_index;


void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim5);
}

// FUNCTION      : timerintr
// DESCRIPTION   : Initialize the timer interrupt function
// RETURNS       : Nothing
void timerIntr()
{
    HAL_NVIC_SetPriority(TIM5_IRQn, 0, 1); // Set inturrupt priority
    HAL_NVIC_EnableIRQ(TIM5_IRQn);

    HAL_TIM_Base_Start_IT(&htim5); // start timer with intrrupt
}


// FUNCTION      : timerint
// DESCRIPTION   : Initialize the timer 
// RETURNS       : Nothing
void timerinit()
{
    RCC->APB1ENR |= (1 << 3); // Enable clock source for timer 5

    htim5.Instance = TIM5;
    htim5.Init.Prescaler = 50 - 1; // 50 000 000/50 = 1,000,000
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = 1000 - 1; // max delay we get here is 1 miliseconds
    htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&htim5);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1 | GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    RCC->APB1ENR |= (1 << 0); // Enable clock source for timer 2
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 500 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000 - 1;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim2);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 5000 - 1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);
    /* USER CODE BEGIN TIM2_Init 2 */

    /* USER CODE END TIM2_Init 2 */
    // HAL_TIM_MspPostInit(&htim2);
}




void spiinit(){
 //printf("SPI initialized");
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
GPIOA->MODER &= ~GPIO_MODER_MODER5;

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

HAL_NVIC_SetPriority(SPI1_IRQn,0,0);
HAL_NVIC_EnableIRQ(SPI1_IRQn);
//Enable SPI
SPI1->CR1 |= SPI_CR1_SPE;
//  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_RESET);


flag = 0;
    RCC->AHB1ENR |= (1 << 0);  // turn on the clock of port A
    //GPIOA->MODER |= (1 << 10); // make PA5 pin as output
    GPIOA->MODER &= ~(1 << 3); // make PA0 pin as alternate

 timerinit();
  //timerIntr();




}

// FUNCTION      : dutySelection
// DESCRIPTION   : duty values is recevied and printings its value in cmd line
// RETURNS       : Nothing
void dutySelection(uint32_t duty)
{
    if(duty > 0 || duty < 100) //Checks whether duty value is between 0 and 100
    {
    pulse = duty * 10; 
    printf("The entered duty value is %ld\n",duty);
    }
    else{
    printf("Enter the duty value between 0 to 100\n");
    }
}

void spiWrite(uint8_t *data,uint8_t length){
  uint8_t i;
  
  for(i = 0;i<length;i++){
    
    while(!(SPI1->SR & SPI_SR_TXE)){};
           SPI1->DR = data[i]; 
           printf("write command %d\n",data[i]);  
  }
  //while (!((SPI1->SR)& SPI_SR_TXE));  // wait for TXE bit to set -> This will indicate that the buffer is empty
  while(SPI1->SR & SPI_SR_BSY);     // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication	
}

void SPI1_IRQHandler(void)
{

      if(SPI1->SR & SPI_SR_TXE){
    SPI1->DR = Brightness;
    //SPI1->DR = Brightness;
  }
  //printf("test");
  if(SPI1->SR & SPI_SR_RXNE)
  {
    rxData = SPI1->DR;
    printf("%d\n",rxData);
    
    Brightness = rxData;

    //if(Brightness > 1 && Brightness < 100){
      dutySelection((uint32_t)Brightness);         //The duty selection funtion
      HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);//Channel 1 is intialized 
      timerIntr();
      HAL_TIM_Base_Init(&htim5);
      HAL_TIM_Base_Start_IT(&htim5); 
  }

}


// FUNCTION      : spi_response
// DESCRIPTION   : response function
// RETURNS       : Nothing
void spi_respone()
{
    printf("respone");
  if(SPI1->SR & SPI_SR_TXE){
    rxData = SPI1->DR;
    printf("%d\n",rxData);

    }
}





void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

    //pulse = pulse*10;
    if (htim == &htim5)
    {
      
        if (flag == 1){
            TIM2->CCR1 = (myindex[count]/2);
            count++;
        if(count == 630){
            count = 0;
        }
        }else{
        TIM2->CCR1 = pulse;  
        }

        if (flag == 1){
            TIM2->CCR2 = (myindex[count2+210]/2);
            count2A++;
        if(count2A == 630){
            count2A = 0;
        }
        }else{
        TIM2->CCR2 = pulse;  
        }

        if (flag == 1){
            TIM2->CCR2 = (myindex[count3+410]/2);
            count3A++;
        if(count3A == 630){
            count3A = 0;
        }
        }else{
        TIM2->CCR3 = pulse;  
        }
        
    }
}


