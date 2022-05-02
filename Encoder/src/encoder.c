/*******************************************************************************
  * File Name          : encoder.c
  * Description        : To control the dc motor using encoder
  * Author:              Naren Subburaj
  * Date:                22/03/2022
  ******************************************************************************
  */


/*
Use cmd :
dcinit
encoderinit
encoder 'User value'
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "common.h"

TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim2;
uint16_t pulse = 0;
int myindex[1000];
int incrimentalPulse = 0;
int timeUs = 0;
int timeCounter = 0;
int flag = 0; 
int i = 0;
int16_t count=0;
int16_t countPos=0;
uint16_t encoderVal = 0;
int counter = 0;
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim5);
}

// FUNCTION      : timerint
// DESCRIPTION   : Initialize the timer 
// RETURNS       : Nothing
void timerinit()
{
    RCC->APB1ENR |= (1 << 3); // Enable clock source for timer 5

    htim5.Instance = TIM5;
    htim5.Init.Prescaler = 500 - 1; // 50 000 000/500 = 1,00,000
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = 1000 - 1; // max delay we get here is 10 milliseconds
    htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&htim5);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
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
    htim2.Init.Period = 500 - 1;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim2);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500 - 1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);
   

   
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  

   
  GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);//PA10 is set to IN1
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); //PB3 is set to IN2
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);   //VS to MCU
}

void us_delay1(int delay)
{
    TIM2->CNT = 0;
    // TIM2->CR1 |= (1<<0);          //start the timer
    while (TIM2->CNT < delay);
}

void ms_delay(uint16_t ms_delay)
{
    for (uint16_t i = 0; i < ms_delay; i++)
    {
        us_delay1(1000);
    }
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim5)
    {
        if(flag  == 0){
        counter = TIM1->CNT;

        count = ((int)counter); 
        //printf("count value : %d\n",countPos);
        //printf("count value : %d\n",abs(countPos));
      if(  abs(count) > abs((int16_t)encoderVal))
      {
          //printf("count value : %d\n",abs(countPos));
        printf("count value : %ld\n",TIM1->CNT);
        TIM1->CNT = 0;
        countPos=0;
        count =0;
        counter =0;
        //printf("encoder value : %d\n",abs((int)encoderVal));
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);//PA10 is set to 0
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); //PB5 is set to 0
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); //PB5 is set to 0
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        
        HAL_TIM_Base_Stop_IT(&htim5); //Timer 5 interrupt is stopped
      }
        }else if(flag == 1){
            counter = TIM1->CNT;

            count = ((int)counter); 
            printf("%d\n",count);
        }
    }
}

// FUNCTION      : encoderValue
// DESCRIPTION   : Direction selection of the dc motor 
// RETURNS       : Nothing
void encoderValue(int encoderValue)
{
    if(0 < encoderValue  ) //If user value is greater than 0 rotate motor in counter clock wise
        {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); 

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        
        }
    else if( 0 > encoderValue) //If user value is lesser than 0 rotate motor in anti clock wise
    {
        
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);  

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        
        }
    else
    {
        flag =1;
        printf("Enter correct value\n");
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);  

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); 

    }

}

// FUNCTION      : encoder_Init
// DESCRIPTION   : All the encoder elements are initialized
// RETURNS       : Nothing
void encoder_init(void)
{
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

//PA08 TIM1 CH1 
//PA09 TIM1 CH2 

GPIOA->MODER |= GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1;
GPIOA->AFR[1] |= GPIO_AFRH_AFRH0_0 | GPIO_AFRH_AFRH1_0;

TIM1->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
TIM1->CCMR1 &=~(TIM_CCMR1_IC1F) |~(TIM_CCMR1_IC2F);
TIM1->CCER &= ~(TIM_CCER_CC1P) |~(TIM_CCER_CC2P);
TIM1->CCER &= ~(TIM_CCER_CC1NP) |~(TIM_CCER_CC2NP);

TIM1->ARR = 0xffff;
// use rising/falling edges for both TI1 and TI2
TIM1->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
TIM1->CR1 |= TIM_CR1_CEN;


RCC->APB1ENR |= (1 << 3); // Enable clock source for timer 5

htim5.Instance = TIM5;
htim5.Init.Prescaler = 500 - 1; // 50 000 000/500 = 1,00,000
htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
htim5.Init.Period = 1000 - 1; // max delay we get here is 10 milliseconds
htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
HAL_TIM_Base_Init(&htim5);


GPIO_InitTypeDef GPIO_InitStruct = {0};
__HAL_RCC_GPIOA_CLK_ENABLE();
GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_5;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
GPIO_InitStruct.Alternate = 0;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);

}


// FUNCTION      : encoderInitprintf("count value : %d\n",abs(countPos));
// DESCRIPTION   : encoder initialize 
// RETURNS       : Nothing
ParserReturnVal_t encoderInit (int mode)
{


return CmdReturnOk;
}
ADD_CMD("encoderinit", encoderInit, "Encoder Initialize")



// FUNCTION      : encoder
// DESCRIPTION   : read the encoder value
// RETURNS       : Nothing
ParserReturnVal_t encoder (int mode)
{
if(mode != CMD_INTERACTIVE) return CmdReturnOk;

encoderVal=0;
TIM1->CNT = 0;
fetch_uint16_arg(&encoderVal);

encoderValue((int16_t)encoderVal);

//counter = TIM1->CNT;

count = ((int)counter);

//printf("User value: %d\n",(int)encoderVal);
//printf("Encoder: %d\n",count);


  timerIntr();
  HAL_TIM_Base_Init(&htim5);    //Timer 5 interrupt is initialized 
  HAL_TIM_Base_Start_IT(&htim5);//Timer 5 interrupt is started  
return CmdReturnOk;
}

ADD_CMD("encoder", encoder, "Encoder output in ticks")



// FUNCTION      : DCIniti
// DESCRIPTION   : Dc motor is intialized 
// RETURNS       : Nothing
ParserReturnVal_t encoderiniti(int mode)
{
    if (mode != CMD_INTERACTIVE)
        return CmdReturnOk;
    flag = 0;
    RCC->AHB1ENR |= (1 << 0);  // turn on the clock of port A
    GPIOA->MODER |= (1 << 10); // make PA5 pin as output
    GPIOA->MODER &= ~(1 << 3); // make PA0 pin as alternate
    encoder_init(); //Encoder initialize

    timerinit();
    timerIntr();
    // TIM2->CR1 |= (1 << 0);     // start the timer

    return CmdReturnOk;
}
ADD_CMD("encoderinit", encoderiniti, "This function initialise the encoder configuration")

